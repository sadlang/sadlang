// ======================================================================
// sir_builder.h - بناء SIR من AST / SIR Builder from AST
// ======================================================================
// الوصف بالعربية:
//   محول شامل من Abstract Syntax Tree إلى S Intermediate Representation
//   يقوم بتحويل العقد النحوية إلى تعليمات SIR مع إدارة السجلات والنطاقات
//
// English Description:
//   Comprehensive converter from Abstract Syntax Tree to S Intermediate Representation
//   Converts syntax nodes to SIR instructions with register and scope management
//
// الميزات / Features:
//   - تحويل التعابير إلى تعليمات / Expression to instruction conversion
//   - تحويل الجمل إلى control flow / Statement to control flow conversion
//   - إدارة السجلات الافتراضية / Virtual register management
//   - إدارة Basic Blocks / Basic block management
//   - بناء Control Flow Graph / CFG construction
//   - معالجة الأخطاء الدلالية / Semantic error handling
//
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: December 3, 2025
// الإصدار / Version: 1.0
// ======================================================================

#pragma once

#include "sir_module.h"
#include "sir_instruction.h"
#include "sir_types.h"
#include "parser/ast/ast_node.h"
#include "parser/ast/expressions.h"
#include "parser/ast/statements.h"
#include "parser/ast/declarations.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stack>

namespace Sad {
namespace Compiler {
namespace SIR {

// ======================================================================
// AST Type Aliases - توافق مع الأسماء القديمة
// AST Type Compatibility Aliases
// ======================================================================

// Map old AST node names to new ones for backward compatibility
// ربط أسماء AST القديمة بالجديدة للتوافق
namespace AST {
    using ProgramNode = Sad::AST::StmtList;
    using StatementNode = Sad::AST::Statement;
    using ExpressionNode = Sad::AST::Expression;
    using FunctionDeclNode = Sad::AST::FunctionDecl;
    using VariableDeclNode = Sad::AST::VarDeclStmt;
    using ClassDeclNode = Sad::AST::ClassDecl;
    using IfStatementNode = Sad::AST::IfStmt;
    using WhileLoopNode = Sad::AST::WhileStmt;
    using ForLoopNode = Sad::AST::ForStmt;
    using ReturnStatementNode = Sad::AST::ReturnStmt;
    using AssignmentNode = Sad::AST::AssignExpr;
    using BinaryOpNode = Sad::AST::BinaryExpr;
    using UnaryOpNode = Sad::AST::UnaryExpr;
    using FunctionCallNode = Sad::AST::CallExpr;
    using MethodCallNode = Sad::AST::MemberExpr;
    using VariableNode = Sad::AST::VariableExpr;
    using LiteralNode = Sad::AST::LiteralExpr;
    
    // (AR) أسماء مباشرة للأصناف الجديدة بدون Node suffix
    // (EN) Direct names for new classes without Node suffix
    using Statement = Sad::AST::Statement;
    using IfStmt = Sad::AST::IfStmt;
    using WhileStmt = Sad::AST::WhileStmt;
    using ForStmt = Sad::AST::ForStmt;
    using ReturnStmt = Sad::AST::ReturnStmt;
    using BreakStmt = Sad::AST::BreakStmt;
    using ContinueStmt = Sad::AST::ContinueStmt;
    using ExprStmt = Sad::AST::ExprStmt;
    using VarDeclStmt = Sad::AST::VarDeclStmt;
    using AssignExpr = Sad::AST::AssignExpr;
    using FieldDecl = Sad::AST::FieldDecl;
    using MethodDecl = Sad::AST::MethodDecl;
    
    // Operator types - enums not classes
    using BinaryOperator = Sad::Lexer::TokenType;
    using UnaryOperator = Sad::Lexer::TokenType;
}

// ======================================================================
// تعريفات مساعدة / Helper Definitions
// ======================================================================

/**
 * @brief (AR) نتيجة بناء تعبير - تحتوي على السجل والنوع
 * @brief (EN) Expression build result - contains register and type
 * 
 * @details
 * (AR) تُستخدم لتمرير نتيجة بناء تعبير مع معلومات إضافية
 * 
 * (EN) Used to pass expression build result with additional information
 */
struct BuildResult {
    std::string registerName;      ///< (AR) اسم السجل الافتراضي / (EN) Virtual register name
    SIRType type;                  ///< (AR) نوع البيانات / (EN) Data type
    bool isConstant = false;       ///< (AR) هل قيمة ثابتة؟ / (EN) Is constant value?
    std::string constantValue;     ///< (AR) القيمة الثابتة (إن وجدت) / (EN) Constant value (if any)
    
