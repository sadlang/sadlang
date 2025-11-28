/**
 * @file ast_printer.cpp
 * @brief (AR) تنفيذ طابعة الشجرة النحوية التجريدية.
 *        (EN) Implementation of AST printer.
 * 
 * @details
 *   (AR) يحتوي على تنفيذ جميع دوال زيارة العُقد لطباعة AST بشكل منسّق ومقروء.
 *   (EN) Contains implementation of all node visitor functions for formatted AST printing.
 * 
 * @author Sad Language Team
 * @date 11 November 2025
 * @copyright Copyright (c) 2025 Sad Language Project
 */

#include "../../../include/parser/ast/ast_printer.h"
#include "../../../include/parser/ast/property_nodes.h"
#include "../../../include/parser/ast/class_nodes.h"
#include <sstream>

namespace Sad {
namespace AST {

// =========================================================================
// Constructor / البناء
// =========================================================================

/**
 * @brief (AR) البناء الافتراضي - يُهيئ الطابعة بحالة فارغة.
 *        (EN) Default constructor - initializes printer with empty state.
 */
ASTPrinter::ASTPrinter() 
    : result_(""), indentLevel_(0) {
}

// =========================================================================
// Public Methods / الدوال العامة
// =========================================================================

/**
 * @brief (AR) يطبع عقدة AST ويُرجع النص الناتج.
 *        (EN) Prints an AST node and returns the resulting text.
 * 
 * @param node (AR) مؤشر للعقدة المراد طباعتها. (EN) Pointer to node to print.
 * @return (AR) النص المُنسّق الممثل للعقدة. (EN) Formatted text representation.
 * 
 * @note (AR) يُعيد تعيين النتيجة قبل الزيارة.
 *       (EN) Resets result before visiting.
 */
std::string ASTPrinter::print(ASTNode* node) {
    result_.clear();
    indentLevel_ = 0;
    
    if (node) {
        node->accept(*this);
    }
    
    return result_;
}

/**
 * @brief (AR) يُضيف إزاحة (مسافات) حسب المستوى الحالي.
 *        (EN) Adds indentation (spaces) based on current level.
 * 
 * @return (AR) نص الإزاحة (مسافتان لكل مستوى).
 *         (EN) Indentation string (2 spaces per level).
 */
std::string ASTPrinter::indent() const {
    return std::string(indentLevel_ * 2, ' ');
}

// =========================================================================
// Expression Visitors / زوار التعابير
// =========================================================================

/**
 * @brief (AR) يزور عقدة التعبير الثنائي - يطبع العامل والمعاملين.
 *        (EN) Visits binary expression node - prints operator and operands.
 * 
 * @param expr (AR) مؤشر لعقدة التعبير الثنائي. (EN) Pointer to binary expression node.
 */
void ASTPrinter::visitBinaryExpr(BinaryExpr& expr) {
    result_ += "(";
    expr.left->accept(*this);
    result_ += " " + Lexer::Token::typeToString(expr.op) + " ";
    expr.right->accept(*this);
    result_ += ")";
}

/**
 * @brief (AR) يزور عقدة التعبير الأحادي - يطبع العامل والمعامل.
 *        (EN) Visits unary expression node - prints operator and operand.
 * 
 * @param expr (AR) مؤشر لعقدة التعبير الأحادي. (EN) Pointer to unary expression node.
 */
void ASTPrinter::visitUnaryExpr(UnaryExpr& expr) {
    result_ += "(";
    result_ += Lexer::Token::typeToString(expr.op);
    expr.operand->accept(*this);
    result_ += ")";
}

/**
 * @brief (AR) يزور عقدة التعبير الثلاثي الشرطي - يطبع بتنسيق (شرط ? صحيح : خطأ).
 *        (EN) Visits ternary conditional expression node - prints in (cond ? true : false) format.
 * 
 * @param expr (AR) مؤشر لعقدة التعبير الثلاثي. (EN) Pointer to ternary expression node.
 * 
 * Format: (condition ? true_value : false_value)
 * مثال: (س > 0 ? "موجب" : "سالب")
 */
void ASTPrinter::visitTernaryExpr(TernaryExpr& expr) {
    result_ += "(";
    expr.condition->accept(*this);
    result_ += " ? ";
    expr.trueExpr->accept(*this);
    result_ += " : ";
    expr.falseExpr->accept(*this);
    result_ += ")";
}

/**
 * @brief (AR) يزور عقدة التعبير الحرفي - يطبع القيمة الحرفية.
 *        (EN) Visits literal expression node - prints literal value.
 * 
 * @param expr (AR) مؤشر لعقدة التعبير الحرفي. (EN) Pointer to literal expression node.
 */
void ASTPrinter::visitLiteralExpr(LiteralExpr& expr) {
    result_ += expr.token.getValue();
}

/**
 * @brief (AR) يزور عقدة تعبير المتغير - يطبع اسم المتغير.
 *        (EN) Visits variable expression node - prints variable name.
 * 
 * @param expr (AR) مؤشر لعقدة تعبير المتغير. (EN) Pointer to variable expression node.
 */
void ASTPrinter::visitVariableExpr(VariableExpr& expr) {
    result_ += expr.name;
}

void ASTPrinter::visitThisExpr(ThisExpr& expr) {
    result_ += "this";
}

void ASTPrinter::visitSuperExpr(SuperExpr& expr) {
    result_ += "super";
    if (!expr.memberName.empty()) {
        result_ += "." + expr.memberName;
    }
}

/**
 * @brief (AR) يزور عقدة تعبير الإسناد - يطبع المتغير والقيمة المُسندة.
 *        (EN) Visits assignment expression node - prints variable and assigned value.
 * 
 * @param expr (AR) مؤشر لعقدة تعبير الإسناد. (EN) Pointer to assignment expression node.
 */
void ASTPrinter::visitAssignExpr(AssignExpr& expr) {
    result_ += "(";
    result_ += expr.name + " = ";
    expr.value->accept(*this);
    result_ += ")";
}

/**
 * @brief (AR) يزور عقدة استدعاء الدالة - يطبع اسم الدالة والمعاملات.
 *        (EN) Visits function call node - prints function name and arguments.
 * 
 * @param expr (AR) مؤشر لعقدة استدعاء الدالة. (EN) Pointer to function call node.
 */
void ASTPrinter::visitCallExpr(CallExpr& expr) {
    expr.callee->accept(*this);
    result_ += "(";
    
    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        if (i > 0) result_ += ", ";
        expr.arguments[i]->accept(*this);
    }
    
