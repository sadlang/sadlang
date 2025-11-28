/**
 * @file ast_printer.h
 * @brief (AR) طابعة الشجرة النحوية التجريدية - تحويل AST إلى نص مقروء.
 *        (EN) AST Printer - Converts AST nodes to readable text representation.
 * 
 * @details
 *   (AR) تطبيق نمط الزائر (Visitor Pattern) لطباعة وتمثيل عُقد الشجرة النحوية
 *        التجريدية بشكل مقروء ومنسّق. مفيد للتنقيح وفهم بنية الكود المُحلل.
 *   (EN) Implements the Visitor Pattern to print and represent AST nodes
 *        in a readable, formatted manner. Useful for debugging and understanding
 *        the parsed code structure.
 * 
 * @author Sad Language Team
 * @date 11 November 2025
 * @copyright Copyright (c) 2025 Sad Language Project
 */

#ifndef SAD_AST_PRINTER_H
#define SAD_AST_PRINTER_H

#include "ast_visitor.h"
#include "ast_node.h"
#include "expressions.h"
#include "statements.h"
#include "declarations.h"
#include <string>
#include <sstream>

namespace Sad {
namespace AST {

/**
 * @class ASTPrinter
 * @brief (AR) فئة طباعة الشجرة النحوية التجريدية.
 *        (EN) AST printing class.
 * 
 * @details
 *   (AR) تقوم بزيارة جميع عُقد AST وإنتاج تمثيل نصي منسّق لها.
 *        يمكن استخدامها لطباعة AST للتنقيح أو لعرض بنية البرنامج.
 *   (EN) Visits all AST nodes and produces a formatted text representation.
 *        Can be used to print AST for debugging or displaying program structure.
 * 
 * @example
 *   ASTPrinter printer;
 *   std::string result = printer.print(astNode);
 *   std::cout << result << std::endl;
 */
class ASTPrinter : public ASTVisitor {
public:
    /**
     * @brief (AR) البناء الافتراضي.
     *        (EN) Default constructor.
     */
    ASTPrinter();
    
    /**
     * @brief (AR) يطبع عقدة AST ويُرجع النص الناتج.
     *        (EN) Prints an AST node and returns the resulting text.
     * 
     * @param node (AR) مؤشر للعقدة المراد طباعتها. (EN) Pointer to node to print.
     * @return (AR) النص المُنسّق الممثل للعقدة. (EN) Formatted text representation.
     */
    std::string print(ASTNode* node);
    
    /**
     * @brief (AR) يُرجع النتيجة الحالية المُخزنة.
     *        (EN) Returns the current stored result.
     * 
     * @return (AR) النص الناتج عن آخر زيارة. (EN) Text from last visit.
     */
    std::string getResult() const { return result_; }
    
    // =====================================================================
    // Expression Visitors / زوار التعابير
    // =====================================================================
    