    // (AR) معلومات إضافية للأنواع المركبة / Additional info for composite types
    SIRType elementType;           ///< (AR) نوع عنصر المصفوفة / (EN) Array element type
    std::string className;         ///< (AR) اسم الصنف للكائنات / (EN) Class name for objects
    
    /**
     * @brief (AR) منشئ افتراضي
     * @brief (EN) Default constructor
     */
    BuildResult() : type(SIRType::VOID), elementType(SIRType::VOID) {}
    
    /**
     * @brief (AR) منشئ بسجل ونوع
     * @brief (EN) Constructor with register and type
     */
    BuildResult(const std::string& reg, SIRType t) 
        : registerName(reg), type(t), isConstant(false), elementType(SIRType::VOID) {}
    
    /**
     * @brief (AR) منشئ لقيمة ثابتة
     * @brief (EN) Constructor for constant value
     */
    BuildResult(const std::string& value, SIRType t, bool isConst) 
        : constantValue(value), type(t), isConstant(isConst), elementType(SIRType::VOID) {}
};

/**
 * @brief (AR) معلومات المتغير في النطاق الحالي
 * @brief (EN) Variable information in current scope
 * 
 * @details
 * (AR) تُستخدم لتتبع المتغيرات عبر النطاقات المختلفة
 * 
 * (EN) Used to track variables across different scopes
 */
struct VariableInfo {
    std::string name;              ///< (AR) الاسم / (EN) Name
    SIRType type;                  ///< (AR) النوع / (EN) Type
    std::string registerName;      ///< (AR) اسم السجل / (EN) Register name
    bool isGlobal;                 ///< (AR) متغير عام؟ / (EN) Is global?
    bool isMutable;                ///< (AR) قابل للتعديل؟ / (EN) Is mutable?
    int scopeLevel;                ///< (AR) مستوى النطاق / (EN) Scope level
    
    /**
     * @brief (AR) منشئ افتراضي
     * @brief (EN) Default constructor
     */
    VariableInfo() : type(SIRType::VOID), isGlobal(false), isMutable(true), scopeLevel(0) {}
};

/**
 * @brief (AR) معلومات الدالة للجدول
 * @brief (EN) Function information for lookup table
 */
struct FunctionInfo {
    std::string name;                          ///< (AR) الاسم / (EN) Name
    SIRType returnType;                        ///< (AR) نوع الإرجاع / (EN) Return type
    std::vector<SIRParameter> parameters;      ///< (AR) المعاملات / (EN) Parameters
    std::shared_ptr<SIRFunction> sirFunction;  ///< (AR) مؤشر لدالة SIR / (EN) Pointer to SIR function
    
    /**
     * @brief (AR) منشئ افتراضي
     * @brief (EN) Default constructor
     */
    FunctionInfo() : returnType(SIRType::VOID) {}
};

/**
 * @brief (AR) سياق الحلقة - لمعالجة break/continue
 * @brief (EN) Loop context - for handling break/continue
 */
struct LoopContext {
    std::string continueLabel;     ///< (AR) تسمية continue / (EN) Continue label
    std::string breakLabel;        ///< (AR) تسمية break / (EN) Break label
};

/**
 * @brief (AR) معلومات معامل النوع العام (Generic Type Parameter)
 * @brief (EN) Generic type parameter information
 * 
 * @details
 * (AR) يُستخدم لدعم الدوال والأصناف العامة (Generics/Templates)
 * مثل: function map<T>(arr: T[], fn: (T) -> T) -> T[]
 * 
 * (EN) Used to support generic functions and classes (Generics/Templates)
 * Example: function map<T>(arr: T[], fn: (T) -> T) -> T[]
 */
struct TypeParameter {
    std::string name;              ///< (AR) اسم معامل النوع (مثل T, U, K, V) / (EN) Type parameter name (e.g., T, U, K, V)
    SIRType constraintType;        ///< (AR) قيد النوع (اختياري) / (EN) Type constraint (optional)
    bool hasConstraint;            ///< (AR) هل يوجد قيد؟ / (EN) Has constraint?
    