    result_ += ")";
}

/**
 * @brief (AR) يزور عقدة تعبير الفهرسة - يطبع الكائن والفهرس.
 *        (EN) Visits index expression node - prints object and index.
 * 
 * @param expr (AR) مؤشر لعقدة تعبير الفهرسة. (EN) Pointer to index expression node.
 */
void ASTPrinter::visitIndexExpr(IndexExpr& expr) {
    expr.object->accept(*this);
    result_ += "[";
    expr.index->accept(*this);
    result_ += "]";
}

/**
 * @brief (AR) يزور عقدة الوصول للعضو - يطبع الكائن واسم العضو.
 *        (EN) Visits member access node - prints object and member name.
 * 
 * @param expr (AR) مؤشر لعقدة الوصول للعضو. (EN) Pointer to member access node.
 */
void ASTPrinter::visitMemberExpr(MemberExpr& expr) {
    expr.object->accept(*this);
    result_ += "." + expr.member;
}

/**
 * @brief (AR) يزور عقدة تعيين قيمة لعضو - يطبع التعيين.
 *        (EN) Visits member assignment node - prints assignment.
 */
void ASTPrinter::visitMemberAssignExpr(MemberAssignExpr& expr) {
    expr.object->accept(*this);
    result_ += "." + expr.member + " = ";
    expr.value->accept(*this);
}

/**
 * @brief (AR) يزور عقدة المصفوفة الحرفية - يطبع العناصر.
 *        (EN) Visits array literal node - prints elements.
 * 
 * @param expr (AR) مؤشر لعقدة المصفوفة الحرفية. (EN) Pointer to array literal node.
 */
void ASTPrinter::visitArrayExpr(ArrayExpr& expr) {
    result_ += "[";
    
    for (size_t i = 0; i < expr.elements.size(); ++i) {
        if (i > 0) result_ += ", ";
        expr.elements[i]->accept(*this);
    }
    
    result_ += "]";
}

/**
 * @brief (AR) يزور عقدة القاموس الحرفي - يطبع أزواج المفتاح-القيمة.
 *        (EN) Visits map literal node - prints key-value pairs.
 * 
 * @param expr (AR) مؤشر لعقدة القاموس الحرفي. (EN) Pointer to map literal node.
 */
void ASTPrinter::visitMapExpr(MapExpr& expr) {
    result_ += "{";
    
    for (size_t i = 0; i < expr.pairs.size(); ++i) {
        if (i > 0) result_ += ", ";
        expr.pairs[i].key->accept(*this);
        result_ += ": ";
        expr.pairs[i].value->accept(*this);
    }
    
    result_ += "}";
}

/**
 * @brief (AR) يزور عقدة تعبير Lambda - يطبع المعاملات والجسم.
 *        (EN) Visits lambda expression node - prints parameters and body.
 * 
 * @param expr (AR) مؤشر لعقدة تعبير Lambda. (EN) Pointer to lambda expression node.
 */
void ASTPrinter::visitLambdaExpr(LambdaExpr& expr) {
    result_ += "(";
    
    for (size_t i = 0; i < expr.parameters.size(); ++i) {
        if (i > 0) result_ += ", ";
        result_ += expr.parameters[i].name;
    }
    
    result_ += ") => ";
    expr.body->accept(*this);
}

/**
 * @brief (AR) يزور عقدة الاستيعاب القائمي - يطبع التعبير والحلقة والشرط.
 *        (EN) Visits list comprehension node - prints expression, loop, and condition.
 * 
 * @param expr (AR) مؤشر لعقدة الاستيعاب القائمي. (EN) Pointer to list comprehension node.
 */
void ASTPrinter::visitListComprehensionExpr(ListComprehensionExpr& expr) {
    result_ += "[";
    expr.element->accept(*this);
    result_ += " for " + expr.variable + " in ";
    expr.iterable->accept(*this);
    
    if (expr.condition) {
        result_ += " if ";
        expr.condition->accept(*this);
    }
    
    result_ += "]";
}

/**
 * @brief (AR) يزور عقدة الاستيعاب القاموسي - يطبع المفتاح والقيمة والحلقة.
 *        (EN) Visits dict comprehension node - prints key, value, and loop.
 * 
 * @param expr (AR) مؤشر لعقدة الاستيعاب القاموسي. (EN) Pointer to dict comprehension node.
 */
void ASTPrinter::visitDictComprehensionExpr(DictComprehensionExpr& expr) {
    result_ += "{";
    expr.key->accept(*this);
    result_ += ": ";
    expr.value->accept(*this);
    result_ += " for " + expr.variable + " in ";
    expr.iterable->accept(*this);
    
    if (expr.condition) {
        result_ += " if ";
        expr.condition->accept(*this);
    }
    
    result_ += "}";
}

/**
 * @brief (AR) يزور عقدة تعبير المولد - يطبع التعبير والحلقة.
 *        (EN) Visits generator expression node - prints expression and loop.
 * 
 * @param expr (AR) مؤشر لعقدة تعبير المولد. (EN) Pointer to generator expression node.
 */
void ASTPrinter::visitGeneratorExpr(GeneratorExpr& expr) {
    result_ += "(";
    expr.element->accept(*this);
    result_ += " for " + expr.variable + " in ";
    expr.iterable->accept(*this);
    
    if (expr.condition) {
        result_ += " if ";
        expr.condition->accept(*this);
    }
    
    result_ += ")";
}

// =========================================================================
// Statement Visitors / زوار الجمل
// =========================================================================

/**
 * @brief (AR) يزور جملة التعبير - يطبع التعبير مع فاصلة منقوطة.
 *        (EN) Visits expression statement - prints expression with semicolon.
 * 
 * @param stmt (AR) مؤشر لجملة التعبير. (EN) Pointer to expression statement.
 */
void ASTPrinter::visitExprStmt(ExprStmt& stmt) {
    result_ += indent();
    stmt.expression->accept(*this);
    result_ += ";\n";
}

/**
 * @brief (AR) يزور جملة تصريح المتغير - يطبع النوع والاسم والقيمة الأولية.
 *        (EN) Visits variable declaration - prints type, name, and initial value.
 * 
 * @param stmt (AR) مؤشر لجملة تصريح المتغير. (EN) Pointer to variable declaration.
 */
void ASTPrinter::visitVarDeclStmt(VarDeclStmt& stmt) {
    result_ += indent();
    result_ += (stmt.isConst ? "const " : "var ");
    result_ += stmt.name;
    
    if (stmt.initializer) {
        result_ += " = ";
        stmt.initializer->accept(*this);
    }
    
    result_ += ";\n";
}

/**
 * @brief (AR) يزور جملة إذا - يطبع الشرط وفروع إذا صح ووإلا.
 *        (EN) Visits if statement - prints condition, then and else branches.
 * 
 * @param stmt (AR) مؤشر لجملة إذا. (EN) Pointer to if statement.
 */
void ASTPrinter::visitIfStmt(IfStmt& stmt) {
    result_ += indent() + "if (";
    stmt.condition->accept(*this);
    result_ += ")\n";
    
    increaseIndent();
    stmt.thenBranch->accept(*this);
    decreaseIndent();
    
    if (stmt.elseBranch) {
        result_ += indent() + "else\n";
        increaseIndent();
        stmt.elseBranch->accept(*this);
        decreaseIndent();
    }
}

/**
 * @brief (AR) يزور جملة بينما - يطبع الشرط والجسم.
 *        (EN) Visits while statement - prints condition and body.
 * 
 * @param stmt (AR) مؤشر لجملة بينما. (EN) Pointer to while statement.
 */
void ASTPrinter::visitWhileStmt(WhileStmt& stmt) {
    result_ += indent() + "while (";
    stmt.condition->accept(*this);
    result_ += ")\n";
    
    increaseIndent();
    stmt.body->accept(*this);
    decreaseIndent();
}

/**
 * @brief (AR) يزور جملة لكل (C-style) - يطبع التهيئة والشرط والزيادة.
 *        (EN) Visits for statement (C-style) - prints init, condition, increment.
 * 
 * @param stmt (AR) مؤشر لجملة لكل. (EN) Pointer to for statement.
 */
void ASTPrinter::visitForStmt(ForStmt& stmt) {
    result_ += indent() + "for (";
    
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }
    result_ += "; ";
    
