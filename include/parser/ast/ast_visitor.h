/**
 * @file ast_visitor.h
 * @brief Visitor Pattern interface for AST traversal / واجهة نمط الزائر لاجتياز AST
 * @author Sad Language Team
 * @date 11 November 2025
 */

#ifndef SAD_AST_VISITOR_H
#define SAD_AST_VISITOR_H

namespace Sad {
namespace AST {

// Forward declarations for all AST node types
// تصريحات مسبقة لجميع أنواع عُقد AST

// Expression nodes / عُقد التعابير
class BinaryExpr;
class UnaryExpr;
class TernaryExpr;        // (AR) التعبير الثلاثي / (EN) Ternary expression
class LiteralExpr;
class VariableExpr;
class AssignExpr;
class CallExpr;
class IndexExpr;
class MemberExpr;
class ArrayExpr;
class MapExpr;
class WalrusExpr;         // (AR) تعبير Walrus / (EN) Walrus expression
class LambdaExpr;
class ListComprehensionExpr;
class DictComprehensionExpr;
class SetComprehensionExpr;
class GeneratorExpr;
class DecoratorExpr;

// OOP Expression nodes / عُقد تعابير OOP
class NewExpr;
class MemberAccessExpr;
class MemberAssignExpr;
class MethodCallExpr;
class ThisExpr;
class SuperExpr;

// Statement nodes / عُقد العبارات
class ExprStmt;
class VarDeclStmt;
class IfStmt;
class WhileStmt;
class ForStmt;
class ForRangeStmt;
class SwitchStmt;         // (AR) جملة حالة / (EN) Switch statement
class ReturnStmt;
class YieldStmt;      // (AR) جملة yield / (EN) Yield statement
class BreakStmt;
class ContinueStmt;
class BlockStmt;
class TryStmt;
class RaiseStmt;
class WithStmt;
class MatchStmt;      // (AR) جملة match لمطابقة الأنماط / (EN) Match statement for pattern matching

// OOP Statement nodes / عُقد عبارات OOP
class ClassDeclStmt;

// Declaration nodes / عُقد التصريحات
class FunctionDecl;
class ClassDecl;
class FieldDecl;
class MethodDecl;
class PropertyDecl;
class ConstructorDecl;
class DestructorDecl;
class EnumDecl;
class ImportStmt;
class FromImportStmt;  // (AR) جملة الاستيراد الانتقائي / (EN) Selective import statement
class ExportStmt;
class ExportDecl;      // (AR) تصريح التصدير / (EN) Export declaration

/**
 * @brief Abstract visitor interface for AST traversal / واجهة الزائر المجردة لاجتياز AST
 * 
 * This interface implements the Visitor design pattern for traversing
 * the Abstract Syntax Tree. Each concrete visitor (e.g., ASTPrinter,
 * Interpreter, CodeGenerator) implements these methods to define
 * what happens when visiting each type of AST node.
 * 
 * هذه الواجهة تنفذ نمط تصميم الزائر لاجتياز شجرة الصيغة المجردة.
 * كل زائر ملموس (مثل ASTPrinter، المفسر، مولد الكود) ينفذ هذه الدوال
 * لتحديد ما يحدث عند زيارة كل نوع من عُقد AST.
 * 
 * @example Usage / الاستخدام:
 * @code{.cpp}
 * class MyVisitor : public ASTVisitor {
 * public:
 *     void visitBinaryExpr(BinaryExpr* expr) override {
 *         // Process binary expression
 *         expr->left->accept(*this);
 *         // Do something with operator
 *         expr->right->accept(*this);
 *     }
 *     // Implement all other visit methods...
 * };
 * 
 * MyVisitor visitor;
 * astRoot->accept(visitor);
 * @endcode
 */
class ASTVisitor {
public:
    /**
     * @brief Virtual destructor / المُدمِّر الافتراضي
     */
    virtual ~ASTVisitor() = default;
    
    // =====================================================================
    // Expression visitors / زوار التعابير
    // =====================================================================
    
