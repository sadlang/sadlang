// ======================================================================
// class_nodes.h - عقد AST للبرمجة الكائنية / OOP AST Nodes
// ======================================================================
// الوصف بالعربية:
//   تعريف جميع عقد شجرة التحليل النحوي (AST) المتعلقة بالبرمجة الكائنية
//   يشمل: تعريف الأصناف، الخصائص، الطرق، البواني، الهدامات، والوراثة
//
// English Description:
//   Definition of all AST nodes related to Object-Oriented Programming
//   Includes: class declarations, fields, methods, constructors, destructors, and inheritance
//
// ملاحظات / Notes:
//   - جميع العقد ترث من ASTNode
//     All nodes inherit from ASTNode
//   - تدعم نمط الزائر (Visitor Pattern) عبر accept()
//     Supports Visitor Pattern through accept()
//   - توثيق ثنائي اللغة (عربي/إنجليزي) لكل صنف
//     Bilingual documentation (Arabic/English) for each class
//   - تدعم الكلمات البديلة (باني/منشئ، يرث/:)
//     Supports alternative keywords (باني/منشئ، يرث/:)
// ======================================================================

#pragma once

#include "statements.h"
#include "expressions.h"
#include "declarations.h"
#include "type.h"
#include <vector>
#include <memory>
#include <string>

namespace Sad
{
    namespace AST
    {

        // Forward declarations and type aliases
        using Expr = Expression;
        using Stmt = Statement;
        using Type = Data::Type;

        // ======================================================================
        // تعدادات وهياكل مساعدة / Enums and Helper Structures
        // ======================================================================

        /**
         * @enum Visibility
         * @brief (AR) رؤية الأعضاء (عام، خاص، محمي)
         * @brief (EN) Member visibility (public, private, protected)
         *
         * ملاحظات إضافية:
         * - AR: يحدد من يمكنه الوصول للعضو (خاصية أو طريقة)
         * - EN: Determines who can access the member (field or method)
         * - AR: PUBLIC = الكل، PRIVATE = الصنف فقط، PROTECTED = الصنف والوارثون
         * - EN: PUBLIC = everyone, PRIVATE = class only, PROTECTED = class and inheritors
         */
        enum class Visibility
        {
            PUBLIC,   ///< عام / public - accessible everywhere
            PRIVATE,  ///< خاص / private - accessible only within class
            PROTECTED ///< محمي / protected - accessible within class and subclasses
        };

        // ملاحظة: Parameter معرّف بالفعل في expressions.h
        // Note: Parameter is already defined in expressions.h

        /**
         * @struct FieldInitializer
         * @brief (AR) تهيئة خاصية في قائمة تهيئة الباني
         * @brief (EN) Field initialization in constructor initializer list
         *
         * مثال:
         * باني(رقم س) : الاسم("افتراضي")، العمر(س)
         * FieldInitializer: {fieldName="الاسم", value="افتراضي"}
         * FieldInitializer: {fieldName="العمر", value=س}
         */
        struct FieldInitializer
        {
            std::string fieldName;       ///< (AR) اسم الخاصية / (EN) field name
            std::unique_ptr<Expr> value; ///< (AR) القيمة / (EN) initialization value
        };

        // ======================================================================
        // ملاحظة: التعريفات التالية موجودة في declarations.h
        // Note: The following declarations are defined in declarations.h
        // - FieldDecl
        // - MethodDecl
        // - ConstructorDecl
        // - DestructorDecl
        // ======================================================================

        // ======================================================================
        // عقدة تعريف صنف / Class Declaration Node
        // ======================================================================

        /**
         * @class ClassDeclStmt
         * @brief (AR) تعريف صنف كامل
         * @brief (EN) Complete class declaration
         *
         * مثال:
         * صنف حيوان
         *     عام نص الاسم
         *
         *     باني(نص اسم)
         *         هذا.الاسم = اسم
         *     نهاية
         *
         *     عام دالة صوت()
         *         اطبع("صوت عام")
         *     نهاية
         * نهاية
         *
         * صنف كلب يرث حيوان
         *     عام دالة صوت()
         *         اطبع("نباح!")
         *     نهاية
         * نهاية
         *
         * ملاحظات إضافية:
         * - AR: يدعم الوراثة الأحادية في المرحلة 1
         * - EN: Supports single inheritance in Phase 1
         * - AR: يدعم كلا من ':' و 'يرث' للوراثة
         * - EN: Supports both ':' and 'يرث' for inheritance
         * - AR: يمكن أن يحتوي على خصائص، طرق، باني، وهدام
         * - EN: Can contain fields, methods, constructor, and destructor
         */
        class ClassDeclStmt : public Statement
        {
        public:
            std::string name;                                 ///< (AR) اسم الصنف / (EN) class name
            std::vector<std::string> baseClasses;             ///< (AR) الأصناف الأساسية / (EN) base classes
            std::vector<std::unique_ptr<FieldDecl>> fields;   ///< (AR) الخصائص / (EN) fields
            std::unique_ptr<ConstructorDecl> constructor;     ///< (AR) الباني / (EN) constructor
            std::unique_ptr<DestructorDecl> destructor;       ///< (AR) الهدام / (EN) destructor
            std::vector<std::unique_ptr<MethodDecl>> methods; ///< (AR) الطرق / (EN) methods