    if (stmt.condition) {
        stmt.condition->accept(*this);
    }
    result_ += "; ";
    
    if (stmt.increment) {
        stmt.increment->accept(*this);
    }
    result_ += ")\n";
    
    increaseIndent();
    stmt.body->accept(*this);
    decreaseIndent();
}

/**
 * @brief (AR) يزور جملة لكل في - يطبع المتغير والعنصر القابل للتكرار.
 *        (EN) Visits for-range statement - prints variable and iterable.
 * 
 * @param stmt (AR) مؤشر لجملة لكل في. (EN) Pointer to for-range statement.
 */
void ASTPrinter::visitForRangeStmt(ForRangeStmt& stmt) {
    result_ += indent() + "for (" + stmt.variable + " in ";
    stmt.iterable->accept(*this);
    result_ += ")\n";
    
    increaseIndent();
    stmt.body->accept(*this);
    decreaseIndent();
}

/**
 * @brief (AR) يزور جملة حالة (switch-case) - يطبع التعبير والحالات.
 *        (EN) Visits switch-case statement - prints expression and cases.
 * 
 * @param stmt (AR) مؤشر لجملة حالة. (EN) Pointer to switch statement.
 * 
 * Format / التنسيق:
 *   حالة <expression>
 *       عندما <value>: <statement>
 *       افتراضي: <statement>
 *   نهاية
 */