    /**
     * @brief Visit binary expression node / زيارة عقدة التعبير الثنائي
     * @param expr Binary expression node (e.g., a + b, x * y)
     * 
     * Examples: 2 + 3, x * y, a && b, left == right
     * أمثلة: 2 + 3، س * ص، أ && ب، يسار == يمين
     */
    virtual void visitBinaryExpr(BinaryExpr& expr) = 0;
    
    /**
     * @brief Visit unary expression node / زيارة عقدة التعبير الأحادي
     * @param expr Unary expression node (e.g., -x, !flag)
     * 
     * Examples: -x, !flag, ~bits, +value
     * أمثلة: -س، !علَم، ~بتات، +قيمة
     */
    virtual void visitUnaryExpr(UnaryExpr& expr) = 0;
    
    /**
     * @brief Visit ternary conditional expression node / زيارة عقدة التعبير الثلاثي الشرطي
     * @param expr Ternary expression node (condition ? true_val : false_val)
     * 
     * Examples: x > 0 ? "positive" : "negative", age >= 18 ? "بالغ" : "قاصر"
     * أمثلة: س > 0 ؟ "موجب" : "سالب"، العمر >= 18 ؟ "بالغ" : "قاصر"
     * 
     * Syntax / النحو:
     *   condition ? true_expression : false_expression
     *   الشرط ؟ تعبير_صحيح : تعبير_خطأ
     * 
     * (AR) التعبير الثلاثي يُقيّم الشرط ويُرجع أحد التعبيرين بناءً على النتيجة
     * (EN) Ternary expression evaluates condition and returns one of two expressions
     */
    virtual void visitTernaryExpr(TernaryExpr& expr) = 0;
    
    /**
     * @brief Visit literal expression node / زيارة عقدة التعبير الحرفي
     * @param expr Literal expression node (e.g., 42, "text", true)
     * 
     * Examples: 42, 3.14, "مرحباً", true, none
     * أمثلة: 42، 3.14، "مرحباً"، صحيح، لاشيء
     */
    virtual void visitLiteralExpr(LiteralExpr& expr) = 0;
    
    /**
     * @brief Visit variable expression node / زيارة عقدة تعبير المتغير
     * @param expr Variable expression node (identifier)
     * 
     * Examples: x, counter, اسم, العمر
     */
    virtual void visitVariableExpr(VariableExpr& expr) = 0;
    
    /**
     * @brief Visit assignment expression node / زيارة عقدة تعبير الإسناد
     * @param expr Assignment expression node (e.g., x = 10)
     * 
     * Examples: x = 10, counter += 1, العمر = 25
     * أمثلة: س = 10، عداد += 1، العمر = 25
     */
    virtual void visitAssignExpr(AssignExpr& expr) = 0;
    
    /**
     * @brief Visit function call expression node / زيارة عقدة تعبير استدعاء الدالة
     * @param expr Call expression node (e.g., func(arg1, arg2))
     * 
     * Examples: print("hello"), sum(1, 2, 3), كائن.دالة()
     */
    virtual void visitCallExpr(CallExpr& expr) = 0;
    
    /**
     * @brief Visit index expression node / زيارة عقدة تعبير الفهرسة
     * @param expr Index expression node (e.g., arr[0], dict["key"])
     * 
     * Examples: arr[0], matrix[i][j], قاموس["مفتاح"]
     */
    virtual void visitIndexExpr(IndexExpr& expr) = 0;
    
    /**
     * @brief Visit member access expression node / زيارة عقدة تعبير الوصول للعضو
     * @param expr Member expression node (e.g., obj.field, obj.method())
     * 
     * Examples: obj.field, person.name, كائن.حقل
     */
    virtual void visitMemberExpr(MemberExpr& expr) = 0;
    
    /**
     * @brief Visit member assignment expression node / زيارة عقدة تعبير تعيين قيمة لعضو
     * @param expr Member assignment expression node (e.g., obj.field = value)
     * 
     * Examples: obj.field = 10, person.name = "أحمد", شخص.اسم = "محمد"
     */
    virtual void visitMemberAssignExpr(MemberAssignExpr& expr) = 0;
    
