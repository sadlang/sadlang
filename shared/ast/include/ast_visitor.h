/**
 * @file ast_visitor.h
 * @brief Visitor Pattern interface for AST traversal / واجهة نمط الزائر لاجتياز AST
 * @author Sad Language Team
 * @date 11 November 2025
 */

#ifndef SAD_AST_VISITOR_H
#define SAD_AST_VISITOR_H

namespace Sad
{
    namespace AST
    {

        // Forward declarations for all AST node types
        // تصريحات مسبقة لجميع أنواع عُقد AST

        // Expression nodes / عُقد التعابير
        class BinaryExpr;
        class UnaryExpr;
        class TernaryExpr; // (AR) التعبير الثلاثي / (EN) Ternary expression
        class LiteralExpr;
        class VariableExpr;
        class AssignExpr;
        class BorrowExpr; // (AR) تعبير الاستعارة / (EN) Borrow expression
        class CallExpr;
        class IndexExpr;
        class MemberExpr;
        class ArrayExpr;
        class MapExpr;
        class WalrusExpr; // (AR) تعبير Walrus / (EN) Walrus expression
        class AwaitExpr;  // (AR) تعبير Await (async/await) / (EN) Await expression
        class LambdaExpr;
        class ListComprehensionExpr;
        class DictComprehensionExpr;
        class SetComprehensionExpr;
        class GeneratorExpr;
        class DecoratorExpr;
        class InlineAsmExpr;      // (AR) تعبير التجميع المضمّن / (EN) Inline assembly expression
        class RangeExpr;          // (AR) تعبير المدى / (EN) Range expression
        class SliceExpr;          // (AR) تعبير الشريحة / (EN) Slice expression
        class OptionalChainExpr;  // (AR) تعبير الوصول الآمن ?. / (EN) Optional chaining expression
        class NullCoalesceExpr;   // (AR) تعبير التجميع الفارغ ?? / (EN) Null coalescing expression
        class ErrorPropagateExpr; // (AR) تعبير نشر الخطأ — انشر / (EN) Error propagation expression
        class TupleExpr;          // (AR) تعبير الصف / (EN) Tuple expression
        class EnumVariantExpr;    // (AR) تعبير بناء عضو تعداد بحمولة (ADT) / (EN) Tagged-enum variant construction

        // Directive nodes / عُقد التوجيهات @
        class UnsafeBlockStmt;     // (AR) @غير_آمن / (EN) @unsafe block
        class ComptimeBlockStmt;   // (AR) @وقت_الترجمة / (EN) @comptime block
        class SizeofExpr;          // (AR) @حجم / (EN) @sizeof expression
        class AtomicExpr;          // (AR) @ذري / (EN) @atomic expression
        class VolatileVarDeclStmt; // (AR) @متطاير / (EN) @volatile declaration
        class AsmBlockStmt;        // (AR) كتلة لهجة التجميع «تجميع … نهاية» (م١ RFC اللهجات) / (EN) assembly dialect block

        // OOP Expression nodes / عُقد تعابير OOP
        class NewExpr;
        class MemberAccessExpr;
        class MemberAssignExpr;
        class IndexAssignExpr;
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
        class SwitchStmt; // (AR) جملة حالة / (EN) Switch statement
        class ReturnStmt;
        class YieldStmt; // (AR) جملة yield / (EN) Yield statement
        class BreakStmt;
        class ContinueStmt;
        class BlockStmt;
        class TryStmt;
        class RaiseStmt;
        class WithStmt;
        class DeferStmt;  // (AR) جملة التأجيل — تنظيف مضمون / (EN) Defer statement — guaranteed cleanup
        class GoStmt;     // (AR) جملة أطلق — تنفيذ متزامن / (EN) Go statement — concurrent execution
        class SelectCase; // (AR) حالة اختر — فرع في select / (EN) Select case — branch in select
        class SelectStmt; // (AR) جملة اختر — انتظار متعدد القنوات / (EN) Select statement — multi-channel wait
        class MatchStmt;  // (AR) جملة match لمطابقة الأنماط / (EN) Match statement for pattern matching

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
        class StructDecl;
        class TestDecl;
        class ImportStmt;
        class FromImportStmt; // (AR) جملة الاستيراد الانتقائي / (EN) Selective import statement
        class ExportStmt;
        class ExportDecl;   // (AR) تصريح التصدير / (EN) Export declaration
        class ReExportStmt; // (AR) جملة إعادة التصدير / (EN) Re-export statement

        // Template/Generic nodes (Phase 7B) / عُقد القوالب
        class TemplateFunctionDecl;  // (AR) تصريح دالة قالب / (EN) Template function declaration
        class TemplateClassDecl;     // (AR) تصريح صنف قالب / (EN) Template class declaration
        class TemplateInstantiation; // (AR) تنفيذ القالب / (EN) Template instantiation
        class NamespaceDecl;         // (AR) تصريح فضاء الأسماء / (EN) Namespace declaration
        class OperatorDecl;          // (AR) تصريح تحميل العامل / (EN) Operator overload declaration
        class TraitDecl;             // (AR) تصريح الواجهة/السمة / (EN) Trait/Interface declaration
        class ImplDecl;              // (AR) تصريح كتلة التنفيذ / (EN) Impl block declaration
        class ExtensionDecl;         // (AR) تصريح كتلة الامتداد / (EN) Extension block declaration
        class MacroDecl;             // (AR) تصريح ماكرو / (EN) Macro declaration
        class TypeAliasDecl;         // (AR) تصريح اسم مستعار للنوع / (EN) Type alias declaration
        class TupleDestructureStmt;  // (AR) تفكيك الصف / (EN) Tuple destructuring statement