    /**
     * @brief (AR) منشئ افتراضي
     * @brief (EN) Default constructor
     */
    TypeParameter() : constraintType(SIRType::VOID), hasConstraint(false) {}
    
    /**
     * @brief (AR) منشئ باسم
     * @brief (EN) Constructor with name
     */
    TypeParameter(const std::string& n) 
        : name(n), constraintType(SIRType::VOID), hasConstraint(false) {}
};

/**
 * @brief (AR) نطاق معاملات الأنواع العامة
 * @brief (EN) Generic type parameters scope
 * 
 * @details
 * (AR) يحتفظ بمعاملات الأنواع العامة والاستبدالات الحالية
 * يُستخدم عند بناء دوال أو أصناف عامة
 * 
 * (EN) Maintains generic type parameters and current substitutions
 * Used when building generic functions or classes
 */
struct GenericScope {
    std::vector<TypeParameter> typeParameters;                     ///< (AR) معاملات الأنواع / (EN) Type parameters
    std::unordered_map<std::string, SIRType> typeSubstitutions;   ///< (AR) الاستبدالات الحالية / (EN) Current substitutions
    
    /**
     * @brief (AR) التحقق من وجود معامل نوع
     * @brief (EN) Check if type parameter exists
     */
    bool hasTypeParameter(const std::string& name) const {
        for (const auto& param : typeParameters) {
            if (param.name == name) return true;
        }
        return false;
    }
    
    /**
     * @brief (AR) الحصول على النوع المستبدل
     * @brief (EN) Get substituted type
     */
    SIRType getSubstitutedType(const std::string& name) const {
        auto it = typeSubstitutions.find(name);
        return (it != typeSubstitutions.end()) ? it->second : SIRType::VOID;
    }
};

// ======================================================================
// فئة SIRBuilder الرئيسية / Main SIRBuilder Class
// ======================================================================

/**
 * @brief (AR) بناء SIR من AST - المحول الرئيسي
 * @brief (EN) Build SIR from AST - Main Converter
 * 
 * @details
 * (AR) محول شامل يقوم بتحويل شجرة AST الناتجة من المحلل النحوي
 * إلى تمثيل SIR المتوسط مع إدارة كاملة للسجلات والنطاقات وControl Flow
 * 
 * (EN) Comprehensive converter that transforms AST tree from parser
 * to SIR intermediate representation with full register, scope, and control flow management
 * 
 * @example
 * ```cpp
 * // AR: بناء SIR من برنامج
 * // EN: Build SIR from program
 * SIRBuilder builder;
 * auto programAST = parser.parse(sourceCode);
 * auto sirModule = builder.buildModule(programAST);
 * 
 * // AR: التحقق من الأخطاء
 * // EN: Check for errors
 * if (builder.hasErrors()) {
 *     for (const auto& error : builder.getErrors()) {
 *         std::cerr << error << std::endl;
 *     }
 * }
 * 
 * // AR: طباعة SIR
 * // EN: Print SIR
 * std::cout << sirModule->toString() << std::endl;
 * ```
 */
class SIRBuilder {
public:
    // ==================================================================
    // المنشئ والمدمر / Constructor & Destructor
    // ==================================================================
    
    /**
     * @brief (AR) منشئ افتراضي - يبدأ الحالة الأولية
     * @brief (EN) Default constructor - initializes state
     */
    SIRBuilder();
    
    /**
     * @brief (AR) مدمر افتراضي
     * @brief (EN) Default destructor
     */
    ~SIRBuilder() = default;
    
    // منع النسخ / Prevent copying
    SIRBuilder(const SIRBuilder&) = delete;
    SIRBuilder& operator=(const SIRBuilder&) = delete;
    
    // ==================================================================
    // الدوال الرئيسية / Main Functions
    // ==================================================================
    