    /**
     * @brief Visit array literal expression node / زيارة عقدة تعبير المصفوفة الحرفية
     * @param expr Array expression node (e.g., [1, 2, 3])
     * 
     * Examples: [1, 2, 3], ["a", "b"], [س، ص، ع]
     */
    virtual void visitArrayExpr(ArrayExpr& expr) = 0;
    
    /**
     * @brief Visit map/dictionary literal expression node / زيارة عقدة تعبير القاموس الحرفي
     * @param expr Map expression node (e.g., {"key": value})
     * 
     * Examples: {"x": 10, "y": 20}, {"الاسم": "أحمد"}
     */
    virtual void visitMapExpr(MapExpr& expr) = 0;
    
    /**
     * @brief Visit walrus (assignment expression) node / زيارة عقدة تعبير Walrus (التعيين في التعبير)
     * @param expr Walrus operator expression node (:= operator)
     * 
     * Examples: if (n := len(items)) > 10, إذا (ع := طول(عناصر)) > 10
     * أمثلة: while (line := file.read()), بينما (سطر := ملف.قراءة())
     * 
     * (AR) عامل Walrus (:=) يسمح بالتعيين داخل التعبير ويُرجع القيمة المُعيّنة
     * (EN) Walrus operator (:=) allows assignment within expression and returns assigned value
     */
    virtual void visitWalrusExpr(WalrusExpr& expr) = 0;
    
    /**
     * @brief Visit lambda expression node / زيارة عقدة تعبير Lambda
     * @param expr Lambda expression node (e.g., (x) => x * 2)
     * 
     * Examples: (x) => x * 2, lambda x: x ** 2
     * أمثلة: (س) => س * 2، دالة س: س ** 2
     */
    virtual void visitLambdaExpr(LambdaExpr& expr) = 0;
    
    /**
     * @brief Visit list comprehension expression node / زيارة عقدة تعبير الاستيعاب القائمي
     * @param expr List comprehension node
     * 
     * Examples: [x * 2 for x in range(10)], [س لكل س في قائمة إذا س > 5]
     */
    virtual void visitListComprehensionExpr(ListComprehensionExpr& expr) = 0;
    
    /**
     * @brief Visit dictionary comprehension expression node / زيارة عقدة تعبير الاستيعاب القاموسي
     * @param expr Dictionary comprehension node
     * 
     * Examples: {x: x**2 for x in range(10)}, {س: س**2 لكل س في مدى(10)}
     */
    virtual void visitDictComprehensionExpr(DictComprehensionExpr& expr) = 0;
    
    /**
     * @brief Visit set comprehension expression node / زيارة عقدة تعبير الاستيعاب المجموعة
     * @param expr Set comprehension node
     * 
     * Examples: {x for x in range(10) if x > 5}, {س لكل س في مدى(10) إذا س > 5}
     */
    virtual void visitSetComprehensionExpr(SetComprehensionExpr& expr) = 0;
    
    /**
     * @brief Visit generator expression node / زيارة عقدة تعبير المولد
     * @param expr Generator expression node
     * 
     * Examples: (x * 2 for x in range(10))
     */
    virtual void visitGeneratorExpr(GeneratorExpr& expr) = 0;
    
    /**
     * @brief Visit decorator expression node / زيارة عقدة تعبير المُزخرِف
     * @param expr Decorator expression node / عقدة تعبير المُزخرِف
     * 
     * Examples / أمثلة:
     * - @decorator
     * - @cache(maxsize=100)
     * - @مُزخرِف
     * 
     * (AR) المُزخرِفات تُطبّق على الدوال والأصناف لتعديل سلوكها
     * (EN) Decorators apply to functions and classes to modify their behavior
     */
    virtual void visitDecoratorExpr(DecoratorExpr& expr) = 0;
    
    // =====================================================================
    // OOP Expression visitors / زوار تعابير OOP
    // =====================================================================
    
    /**
     * @brief Visit new expression node / زيارة عقدة تعبير new
     * @param expr New expression node (object instantiation)
     * 
     * Examples: new Person("أحمد", 25), جديد شخص("أحمد"، 25)
     */
    virtual void visitNewExpr(NewExpr& expr) = 0;
    