        // UI Declarative nodes (Phase UI) / عُقد الواجهات التصريحية
        class UIDeclarationNode;  // (AR) تعريف مكون واجهة (واجهة ... نهاية) / (EN) UI component declaration
        class UIWidgetExprNode;   // (AR) تعبير عنصر واجهة (نص، عمود، زر...) / (EN) Widget expression
        class UIModifierNode;     // (AR) معدّل سلسلي (.حجم، .لون...) / (EN) Chained modifier
        class UIEventHandlerNode; // (AR) معالج حدث (.عند_النقر...) / (EN) Event handler
        class UIStateDecl;        // (AR) تصريح حالة (@حالة، @ربط...) / (EN) State declaration
        class UIConditionalNode;  // (AR) عقدة شرطية داخل شجرة الواجهة / (EN) UI conditional node
        class UILoopNode;         // (AR) عقدة تكرار داخل شجرة الواجهة / (EN) UI loop node
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
        class ASTVisitor
        {
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
            virtual void visitBinaryExpr(BinaryExpr &expr) = 0;

            /**
             * @brief Visit unary expression node / زيارة عقدة التعبير الأحادي
             * @param expr Unary expression node (e.g., -x, !flag)
             *
             * Examples: -x, !flag, ~bits, +value
             * أمثلة: -س، !علَم، ~بتات، +قيمة
             */
            virtual void visitUnaryExpr(UnaryExpr &expr) = 0;

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
            virtual void visitTernaryExpr(TernaryExpr &expr) = 0;

            /**
             * @brief Visit literal expression node / زيارة عقدة التعبير الحرفي
             * @param expr Literal expression node (e.g., 42, "text", true)
             *
             * Examples: 42, 3.14, "مرحباً", true, none
             * أمثلة: 42، 3.14، "مرحباً"، صحيح، لاشيء
             */
            virtual void visitLiteralExpr(LiteralExpr &expr) = 0;

            /**
             * @brief Visit variable expression node / زيارة عقدة تعبير المتغير
             * @param expr Variable expression node (identifier)
             *
             * Examples: x, counter, اسم, العمر
             */
            virtual void visitVariableExpr(VariableExpr &expr) = 0;

            /**
             * @brief Visit borrow expression node / زيارة عقدة تعبير الاستعارة
             * @param expr Borrow expression node (e.g., &x, &متغير x)
             *
             * Examples: &x, &mut counter, &اسم
             */
            virtual void visitBorrowExpr(BorrowExpr &expr) = 0;

            /**
             * @brief Visit assignment expression node / زيارة عقدة تعبير الإسناد
             * @param expr Assignment expression node (e.g., x = 10)
             *
             * Examples: x = 10, counter += 1, العمر = 25
             * أمثلة: س = 10، عداد += 1، العمر = 25
             */
            virtual void visitAssignExpr(AssignExpr &expr) = 0;

            /**
             * @brief Visit function call expression node / زيارة عقدة تعبير استدعاء الدالة
             * @param expr Call expression node (e.g., func(arg1, arg2))
             *
             * Examples: print("hello"), sum(1, 2, 3), كائن.دالة()
             */
            virtual void visitCallExpr(CallExpr &expr) = 0;

            /**
             * @brief Visit index expression node / زيارة عقدة تعبير الفهرسة
             * @param expr Index expression node (e.g., arr[0], dict["key"])
             *
             * Examples: arr[0], matrix[i][j], قاموس["مفتاح"]
             */
            virtual void visitIndexExpr(IndexExpr &expr) = 0;

            /**
             * @brief Visit member access expression node / زيارة عقدة تعبير الوصول للعضو
             * @param expr Member expression node (e.g., obj.field, obj.method())
             *
             * Examples: obj.field, person.name, كائن.حقل
             */
            virtual void visitMemberExpr(MemberExpr &expr) = 0;

            /**
             * @brief Visit member assignment expression node / زيارة عقدة تعبير تعيين قيمة لعضو
             * @param expr Member assignment expression node (e.g., obj.field = value)
             *
             * Examples: obj.field = 10, person.name = "أحمد", شخص.اسم = "محمد"
             */
            virtual void visitMemberAssignExpr(MemberAssignExpr &expr) = 0;

            /**
             * @brief Visit index assignment expression / زيارة تعبير الإسناد بالفهرس
             * @param expr Index assignment node (e.g., arr[0] = 5, م[1] = "قيمة")
             *
             * Examples: م[0] = 5, قاموس["مفتاح"] = قيمة
             */
            virtual void visitIndexAssignExpr(IndexAssignExpr &expr) = 0;

            /**
             * @brief Visit array literal expression node / زيارة عقدة تعبير المصفوفة الحرفية
             * @param expr Array expression node (e.g., [1, 2, 3])
             *
             * Examples: [1, 2, 3], ["a", "b"], [س، ص، ع]
             */
            virtual void visitArrayExpr(ArrayExpr &expr) = 0;

            /**
             * @brief Visit map/dictionary literal expression node / زيارة عقدة تعبير القاموس الحرفي
             * @param expr Map expression node (e.g., {"key": value})
             *
             * Examples: {"x": 10, "y": 20}, {"الاسم": "أحمد"}
             */
            virtual void visitMapExpr(MapExpr &expr) = 0;

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
            virtual void visitWalrusExpr(WalrusExpr &expr) = 0;