void ASTPrinter::visitSwitchStmt(SwitchStmt& stmt) {
    result_ += indent() + "switch (";
    stmt.expression->accept(*this);
    result_ += ")\n";
    
    result_ += indent() + "{\n";
    increaseIndent();
    
    // Print all case branches / طباعة جميع فروع الحالات
    for (const auto& caseItem : stmt.cases) {
        result_ += indent() + "case ";
        caseItem.value->accept(*this);
        result_ += ":\n";
        
        increaseIndent();
        caseItem.body->accept(*this);
        decreaseIndent();
    }
    
    // Print default case if exists / طباعة الحالة الافتراضية إن وُجدت
    if (stmt.defaultCase) {
        result_ += indent() + "default:\n";
        increaseIndent();
        stmt.defaultCase->accept(*this);
        decreaseIndent();
    }
    
    decreaseIndent();
    result_ += indent() + "}\n";
}

/**
 * @brief (AR) يزور جملة إرجاع - يطبع القيمة المُرجعة.
 *        (EN) Visits return statement - prints returned value.
 * 
 * @param stmt (AR) مؤشر لجملة إرجاع. (EN) Pointer to return statement.
 */
void ASTPrinter::visitReturnStmt(ReturnStmt& stmt) {
    result_ += indent() + "return";
    
    if (stmt.value) {
        result_ += " ";
        stmt.value->accept(*this);
    }
    
    result_ += ";\n";
}