    /**
     * @brief (AR) بناء وحدة SIR كاملة من شجرة AST
     * @brief (EN) Build complete SIR module from AST tree
     * 
     * @param program (AR) عقدة البرنامج من المحلل النحوي / (EN) Program node from parser
     * @return (AR) وحدة SIR كاملة أو nullptr في حالة الخطأ / (EN) Complete SIR module or nullptr on error
     * 
     * @details
     * (AR) هذه الدالة هي نقطة الدخول الرئيسية لبناء SIR
     * تقوم بمعالجة جميع التصريحات العامة والدوال والأصناف
     * 
     * (EN) This is the main entry point for building SIR
     * Processes all global declarations, functions, and classes
     */
    std::shared_ptr<SIRModule> buildModule(AST::ProgramNode* program);
    
    // ==================================================================
    // بناء التصريحات / Building Declarations
    // ==================================================================
    
    /**
     * @brief (AR) بناء دالة كاملة
     * @brief (EN) Build complete function
     * 
     * @param funcDecl (AR) تصريح الدالة / (EN) Function declaration
     * 
     * @details
     * (AR) يبني الدالة مع جميع معاملاتها وجسمها
     * (EN) Builds function with all parameters and body
     */
    void buildFunction(AST::FunctionDeclNode* funcDecl);
    
    /**
     * @brief (AR) بناء متغير عام
     * @brief (EN) Build global variable
     * 
     * @param varDecl (AR) تصريح المتغير / (EN) Variable declaration
     */
    void buildGlobalVariable(AST::VariableDeclNode* varDecl);
    
    /**
     * @brief (AR) بناء صنف
     * @brief (EN) Build class
     * 
     * @param classDecl (AR) تصريح الصنف / (EN) Class declaration
     */
    void buildClass(AST::ClassDeclNode* classDecl);
    
    // ==================================================================
    // بناء الجمل / Building Statements
    // ==================================================================
    
    /**
     * @brief (AR) بناء جملة - موزع للأنواع المختلفة
     * @brief (EN) Build statement - dispatcher for different types
     * 
     * @param stmt (AR) عقدة الجملة / (EN) Statement node
     */
    void buildStatement(AST::Statement* stmt);
    
    /**
     * @brief (AR) بناء جملة if مع else اختياري
     * @brief (EN) Build if statement with optional else
     * 
     * @param ifStmt (AR) جملة if / (EN) If statement
     * 
     * @details
     * (AR) يبني CFG مع basic blocks للشرط وthen وelse
     * (EN) Builds CFG with basic blocks for condition, then, and else
     */
    void buildIfStatement(AST::IfStmt* ifStmt);
    
    /**
     * @brief (AR) بناء حلقة while
     * @brief (EN) Build while loop
     * 
     * @param whileLoop (AR) حلقة while / (EN) While loop
     */
    void buildWhileLoop(AST::WhileStmt* whileLoop);
    
    /**
     * @brief (AR) بناء حلقة for
     * @brief (EN) Build for loop
     * 
     * @param forLoop (AR) حلقة for / (EN) For loop
     */
    void buildForLoop(AST::ForStmt* forLoop);
    
    /**
     * @brief (AR) بناء جملة return
     * @brief (EN) Build return statement
     * 
     * @param retStmt (AR) جملة return / (EN) Return statement
     */
    void buildReturnStatement(AST::ReturnStmt* retStmt);
    
    /**
     * @brief (AR) بناء جملة break
     * @brief (EN) Build break statement
     * 
     * @param breakStmt (AR) جملة break / (EN) Break statement
     */
    void buildBreakStatement(AST::BreakStmt* breakStmt);
    
    /**
     * @brief (AR) بناء جملة continue
     * @brief (EN) Build continue statement
     * 
     * @param continueStmt (AR) جملة continue / (EN) Continue statement
     */
    void buildContinueStatement(AST::ContinueStmt* continueStmt);
    
    /**
     * @brief (AR) بناء إسناد متغير
     * @brief (EN) Build variable assignment
     * 
     * @param assignment (AR) جملة الإسناد / (EN) Assignment statement
     */
    void buildAssignment(AST::AssignExpr* assignment);
    
    /**
     * @brief (AR) بناء تصريح متغير محلي
     * @brief (EN) Build local variable declaration
     * 
     * @param varDecl (AR) تصريح المتغير / (EN) Variable declaration
     */
    void buildLocalVariable(AST::VarDeclStmt* varDecl);
    