            /**
             * @brief Visit await expression node / زيارة عقدة تعبير Await
             * @param expr Await expression node (e.g., await fetchData())
             *
             * Examples: await fetchData(), await readFile("data.txt")
             * أمثلة: انتظر جلب_البيانات()، انتظر قراءة_ملف("بيانات.txt")
             *
             * (AR) await يُستخدم لانتظار نتيجة دالة async - يمكن استخدامه فقط داخل دالة async
             * (EN) await is used to wait for async function result - can only be used inside async function
             *
             * Syntax / النحو:
             *   await expression
             *   انتظر تعبير
             *
             * Note / ملاحظة:
             * (AR) await يُوقف تنفيذ الدالة الحالية حتى يكتمل التعبير المنتظر
             * (EN) await suspends current function execution until awaited expression completes
             */
            virtual void visitAwaitExpr(AwaitExpr &expr) = 0;

            /**
             * @brief Visit lambda expression node / زيارة عقدة تعبير Lambda
             * @param expr Lambda expression node (e.g., (x) => x * 2)
             *
             * Examples: (x) => x * 2, lambda x: x ** 2
             * أمثلة: (س) => س * 2، دالة س: س ** 2
             */
            virtual void visitLambdaExpr(LambdaExpr &expr) = 0;

            /**
             * @brief Visit list comprehension expression node / زيارة عقدة تعبير الاستيعاب القائمي
             * @param expr List comprehension node
             *
             * Examples: [x * 2 for x in range(10)], [س لكل س في قائمة إذا س > 5]
             */
            virtual void visitListComprehensionExpr(ListComprehensionExpr &expr) = 0;

            /**
             * @brief Visit dictionary comprehension expression node / زيارة عقدة تعبير الاستيعاب القاموسي
             * @param expr Dictionary comprehension node
             *
             * Examples: {x: x**2 for x in range(10)}, {س: س**2 لكل س في مدى(10)}
             */
            virtual void visitDictComprehensionExpr(DictComprehensionExpr &expr) = 0;

            /**
             * @brief Visit set comprehension expression node / زيارة عقدة تعبير الاستيعاب المجموعة
             * @param expr Set comprehension node
             *
             * Examples: {x for x in range(10) if x > 5}, {س لكل س في مدى(10) إذا س > 5}
             */
            virtual void visitSetComprehensionExpr(SetComprehensionExpr &expr) = 0;

            /**
             * @brief Visit generator expression node / زيارة عقدة تعبير المولد
             * @param expr Generator expression node
             *
             * Examples: (x * 2 for x in range(10))
             */
            virtual void visitGeneratorExpr(GeneratorExpr &expr) = 0;

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
            virtual void visitDecoratorExpr(DecoratorExpr &expr) = 0;

            /**
             * @brief (AR) زيارة عقدة التجميع المضمّن — ضرورية لبرمجة أنظمة التشغيل
             * @brief (EN) Visit inline assembly node — essential for OS development
             *
             * (AR) التجميع المضمّن يسمح بكتابة أوامر المعالج مباشرة في كود لغة ص.
             *      يُستخدم في: تعطيل المقاطعات، قراءة/كتابة المنافذ، تبديل السياق
             * (EN) Inline asm allows writing CPU instructions directly in Sad code.
             *      Used for: disabling interrupts, port I/O, context switching
             */
            virtual void visitInlineAsmExpr(InlineAsmExpr &expr) = 0;
            virtual void visitRangeExpr(RangeExpr &expr) = 0;
            virtual void visitSliceExpr(SliceExpr &expr) = 0;

            /**
             * @brief (AR) وصول آمن — كائن?.عضو
             * @brief (EN) Optional chaining — object?.member
             */
            virtual void visitOptionalChainExpr(OptionalChainExpr &expr) = 0;

            /**
             * @brief (AR) تجميع فارغ — قيمة ?? بديل
             * @brief (EN) Null coalescing — value ?? fallback
             */
            virtual void visitNullCoalesceExpr(NullCoalesceExpr &expr) = 0;

            /**
             * @brief (AR) نشر الخطأ — انشر تعبير (إرجاع مبكر عند خطأ/عدم)
             * @brief (EN) Error propagation — propagate expr (early return on error/none)
             */
            virtual void visitErrorPropagateExpr(ErrorPropagateExpr &expr) = 0;

            /**
             * @brief (AR) تعبير الصف — (قيمة1، قيمة2، ...)
             * @brief (EN) Tuple expression — (value1, value2, ...)
             */
            virtual void visitTupleExpr(TupleExpr &expr) = 0;

            /**
             * @brief (AR) تعبير بناء عضو تعداد بحمولة (ADT) — سِقالة أ-م١.
             *        افتراضٌ عدميّ (غير خالص) كي لا يُلزم كلّ الزوّار القائمين بتحقيقه؛
             *        يُوصَل في التحويل الدلاليّ/المفسّر (أ-م٢/أ-م٣).
             * @brief (EN) Tagged-enum variant construction — phase A-M1 scaffold.
             *        Non-pure default so existing visitors need not implement it yet;
             *        wired in semantic lowering / interpreter (A-M2/A-M3).
             */
            virtual void visitEnumVariantExpr(EnumVariantExpr &expr) {}