/**
 * @brief (AR) يزور جملة خروج - يطبع كلمة break.
 *        (EN) Visits break statement - prints break keyword.
 * 
 * @param stmt (AR) مؤشر لجملة خروج. (EN) Pointer to break statement.
 */
void ASTPrinter::visitBreakStmt(BreakStmt& stmt) {
    result_ += indent() + "break;\n";
}

/**
 * @brief (AR) يزور جملة تابع - يطبع كلمة continue.
 *        (EN) Visits continue statement - prints continue keyword.
 * 
 * @param stmt (AR) مؤشر لجملة تابع. (EN) Pointer to continue statement.
 */
void ASTPrinter::visitContinueStmt(ContinueStmt& stmt) {
    result_ += indent() + "continue;\n";
}

/**
 * @brief (AR) يزور جملة الكتلة - يطبع الأقواس والجمل الداخلية.
 *        (EN) Visits block statement - prints braces and inner statements.
 * 
 * @param stmt (AR) مؤشر لجملة الكتلة. (EN) Pointer to block statement.
 */
void ASTPrinter::visitBlockStmt(BlockStmt& stmt) {
    result_ += indent() + "{\n";
    
    increaseIndent();
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    decreaseIndent();
    
    result_ += indent() + "}\n";
}

/**
 * @brief (AR) يزور جملة محاولة-التقاط - يطبع كتل المحاولة والالتقاط وأخيراً.
 *        (EN) Visits try-catch statement - prints try, catch, and finally blocks.
 * 
 * @param stmt (AR) مؤشر لجملة محاولة-التقاط. (EN) Pointer to try-catch statement.
 */
void ASTPrinter::visitTryStmt(TryStmt& stmt) {
    result_ += indent() + "try\n";
    stmt.tryBlock->accept(*this);
    
    for (const auto& catchClause : stmt.catchClauses) {
        result_ += indent() + "catch (" + catchClause.exceptionVar + ")\n";
        catchClause.body->accept(*this);
    }
    
    if (stmt.finallyBlock) {
        result_ += indent() + "finally\n";
        stmt.finallyBlock->accept(*this);
    }
}