            /**
             * @brief (AR) منشئ مع اسم الصنف
             * @brief (EN) Constructor with class name
             *
             * @param n (std::string) — (AR) اسم الصنف / (EN) class name
             */
            explicit ClassDeclStmt(const std::string &n) : name(n) {}
            void accept(ASTVisitor &visitor) override;
            std::string toString() const override;
        };

        // ======================================================================
        // عقدة إنشاء كائن / Object Instantiation Node
        // ======================================================================

        /**
         * @class NewExpr
         * @brief (AR) تعبير إنشاء كائن جديد
         * @brief (EN) New object instantiation expression
         *
         * مثال:
         * شخص أحمد = جديد شخص("أحمد"، 25)
         *
         * ملاحظات إضافية:
         * - AR: يستدعي الباني للصنف
         * - EN: Calls the constructor of the class
         * - AR: يُرجع مرجع للكائن الجديد
         * - EN: Returns reference to new object
         */
        class NewExpr : public Expression
        {
        public:
            std::string className;                          ///< (AR) اسم الصنف / (EN) class name
            std::vector<std::unique_ptr<Expr>> arguments;   ///< (AR) معاملات الباني / (EN) constructor arguments
            std::vector<Types::SadTypeKind> templateArguments;  ///< (AR) أنواع القالب / (EN) template type arguments
            std::vector<std::string> templateArgumentNames; ///< (AR) أسماء أنواع القالب / (EN) template type argument names

            /**
             * @brief (AR) منشئ مع اسم الصنف
             * @brief (EN) Constructor with class name
             *
             * @param cls (std::string) — (AR) اسم الصنف / (EN) class name
             */
            explicit NewExpr(const std::string &cls) : className(cls) {}

            /**
             * @brief (AR) الحصول على نوع بيانات التعبير
             * @brief (EN) Get expression data type
             */
            Types::SadTypePtr getType() const override { return Types::SadTypeRegistry::instance().getAny(); }

            void accept(ASTVisitor &visitor) override;
            std::string toString() const override;
        };

        // ======================================================================
        // عقدة الوصول لعضو / Member Access Node
        // ======================================================================

        /**
         * @class MemberAccessExpr
         * @brief (AR) تعبير الوصول لخاصية في كائن
         * @brief (EN) Member access expression
         *
         * مثال:
         * أحمد.الاسم
         * كائن.خاصية
         *
         * ملاحظات إضافية:
         * - AR: يستخدم نقطة '.' للوصول
         * - EN: Uses dot '.' for access
         * - AR: يفحص الرؤية (عام، خاص، محمي)
         * - EN: Checks visibility (public, private, protected)
         */
        class MemberAccessExpr : public Expression
        {
        public:
            std::unique_ptr<Expr> object; ///< (AR) الكائن / (EN) object
            std::string memberName;       ///< (AR) اسم العضو / (EN) member name

            /**
             * @brief (AR) منشئ مع الكائن واسم العضو
             * @brief (EN) Constructor with object and member name
             *
             * @param obj (std::unique_ptr<Expr>) — (AR) الكائن / (EN) object
             * @param member (std::string) — (AR) اسم العضو / (EN) member name
             */
            MemberAccessExpr(std::unique_ptr<Expr> obj, const std::string &member)
                : object(std::move(obj)), memberName(member) {}

            Types::SadTypePtr getType() const override { return nullptr; }
            void accept(ASTVisitor &visitor) override;
            std::string toString() const override;
        };

        // ======================================================================
        // عقدة استدعاء طريقة / Method Call Node
        // ======================================================================