            // =====================================================================
            // Directive visitors / زوار التوجيهات @
            // =====================================================================

            /**
             * @brief (AR) @غير_آمن — كتلة غير آمنة
             * @brief (EN) @unsafe — unsafe block statement
             */
            virtual void visitUnsafeBlockStmt(UnsafeBlockStmt &stmt) = 0;

            /**
             * @brief (AR) @وقت_الترجمة — كتلة تنفيذ وقت الترجمة
             * @brief (EN) @comptime — compile-time execution block
             */
            virtual void visitComptimeBlockStmt(ComptimeBlockStmt &stmt) = 0;

            /**
             * @brief (AR) @حجم(نوع) — حجم النوع
             * @brief (EN) @sizeof(type) — size of type expression
             */
            virtual void visitSizeofExpr(SizeofExpr &expr) = 0;

            /**
             * @brief (AR) @ذري(عملية, ...) — عملية ذرية
             * @brief (EN) @atomic(op, ...) — atomic operation expression
             */
            virtual void visitAtomicExpr(AtomicExpr &expr) = 0;

            /**
             * @brief (AR) @متطاير — إعلان متغير متطاير
             * @brief (EN) @volatile — volatile variable declaration
             */
            virtual void visitVolatileVarDeclStmt(VolatileVarDeclStmt &stmt) = 0;

            /**
             * @brief (AR) زيارة كتلة لهجة التجميع «تجميع … نهاية» (م١ RFC اللهجات الأصيلة)
             * @brief (EN) Visit assembly dialect block "تجميع … نهاية"
             *
             * (AR) افتراضيّ فارغ (كنمط عقد الواجهة) — يعيد تعريفه من يحتاجه فقط:
             *      المفسّر (بوّابة SEM027) والمترجم (خفض InlineAsm — عبر dynamic_cast لا الزائر).
             */
            virtual void visitAsmBlockStmt(AsmBlockStmt &stmt) {}

            // =====================================================================
            // OOP Expression visitors / زوار تعابير OOP
            // =====================================================================

            /**
             * @brief Visit new expression node / زيارة عقدة تعبير new
             * @param expr New expression node (object instantiation)
             *
             * Examples: new Person("أحمد", 25), جديد شخص("أحمد"، 25)
             */
            virtual void visitNewExpr(NewExpr &expr) = 0;

            /**
             * @brief Visit member access expression node / زيارة عقدة تعبير الوصول للعضو
             * @param expr Member access expression node
             *
             * Examples: object.field, person.name, كائن.حقل
             */
            virtual void visitMemberAccessExpr(MemberAccessExpr &expr) = 0;

            /**
             * @brief Visit method call expression node / زيارة عقدة تعبير استدعاء الطريقة
             * @param expr Method call expression node
             *
             * Examples: object.method(), person.sayHello(), كائن.طريقة()
             */
            virtual void visitMethodCallExpr(MethodCallExpr &expr) = 0;

            /**
             * @brief Visit this expression node / زيارة عقدة تعبير this
             * @param expr This expression node
             *
             * Examples: this, this.field, هذا، هذا.حقل
             */
            virtual void visitThisExpr(ThisExpr &expr) = 0;

            /**
             * @brief Visit super expression node / زيارة عقدة تعبير super
             * @param expr Super expression node
             *
             * Examples: super.method(), الأساس.طريقة()
             */
            virtual void visitSuperExpr(SuperExpr &expr) = 0;

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
            virtual void visitExprStmt(ExprStmt &stmt) = 0;

            /**
             * @brief Visit variable declaration statement node / زيارة عقدة عبارة تصريح المتغير
             * @param stmt Variable declaration node (e.g., int x = 10;)
             *
             * Examples: int x = 10;, رقم عمر = 25;
             */
            virtual void visitVarDeclStmt(VarDeclStmt &stmt) = 0;

            /**
             * @brief Visit if statement node / زيارة عقدة عبارة if
             * @param stmt If statement node
             *
             * Examples: if (x > 0) {...}, إذا (س > 0) {...}
             */
            virtual void visitIfStmt(IfStmt &stmt) = 0;

            /**
             * @brief Visit while statement node / زيارة عقدة عبارة while
             * @param stmt While statement node
             *
             * Examples: while (x < 10) {...}, بينما (س < 10) {...}
             */
            virtual void visitWhileStmt(WhileStmt &stmt) = 0;

            /**
             * @brief Visit for-each statement node / زيارة عقدة عبارة for-each
             * @param stmt For statement node (foreach variant)
             *
             * Examples: for item in list {...}, لكل عنصر في قائمة {...}
             */
            virtual void visitForStmt(ForStmt &stmt) = 0;

            /**
             * @brief Visit for-range statement node / زيارة عقدة عبارة for-range
             * @param stmt For statement node (C-style variant)
             *
             * Examples: for (int i = 0; i < 10; i++) {...}
             * أمثلة: لكل (رقم ع = 0؛ ع < 10؛ ع++) {...}
             */
            virtual void visitForRangeStmt(ForRangeStmt &stmt) = 0;

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
            virtual void visitSwitchStmt(SwitchStmt &stmt) = 0;

            /**
             * @brief Visit return statement node / زيارة عقدة عبارة return
             * @param stmt Return statement node
             *
             * Examples: return x;, إرجاع س;
             */
            virtual void visitReturnStmt(ReturnStmt &stmt) = 0;