    /**
     * @brief (AR) يزور عقدة التعبير الثنائي.
     *        (EN) Visits binary expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitBinaryExpr(BinaryExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة التعبير الأحادي.
     *        (EN) Visits unary expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitUnaryExpr(UnaryExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة التعبير الثلاثي الشرطي.
     *        (EN) Visits ternary conditional expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     * 
     * Format: (condition ? true_value : false_value)
     * التنسيق: (الشرط ؟ القيمة_الصحيحة : القيمة_الخاطئة)
     */
    void visitTernaryExpr(TernaryExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة التعبير الحرفي.
     *        (EN) Visits literal expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitLiteralExpr(LiteralExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة تعبير المتغير.
     *        (EN) Visits variable expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitVariableExpr(VariableExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة تعبير هذا (this).
     *        (EN) Visits this expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitThisExpr(ThisExpr& expr) override;
    void visitSuperExpr(SuperExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة تعبير الإسناد.
     *        (EN) Visits assignment expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitAssignExpr(AssignExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة استدعاء الدالة.
     *        (EN) Visits function call expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitCallExpr(CallExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة تعبير الفهرسة.
     *        (EN) Visits index expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitIndexExpr(IndexExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة الوصول للعضو.
     *        (EN) Visits member access expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitMemberExpr(MemberExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة تعيين قيمة لعضو.
     *        (EN) Visits member assignment expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitMemberAssignExpr(MemberAssignExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة المصفوفة الحرفية.
     *        (EN) Visits array literal expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitArrayExpr(ArrayExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة القاموس الحرفي.
     *        (EN) Visits map literal expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitMapExpr(MapExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة تعبير Lambda.
     *        (EN) Visits lambda expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitLambdaExpr(LambdaExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة الاستيعاب القائمي.
     *        (EN) Visits list comprehension node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitListComprehensionExpr(ListComprehensionExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة الاستيعاب القاموسي.
     *        (EN) Visits dictionary comprehension node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitDictComprehensionExpr(DictComprehensionExpr& expr) override;
    
    /**
     * @brief (AR) يزور عقدة تعبير المولد.
     *        (EN) Visits generator expression node.
     * @param expr (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitGeneratorExpr(GeneratorExpr& expr) override;
    
    // =====================================================================
    // Statement Visitors / زوار الجمل
    // =====================================================================
    
    /**
     * @brief (AR) يزور جملة التعبير.
     *        (EN) Visits expression statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitExprStmt(ExprStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة تصريح المتغير.
     *        (EN) Visits variable declaration statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitVarDeclStmt(VarDeclStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة إذا.
     *        (EN) Visits if statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitIfStmt(IfStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة بينما.
     *        (EN) Visits while statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitWhileStmt(WhileStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة لكل (C-style).
     *        (EN) Visits for statement (C-style).
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitForStmt(ForStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة لكل في (range-based).
     *        (EN) Visits for-range statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitForRangeStmt(ForRangeStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة حالة (switch-case).
     *        (EN) Visits switch-case statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     * 
     * Format / التنسيق:
     *   حالة <expr>
     *       عندما <value>: <statement>
     *       افتراضي: <statement>
     *   نهاية
     */
    void visitSwitchStmt(SwitchStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة إرجاع.
     *        (EN) Visits return statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitReturnStmt(ReturnStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة خروج.
     *        (EN) Visits break statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitBreakStmt(BreakStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة تابع.
     *        (EN) Visits continue statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitContinueStmt(ContinueStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة الكتلة.
     *        (EN) Visits block statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitBlockStmt(BlockStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة محاولة-التقاط.
     *        (EN) Visits try-catch statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitTryStmt(TryStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة رفع الاستثناء.
     *        (EN) Visits raise statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitRaiseStmt(RaiseStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة مع.
     *        (EN) Visits with statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitWithStmt(WithStmt& stmt) override;
    
    // =====================================================================
    // Declaration Visitors / زوار التصريحات
    // =====================================================================
    
    /**
     * @brief (AR) يزور تصريح الدالة.
     *        (EN) Visits function declaration.
     * @param decl (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitFunctionDecl(FunctionDecl& decl) override;
    
    /**
     * @brief (AR) يزور تصريح الصنف.
     *        (EN) Visits class declaration.
     * @param decl (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitClassDecl(ClassDecl& decl) override;
    
    /**
     * @brief (AR) يزور تصريح الحقل.
     *        (EN) Visits field declaration.
     * @param decl (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitFieldDecl(FieldDecl& decl) override;
    
    /**
     * @brief (AR) يزور تصريح الطريقة.
     *        (EN) Visits method declaration.
     * @param decl (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitMethodDecl(MethodDecl& decl) override;
    
    /**
     * @brief (AR) يزور تصريح خاصية (Property).
     *        (EN) Visits property declaration.
     * @param decl (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitPropertyDecl(PropertyDecl& decl) override;
    
    /**
     * @brief (AR) يزور تصريح الباني.
     *        (EN) Visits constructor declaration.
     * @param decl (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitConstructorDecl(ConstructorDecl& decl) override;
    
    /**
     * @brief (AR) يزور تصريح الهادم.
     *        (EN) Visits destructor declaration.
     * @param decl (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitDestructorDecl(DestructorDecl& decl) override;
    
    /**
     * @brief (AR) يزور تصريح التعداد.
     *        (EN) Visits enum declaration.
     * @param decl (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitEnumDecl(EnumDecl& decl) override;
    
    /**
     * @brief (AR) يزور جملة الاستيراد.
     *        (EN) Visits import statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitImportStmt(ImportStmt& stmt) override;
    
    /**
     * @brief (AR) يزور جملة التصدير.
     *        (EN) Visits export statement.
     * @param stmt (AR) مؤشر للعقدة. (EN) Pointer to node.
     */
    void visitExportStmt(ExportStmt& stmt) override;

private:
    std::string result_;        ///< (AR) النتيجة المُخزنة. (EN) Stored result.
    int indentLevel_;           ///< (AR) مستوى الإزاحة الحالي. (EN) Current indent level.
    
    /**
     * @brief (AR) يُضيف إزاحة (مسافات) حسب المستوى الحالي.
     *        (EN) Adds indentation (spaces) based on current level.
     * @return (AR) نص الإزاحة. (EN) Indentation string.
     */
    std::string indent() const;
    
    /**
     * @brief (AR) يزيد مستوى الإزاحة.
     *        (EN) Increases indentation level.
     */
    void increaseIndent() { indentLevel_++; }
    
    /**
     * @brief (AR) ينقص مستوى الإزاحة.
     *        (EN) Decreases indentation level.
     */
    void decreaseIndent() { if (indentLevel_ > 0) indentLevel_--; }
};

} // namespace AST
} // namespace Sad

#endif // SAD_AST_PRINTER_H