    /**
     * @brief Visit member access expression node / زيارة عقدة تعبير الوصول للعضو
     * @param expr Member access expression node
     * 
     * Examples: object.field, person.name, كائن.حقل
     */
    virtual void visitMemberAccessExpr(MemberAccessExpr& expr) = 0;
    
    /**
     * @brief Visit method call expression node / زيارة عقدة تعبير استدعاء الطريقة
     * @param expr Method call expression node
     * 
     * Examples: object.method(), person.sayHello(), كائن.طريقة()
     */
    virtual void visitMethodCallExpr(MethodCallExpr& expr) = 0;
    
    /**
     * @brief Visit this expression node / زيارة عقدة تعبير this
     * @param expr This expression node
     * 
     * Examples: this, this.field, هذا، هذا.حقل
     */
    virtual void visitThisExpr(ThisExpr& expr) = 0;
    
    /**
     * @brief Visit super expression node / زيارة عقدة تعبير super
     * @param expr Super expression node
     * 
     * Examples: super.method(), الأساس.طريقة()
     */
    virtual void visitSuperExpr(SuperExpr& expr) = 0;
    
    // =====================================================================
    // Statement visitors / زوار العبارات
    // =====================================================================
    
    /**
     * @brief Visit expression statement node / زيارة عقدة عبارة التعبير
     * @param stmt Expression statement node (e.g., print(x);)
     * 
     * An expression used as a statement.
     * تعبير مُستخدم كعبارة.
     */
    virtual void visitExprStmt(ExprStmt& stmt) = 0;
    
    /**
     * @brief Visit variable declaration statement node / زيارة عقدة عبارة تصريح المتغير
     * @param stmt Variable declaration node (e.g., int x = 10;)
     * 
     * Examples: int x = 10;, رقم عمر = 25;
     */
    virtual void visitVarDeclStmt(VarDeclStmt& stmt) = 0;
    
    /**
     * @brief Visit if statement node / زيارة عقدة عبارة if
     * @param stmt If statement node
     * 
     * Examples: if (x > 0) {...}, إذا (س > 0) {...}
     */
    virtual void visitIfStmt(IfStmt& stmt) = 0;
    
    /**
     * @brief Visit while statement node / زيارة عقدة عبارة while
     * @param stmt While statement node
     * 
     * Examples: while (x < 10) {...}, بينما (س < 10) {...}
     */
    virtual void visitWhileStmt(WhileStmt& stmt) = 0;
    
    /**
     * @brief Visit for-each statement node / زيارة عقدة عبارة for-each
     * @param stmt For statement node (foreach variant)
     * 
     * Examples: for item in list {...}, لكل عنصر في قائمة {...}
     */
    virtual void visitForStmt(ForStmt& stmt) = 0;
    
    /**
     * @brief Visit for-range statement node / زيارة عقدة عبارة for-range
     * @param stmt For statement node (C-style variant)
     * 
     * Examples: for (int i = 0; i < 10; i++) {...}
     * أمثلة: لكل (رقم ع = 0؛ ع < 10؛ ع++) {...}
     */
    virtual void visitForRangeStmt(ForRangeStmt& stmt) = 0;
    
    /**
     * @brief Visit switch-case statement node / زيارة عقدة عبارة switch-case
     * @param stmt Switch statement node
     * 
     * Examples / أمثلة:
     * 
     *   حالة قيمة
     *       عندما 1:
     *           اطبع("واحد")
     *       عندما 2:
     *           اطبع("اثنان")
     *       افتراضي:
     *           اطبع("آخر")
     *   نهاية
     * 
     *   switch value
     *       case 1:
     *           print("one")
     *       case 2:
     *           print("two")
     *       default:
     *           print("other")
     *   end
     * 
     * Syntax / النحو:
     *   حالة <expression>
     *       عندما <value>: <statement>
     *       [افتراضي: <statement>]
     *   نهاية
     * 
     * (AR) جملة switch تقارن تعبيراً بعدة قيم وتنفذ الكود المطابق
     * (EN) Switch statement compares an expression against multiple values
     * 
     * Spec: docs/language_spec/rules/04_syntax.md
     */
    virtual void visitSwitchStmt(SwitchStmt& stmt) = 0;
    