            /**
             * @brief Visit yield statement node / زيارة عقدة عبارة yield
             * @param stmt Yield statement node
             *
             * Examples: yield 42;, yield from list;, اعطِ قيمة;
             *
             * Yield statements pause generator execution and produce values.
             * جمل yield توقف تنفيذ المولّد وتُنتج قيم.
             */
            virtual void visitYieldStmt(YieldStmt &stmt) = 0;

            /**
             * @brief Visit break statement node / زيارة عقدة عبارة break
             * @param stmt Break statement node
             *
             * Examples: break;, اخرج;
             */
            virtual void visitBreakStmt(BreakStmt &stmt) = 0;

            /**
             * @brief Visit continue statement node / زيارة عقدة عبارة continue
             * @param stmt Continue statement node
             *
             * Examples: continue;, تابع;
             */
            virtual void visitContinueStmt(ContinueStmt &stmt) = 0;

            /**
             * @brief Visit block statement node / زيارة عقدة عبارة الكتلة
             * @param stmt Block statement node
             *
             * A sequence of statements enclosed in braces or نهاية.
             * تسلسل من العبارات محاطة بأقواس أو نهاية.
             */
            virtual void visitBlockStmt(BlockStmt &stmt) = 0;

            /**
             * @brief Visit try statement node / زيارة عقدة عبارة try
             * @param stmt Try statement node
             *
             * Examples: try {...} catch (e) {...}, جرب {...} اعترض (خ) {...}
             */
            virtual void visitTryStmt(TryStmt &stmt) = 0;

            /**
             * @brief Visit raise statement node / زيارة عقدة عبارة raise
             * @param stmt Raise statement node
             *
             * Examples: raise exception;, ارفع استثناء;
             */
            virtual void visitRaiseStmt(RaiseStmt &stmt) = 0;

            /**
             * @brief Visit with statement node / زيارة عقدة عبارة with
             * @param stmt With statement node (context manager)
             *
             * Examples: with file as f {...}, مع ملف كـ م {...}
             */
            virtual void visitWithStmt(WithStmt &stmt) = 0;

            /**
             * @brief Visit match statement node / زيارة عقدة عبارة match
             * @param stmt Match statement node (pattern matching)
             *
             * Examples: match x { case 0: ... }, طابق س { في_حالة 0: ... }
             *
             * (AR) تستخدم لمطابقة الأنماط - مشابهة لـ switch لكن أقوى
             * (EN) Used for pattern matching - similar to switch but more powerful
             */
            virtual void visitMatchStmt(MatchStmt &stmt) = 0;

            /**
             * @brief (AR) زيارة عقدة جملة التأجيل — تنظيف مضمون عند خروج الدالة
             * @brief (EN) Visit defer statement node — guaranteed cleanup on function exit
             * @param stmt Defer statement node
             *
             * Examples: أجّل اطبع("تنظيف"), أجّل ... نهاية
             */
            virtual void visitDeferStmt(DeferStmt &stmt) = 0;

            /**
             * @brief (AR) زيارة جملة أطلق — إطلاق goroutine
             * @brief (EN) Visit go statement — launch a goroutine
             *
             * Examples: أطلق دالة(), أطلق لامدا() => ..., أطلق ... نهاية
             */
            virtual void visitGoStmt(GoStmt &stmt) = 0;

            /**
             * @brief (AR) زيارة حالة اختيار من قناة
             * @brief (EN) Visit select case
             */
            virtual void visitSelectCase(SelectCase &stmt) = 0;

            /**
             * @brief (AR) زيارة جملة اختر — اختيار من قنوات متعددة
             * @brief (EN) Visit select statement — multiplex on channels
             *
             * Examples: اختر عندما ق.استقبل(): ... افتراضي: ... نهاية
             */
            virtual void visitSelectStmt(SelectStmt &stmt) = 0;

            /**
             * @brief Visit class declaration statement node / زيارة عقدة عبارة تصريح الصنف
             * @param stmt Class declaration statement node
             *
             * Examples: class Person {...}, صنف شخص {...}
             */
            virtual void visitClassDeclStmt(ClassDeclStmt &stmt) = 0;

            // =====================================================================
            // Declaration visitors / زوار التصريحات
            // =====================================================================

            /**
             * @brief Visit function declaration node / زيارة عقدة تصريح الدالة
             * @param decl Function declaration node
             *
             * Examples: function add(x, y) {...}, دالة جمع(س، ص) {...}
             */
            virtual void visitFunctionDecl(FunctionDecl &decl) = 0;

            /**
             * @brief Visit class declaration node / زيارة عقدة تصريح الصنف
             * @param decl Class declaration node
             *
             * Examples: class Person {...}, صنف شخص {...}
             */
            virtual void visitClassDecl(ClassDecl &decl) = 0;

            /**
             * @brief Visit field declaration node / زيارة عقدة تصريح الحقل
             * @param decl Field declaration node
             *
             * Class member variable.
             * متغير عضو في الصنف.
             */
            virtual void visitFieldDecl(FieldDecl &decl) = 0;

            /**
             * @brief Visit method declaration node / زيارة عقدة تصريح الطريقة
             * @param decl Method declaration node
             *
             * Class member function.
             * دالة عضو في الصنف.
             */
            virtual void visitMethodDecl(MethodDecl &decl) = 0;