        /**
         * @class MethodCallExpr
         * @brief (AR) تعبير استدعاء طريقة على كائن
         * @brief (EN) Method call expression on object
         *
         * مثال:
         * أحمد.اطبع_معلومات()
         * كائن.طريقة(معامل1، معامل2)
         *
         * ملاحظات إضافية:
         * - AR: يستخدم نقطة '.' متبوعة باسم الطريقة وأقواس
         * - EN: Uses dot '.' followed by method name and parentheses
         * - AR: يفحص وجود الطريقة والرؤية
         * - EN: Checks method existence and visibility
         */
        class MethodCallExpr : public Expression
        {
        public:
            std::unique_ptr<Expr> object;                 ///< (AR) الكائن / (EN) object
            std::string methodName;                       ///< (AR) اسم الطريقة / (EN) method name
            std::vector<std::unique_ptr<Expr>> arguments; ///< (AR) المعاملات / (EN) arguments

            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 أنداءٌ **آمنٌ** هو (`س؟.م()`)؟ — والعَلَمُ هنا لا عقدةٌ ثانية.
            //
            //      وسببُ وجودِه أنّ `؟.` كانت في المحلّلِ **وصولَ عضوٍ فقط**: فرعُها
            //      لا يفحص `(` أصلًا، بينما فرعُ `.` يفحصه ويبني نداءَ طريقة. فكان
            //      `س؟.طول()` يُبنى وصولًا إلى عضوٍ اسمُه «طول» — ثمّ يُرفَع RUN033
            //      **على مستقبِلٍ حيٍّ غيرِ عدميّ**. أي أنّ مُشغِّلَ الأمانِ كان يرفض
            //      ما يقبله المُشغِّلُ غيرُ الآمن: `س.طول()` تعمل و`س؟.طول()` تُخفِق.
            //
            //      ⚠️ وعَلَمٌ لا عقدةٌ ثانيةٌ عمدًا: `OptionalChainExpr` نظيرٌ
            //      لـ`MemberExpr`، ولو صُنِعت لها نظيرةٌ ثالثةٌ لنداءِ الطريقةِ
            //      لَلَزِم كلَّ زائرٍ في المحرّكَين أن يعرفها، ومن ينساها يسقط إلى
            //      السلوكِ غيرِ الآمنِ **صامتًا**. والعَلَمُ يجعل الناسيَ يُنتِج
            //      الدلالةَ القديمةَ في موضعٍ واحدٍ معروفٍ يُقاس.
            // (EN) Is this a SAFE call (`x?.m()`)? A flag, not a second node.
            //      `؟.` was member-access-only in the parser: its branch never checked
            //      for `(` while the `.` branch did, so `س؟.طول()` became a member
            //      access named «طول» and raised RUN033 on a LIVE receiver — the safe
            //      operator rejecting what the unsafe one accepts. A flag rather than a
            //      third node type: a new node would have to be taught to every visitor
            //      in both engines, and whoever forgot it would silently fall back to
            //      unsafe behaviour.
            // ════════════════════════════════════════════════════════════════
            bool isOptional = false;

            /**
             * @brief (AR) منشئ مع الكائن واسم الطريقة
             * @brief (EN) Constructor with object and method name
             *
             * @param obj (std::unique_ptr<Expr>) — (AR) الكائن / (EN) object
             * @param method (std::string) — (AR) اسم الطريقة / (EN) method name
             */
            MethodCallExpr(std::unique_ptr<Expr> obj, const std::string &method)
                : object(std::move(obj)), methodName(method) {}

            Types::SadTypePtr getType() const override { return nullptr; }
            void accept(ASTVisitor &visitor) override;
            std::string toString() const override;
        };

        // ======================================================================
        // عقدة this (هذا) / This Expression Node
        // ======================================================================

        /**
         * @class ThisExpr
         * @brief (AR) تعبير 'هذا' - يشير للكائن الحالي
         * @brief (EN) 'This' expression - refers to current object
         *
         * مثال:
         * صنف مثال
         *     عام دالة استخدم_هذا()
         *         هذا.خاصية = قيمة
         *     نهاية
         * نهاية
         *
         * ملاحظات إضافية:
         * - AR: يمكن استخدامها فقط داخل طرق الصنف
         * - EN: Can only be used inside class methods
         * - AR: تشير للكائن الذي استُدعيت عليه الطريقة
         * - EN: Refers to object on which method was called
         */
        class ThisExpr : public Expression
        {
        public:
            /**
             * @brief (AR) منشئ افتراضي
             * @brief (EN) Default constructor
             */
            ThisExpr() = default;

            Types::SadTypePtr getType() const override { return Types::SadTypeRegistry::instance().getAny(); }
            void accept(ASTVisitor &visitor) override;
            std::string toString() const override;
        };

        // ======================================================================
        // عقدة super (الأساس) / Super Expression Node
        // ======================================================================

        /**
         * @class SuperExpr
         * @brief (AR) تعبير 'الأساس' - يشير للصنف الأساسي
         * @brief (EN) 'Super' expression - refers to base class
         *
         * مثال:
         * صنف كلب يرث حيوان
         *     عام دالة صوت()
         *         الأساس.صوت()  // استدعاء طريقة الصنف الأساسي
         *         اطبع("نباح!")
         *     نهاية
         * نهاية
         *
         * ملاحظات إضافية:
         * - AR: يُستخدم لاستدعاء طرق الصنف الأساسي
         * - EN: Used to call base class methods
         * - AR: يمكن استخدامها فقط في صنف يرث من آخر
         * - EN: Can only be used in class that inherits from another
         */
        class SuperExpr : public Expression
        {
        public:
            std::string memberName; ///< (AR) اسم العضو في الصنف الأساسي / (EN) member name in base class

            /**
             * @brief (AR) منشئ افتراضي
             * @brief (EN) Default constructor
             */
            SuperExpr() = default;

            /**
             * @brief (AR) منشئ مع اسم العضو
             * @brief (EN) Constructor with member name
             *
             * @param member (std::string) — (AR) اسم العضو / (EN) member name
             */
            explicit SuperExpr(const std::string &member) : memberName(member) {}

            Types::SadTypePtr getType() const override { return Types::SadTypeRegistry::instance().getAny(); }
            void accept(ASTVisitor &visitor) override;
            std::string toString() const override;
        };

    } // namespace AST
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