    /**
     * @brief Visit return statement node / زيارة عقدة عبارة return
     * @param stmt Return statement node
     * 
     * Examples: return x;, إرجاع س;
     */
    virtual void visitReturnStmt(ReturnStmt& stmt) = 0;
    
    /**
     * @brief Visit yield statement node / زيارة عقدة عبارة yield
     * @param stmt Yield statement node
     * 
     * Examples: yield 42;, yield from list;, اعطِ قيمة;
     * 
     * Yield statements pause generator execution and produce values.
     * جمل yield توقف تنفيذ المولّد وتُنتج قيم.
     */
    virtual void visitYieldStmt(YieldStmt& stmt) = 0;
    
    /**
     * @brief Visit break statement node / زيارة عقدة عبارة break
     * @param stmt Break statement node
     * 
     * Examples: break;, اخرج;
     */
    virtual void visitBreakStmt(BreakStmt& stmt) = 0;
    
    /**
     * @brief Visit continue statement node / زيارة عقدة عبارة continue
     * @param stmt Continue statement node
     * 
     * Examples: continue;, تابع;
     */
    virtual void visitContinueStmt(ContinueStmt& stmt) = 0;
    
    /**
     * @brief Visit block statement node / زيارة عقدة عبارة الكتلة
     * @param stmt Block statement node
     * 
     * A sequence of statements enclosed in braces or نهاية.
     * تسلسل من العبارات محاطة بأقواس أو نهاية.
     */
    virtual void visitBlockStmt(BlockStmt& stmt) = 0;
    
    /**
     * @brief Visit try statement node / زيارة عقدة عبارة try
     * @param stmt Try statement node
     * 
     * Examples: try {...} catch (e) {...}, جرب {...} اعترض (خ) {...}
     */
    virtual void visitTryStmt(TryStmt& stmt) = 0;
    
    /**
     * @brief Visit raise statement node / زيارة عقدة عبارة raise
     * @param stmt Raise statement node
     * 
     * Examples: raise exception;, ارفع استثناء;
     */
    virtual void visitRaiseStmt(RaiseStmt& stmt) = 0;
    
    /**
     * @brief Visit with statement node / زيارة عقدة عبارة with
     * @param stmt With statement node (context manager)
     * 
     * Examples: with file as f {...}, مع ملف كـ م {...}
     */
    virtual void visitWithStmt(WithStmt& stmt) = 0;
    
    /**
     * @brief Visit match statement node / زيارة عقدة عبارة match
     * @param stmt Match statement node (pattern matching)
     * 
     * Examples: match x { case 0: ... }, طابق س { في_حالة 0: ... }
     * 
     * (AR) تستخدم لمطابقة الأنماط - مشابهة لـ switch لكن أقوى
     * (EN) Used for pattern matching - similar to switch but more powerful
     */
    virtual void visitMatchStmt(MatchStmt& stmt) = 0;
    
    /**
     * @brief Visit class declaration statement node / زيارة عقدة عبارة تصريح الصنف
     * @param stmt Class declaration statement node
     * 
     * Examples: class Person {...}, صنف شخص {...}
     */
    virtual void visitClassDeclStmt(ClassDeclStmt& stmt) = 0;
    
    // =====================================================================
    // Declaration visitors / زوار التصريحات
    // =====================================================================
    
    /**
     * @brief Visit function declaration node / زيارة عقدة تصريح الدالة
     * @param decl Function declaration node
     * 
     * Examples: function add(x, y) {...}, دالة جمع(س، ص) {...}
     */
    virtual void visitFunctionDecl(FunctionDecl& decl) = 0;
    
    /**
     * @brief Visit class declaration node / زيارة عقدة تصريح الصنف
     * @param decl Class declaration node
     * 
     * Examples: class Person {...}, صنف شخص {...}
     */
    virtual void visitClassDecl(ClassDecl& decl) = 0;
    