            /**
             * @brief Visit property declaration node / زيارة عقدة تصريح الخاصية
             * @param decl Property declaration node
             *
             * Property with getter/setter blocks. (Phase 6.3)
             * خاصية مع كتل القراءة/الكتابة.
             */
            virtual void visitPropertyDecl(PropertyDecl &decl) = 0;

            /**
             * @brief Visit constructor declaration node / زيارة عقدة تصريح الباني
             * @param decl Constructor declaration node
             *
             * Examples: constructor() {...}, باني() {...}
             */
            virtual void visitConstructorDecl(ConstructorDecl &decl) = 0;

            /**
             * @brief Visit destructor declaration node / زيارة عقدة تصريح المدمر
             * @param decl Destructor declaration node
             *
             * Examples: ~constructor() {...}, مدمر() {...}
             */
            virtual void visitDestructorDecl(DestructorDecl &decl) = 0;

            /**
             * @brief Visit enum declaration node / زيارة عقدة تصريح التعداد
             * @param decl Enum declaration node
             *
             * Examples: enum Color {RED, GREEN, BLUE}
             */
            virtual void visitEnumDecl(EnumDecl &decl) = 0;

            /**
             * @brief Visit struct declaration node / زيارة عقدة تصريح البنية
             * @param decl Struct declaration node
             */
            virtual void visitStructDecl(StructDecl &decl) = 0;

            /**
             * @brief Visit test declaration node / زيارة عقدة تصريح الاختبار
             * @param decl Test declaration node
             */
            virtual void visitTestDecl(TestDecl &decl) = 0;

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
            virtual void visitImportStmt(ImportStmt &stmt) = 0;

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
            virtual void visitFromImportStmt(FromImportStmt &stmt) = 0;

            /**
             * @brief Visit export statement node / زيارة عقدة عبارة التصدير (قديم)
             * @param stmt Export statement node (deprecated)
             *
             * Note: This is kept for backward compatibility. Use ExportDecl instead.
             * ملاحظة: هذا محفوظ للتوافق مع الإصدارات القديمة. استخدم ExportDecl بدلاً منه.
             */
            virtual void visitExportStmt(ExportStmt &stmt) = 0;

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
            virtual void visitExportDecl(ExportDecl &decl) = 0;

            /**
             * @brief Visit re-export statement / زيارة عقدة إعادة التصدير
             * @param stmt Re-export statement node
             *
             * Syntax: صدّر * من وحدة / صدّر عنصر1، عنصر2 من وحدة
             */
            virtual void visitReExportStmt(ReExportStmt &stmt) = 0;

            // =====================================================================
            // Template/Generic Visitors (Phase 7B) / زوار القوالب
            // =====================================================================

            /**
             * @brief Visit template function declaration / زيارة تصريح دالة قالب
             * @param decl Template function declaration node
             *
             * Examples:
             *   قالب<نوع ت> دالة أكبر(ت أ، ت ب) ت { ... }
             *   template<typename T> func max(a: T, b: T) -> T { ... }
             */
            virtual void visitTemplateFunctionDecl(TemplateFunctionDecl &decl) = 0;

            /**
             * @brief Visit template class declaration / زيارة تصريح صنف قالب
             * @param decl Template class declaration node
             *
             * Examples:
             *   قالب<نوع ت> صنف صندوق { ... نهاية }
             *   template<typename T> class Box { ... }
             */
            virtual void visitTemplateClassDecl(TemplateClassDecl &decl) = 0;

            /**
             * @brief Visit template instantiation / زيارة تنفيذ القالب
             * @param expr Template instantiation expression
             *
             * Examples:
             *   صندوق<رقم> ص = جديد صندوق<رقم>(42)
             *   Box<int> b = new Box<int>(42)
             */
            virtual void visitTemplateInstantiation(TemplateInstantiation &expr) = 0;

            /**
             * @brief Visit namespace declaration / زيارة تصريح فضاء الأسماء
             * @param decl Namespace declaration node
             *
             * Examples:
             *   فضاء رياضيات ... نهاية_فضاء
             *   namespace math { ... }
             */
            virtual void visitNamespaceDecl(NamespaceDecl &decl) = 0;

            /**
             * @brief Visit operator overload declaration / زيارة تصريح تحميل العامل
             * @param decl Operator declaration node
             *
             * Examples:
             *   عامل +(كسر آخر) كسر { ... }
             *   operator +(other: Fraction) -> Fraction { ... }
             */
            virtual void visitOperatorDecl(OperatorDecl &decl) = 0;

            // Trait/Interface visitors / زوار الواجهات والسمات
            virtual void visitTraitDecl(TraitDecl &decl) = 0;
            virtual void visitImplDecl(ImplDecl &decl) = 0;

            /**
             * @brief (AR) زيارة تصريح كتلة الامتداد — إضافة دوال لنوع موجود
             * @brief (EN) Visit extension block declaration — add methods to existing type
             *
             * (AR) الصيغة: امتداد اسم_النوع ... نهاية
             * (EN) Syntax: extension TypeName ... end
             */
            virtual void visitExtensionDecl(ExtensionDecl &decl) = 0;

            /**
             * @brief (AR) زيارة تصريح ماكرو — تعريف نمط قابل للتوسيع
             * @brief (EN) Visit macro declaration — define expandable pattern
             *
             * (AR) الصيغة: ماكرو اسم(معاملات) ... نهاية
             * (EN) Syntax: macro name(params) ... end
             */
            virtual void visitMacroDecl(MacroDecl &decl) = 0;

