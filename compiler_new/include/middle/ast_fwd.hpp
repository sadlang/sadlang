/**
 * @file ast_fwd.hpp
 * @brief (AR) تصريحات أمامية لجميع أنواع AST المستخدمة في مرحلة التحسين الوسطى
 *        (EN) Forward declarations for all AST types used in middle-end optimization
 * 
 * (AR) هذا الملف يوفّر تصريحات أمامية وأنواع بسيطة تمثل شجرة AST
 *      بشكل مستقل عن المحلل النحوي (parser). يُستخدم في مرحلة async_transform
 *      وأي تحسينات وسطى أخرى تحتاج الوصول إلى بنية AST.
 * 
 * (AR) ملاحظة مهمة:
 *      هذه الأنواع ليست نفس أنواع AST في shared/ast/ — هي أنواع محلية
 *      تُستخدم فقط في سياق تحويلات وتحسينات المترجم.
 */

#ifndef SAD_MIDDLE_AST_FWD_HPP
#define SAD_MIDDLE_AST_FWD_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace ast {

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) تعداد نوع العقدة — يُحدد نوع كل عقدة في شجرة AST
// (EN) Node kind enumeration — identifies each AST node type
// ═══════════════════════════════════════════════════════════════════════════════

enum class NodeKind {
    // (AR) التصريحات / (EN) Declarations
    LetDecl,            // (AR) تصريح متغير: دع س = قيمة
    FunctionDecl,       // (AR) تصريح دالة
    StructDecl,         // (AR) تصريح هيكل
    EnumDecl,           // (AR) تصريح تعداد
    ImplDecl,           // (AR) تصريح تطبيق (impl)
    ModuleDecl,         // (AR) تصريح وحدة
    
    // (AR) الجمل / (EN) Statements
    ExprStmt,           // (AR) جملة تعبير
    ReturnStmt,         // (AR) جملة إرجاع
    Return = ReturnStmt,// (AR) اختصار
    AssignStmt,         // (AR) جملة تعيين
    
    // (AR) التعابير / (EN) Expressions
    If,                 // (AR) تعبير شرط
    While,              // (AR) تعبير حلقة while
    For,                // (AR) تعبير حلقة for
    Match,              // (AR) تعبير مطابقة
    Call,               // (AR) استدعاء دالة
    MethodCall,         // (AR) استدعاء طريقة
    FieldAccess,        // (AR) الوصول لحقل
    Identifier,         // (AR) معرّف
    MacroCall,          // (AR) استدعاء ماكرو
    
    // (AR) أنواع / (EN) Types
    IdentifierType,     // (AR) نوع معرّف
    RefType,            // (AR) نوع مرجع
    GenericType,        // (AR) نوع معمم (قالب)
    
    // (AR) أنماط / (EN) Patterns
    BindingPattern,     // (AR) نمط ربط
    EnumPattern,        // (AR) نمط تعداد
    
    // (AR) async / (EN) Async
    AwaitExpr,          // (AR) تعبير انتظار
    YieldExpr,          // (AR) تعبير إنتاج
    AsyncFunction,      // (AR) دالة غير متزامنة
    
    // (AR) عام / (EN) General
    Block,              // (AR) كتلة أوامر
    Expression,         // (AR) تعبير عام
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) أنواع الدوال غير المتزامنة
// (EN) Async function kinds
// ═══════════════════════════════════════════════════════════════════════════════

enum class AsyncFunctionKind {
    ASYNC,              // (AR) دالة غير متزامنة عادية
    ASYNC_GENERATOR,    // (AR) مولد غير متزامن
    SYNC,               // (AR) دالة متزامنة عادية
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) العقدة الأساسية — أم جميع عقد AST
// (EN) Base node — parent of all AST nodes
// ═══════════════════════════════════════════════════════════════════════════════

struct ASTNode {
    NodeKind kind = NodeKind::Expression;
    int line = 0;
    int column = 0;
    virtual ~ASTNode() = default;
    
    // (AR) دالة الحصول على نوع العقدة — مطلوبة لـ switch في async_transform
    NodeKind getKind() const { return kind; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) عقد الأنواع / (EN) Type nodes
// ═══════════════════════════════════════════════════════════════════════════════

struct TypeNode : ASTNode {
    std::string name;
    TypeNode() { kind = NodeKind::IdentifierType; }
    
