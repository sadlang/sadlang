#pragma once

/**
 * ═══════════════════════════════════════════════════════════════════════════
 * ملف: ast_analysis_visitor.h
 * الوصف: الزائر الرئيسي الذي ينفذ قواعد التحليل الساكن فوق AST
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * هذا الزائر هو قلب النسخة الأولى من المحلل. دوره ليس فقط المرور على AST،
 * بل بناء صورة دلالية عملية للبرنامج عبر:
 *
 * - تتبع النطاقات المتداخلة
 * - تسجيل الرموز والتعاريف
 * - كشف إعادة التعريف والظلال غير المقصودة
 * - رصد الاستخدام قبل التعريف أو الاستدعاء لرموز غير معروفة
 * - اكتشاف الشيفرة غير القابلة للوصول وبعض الأنماط المشبوهة
 *
 * تم تصميمه بحيث يكون قابلاً لتوسيع القواعد لاحقاً من دون هدم البنية.
 */

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "analysis_types.h"
#include "ast_visitor.h"
#include "class_nodes.h"
#include "declarations.h"
#include "expressions.h"
#include "module_nodes.h"
#include "property_nodes.h"
#include "statements.h"

namespace Sad::Analizer
{

    enum class SymbolKind
    {
        Variable,
        Constant,
        Parameter,
        Function,
        Class,
        Method,
        Field,
        Property,
        ImportAlias,
        EnumName
    };

    enum class ScopeKind
    {
        Global,
        Function,
        Block,
        Class,
        Loop,
        Conditional,
        Exception
    };

    struct SymbolInfo
    {
        std::string name;
        SymbolKind kind = SymbolKind::Variable;
        Errors::SourceLocation location;
        bool isConst = false;
        bool isCallable = false;
        bool isBuiltin = false;
        bool mayBeNull = true;
        std::size_t readCount = 0;
        std::size_t writeCount = 0;
    };

    struct ScopeFrame
    {
        ScopeKind kind = ScopeKind::Block;
        std::string label;
        std::unordered_map<std::string, SymbolInfo> symbols;
    };

    class AstAnalysisVisitor : public AST::BaseASTVisitor
    {
    public:
        AstAnalysisVisitor(std::string filePath, AnalyzerOptions options);

        void predeclareProgram(AST::StmtList &program);
        void analyzeProgram(AST::StmtList &program);
        AnalysisResult takeResult();

        void visitExprStmt(AST::ExprStmt &stmt) override;
        void visitVarDeclStmt(AST::VarDeclStmt &stmt) override;
        void visitIfStmt(AST::IfStmt &stmt) override;
        void visitWhileStmt(AST::WhileStmt &stmt) override;
        void visitForStmt(AST::ForStmt &stmt) override;
        void visitForRangeStmt(AST::ForRangeStmt &stmt) override;
        void visitReturnStmt(AST::ReturnStmt &stmt) override;
        void visitBreakStmt(AST::BreakStmt &stmt) override;
        void visitContinueStmt(AST::ContinueStmt &stmt) override;
        void visitBlockStmt(AST::BlockStmt &stmt) override;
        void visitTryStmt(AST::TryStmt &stmt) override;
        void visitRaiseStmt(AST::RaiseStmt &stmt) override;
        void visitWithStmt(AST::WithStmt &stmt) override;
        void visitGoStmt(AST::GoStmt &stmt) override;         // (AR) أطلق — تنفيذ متزامن / (EN) Go statement
        void visitSelectCase(AST::SelectCase &stmt) override; // (AR) حالة اختر / (EN) Select case
        void visitSelectStmt(AST::SelectStmt &stmt) override; // (AR) جملة اختر / (EN) Select statement

        void visitFunctionDecl(AST::FunctionDecl &decl) override;
        void visitClassDecl(AST::ClassDecl &decl) override;
        void visitClassDeclStmt(AST::ClassDeclStmt &stmt) override;
        void visitFieldDecl(AST::FieldDecl &decl) override;
        void visitMethodDecl(AST::MethodDecl &decl) override;
        void visitPropertyDecl(AST::PropertyDecl &decl) override;
        void visitConstructorDecl(AST::ConstructorDecl &decl) override;
        void visitDestructorDecl(AST::DestructorDecl &decl) override;
        void visitEnumDecl(AST::EnumDecl &decl) override;
        void visitImportStmt(AST::ImportStmt &stmt) override;
        void visitFromImportStmt(AST::FromImportStmt &stmt) override;
        void visitExportStmt(AST::ExportStmt &stmt) override;
        void visitExportDecl(AST::ExportDecl &decl) override;

