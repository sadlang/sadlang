/**
 * @file ast_clone.cpp
 * @brief (AR) تنفيذ استنساخ عميق لجميع عُقد AST الأساسية
 * @brief (EN) Implementation of deep clone for all core AST nodes
 * 
 * (AR) يوفر استنساخاً عميقاً لعُقد شجرة التركيب المجرد.
 *      متوافق مع وضع freestanding — لا يستخدم استثناءات.
 *      يُستخدم لنسخ AST في: قوالب، ماكرو، دوال سمات افتراضية.
 * (EN) Provides deep cloning for AST nodes.
 *      Freestanding compatible — no exceptions.
 *      Used for AST copying in: templates, macros, trait default methods.
 * 
 * @author Sad Language Development Team
 * @date 2025
 */

#include "ast_clone.h"
#include "expressions.h"
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"

namespace Sad {
namespace AST {

// ═══════════════════════════════════════════════════════════════════════════
// (AR) تصريحات أمامية / (EN) Forward declarations
// ═══════════════════════════════════════════════════════════════════════════

static ExprPtr cloneE(const ExprPtr& e);
static StmtPtr cloneS(const StmtPtr& s);

static ExprList cloneEList(const ExprList& list) {
    ExprList result;
    result.reserve(list.size());
    for (const auto& e : list)
        result.push_back(cloneE(e));
    return result;
}

static StmtList cloneSList(const StmtList& list) {
    StmtList result;
    result.reserve(list.size());
    for (const auto& s : list)
        result.push_back(cloneS(s));
    return result;
}

static std::vector<Parameter> cloneParams(const std::vector<Parameter>& params) {
    std::vector<Parameter> result;
    result.reserve(params.size());
    for (const auto& p : params) {
        Parameter np(p.name, p.type);
        np.defaultValue = p.defaultValue; // shared_ptr — safe copy
        result.push_back(std::move(np));
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════
// (AR) استنساخ التعابير / (EN) Expression Cloning
// ═══════════════════════════════════════════════════════════════════════════

ExprPtr cloneExpression(const Expression& expr) {
    // (AR) مطابقة نوع التعبير عبر dynamic_cast
    // (EN) Match expression type via dynamic_cast

    if (auto* p = dynamic_cast<const LiteralExpr*>(&expr))
        return std::make_unique<LiteralExpr>(p->token);
    
    if (auto* p = dynamic_cast<const VariableExpr*>(&expr))
        return std::make_unique<VariableExpr>(p->name, p->position);
    
    if (auto* p = dynamic_cast<const BinaryExpr*>(&expr))
        return std::make_unique<BinaryExpr>(
            cloneE(p->left), p->op, cloneE(p->right), p->position);
    
    if (auto* p = dynamic_cast<const UnaryExpr*>(&expr))
        return std::make_unique<UnaryExpr>(p->op, cloneE(p->operand), p->position);
    
    if (auto* p = dynamic_cast<const TernaryExpr*>(&expr))
        return std::make_unique<TernaryExpr>(
            cloneE(p->condition), cloneE(p->trueExpr), cloneE(p->falseExpr), p->position);
    
    if (auto* p = dynamic_cast<const AssignExpr*>(&expr))
        return std::make_unique<AssignExpr>(p->name, cloneE(p->value), p->position);
    
    if (auto* p = dynamic_cast<const CallExpr*>(&expr))
        return std::make_unique<CallExpr>(
            cloneE(p->callee), cloneEList(p->arguments), p->position);
    
    if (auto* p = dynamic_cast<const IndexExpr*>(&expr))
        return std::make_unique<IndexExpr>(
            cloneE(p->object), cloneE(p->index), p->position);
    
    if (auto* p = dynamic_cast<const SliceExpr*>(&expr))
        return std::make_unique<SliceExpr>(
            cloneE(p->object), cloneE(p->start), cloneE(p->end), cloneE(p->step), p->position);
    
    if (auto* p = dynamic_cast<const MemberExpr*>(&expr))
        return std::make_unique<MemberExpr>(cloneE(p->object), p->member, p->position);
    
    if (auto* p = dynamic_cast<const MemberAssignExpr*>(&expr))
        return std::make_unique<MemberAssignExpr>(
            cloneE(p->object), p->member, cloneE(p->value), p->position);
    
    if (auto* p = dynamic_cast<const IndexAssignExpr*>(&expr))
        return std::make_unique<IndexAssignExpr>(
            cloneE(p->object), cloneE(p->index), cloneE(p->value), p->position);
    
    if (auto* p = dynamic_cast<const ArrayExpr*>(&expr))
        return std::make_unique<ArrayExpr>(cloneEList(p->elements), p->position);
    
    if (auto* p = dynamic_cast<const MapExpr*>(&expr)) {
        std::vector<MapPair> clonedPairs;
        clonedPairs.reserve(p->pairs.size());
        for (const auto& mp : p->pairs)
            clonedPairs.push_back(MapPair{cloneE(mp.key), cloneE(mp.value)});
        return std::make_unique<MapExpr>(std::move(clonedPairs), p->position);
    }
    
    if (auto* p = dynamic_cast<const LambdaExpr*>(&expr)) {
        auto r = std::make_unique<LambdaExpr>(
            cloneParams(p->parameters), cloneE(p->body), p->position);
        r->blockBody = cloneS(p->blockBody);
        return r;
    }
    
    if (auto* p = dynamic_cast<const NamedArgExpr*>(&expr))
        return std::make_unique<NamedArgExpr>(p->name, cloneE(p->value), p->position);
    
    if (auto* p = dynamic_cast<const BorrowExpr*>(&expr))
        return std::make_unique<BorrowExpr>(p->variableName, p->isMutable, p->position);
    
    if (auto* p = dynamic_cast<const WalrusExpr*>(&expr))
        return std::make_unique<WalrusExpr>(p->variable, cloneE(p->value), p->position);
    
    if (auto* p = dynamic_cast<const ListComprehensionExpr*>(&expr))
        return std::make_unique<ListComprehensionExpr>(
            cloneE(p->element), p->variable, cloneE(p->iterable), cloneE(p->condition), p->position);
    
    if (auto* p = dynamic_cast<const DictComprehensionExpr*>(&expr))
        return std::make_unique<DictComprehensionExpr>(
            cloneE(p->key), cloneE(p->value), p->variable, cloneE(p->iterable), cloneE(p->condition), p->position);
    
    if (auto* p = dynamic_cast<const GeneratorExpr*>(&expr))
        return std::make_unique<GeneratorExpr>(
            cloneE(p->element), p->variable, cloneE(p->iterable), cloneE(p->condition), p->position);
    
    if (auto* p = dynamic_cast<const SetComprehensionExpr*>(&expr))
        return std::make_unique<SetComprehensionExpr>(
            cloneE(p->expression), p->variable, cloneE(p->iterable), cloneE(p->condition), p->position);
    
    if (dynamic_cast<const ThisExpr*>(&expr))
        return std::make_unique<ThisExpr>();
    
    if (auto* p = dynamic_cast<const NewExpr*>(&expr)) {
        auto r = std::make_unique<NewExpr>(p->className);
        r->position = p->position;
        for (const auto& arg : p->arguments)
            r->arguments.push_back(cloneE(arg));
        r->templateArguments = p->templateArguments;
        r->templateArgumentNames = p->templateArgumentNames;
        return r;
    }
    
    if (auto* p = dynamic_cast<const MethodCallExpr*>(&expr)) {
        auto r = std::make_unique<MethodCallExpr>(cloneE(p->object), p->methodName);
        r->position = p->position;
        for (const auto& arg : p->arguments)
            r->arguments.push_back(cloneE(arg));
        return r;
    }

    // (AR) نوع غير مدعوم — أرجع nullptr
    // (EN) Unsupported type — return nullptr
    return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// (AR) استنساخ العبارات / (EN) Statement Cloning
// ═══════════════════════════════════════════════════════════════════════════

StmtPtr cloneStatement(const Statement& stmt) {
    // (AR) مطابقة نوع العبارة عبر dynamic_cast
    // (EN) Match statement type via dynamic_cast
    
    if (auto* p = dynamic_cast<const BlockStmt*>(&stmt))
        return std::make_unique<BlockStmt>(cloneSList(p->statements), p->position);
    
    if (auto* p = dynamic_cast<const ExprStmt*>(&stmt))
        return std::make_unique<ExprStmt>(cloneE(p->expression));
    
    if (auto* p = dynamic_cast<const ReturnStmt*>(&stmt))
        return std::make_unique<ReturnStmt>(cloneE(p->value), p->position);
    
    if (auto* p = dynamic_cast<const VarDeclStmt*>(&stmt))
        return std::make_unique<VarDeclStmt>(p->name, p->type, cloneE(p->initializer), p->isConst, p->position);
    
    if (auto* p = dynamic_cast<const MultiVarDeclStmt*>(&stmt))
        return std::make_unique<MultiVarDeclStmt>(cloneSList(p->declarations), p->position);
    
    if (auto* p = dynamic_cast<const IfStmt*>(&stmt))
        return std::make_unique<IfStmt>(
            cloneE(p->condition), cloneS(p->thenBranch), cloneS(p->elseBranch), p->position);
    
    if (auto* p = dynamic_cast<const WhileStmt*>(&stmt))
        return std::make_unique<WhileStmt>(cloneE(p->condition), cloneS(p->body), p->position);
    
    if (auto* p = dynamic_cast<const ForStmt*>(&stmt))
        return std::make_unique<ForStmt>(
            cloneS(p->initializer), cloneE(p->condition), cloneE(p->increment), cloneS(p->body), p->position);
    
    if (auto* p = dynamic_cast<const ForRangeStmt*>(&stmt)) {
        auto r = std::make_unique<ForRangeStmt>(p->variable, cloneE(p->iterable), cloneS(p->body), p->valueVar, p->position);
        return r;
    }
    
    if (auto* p = dynamic_cast<const YieldStmt*>(&stmt)) {
        auto r = std::make_unique<YieldStmt>(cloneE(p->value), p->position);
        r->isYieldFrom = p->isYieldFrom;
        return r;
    }
    
    if (dynamic_cast<const BreakStmt*>(&stmt))
        return std::make_unique<BreakStmt>(stmt.position);
    
    if (dynamic_cast<const ContinueStmt*>(&stmt))
        return std::make_unique<ContinueStmt>(stmt.position);
    
    if (auto* p = dynamic_cast<const RaiseStmt*>(&stmt))
        return std::make_unique<RaiseStmt>(cloneE(p->exception), p->position);
    
    if (auto* p = dynamic_cast<const WithStmt*>(&stmt))
        return std::make_unique<WithStmt>(cloneE(p->resource), p->alias, cloneS(p->body), p->position);
    
    if (auto* p = dynamic_cast<const TryStmt*>(&stmt)) {
        std::vector<CatchClause> cc;
        for (const auto& c : p->catchClauses)
            cc.push_back(CatchClause(c.exceptionVar, c.exceptionType, cloneS(c.body), c.exceptionTypeName));
        return std::make_unique<TryStmt>(cloneS(p->tryBlock), std::move(cc), cloneS(p->finallyBlock), p->position);
    }
    
    if (auto* p = dynamic_cast<const SwitchStmt*>(&stmt)) {
        std::vector<CaseBranch> cb;
        for (const auto& b : p->cases)
            cb.push_back(CaseBranch(cloneE(b.value), cloneS(b.body)));
        return std::make_unique<SwitchStmt>(cloneE(p->expression), std::move(cb), cloneS(p->defaultCase), p->position);
    }
    
    if (auto* p = dynamic_cast<const FunctionDecl*>(&stmt)) {
        auto r = std::make_unique<FunctionDecl>(
            p->name, cloneParams(p->parameters), p->returnType, cloneS(p->body),
            p->isExported, p->is_async, p->isGenerator, p->position);
        r->isMainFunction = p->isMainFunction;
        r->isExtern = p->isExtern;
        r->linkName = p->linkName;
        r->decorators = cloneEList(p->decorators);
        return r;
    }
    
    if (auto* p = dynamic_cast<const FieldDecl*>(&stmt)) {
        return std::make_unique<FieldDecl>(p->name, p->type, cloneE(p->initializer), p->access, p->isStatic, p->position);
    }
    
    if (auto* p = dynamic_cast<const MethodDecl*>(&stmt)) {
        return std::make_unique<MethodDecl>(
            p->name, cloneParams(p->parameters), p->returnType, cloneS(p->body),
            p->access, p->isStatic, p->isVirtual, p->isOverride, p->isAbstract,
            p->position, p->isAsync);
    }
    
    if (auto* p = dynamic_cast<const ConstructorDecl*>(&stmt))
        return std::make_unique<ConstructorDecl>(
            cloneParams(p->parameters), cloneS(p->body), cloneEList(p->superArgs), p->position);
    
    if (auto* p = dynamic_cast<const DestructorDecl*>(&stmt))
        return std::make_unique<DestructorDecl>(cloneS(p->body), p->position);
    
    if (auto* p = dynamic_cast<const ExportStmt*>(&stmt))
        return std::make_unique<ExportStmt>(cloneS(p->declaration), p->position);
    
    // (AR) نوع غير مدعوم — أرجع nullptr
    // (EN) Unsupported type — return nullptr
    return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// (AR) مساعدات داخلية / (EN) Internal helpers
// ═══════════════════════════════════════════════════════════════════════════

static ExprPtr cloneE(const ExprPtr& e) {
    return e ? cloneExpression(*e) : nullptr;
}

static StmtPtr cloneS(const StmtPtr& s) {
    return s ? cloneStatement(*s) : nullptr;
}

} // namespace AST
} // namespace Sad