    // (AR) استنساخ النوع — مطلوب لنسخ معلومات الأنواع
    virtual std::unique_ptr<TypeNode> clone() const {
        auto c = std::make_unique<TypeNode>();
        c->name = name;
        return c;
    }
};

struct IdentifierTypeNode : TypeNode {
    IdentifierTypeNode() { kind = NodeKind::IdentifierType; }
    explicit IdentifierTypeNode(const std::string& n) { kind = NodeKind::IdentifierType; name = n; }
};

struct RefTypeNode : TypeNode {
    bool is_mutable = false;
    std::unique_ptr<TypeNode> inner;
    RefTypeNode() { kind = NodeKind::RefType; }
    RefTypeNode(std::unique_ptr<TypeNode> inner_type, bool mut = false)
        : is_mutable(mut), inner(std::move(inner_type)) { kind = NodeKind::RefType; }
};

struct GenericTypeNode : TypeNode {
    std::vector<std::unique_ptr<TypeNode>> type_args;
    std::vector<std::unique_ptr<TypeNode>> type_arguments;  // (AR) اسم بديل يستخدمه async_transform
    GenericTypeNode() { kind = NodeKind::GenericType; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) عقد الأنماط / (EN) Pattern nodes
// ═══════════════════════════════════════════════════════════════════════════════

struct PatternNode : ASTNode {
    std::string name;
};

struct BindingPatternNode : PatternNode {
    bool is_mutable = false;
    BindingPatternNode() { kind = NodeKind::BindingPattern; }
    explicit BindingPatternNode(const std::string& n) { kind = NodeKind::BindingPattern; name = n; }
};

struct EnumPatternNode : PatternNode {
    std::string variant;
    std::vector<std::unique_ptr<PatternNode>> fields;
    EnumPatternNode() { kind = NodeKind::EnumPattern; }
    EnumPatternNode(const std::string& enum_name, const std::string& var)
        : variant(var) { kind = NodeKind::EnumPattern; name = enum_name; }
    EnumPatternNode(const std::string& enum_name, const std::string& var,
                    std::vector<std::unique_ptr<PatternNode>> f)
        : variant(var), fields(std::move(f)) { kind = NodeKind::EnumPattern; name = enum_name; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) عقد التعابير / (EN) Expression nodes
// ═══════════════════════════════════════════════════════════════════════════════

struct ExpressionNode : ASTNode {};

struct IdentifierNode : ExpressionNode {
    std::string name;
    IdentifierNode() { kind = NodeKind::Identifier; }
    explicit IdentifierNode(const std::string& n) : name(n) { kind = NodeKind::Identifier; }
};

struct CallNode : ExpressionNode {
    std::unique_ptr<ASTNode> callee;   // (AR) يمكن أن يكون معرّف أو تعبير
    std::vector<std::unique_ptr<ASTNode>> arguments;
    CallNode() { kind = NodeKind::Call; }
};

struct MethodCallNode : ExpressionNode {
    std::unique_ptr<ASTNode> receiver;
    std::string method_name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    MethodCallNode() { kind = NodeKind::MethodCall; }
    MethodCallNode(std::unique_ptr<ASTNode> recv, const std::string& method)
        : receiver(std::move(recv)), method_name(method) { kind = NodeKind::MethodCall; }
};

struct FieldAccessNode : ExpressionNode {
    std::unique_ptr<ASTNode> receiver;
    std::string field;
    FieldAccessNode() { kind = NodeKind::FieldAccess; }
    FieldAccessNode(std::unique_ptr<ASTNode> recv, const std::string& f)
        : receiver(std::move(recv)), field(f) { kind = NodeKind::FieldAccess; }
};

struct AwaitExprNode : ExpressionNode {
    std::unique_ptr<ASTNode> expr;
    AwaitExprNode() { kind = NodeKind::AwaitExpr; }
};

struct YieldExprNode : ExpressionNode {
    std::unique_ptr<ASTNode> value;
    YieldExprNode() { kind = NodeKind::YieldExpr; }
};

struct MacroCallNode : ExpressionNode {
    std::string macro_name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    MacroCallNode() { kind = NodeKind::MacroCall; }
    MacroCallNode(const std::string& name, std::unique_ptr<ASTNode> arg)
        : macro_name(name) {
        kind = NodeKind::MacroCall;
        arguments.push_back(std::move(arg));
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) عقد الجمل / (EN) Statement nodes
// ═══════════════════════════════════════════════════════════════════════════════

struct BlockNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;   // (AR) جمل الكتلة
    BlockNode() { kind = NodeKind::Block; }
};

struct ReturnNode : ASTNode {
    std::unique_ptr<ASTNode> value;   // (AR) قيمة الإرجاع
    ReturnNode() { kind = NodeKind::ReturnStmt; }
};

struct ExprStmtNode : ASTNode {
    std::unique_ptr<ASTNode> expression;
    ExprStmtNode() { kind = NodeKind::ExprStmt; }
    explicit ExprStmtNode(std::unique_ptr<ASTNode> expr)
        : expression(std::move(expr)) { kind = NodeKind::ExprStmt; }
};

struct AssignNode : ASTNode {
    std::unique_ptr<ASTNode> target;  // (AR) الهدف (حقل أو معرّف)
    std::unique_ptr<ASTNode> value;   // (AR) القيمة الجديدة
    AssignNode() { kind = NodeKind::AssignStmt; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) عقد التحكم في التدفق / (EN) Control flow nodes
// ═══════════════════════════════════════════════════════════════════════════════

struct IfNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<BlockNode> then_block;      // (AR) فرع then
    std::unique_ptr<BlockNode> else_block;      // (AR) فرع else
    IfNode() { kind = NodeKind::If; }
};

struct WhileNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<BlockNode> body;            // (AR) جسم الحلقة
    WhileNode() { kind = NodeKind::While; }
};

struct ForNode : ASTNode {
    std::string variable;
    std::unique_ptr<ASTNode> iterable;
    std::unique_ptr<BlockNode> body;            // (AR) جسم الحلقة
    ForNode() { kind = NodeKind::For; }
};

struct MatchArm {
    std::unique_ptr<PatternNode> pattern;
    std::unique_ptr<ASTNode> guard;
    std::unique_ptr<ASTNode> body;
};

struct MatchNode : ASTNode {
    std::unique_ptr<ASTNode> scrutinee;
    std::vector<MatchArm> arms;
    MatchNode() { kind = NodeKind::Match; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) عقد التصريحات / (EN) Declaration nodes
// ═══════════════════════════════════════════════════════════════════════════════

struct Parameter {
    std::string name;
    std::unique_ptr<TypeNode> type;
    bool is_mutable = false;
    bool is_mut_ref = false;  // (AR) هل هو مرجع قابل للتغيير (&mut)
    