        void visitBinaryExpr(AST::BinaryExpr &expr) override;
        void visitUnaryExpr(AST::UnaryExpr &expr) override;
        void visitTernaryExpr(AST::TernaryExpr &expr) override;
        void visitLiteralExpr(AST::LiteralExpr &expr) override;
        void visitVariableExpr(AST::VariableExpr &expr) override;
        void visitBorrowExpr(AST::BorrowExpr &expr) override;
        void visitAssignExpr(AST::AssignExpr &expr) override;
        void visitCallExpr(AST::CallExpr &expr) override;
        void visitIndexExpr(AST::IndexExpr &expr) override;
        void visitMemberExpr(AST::MemberExpr &expr) override;
        void visitMemberAssignExpr(AST::MemberAssignExpr &expr) override;
        void visitIndexAssignExpr(AST::IndexAssignExpr &expr) override;
        void visitArrayExpr(AST::ArrayExpr &expr) override;
        void visitMapExpr(AST::MapExpr &expr) override;
        void visitWalrusExpr(AST::WalrusExpr &expr) override;
        void visitLambdaExpr(AST::LambdaExpr &expr) override;
        void visitNewExpr(AST::NewExpr &expr) override;
        void visitMethodCallExpr(AST::MethodCallExpr &expr) override;
        void visitMemberAccessExpr(AST::MemberAccessExpr &expr) override;
        void visitThisExpr(AST::ThisExpr &expr) override;
        void visitSuperExpr(AST::SuperExpr &expr) override;

    private:
        std::string filePath_;
        AnalyzerOptions options_;
        AnalysisResult result_;
        std::vector<ScopeFrame> scopes_;
        std::unordered_set<std::string> builtinNames_;
        std::size_t currentNestingDepth_ = 0;
        std::size_t currentExpressionComplexity_ = 0;

        void initializeBuiltins();
        void enterScope(ScopeKind kind, const std::string &label);
        void leaveScope();
        ScopeFrame &currentScope();
        const ScopeFrame &currentScope() const;

        Errors::SourceLocation makeLocation(const Lexer::Position &position) const;

        void addIssue(const std::string &ruleId,
                      IssueKind kind,
                      Errors::DiagnosticSeverity severity,
                      const Errors::SourceLocation &location,
                      const std::string &arabicMessage,
                      const std::string &englishMessage,
                      const std::string &symbolName = "");

        bool declareSymbol(const SymbolInfo &symbol, bool warnOnShadowing = true);
        SymbolInfo *resolveSymbol(const std::string &name);
        const SymbolInfo *resolveSymbol(const std::string &name) const;
        bool existsInParentScopes(const std::string &name) const;
        bool isSuppressedUnusedName(const std::string &name) const;
        bool isTerminatingStatement(const AST::Statement *statement) const;
        void analyzeStatementList(AST::StmtList &statements);
        void analyzeParameters(const std::vector<AST::Parameter> &parameters,
                               const std::string &ownerName);
        void predeclareTopLevelStatement(AST::Statement *statement);
        void predeclareClassMembers(AST::ClassDecl &decl);
        void predeclareClassMembers(AST::ClassDeclStmt &stmt);
        void flushUnusedWarnings(const ScopeFrame &scope);
        void markSymbolRead(const std::string &name, const Lexer::Position &position, bool requireCallable = false);
        void visitIfPresent(AST::Expression *expression);
        void visitIfPresent(AST::Statement *statement);
        void checkConstantCondition(AST::Expression *expression, const Lexer::Position &position, const std::string &ownerLabel);
        void enterNestedControlFlow(const Lexer::Position &position, const std::string &ownerLabel);
        void leaveNestedControlFlow();
        void checkFunctionComplexity(const std::string &functionName,
                                     const Lexer::Position &position,
                                     std::size_t statementCount,
                                     std::size_t parameterCount);
        void checkClassComplexity(const std::string &className,
                                  const Lexer::Position &position,
                                  std::size_t methodCount);
        void checkMagicNumber(const AST::Expression *expression, const Lexer::Position &position);
        void checkRepeatedCallPattern(const AST::CallExpr &expr);
        bool isNullLiteral(const AST::Expression *expression) const;
        bool isDefinitelyNonNullExpression(const AST::Expression *expression) const;
        void checkPossibleNullDereference(const AST::Expression *target, const Lexer::Position &position, const std::string &contextLabel);
    };

} // namespace Sad::Analizer