    /**
     * @brief Visit field declaration node / زيارة عقدة تصريح الحقل
     * @param decl Field declaration node
     * 
     * Class member variable.
     * متغير عضو في الصنف.
     */
    virtual void visitFieldDecl(FieldDecl& decl) = 0;
    
    /**
     * @brief Visit method declaration node / زيارة عقدة تصريح الطريقة
     * @param decl Method declaration node
     * 
     * Class member function.
     * دالة عضو في الصنف.
     */
    virtual void visitMethodDecl(MethodDecl& decl) = 0;
    
    /**
     * @brief Visit property declaration node / زيارة عقدة تصريح الخاصية
     * @param decl Property declaration node
     * 
     * Property with getter/setter blocks. (Phase 6.3)
     * خاصية مع كتل القراءة/الكتابة.
     */
    virtual void visitPropertyDecl(PropertyDecl& decl) = 0;
    
    /**
     * @brief Visit constructor declaration node / زيارة عقدة تصريح الباني
     * @param decl Constructor declaration node
     * 
     * Examples: constructor() {...}, باني() {...}
     */
    virtual void visitConstructorDecl(ConstructorDecl& decl) = 0;
    
    /**
     * @brief Visit destructor declaration node / زيارة عقدة تصريح المدمر
     * @param decl Destructor declaration node
     * 
     * Examples: ~constructor() {...}, مدمر() {...}
     */
    virtual void visitDestructorDecl(DestructorDecl& decl) = 0;
    
    /**
     * @brief Visit enum declaration node / زيارة عقدة تصريح التعداد
     * @param decl Enum declaration node
     * 
     * Examples: enum Color {RED, GREEN, BLUE}
     */
    virtual void visitEnumDecl(EnumDecl& decl) = 0;
    
    /**
     * @brief Visit import statement node / زيارة عقدة عبارة الاستيراد
     * @param stmt Import statement node
     * 
     * Examples: import math;, استورد رياضيات;, استورد رياضيات كـ م;
     * 
     * Syntax / النحو:
     *   import [module] [as alias]
     *   استورد [وحدة] [كـ اسم_مستعار]
     */
    virtual void visitImportStmt(ImportStmt& stmt) = 0;
    
    /**
     * @brief Visit from-import statement node / زيارة عقدة عبارة الاستيراد الانتقائي
     * @param stmt From-import statement node
     * 
     * Examples: from math import sqrt;, من رياضيات استورد جذر;
     * 
     * Syntax / النحو:
     *   from [module] import [items]
     *   من [وحدة] استورد [عناصر]
     * 
     * (AR) يُستخدم لاستيراد رموز محددة من وحدة بدلاً من استيراد الوحدة كاملة
     * (EN) Used to import specific symbols from a module instead of importing the whole module
     */
    virtual void visitFromImportStmt(FromImportStmt& stmt) = 0;
    
    /**
     * @brief Visit export statement node / زيارة عقدة عبارة التصدير (قديم)
     * @param stmt Export statement node (deprecated)
     * 
     * Note: This is kept for backward compatibility. Use ExportDecl instead.
     * ملاحظة: هذا محفوظ للتوافق مع الإصدارات القديمة. استخدم ExportDecl بدلاً منه.
     */
    virtual void visitExportStmt(ExportStmt& stmt) = 0;
    