/**
 * @brief (AR) يزور جملة رفع الاستثناء - يطبع تعبير الاستثناء.
 *        (EN) Visits raise statement - prints exception expression.
 * 
 * @param stmt (AR) مؤشر لجملة رفع الاستثناء. (EN) Pointer to raise statement.
 */
void ASTPrinter::visitRaiseStmt(RaiseStmt& stmt) {
    result_ += indent() + "raise ";
    stmt.exception->accept(*this);
    result_ += ";\n";
}

/**
 * @brief (AR) يزور جملة مع - يطبع المتغير والمورد والجسم.
 *        (EN) Visits with statement - prints variable, resource, and body.
 * 
 * @param stmt (AR) مؤشر لجملة مع. (EN) Pointer to with statement.
 */
void ASTPrinter::visitWithStmt(WithStmt& stmt) {
    result_ += indent() + "with (" + stmt.variable + " = ";
    stmt.resource->accept(*this);
    result_ += ")\n";
    
    increaseIndent();
    stmt.body->accept(*this);
    decreaseIndent();
}

// =========================================================================
// Declaration Visitors / زوار التصريحات
// =========================================================================

/**
 * @brief (AR) يزور تصريح الدالة - يطبع الاسم والمعاملات ونوع الإرجاع والجسم.
 *        (EN) Visits function declaration - prints name, parameters, return type, body.
 * 
 * @param decl (AR) مؤشر لتصريح الدالة. (EN) Pointer to function declaration.
 */
void ASTPrinter::visitFunctionDecl(FunctionDecl& decl) {
    result_ += indent() + "func " + decl.name + "(";
    
    for (size_t i = 0; i < decl.parameters.size(); ++i) {
        if (i > 0) result_ += ", ";
        result_ += decl.parameters[i].name;
    }
    
    result_ += ")\n";
    decl.body->accept(*this);
}

/**
 * @brief (AR) يزور تصريح الصنف - يطبع الاسم والصنف الأب والأعضاء.
 *        (EN) Visits class declaration - prints name, superclass, and members.
 * 
 * @param decl (AR) مؤشر لتصريح الصنف. (EN) Pointer to class declaration.
 */
void ASTPrinter::visitClassDecl(ClassDecl& decl) {
    result_ += indent() + "class " + decl.name;
    
    if (!decl.superclass.empty()) {
        result_ += " extends " + decl.superclass;
    }
    
    result_ += "\n" + indent() + "{\n";
    
    increaseIndent();
    for (const auto& member : decl.members) {
        member->accept(*this);
    }
    decreaseIndent();
    
    result_ += indent() + "}\n";
}

/**
 * @brief (AR) يزور تصريح الحقل - يطبع معدّل الوصول والاسم والنوع.
 *        (EN) Visits field declaration - prints access modifier, name, and type.
 * 
 * @param decl (AR) مؤشر لتصريح الحقل. (EN) Pointer to field declaration.
 */
void ASTPrinter::visitFieldDecl(FieldDecl& decl) {
    result_ += indent() + "var " + decl.name;
    
    if (decl.initializer) {
        result_ += " = ";
        decl.initializer->accept(*this);
    }
    
    result_ += ";\n";
}

/**
 * @brief (AR) يزور تصريح الطريقة - يطبع معدّل الوصول والاسم والمعاملات.
 *        (EN) Visits method declaration - prints access, name, and parameters.
 * 
 * @param decl (AR) مؤشر لتصريح الطريقة. (EN) Pointer to method declaration.
 */
void ASTPrinter::visitMethodDecl(MethodDecl& decl) {
    result_ += indent() + "func " + decl.name + "(";
    
    for (size_t i = 0; i < decl.parameters.size(); ++i) {
        if (i > 0) result_ += ", ";
        result_ += decl.parameters[i].name;
    }
    
    result_ += ")\n";
    decl.body->accept(*this);
}