    // ==================================================================
    // بناء التعابير / Building Expressions
    // ==================================================================
    
    /**
     * @brief (AR) بناء تعبير - موزع للأنواع المختلفة
     * @brief (EN) Build expression - dispatcher for different types
     * 
     * @param expr (AR) عقدة التعبير / (EN) Expression node
     * @return (AR) نتيجة البناء مع السجل والنوع / (EN) Build result with register and type
     */
    BuildResult buildExpression(AST::ExpressionNode* expr);
    
    /**
     * @brief (AR) بناء عملية ثنائية (+ - * / % < > == && ||)
     * @brief (EN) Build binary operation (+ - * / % < > == && ||)
     * 
     * @param binOp (AR) عملية ثنائية / (EN) Binary operation
     * @return (AR) نتيجة العملية / (EN) Operation result
     */
    BuildResult buildBinaryOp(AST::BinaryOpNode* binOp);
    
    /**
     * @brief (AR) بناء عملية أحادية (- !)
     * @brief (EN) Build unary operation (- !)
     * 
     * @param unOp (AR) عملية أحادية / (EN) Unary operation
     * @return (AR) نتيجة العملية / (EN) Operation result
     */
    BuildResult buildUnaryOp(AST::UnaryOpNode* unOp);
    
    /**
     * @brief (AR) بناء استدعاء دالة
     * @brief (EN) Build function call
     * 
     * @param call (AR) استدعاء دالة / (EN) Function call
     * @return (AR) قيمة الإرجاع / (EN) Return value
     */
    BuildResult buildFunctionCall(AST::FunctionCallNode* call);
    
    /**
     * @brief (AR) بناء وصول لمتغير
     * @brief (EN) Build variable access
     * 
     * @param var (AR) متغير / (EN) Variable
     * @return (AR) قيمة المتغير / (EN) Variable value
     */
    BuildResult buildVariableAccess(AST::VariableNode* var);
    
    /**
     * @brief (AR) بناء ثابت (رقم، نص، منطقي)
     * @brief (EN) Build literal (number, string, boolean)
     * 
     * @param literal (AR) ثابت / (EN) Literal
     * @return (AR) القيمة الثابتة / (EN) Constant value
     */
    BuildResult buildLiteral(AST::LiteralNode* literal);
    
    /**
     * @brief (AR) بناء استدعاء method على نص
     * @brief (EN) Build string method call
     * 
     * @param methodCall (AR) استدعاء method / (EN) Method call
     * @return (AR) نتيجة البناء / (EN) Build result
     * 
     * @details
     * (AR) يدعم methods مثل: length(), substring(), indexOf(), charAt()
     * (EN) Supports methods like: length(), substring(), indexOf(), charAt()
     */
    BuildResult buildStringMethod(AST::MethodCallNode* methodCall);
    
    // ==================================================================
    // إدارة السجلات / Register Management
    // ==================================================================
    
    /**
     * @brief (AR) إنشاء سجل مؤقت جديد بشكل تلقائي
     * @brief (EN) Create new temporary register automatically
     * 
     * @return (AR) اسم السجل (مثل %0, %1, %2) / (EN) Register name (e.g., %0, %1, %2)
     */
    std::string newTempRegister();
    
    /**
     * @brief (AR) إنشاء تسمية جديدة لـ basic block
     * @brief (EN) Create new label for basic block
     * 
     * @param prefix (AR) بادئة التسمية (L, if, loop, etc) / (EN) Label prefix (L, if, loop, etc)
     * @return (AR) اسم التسمية الكامل / (EN) Full label name
     */
    std::string newLabel(const std::string& prefix = "L");
    
    // ==================================================================
    // إدارة Basic Blocks / Basic Block Management
    // ==================================================================
    
    /**
     * @brief (AR) إنشاء كتلة أساسية جديدة
     * @brief (EN) Create new basic block
     * 
     * @param name (AR) اسم الكتلة / (EN) Block name
     * @return (AR) مؤشر للكتلة / (EN) Pointer to block
     */
    std::shared_ptr<SIRBasicBlock> createBasicBlock(const std::string& name);
    