            /**
             * @brief (AR) اسم مستعار للنوع — نوع اسم = نوع_هدف
             * @brief (EN) Type alias — type name = target_type
             */
            virtual void visitTypeAliasDecl(TypeAliasDecl &decl) = 0;

            /**
             * @brief (AR) تفكيك الصف — متغير (أ، ب) = صف
             * @brief (EN) Tuple destructuring — var (a, b) = tuple
             */
            virtual void visitTupleDestructureStmt(TupleDestructureStmt &stmt) = 0;

            // =====================================================================
            // UI Declarative visitors / زوار الواجهات التصريحية
            // =====================================================================

            /**
             * @brief (AR) زيارة تعريف مكون واجهة — واجهة اسم ... نهاية
             * @brief (EN) Visit UI component declaration — واجهة name ... نهاية
             */
            virtual void visitUIDeclaration(UIDeclarationNode &decl) = 0;

            /**
             * @brief (AR) زيارة تعبير عنصر واجهة — نص()، عمود، زر() ...
             * @brief (EN) Visit widget expression — نص()، عمود، زر() ...
             */
            virtual void visitUIWidgetExpr(UIWidgetExprNode &expr) = 0;

            /**
             * @brief (AR) زيارة معدّل سلسلي — .حجم(32)، .لون(.أزرق) ...
             * @brief (EN) Visit chained modifier — .حجم(32)، .لون(.أزرق) ...
             */
            virtual void visitUIModifier(UIModifierNode &expr) = 0;

            /**
             * @brief (AR) زيارة معالج حدث — .عند_النقر => ...، .عند_النقر ... نهاية
             * @brief (EN) Visit event handler — .عند_النقر => ...، .عند_النقر ... نهاية
             */
            virtual void visitUIEventHandler(UIEventHandlerNode &expr) = 0;

            /**
             * @brief (AR) زيارة تصريح حالة — @حالة، @ربط، @بيئة، @محسوب
             * @brief (EN) Visit state declaration — @حالة، @ربط، @بيئة، @محسوب
             */
            virtual void visitUIStateDecl(UIStateDecl &decl) {}

            /**
             * @brief (AR) زيارة عقدة رسم شرطي — إذا/وإلا داخل حاوية (ADR-UI-01)
             * @brief (EN) Visit conditional rendering node — if/else inside container
             */
            virtual void visitUIConditional(UIConditionalNode &node) {}

            /**
             * @brief (AR) زيارة عقدة حلقة رسم — لكل/بينما داخل حاوية (ADR-UI-01)
             * @brief (EN) Visit loop rendering node — for-each/while inside container
             */
            virtual void visitUILoop(UILoopNode &node) {}
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
        class BaseASTVisitor : public ASTVisitor
        {
        public:
            // Expression visitors / زوار التعابير
            void visitBinaryExpr(BinaryExpr &expr) override {}
            void visitUnaryExpr(UnaryExpr &expr) override {}
            void visitTernaryExpr(TernaryExpr &expr) override {}
            void visitLiteralExpr(LiteralExpr &expr) override {}
            void visitVariableExpr(VariableExpr &expr) override {}
            void visitBorrowExpr(BorrowExpr &expr) override {}
            void visitAssignExpr(AssignExpr &expr) override {}
            void visitCallExpr(CallExpr &expr) override {}
            void visitIndexExpr(IndexExpr &expr) override {}
            void visitMemberExpr(MemberExpr &expr) override {}
            void visitMemberAssignExpr(MemberAssignExpr &expr) override {}
            void visitIndexAssignExpr(IndexAssignExpr &expr) override {}
            void visitArrayExpr(ArrayExpr &expr) override {}
            void visitMapExpr(MapExpr &expr) override {}
            void visitLambdaExpr(LambdaExpr &expr) override {}
            void visitWalrusExpr(WalrusExpr &expr) override {}
            void visitAwaitExpr(AwaitExpr &expr) override {} // (AR) تعبير await للـ async/await / (EN) await expression for async/await
            void visitListComprehensionExpr(ListComprehensionExpr &expr) override {}
            void visitDictComprehensionExpr(DictComprehensionExpr &expr) override {}
            void visitSetComprehensionExpr(SetComprehensionExpr &expr) override {}
            void visitGeneratorExpr(GeneratorExpr &expr) override {}
            void visitDecoratorExpr(DecoratorExpr &expr) override {}
            void visitInlineAsmExpr(InlineAsmExpr &expr) override {}
            void visitRangeExpr(RangeExpr &expr) override {}
            void visitSliceExpr(SliceExpr &expr) override {}
            void visitOptionalChainExpr(OptionalChainExpr &expr) override {}
            void visitNullCoalesceExpr(NullCoalesceExpr &expr) override {}
            void visitErrorPropagateExpr(ErrorPropagateExpr &expr) override {} ///< (AR) نشر الخطأ — انشر / (EN) Error propagation
            void visitTupleExpr(TupleExpr &expr) override {}                   ///< (AR) تعبير الصف / (EN) Tuple expression

            // Directive visitors / زوار التوجيهات @
            void visitUnsafeBlockStmt(UnsafeBlockStmt &stmt) override {}
            void visitComptimeBlockStmt(ComptimeBlockStmt &stmt) override {}
            void visitSizeofExpr(SizeofExpr &expr) override {}
            void visitAtomicExpr(AtomicExpr &expr) override {}
            void visitVolatileVarDeclStmt(VolatileVarDeclStmt &stmt) override {}