/**
 * @brief (AR) يزور تصريح الباني - يطبع المعاملات والجسم.
 *        (EN) Visits constructor declaration - prints parameters and body.
 * 
 * @param decl (AR) مؤشر لتصريح الباني. (EN) Pointer to constructor declaration.
 */
void ASTPrinter::visitConstructorDecl(ConstructorDecl& decl) {
    result_ += indent() + "constructor(";
    
    for (size_t i = 0; i < decl.parameters.size(); ++i) {
        if (i > 0) result_ += ", ";
        result_ += decl.parameters[i].name;
    }
    
    result_ += ")\n";
    decl.body->accept(*this);
}

/**
 * @brief (AR) يزور تصريح الخاصية - يطبع الاسم والـ getter/setter.
 *        (EN) Visits property declaration - prints name and getter/setter.
 * 
 * @param decl (AR) مؤشر لتصريح الخاصية. (EN) Pointer to property declaration.
 */
void ASTPrinter::visitPropertyDecl(PropertyDecl& decl) {
    result_ += indent() + "property " + decl.name;
    
    if (decl.isStatic) {
        result_ += " (static)";
    }
    
    if (decl.isReadOnly()) {
        result_ += " (read-only)";
    } else if (decl.isWriteOnly()) {
        result_ += " (write-only)";
    }
    
    result_ += "\n";
    
    // Print getter
    if (decl.getter) {
        result_ += indent() + "  get:\n";
        indentLevel_ += 2;
        if (decl.getter->body) {
            decl.getter->body->accept(*this);
        }
        indentLevel_ -= 2;
    }
    
    // Print setter if exists
    if (decl.setter) {
        result_ += indent() + "  set(" + decl.setter->parameterName + "):\n";
        indentLevel_ += 2;
        if (decl.setter->body) {
            decl.setter->body->accept(*this);
        }
        indentLevel_ -= 2;
    }
}

/**
 * @brief (AR) يزور تصريح الهادم - يطبع الجسم.
 *        (EN) Visits destructor declaration - prints body.
 * 
 * @param decl (AR) مؤشر لتصريح الهادم. (EN) Pointer to destructor declaration.
 */
void ASTPrinter::visitDestructorDecl(DestructorDecl& decl) {
    result_ += indent() + "destructor\n";
    decl.body->accept(*this);
}

/**
 * @brief (AR) يزور تصريح التعداد - يطبع الاسم والأعضاء.
 *        (EN) Visits enum declaration - prints name and members.
 * 
 * @param decl (AR) مؤشر لتصريح التعداد. (EN) Pointer to enum declaration.
 */
void ASTPrinter::visitEnumDecl(EnumDecl& decl) {
    result_ += indent() + "enum " + decl.name + "\n";
    result_ += indent() + "{\n";
    
    increaseIndent();
    for (size_t i = 0; i < decl.members.size(); ++i) {
        result_ += indent() + decl.members[i].name;
        if (i < decl.members.size() - 1) result_ += ",";
        result_ += "\n";
    }
    decreaseIndent();
    
    result_ += indent() + "}\n";
}

/**
 * @brief (AR) يزور جملة الاستيراد - يطبع مسار الوحدة والرموز.
 *        (EN) Visits import statement - prints module path and symbols.
 * 
 * @param stmt (AR) مؤشر لجملة الاستيراد. (EN) Pointer to import statement.
 */
void ASTPrinter::visitImportStmt(ImportStmt& stmt) {
    result_ += indent() + "import \"" + stmt.modulePath + "\"";
    
    if (!stmt.alias.empty()) {
        result_ += " as " + stmt.alias;
    }
    
    result_ += ";\n";
}

/**
 * @brief (AR) يزور جملة التصدير - يطبع التصريح المصدَّر.
 *        (EN) Visits export statement - prints exported declaration.
 * 
 * @param stmt (AR) مؤشر لجملة التصدير. (EN) Pointer to export statement.
 */
void ASTPrinter::visitExportStmt(ExportStmt& stmt) {
    result_ += indent() + "export ";
    stmt.declaration->accept(*this);
}

} // namespace AST
} // namespace Sad