    /**
     * @brief (AR) تعيين الكتلة الحالية للكتابة فيها
     * @brief (EN) Set current block for writing
     * 
     * @param block (AR) الكتلة الجديدة / (EN) New block
     */
    void setCurrentBlock(std::shared_ptr<SIRBasicBlock> block);
    
    /**
     * @brief (AR) الحصول على الكتلة الحالية
     * @brief (EN) Get current block
     * 
     * @return (AR) الكتلة الحالية / (EN) Current block
     */
    std::shared_ptr<SIRBasicBlock> getCurrentBlock() const;
    
    /**
     * @brief (AR) إضافة تعليمة للكتلة الحالية
     * @brief (EN) Add instruction to current block
     * 
     * @param inst (AR) التعليمة / (EN) Instruction
     */
    void addInstruction(const SIRInstruction& inst);
    
    // ==================================================================
    // إدارة النطاقات / Scope Management
    // ==================================================================
    
    /**
     * @brief (AR) فتح نطاق جديد (عند دخول {})
     * @brief (EN) Enter new scope (when entering {})
     */
    void enterScope();
    
    /**
     * @brief (AR) إغلاق النطاق الحالي (عند الخروج من {})
     * @brief (EN) Exit current scope (when exiting {})
     */
    void exitScope();
    
    /**
     * @brief (AR) إضافة متغير للنطاق الحالي
     * @brief (EN) Add variable to current scope
     * 
     * @param varInfo (AR) معلومات المتغير / (EN) Variable info
     */
    void addVariable(const VariableInfo& varInfo);
    
    /**
     * @brief (AR) البحث عن متغير في النطاقات
     * @brief (EN) Lookup variable in scopes
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) معلومات المتغير أو nullptr / (EN) Variable info or nullptr
     */
    VariableInfo* lookupVariable(const std::string& name);
    
    // ==================================================================
    // إدارة الحلقات / Loop Management
    // ==================================================================
    
    /**
     * @brief (AR) دخول سياق حلقة
     * @brief (EN) Enter loop context
     * 
     * @param ctx (AR) سياق الحلقة / (EN) Loop context
     */
    void enterLoop(const LoopContext& ctx);
    
    /**
     * @brief (AR) الخروج من سياق الحلقة
     * @brief (EN) Exit loop context
     */
    void exitLoop();
    
    /**
     * @brief (AR) الحصول على سياق الحلقة الحالية
     * @brief (EN) Get current loop context
     * 
     * @return (AR) سياق الحلقة أو nullptr / (EN) Loop context or nullptr
     */
    LoopContext* getCurrentLoop();
    
    // ==================================================================
    // إدارة الأنواع العامة / Generic Types Management
    // ==================================================================
    
    /**
     * @brief (AR) دخول نطاق أنواع عامة (عند تعريف دالة أو صنف عام)
     * @brief (EN) Enter generic types scope (when defining generic function/class)
     * 
     * @param typeParams (AR) قائمة معاملات الأنواع / (EN) Type parameters list
     */
    void enterGenericScope(const std::vector<TypeParameter>& typeParams);
    
    /**
     * @brief (AR) الخروج من نطاق الأنواع العامة
     * @brief (EN) Exit generic types scope
     */
    void exitGenericScope();
    
    /**
     * @brief (AR) تعيين استبدال نوع (عند استدعاء دالة عامة)
     * @brief (EN) Set type substitution (when calling generic function)
     * 
     * @param paramName (AR) اسم معامل النوع / (EN) Type parameter name
     * @param concreteType (AR) النوع الفعلي / (EN) Concrete type
     */
    void setTypeSubstitution(const std::string& paramName, SIRType concreteType);
    
    /**
     * @brief (AR) حل نوع (قد يكون معامل نوع عام)
     * @brief (EN) Resolve type (might be generic type parameter)
     * 
     * @param typeName (AR) اسم النوع / (EN) Type name
     * @return (AR) النوع المحلول (SIRType فعلي أو VOID إذا لم يُحل) / (EN) Resolved type (actual SIRType or VOID if not resolved)
     */
    SIRType resolveType(const std::string& typeName);
    