            // OOP Expression visitors / زوار تعابير OOP
            void visitNewExpr(NewExpr &expr) override {}
            void visitMemberAccessExpr(MemberAccessExpr &expr) override {}
            void visitMethodCallExpr(MethodCallExpr &expr) override {}
            void visitThisExpr(ThisExpr &expr) override {}
            void visitSuperExpr(SuperExpr &expr) override {}

            // Statement visitors / زوار العبارات
            void visitExprStmt(ExprStmt &stmt) override {}
            void visitVarDeclStmt(VarDeclStmt &stmt) override {}
            void visitIfStmt(IfStmt &stmt) override {}
            void visitWhileStmt(WhileStmt &stmt) override {}
            void visitForStmt(ForStmt &stmt) override {}
            void visitForRangeStmt(ForRangeStmt &stmt) override {}
            void visitSwitchStmt(SwitchStmt &stmt) override {}
            void visitReturnStmt(ReturnStmt &stmt) override {}
            void visitYieldStmt(YieldStmt &stmt) override {}
            void visitBreakStmt(BreakStmt &stmt) override {}
            void visitContinueStmt(ContinueStmt &stmt) override {}
            void visitBlockStmt(BlockStmt &stmt) override {}
            void visitTryStmt(TryStmt &stmt) override {}
            void visitRaiseStmt(RaiseStmt &stmt) override {}
            void visitWithStmt(WithStmt &stmt) override {}
            void visitMatchStmt(MatchStmt &stmt) override {}   // (AR) مطابقة الأنماط / (EN) Pattern matching
            void visitDeferStmt(DeferStmt &stmt) override {}   // (AR) تأجيل / (EN) Defer statement
            void visitGoStmt(GoStmt &stmt) override {}         // (AR) أطلق goroutine / (EN) Go statement
            void visitSelectCase(SelectCase &stmt) override {} // (AR) حالة اختيار / (EN) Select case
            void visitSelectStmt(SelectStmt &stmt) override {} // (AR) اختر / (EN) Select statement
            void visitClassDeclStmt(ClassDeclStmt &stmt) override {}

            // Declaration visitors / زوار التصريحات
            void visitFunctionDecl(FunctionDecl &decl) override {}
            void visitClassDecl(ClassDecl &decl) override {}
            void visitFieldDecl(FieldDecl &decl) override {}
            void visitMethodDecl(MethodDecl &decl) override {}
            void visitPropertyDecl(PropertyDecl &decl) override {}
            void visitConstructorDecl(ConstructorDecl &decl) override {}
            void visitDestructorDecl(DestructorDecl &decl) override {}
            void visitEnumDecl(EnumDecl &decl) override {}
            void visitStructDecl(StructDecl &decl) override {}
            void visitTestDecl(TestDecl &decl) override {}
            void visitImportStmt(ImportStmt &stmt) override {}
            void visitFromImportStmt(FromImportStmt &stmt) override {}
            void visitExportStmt(ExportStmt &stmt) override {}
            void visitExportDecl(ExportDecl &decl) override {}
            void visitReExportStmt(ReExportStmt &stmt) override {}

            // Template/Generic visitors (Phase 7B) / زوار القوالب
            void visitTemplateFunctionDecl(TemplateFunctionDecl &decl) override {}
            void visitTemplateClassDecl(TemplateClassDecl &decl) override {}
            void visitTemplateInstantiation(TemplateInstantiation &expr) override {}
            void visitNamespaceDecl(NamespaceDecl &decl) override {}
            void visitOperatorDecl(OperatorDecl &decl) override {}

            // Trait/Interface visitors / زوار الواجهات والسمات
            void visitTraitDecl(TraitDecl &decl) override {}
            void visitImplDecl(ImplDecl &decl) override {}

            // Extension visitors / زوار الامتداد
            void visitExtensionDecl(ExtensionDecl &decl) override {}

            // Macro visitors / زوار الماكرو
            void visitMacroDecl(MacroDecl &decl) override {}
            void visitTypeAliasDecl(TypeAliasDecl &decl) override {}               ///< (AR) اسم مستعار للنوع / (EN) Type alias
            void visitTupleDestructureStmt(TupleDestructureStmt &stmt) override {} ///< (AR) تفكيك الصف / (EN) Tuple destructuring

            // UI Declarative visitors / زوار الواجهات التصريحية
            void visitUIDeclaration(UIDeclarationNode &decl) override {}   ///< (AR) تعريف مكون واجهة / (EN) UI component
            void visitUIWidgetExpr(UIWidgetExprNode &expr) override {}     ///< (AR) تعبير عنصر / (EN) Widget expression
            void visitUIModifier(UIModifierNode &expr) override {}         ///< (AR) معدّل سلسلي / (EN) Chained modifier
            void visitUIEventHandler(UIEventHandlerNode &expr) override {} ///< (AR) معالج حدث / (EN) Event handler
            void visitUIStateDecl(UIStateDecl &decl) override {}           ///< (AR) تصريح حالة / (EN) State declaration
            void visitUIConditional(UIConditionalNode &node) override {}   ///< (AR) رسم شرطي / (EN) Conditional rendering
            void visitUILoop(UILoopNode &node) override {}                 ///< (AR) حلقة رسم / (EN) Loop rendering
        };

    } // namespace AST
} // namespace Sad

#endif // SAD_AST_VISITOR_H