    /**
     * @brief Visit export declaration node / زيارة عقدة تصريح التصدير
     * @param decl Export declaration node
     * 
     * Examples: export function add() {...}, صدّر دالة جمع() {...}
     * 
     * Syntax / النحو:
     *   export [declaration]
     *   صدّر [تصريح]
     * 
     * (AR) يُستخدم لجعل رمز (دالة/صنف/متغير) متاحاً للوحدات الأخرى
     * (EN) Used to make a symbol (function/class/variable) available to other modules
     */
    virtual void visitExportDecl(ExportDecl& decl) = 0;
};

/**
 * @brief Base visitor with empty default implementations
 * 
 * Provides empty default implementations for all visit methods.
 * Derived visitors can override only the methods they need.
 * 
 * يوفر تطبيقات افتراضية فارغة لكل دوال الزيارة.
 * الزوار المشتقة يمكن أن تعيد تعريف فقط الدوال التي تحتاجها.
 */
class BaseASTVisitor : public ASTVisitor {
public:
    // Expression visitors / زوار التعابير
    void visitBinaryExpr(BinaryExpr& expr) override {}
    void visitUnaryExpr(UnaryExpr& expr) override {}
    void visitTernaryExpr(TernaryExpr& expr) override {}
    void visitLiteralExpr(LiteralExpr& expr) override {}
    void visitVariableExpr(VariableExpr& expr) override {}
    void visitAssignExpr(AssignExpr& expr) override {}
    void visitCallExpr(CallExpr& expr) override {}
    void visitIndexExpr(IndexExpr& expr) override {}
    void visitMemberExpr(MemberExpr& expr) override {}
    void visitMemberAssignExpr(MemberAssignExpr& expr) override {}
    void visitArrayExpr(ArrayExpr& expr) override {}
    void visitMapExpr(MapExpr& expr) override {}
    void visitLambdaExpr(LambdaExpr& expr) override {}
    void visitWalrusExpr(WalrusExpr& expr) override {}
    void visitListComprehensionExpr(ListComprehensionExpr& expr) override {}
    void visitDictComprehensionExpr(DictComprehensionExpr& expr) override {}
    void visitSetComprehensionExpr(SetComprehensionExpr& expr) override {}
    void visitGeneratorExpr(GeneratorExpr& expr) override {}
    void visitDecoratorExpr(DecoratorExpr& expr) override {}
    
    // OOP Expression visitors / زوار تعابير OOP
    void visitNewExpr(NewExpr& expr) override {}
    void visitMemberAccessExpr(MemberAccessExpr& expr) override {}
    void visitMethodCallExpr(MethodCallExpr& expr) override {}
    void visitThisExpr(ThisExpr& expr) override {}
    void visitSuperExpr(SuperExpr& expr) override {}
    
    // Statement visitors / زوار العبارات
    void visitExprStmt(ExprStmt& stmt) override {}
    void visitVarDeclStmt(VarDeclStmt& stmt) override {}
    void visitIfStmt(IfStmt& stmt) override {}
    void visitWhileStmt(WhileStmt& stmt) override {}
    void visitForStmt(ForStmt& stmt) override {}
    void visitForRangeStmt(ForRangeStmt& stmt) override {}
    void visitSwitchStmt(SwitchStmt& stmt) override {}
    void visitReturnStmt(ReturnStmt& stmt) override {}
    void visitYieldStmt(YieldStmt& stmt) override {}
    void visitBreakStmt(BreakStmt& stmt) override {}
    void visitContinueStmt(ContinueStmt& stmt) override {}
    void visitBlockStmt(BlockStmt& stmt) override {}
    void visitTryStmt(TryStmt& stmt) override {}
    void visitRaiseStmt(RaiseStmt& stmt) override {}
    void visitWithStmt(WithStmt& stmt) override {}
    void visitMatchStmt(MatchStmt& stmt) override {}  // (AR) مطابقة الأنماط / (EN) Pattern matching
    void visitClassDeclStmt(ClassDeclStmt& stmt) override {}
    
    // Declaration visitors / زوار التصريحات
    void visitFunctionDecl(FunctionDecl& decl) override {}
    void visitClassDecl(ClassDecl& decl) override {}
    void visitFieldDecl(FieldDecl& decl) override {}
    void visitMethodDecl(MethodDecl& decl) override {}
    void visitPropertyDecl(PropertyDecl& decl) override {}
    void visitConstructorDecl(ConstructorDecl& decl) override {}
    void visitDestructorDecl(DestructorDecl& decl) override {}
    void visitEnumDecl(EnumDecl& decl) override {}
    void visitImportStmt(ImportStmt& stmt) override {}
    void visitFromImportStmt(FromImportStmt& stmt) override {}
    void visitExportStmt(ExportStmt& stmt) override {}
    void visitExportDecl(ExportDecl& decl) override {}
};

} // namespace AST
} // namespace Sad

#endif // SAD_AST_VISITOR_H