    /**
     * @brief (AR) التحقق من صحة معاملات الأنواع
     * @brief (EN) Validate type parameters
     * 
     * @param typeParams (AR) معاملات الأنواع / (EN) Type parameters
     * @return true (AR) إذا كانت صحيحة / (EN) if valid
     */
    bool validateTypeParameters(const std::vector<TypeParameter>& typeParams);
    
    // ==================================================================
    // معالجة الأخطاء / Error Handling
    // ==================================================================
    
    /**
     * @brief (AR) إصدار خطأ مع رسالة
     * @brief (EN) Report error with message
     * 
     * @param message (AR) رسالة الخطأ / (EN) Error message
     */
    void reportError(const std::string& message);
    
    /**
     * @brief (AR) هل توجد أخطاء؟
     * @brief (EN) Has errors?
     * 
     * @return true (AR) إذا كان هناك أخطاء / (EN) if there are errors
     */
    bool hasErrors() const { return !errors_.empty(); }
    
    /**
     * @brief (AR) الحصول على قائمة الأخطاء
     * @brief (EN) Get list of errors
     * 
     * @return (AR) مصفوفة الأخطاء / (EN) Error array
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
private:
    // ==================================================================
    // الحالة الداخلية / Internal State
    // ==================================================================
    
    std::shared_ptr<SIRModule> module_;                 ///< (AR) الوحدة الحالية / (EN) Current module
    std::shared_ptr<SIRFunction> currentFunction_;      ///< (AR) الدالة الحالية / (EN) Current function
    std::shared_ptr<SIRBasicBlock> currentBlock_;       ///< (AR) الكتلة الحالية / (EN) Current block
    
    int nextTempRegister_;                              ///< (AR) رقم السجل المؤقت التالي / (EN) Next temp register number
    int nextLabel_;                                     ///< (AR) رقم التسمية التالية / (EN) Next label number
    int currentScopeLevel_;                             ///< (AR) مستوى النطاق الحالي / (EN) Current scope level
    
    // (AR) مكدس النطاقات - كل مستوى يحتوي على خريطة المتغيرات
    // (EN) Scope stack - each level contains variable map
    std::vector<std::unordered_map<std::string, VariableInfo>> scopeStack_;
    
    // (AR) جدول الدوال / (EN) Function table
    std::unordered_map<std::string, FunctionInfo> functionTable_;
    
    // (AR) جدول الأصناف / (EN) Class table
    std::unordered_map<std::string, std::shared_ptr<SIRClass>> classTable_;
    
    // (AR) مكدس سياق الحلقات / (EN) Loop context stack
    std::vector<LoopContext> loopStack_;
    
    // (AR) مكدس نطاقات الأنواع العامة / (EN) Generic scopes stack
    std::vector<GenericScope> genericScopeStack_;
    
    // (AR) قائمة الأخطاء / (EN) Error list
    std::vector<std::string> errors_;
    
    // ==================================================================
    // دوال مساعدة خاصة / Private Helper Functions
    // ==================================================================
    
    /**
     * @brief (AR) تحويل نوع AST إلى SIRType
     * @brief (EN) Convert AST Type to SIRType
     */
    SIRType astTypeToSIRType(const Sad::Data::DataType& astType);
    
    /**
     * @brief (AR) تحويل عامل ثنائي AST إلى SIR opcode
     * @brief (EN) Convert AST binary operator to SIR opcode
     */
    SIROpcode binaryOpToOpcode(AST::BinaryOperator op, SIRType leftType);
    
    /**
     * @brief (AR) تحويل عامل أحادي AST إلى SIR opcode
     * @brief (EN) Convert AST unary operator to SIR opcode
     */
    SIROpcode unaryOpToOpcode(AST::UnaryOperator op, SIRType operandType);
    
    /**
     * @brief (AR) التحقق من توافق الأنواع
     * @brief (EN) Check type compatibility
     */
    bool areTypesCompatible(SIRType t1, SIRType t2);
    
    /**
     * @brief (AR) إنشاء تعليمة تحويل نوع إذا لزم
     * @brief (EN) Create type conversion instruction if needed
     */
    BuildResult convertType(const BuildResult& value, SIRType targetType);
};

} // namespace SIR
} // namespace Compiler
} // namespace Sad