    // (AR) استنساخ المعامل
    std::unique_ptr<TypeNode> cloneType() const {
        return type ? type->clone() : nullptr;
    }
};

struct LetDeclNode : ASTNode {
    std::string name;
    bool is_mutable = false;
    std::unique_ptr<TypeNode> type;
    std::unique_ptr<ASTNode> value;
    std::unique_ptr<ASTNode> initializer;   // (AR) اسم بديل للقيمة الأولية
    LetDeclNode() { kind = NodeKind::LetDecl; }
};

struct AssociatedTypeDecl {
    std::string name;
    std::unique_ptr<TypeNode> default_type;
    std::unique_ptr<TypeNode> type;         // (AR) النوع المرتبط
};

struct FunctionNode : ASTNode {
    std::string name;
    std::vector<Parameter> parameters;
    std::unique_ptr<TypeNode> return_type;
    std::unique_ptr<BlockNode> body;
    bool is_public = false;
    FunctionNode() { kind = NodeKind::FunctionDecl; }
};

struct AsyncFunctionNode : ASTNode {
    std::string name;
    AsyncFunctionKind func_kind = AsyncFunctionKind::ASYNC;
    std::vector<Parameter> parameters;
    std::unique_ptr<TypeNode> return_type;
    std::unique_ptr<BlockNode> body;                    // (AR) جسم الدالة (مؤشر ذكي)
    
    AsyncFunctionNode() { kind = NodeKind::AsyncFunction; }
};

struct StructField {
    std::string name;
    std::unique_ptr<TypeNode> type;
    bool is_public = false;
};

struct StructNode : ASTNode {
    std::string name;
    std::vector<StructField> fields;
    std::vector<Parameter> type_params;
    StructNode() { kind = NodeKind::StructDecl; }
};

struct EnumVariant {
    std::string name;
    std::vector<std::unique_ptr<TypeNode>> fields;
};

// (AR) عقدة إنشاء قيمة تعداد (تعبير) — مثل: حالة_استطلاع::جاهز
// (EN) Enum variant value construction expression
struct EnumVariantNode : ExpressionNode {
    std::string enum_name;
    std::string variant_name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    EnumVariantNode() { kind = NodeKind::Expression; }
    EnumVariantNode(const std::string& en, const std::string& vn)
        : enum_name(en), variant_name(vn) { kind = NodeKind::Expression; }
};

struct EnumNode : ASTNode {
    std::string name;
    std::vector<EnumVariant> variants;
    EnumNode() { kind = NodeKind::EnumDecl; }
};

struct ImplNode : ASTNode {
    std::string type_name;
    std::string trait_name;
    std::vector<std::unique_ptr<FunctionNode>> methods;
    std::vector<AssociatedTypeDecl> associated_types;
    ImplNode() { kind = NodeKind::ImplDecl; }
};

struct ModuleNode : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> items;
    ModuleNode() { kind = NodeKind::ModuleDecl; }
};

} // namespace ast

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) أنواع البيانات / (EN) Data types
// (AR) types::Type هو اسم بديل لـ ast::TypeNode لتوحيد الواجهة
// ═══════════════════════════════════════════════════════════════════════════════

namespace types {

using Type = ast::TypeNode;

struct FutureType : Type {
    std::unique_ptr<Type> inner_type;
    FutureType() { name = "مستقبل"; }
    explicit FutureType(std::unique_ptr<Type> inner)
        : inner_type(std::move(inner)) { name = "مستقبل"; }
};

} // namespace types

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) مبلّغ الأخطاء / (EN) Error reporter
// ═══════════════════════════════════════════════════════════════════════════════

namespace errors {

class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    virtual void error(const std::string& msg) {}
    virtual void warning(const std::string& msg) {}
};

} // namespace errors

#endif // SAD_MIDDLE_AST_FWD_HPP
