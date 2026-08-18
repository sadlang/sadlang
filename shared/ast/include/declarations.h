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
#include "module_nodes.h"
#include <vector>
#include <string>

namespace Sad
{
    namespace AST
    {

        // =========================================================================
        // قيود الأنواع (Type Constraints) / Where Clause
        // =========================================================================

        /// (AR) قيد على نوع معمّم: ت: سمة1 + سمة2
        /// (EN) Constraint on a generic type parameter: T: Trait1 + Trait2
        struct WhereConstraint
        {
            std::string paramName;           ///< اسم المعامل (ت) / Parameter name (T)
            std::vector<std::string> traits; ///< أسماء السمات المطلوبة / Required trait names
        };

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
        class FunctionDecl : public Statement
        {
        public:
            std::string name;                  ///< Function name / اسم الدالة
            std::vector<Parameter> parameters; ///< Parameters / المعاملات
            Types::SadTypeKind returnType;         ///< Return type (legacy) / نوع الإرجاع (قديم)
            // (AR) [Phase 5e] اسم نوع الإرجاع للأصناف المُعرَّفة من المستخدم.
            // (EN) [Phase 5e] Return type name for user-defined classes.
            std::string returnTypeName;
            Types::SadTypePtr sadReturnType;         ///< (AR) نوع الإرجاع الموحد / (EN) Unified return type
            StmtPtr body;                            ///< Function body / جسم الدالة
            bool isExported;                         ///< Is exported? / مصدّر؟
            bool isMainFunction;                     ///< Is main function? / هل هي الدالة الرئيسية؟
            bool is_async;                           ///< Is async function? / دالة غير متزامنة؟
            bool isGenerator;                        ///< Is generator function? / دالة مولد؟ (Phase 7)
            bool isExtern;                           ///< Is external function? / دالة خارجية؟ (FFI)
            bool isNoReturn = false;                 ///< (AR) دالة لا_ترجع — لا تعود أبداً / (EN) noreturn modifier → LLVM NoReturn
            std::string linkName;                    ///< FFI link name (empty = use function name) / اسم الربط الخارجي
            ExprList decorators;                     ///< Decorators (@decorator) / المُزخرِفات
            std::vector<std::string> lifetimeParams; ///< Lifetime parameters <'أ, 'ب> / معاملات العمر

            /// (AR) سمات الدالة [[سمة]] — تُترجم إلى LLVM function attributes
            ///      الأسماء المدعومة: مضمن_دائماً، لا_تضمن، بارد، ساخن، لا_تعرّج، محاذاة(N)
            /// (EN) Function attributes [[attr]] — lowered to LLVM function attributes
            ///      Supported: مضمن_دائماً(AlwaysInline), لا_تضمن(NoInline),
            ///      بارد(Cold), ساخن(Hot), لا_تعرّج(NoAlias), محاذاة(N)(Alignment)
            std::vector<std::string> attributes;

            // ═══ العقود البرمجية (Design by Contract) ═══
            /// (AR) شروط مسبقة: يتطلب تعبير — تُفحص قبل تنفيذ الدالة
            /// (EN) Preconditions: requires expression — checked before function execution
            ExprList preconditions;
            /// (AR) شروط لاحقة: يضمن تعبير — تُفحص بعد تنفيذ الدالة
            /// (EN) Postconditions: ensures expression — checked after function execution
            ExprList postconditions;
            /// (AR) قيود الأنواع: حيث ت: سمة / (EN) Type constraints: where T: Trait
            std::vector<WhereConstraint> whereConstraints;

            /**
             * @brief Constructor without decorators / البناء بدون مُزخرِفات
             * @param name Function name / اسم الدالة
             * @param params Parameter list / قائمة المعاملات
             * @param retType Return type / نوع الإرجاع
             * @param body Function body / جسم الدالة
             * @param exported Is exported / مصدّر
             * @param async_func Is async function / دالة async
             * @param generator Is generator function / دالة مولد
             * @param pos Source position / الموقع في الكود
             */
            FunctionDecl(const std::string &name, std::vector<Parameter> params,
                         Types::SadTypeKind retType, StmtPtr body, bool exported = false,
                         bool async_func = false, bool generator = false,
                         const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), parameters(std::move(params)),
                  returnType(retType), body(std::move(body)), isExported(exported),
                  isMainFunction(false), is_async(async_func), isGenerator(generator), isExtern(false), linkName(), decorators(),
                  sadReturnType(Types::SadType::fromValueType(retType)) {}

            /**
             * @brief Constructor with decorators / البناء مع مُزخرِفات
             * @param name Function name / اسم الدالة
             * @param params Parameter list / قائمة المعاملات
             * @param retType Return type / نوع الإرجاع
             * @param body Function body / جسم الدالة
             * @param decs Decorator list / قائمة المُزخرِفات
             * @param exported Is exported / مصدّر
             * @param async_func Is async function / دالة async
             * @param generator Is generator function / دالة مولد
             * @param pos Source position / الموقع في الكود
             */
            FunctionDecl(const std::string &name, std::vector<Parameter> params,
                         Types::SadTypeKind retType, StmtPtr body, ExprList decs,
                         bool exported = false, bool async_func = false, bool generator = false,
                         const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), parameters(std::move(params)),
                  returnType(retType), body(std::move(body)), isExported(exported),
                  isMainFunction(false), is_async(async_func), isGenerator(generator), isExtern(false), linkName(), decorators(std::move(decs)),
                  sadReturnType(Types::SadType::fromValueType(retType)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitFunctionDecl(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // Class Declaration / تصريح الصنف
        // =========================================================================

        // (AR) AccessModifier انتقل إلى ast_node.h ليراه VarDeclStmt أيضًا (ISSUE-120).
        //      لا اسم مستعار هنا: النقل أبقى الاسم والفضاء كما هما، فمستعملوه لم يتغيّروا.
        // (EN) AccessModifier moved to ast_node.h so VarDeclStmt can use it too.

        /**
         * @brief Class declaration node / عقدة تصريح الصنف
         *
         * Represents a class declaration with optional multiple inheritance.
         * يمثل تصريح صنف مع إمكانية الوراثة المتعددة.
         *
         * @example Examples / أمثلة:
         * - class Person { ... }
         * - صنف شخص { ... }
         * - class Student extends Person { ... }
         * - class Assistant extends Teacher, Employee { ... }
         * - صنف مساعد يرث معلم، موظف { ... }
         *
         * @note (AR) دعم الوراثة المتعددة: يمكن للصنف أن يرث من أكثر من صنف أب
         *       (EN) Multiple inheritance support: A class can inherit from multiple base classes
         */
        class ClassDecl : public Statement
        {
        public:
            std::string name;                      ///< Class name / اسم الصنف
            std::vector<std::string> superclasses; ///< Base class names (multiple inheritance) / أسماء الأصناف الأساسية
            StmtList members;                      ///< Class members / أعضاء الصنف
            bool isExported;                       ///< Is exported? / مصدّر؟
            bool isAbstract;                       ///< Is abstract class? / صنف مجرد؟
            bool isSealed = false;                 ///< Is sealed class? / صنف محكم؟ (الوراثة مسموحة فقط في نفس الملف)
            std::string sourceFile;                ///< Source file path / مسار الملف المصدري (لفحص الوراثة من صنف محكم)
            bool isContract = false;               ///< Is smart contract? / عقد ذكي؟
            ExprList decorators;                   ///< Decorators (@decorator) / المُزخرِفات

            /**
             * @brief Constructor with multiple base classes / البناء مع أصناف أساسية متعددة
             * @param name Class name / اسم الصنف
             * @param bases Base class names / أسماء الأصناف الأساسية
             * @param members Member list / قائمة الأعضاء
             * @param exported Is exported / مصدّر
             * @param pos Source position / الموقع في الكود
             */
            ClassDecl(const std::string &name, const std::vector<std::string> &bases,
                      StmtList members, bool exported = false,
                      const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), superclasses(bases),
                  members(std::move(members)), isExported(exported), isAbstract(false) {}

            /**
             * @brief Constructor with single base class (backward compatibility) / البناء مع صنف أساسي واحد
             * @param name Class name / اسم الصنف
             * @param superclass Single base class name / اسم الصنف الأساسي الواحد
             * @param members Member list / قائمة الأعضاء
             * @param exported Is exported / مصدّر
             * @param pos Source position / الموقع في الكود
             */
            ClassDecl(const std::string &name, const std::string &superclass,
                      StmtList members, bool exported = false,
                      const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name),
                  superclasses(superclass.empty() ? std::vector<std::string>() : std::vector<std::string>{superclass}),
                  members(std::move(members)), isExported(exported), isAbstract(false) {}

            void accept(ASTVisitor &visitor) override
            {
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
        class FieldDecl : public Statement
        {
        public:
            std::string name;      ///< Field name / اسم الحقل
            Types::SadTypeKind type;   ///< Field type / نوع الحقل
            ExprPtr initializer;   ///< Initial value (optional) / القيمة الأولية
            AccessModifier access; ///< Access modifier / معدّل الوصول
            bool isStatic;         ///< Is static? / ثابت؟

            // ════════════════════════════════════════════════════════════════
            // (AR) [م‑ز] النوعُ الغنيُّ — نظيرُ `VarDeclStmt::sadType` في **الغرضِ**
            //      لا في السلوك: ذاك يُهيَّأ في الباني فلا يكون عدمًا قطّ، وهذا
            //      **لا يُبنى إلّا للحقلِ العدميّ** ويبقى عدمًا لغيرِه — فالقارئُ
            //      يجب أن يفحصَه. ووُصِف أوّلًا بـ«حرفًا بحرف» وهو خطأ: التعليقُ
            //      في هذا المستودعِ سِجِلٌّ يُبنى عليه لا زينة.
            //
            //      🔑 وسببُ لزومِه أنّ `type` **صنفٌ لا نوع**: `SadTypeKind` وحدَه
            //      يقول «عدميّ» ولا يقول «عدميُّ ماذا». والخانةُ التي تُبنى للحقلِ
            //      تختلف باختلافِ الداخل: `منطقي؟` تُوسَم خارجَ النطاقِ و`رقم؟`
            //      تبقى داخلَه (م‑هـ). فبلا الداخلِ لا يمكن أن يُنادى قرارُ الخانةِ
            //      أصلًا، ويصير كلُّ عدميٍّ خانةً واحدةً — وهو عينُ الخلطِ الذي
            //      تُصلحه هذه الحملة.
            //
            //      ⚠️ ولا يُفتَح للحقلِ بابُ قرارٍ ثانٍ: المُقرِّرُ هو
            //      `SIRBuilder::resolveDeclaredStorageKind` نفسُه الذي يقرّر
            //      للمتغيّرِ والمعامِلِ والإرجاعِ والعامّ. وسلطتانِ تختلفان في نوعٍ
            //      واحدٍ تُعيدان التباعُدَ الذي وُضِعت السلطةُ الواحدةُ لسدِّه.
            // (EN) [م‑ز] The rich type — the exact twin of VarDeclStmt::sadType.
            //      `type` alone says "nullable" but not "nullable of WHAT", and the
            //      slot built for the field depends on the inner kind (`bool?` is
            //      tagged out-of-band, `int?` is not yet). Without the inner type the
            //      storage decision cannot even be asked. The decider stays the SAME
            //      resolveDeclaredStorageKind used by variables, parameters, returns
            //      and globals — a second authority would reinstate the divergence.
            // ════════════════════════════════════════════════════════════════
            Types::SadTypePtr sadType; ///< (AR) Optional<T> للحقلِ العدميِّ وحدَه / (EN) Optional<T>, nullable fields only

            /**
             * @brief Constructor / البناء
             */
            FieldDecl(const std::string &name, Types::SadTypeKind type, ExprPtr init,
                      AccessModifier access = AccessModifier::PUBLIC,
                      bool isStatic = false,
                      const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), type(type), initializer(std::move(init)),
                  access(access), isStatic(isStatic) {}

            void accept(ASTVisitor &visitor) override
            {
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
        class MethodDecl : public Statement
        {
        public:
            std::string name;                  ///< Method name / اسم الطريقة
            std::vector<Parameter> parameters; ///< Parameters / المعاملات
            Types::SadTypeKind returnType;         ///< Return type / نوع الإرجاع
            // (AR) [Phase 5e] اسم نوع الإرجاع للأصناف المُعرَّفة من المستخدم.
            // (EN) [Phase 5e] Return type name for user-defined classes.
            std::string returnTypeName;
            StmtPtr body;          ///< Method body / جسم الطريقة
            AccessModifier access; ///< Access modifier / معدّل الوصول
            bool isStatic;         ///< Is static? / ثابت؟
            bool isVirtual;        ///< Is virtual? / افتراضي؟
            bool isOverride;       ///< Is override? / تجاوز؟
            bool isAbstract;       ///< Is abstract? / مجرد؟
            bool isAsync;          ///< Is async? / غير متزامن؟

            // ═══ العقود البرمجية (Design by Contract) ═══
            /// (AR) شروط مسبقة: يتطلب تعبير — تُفحص قبل تنفيذ الطريقة
            /// (EN) Preconditions: requires expression — checked before method execution
            ExprList preconditions;
            /// (AR) شروط لاحقة: يضمن تعبير — تُفحص بعد تنفيذ الطريقة
            /// (EN) Postconditions: ensures expression — checked after method execution
            ExprList postconditions;

            /**
             * @brief Constructor / البناء
             */
            MethodDecl(const std::string &name, std::vector<Parameter> params,
                       Types::SadTypeKind retType, StmtPtr body,
                       AccessModifier access = AccessModifier::PUBLIC,
                       bool isStatic = false, bool isVirtual = false,
                       bool isOverride = false, bool isAbstract = false,
                       const Lexer::Position &pos = Lexer::Position(),
                       bool isAsync = false)
                : Statement(pos), name(name), parameters(std::move(params)),
                  returnType(retType), body(std::move(body)), access(access),
                  isStatic(isStatic), isVirtual(isVirtual), isOverride(isOverride),
                  isAbstract(isAbstract), isAsync(isAsync) {}

            void accept(ASTVisitor &visitor) override
            {
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
        class ConstructorDecl : public Statement
        {
        public:
            std::vector<Parameter> parameters; ///< Parameters / المعاملات
            StmtPtr body;                      ///< Constructor body / جسم الباني
            ExprList superArgs;                ///< Super constructor args / معاملات الباني الأب

            // (AR) هل وردَ نداءُ الأساسِ نصًّا؟ قائمةُ الوسائطِ وحدَها لا تُميّزُ «لا نداءَ
            //      للأساس» من «الأساس() بلا وسائط»: كلتاهما قائمةٌ فارغة. وبغيرِ هذا التمييزِ
            //      يُلغى النداءُ كلُّه بدلَ أن تُبطَّنَ خاناتُه عدمًا، فلا يُنفَّذُ بانِي الأبِ
            //      أصلًا — خلافًا للمادّةِ (٣) بمنطوقِها، وفي المحرّكَينِ معًا.
            // (EN) Was a base call written in the source? The argument list alone cannot separate
            //      "no base call" from "base() with no arguments": both are an empty list. Without
            //      this distinction the whole call is dropped instead of having its slots padded
            //      with null, so the parent constructor never runs at all — contrary to Article (3)
            //      as written, and in both engines.
            bool hasBaseCall = false;          ///< Base call written? / نداءُ الأساسِ مكتوب؟

            /**
             * @brief Constructor / البناء
             */
            ConstructorDecl(std::vector<Parameter> params, StmtPtr body,
                            ExprList superArgs = ExprList(),
                            const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), parameters(std::move(params)), body(std::move(body)),
                  superArgs(std::move(superArgs)) {}

            void accept(ASTVisitor &visitor) override
            {
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
        class DestructorDecl : public Statement
        {
        public:
            StmtPtr body; ///< Destructor body / جسم الهادم

            /**
             * @brief Constructor / البناء
             */
            DestructorDecl(StmtPtr body, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), body(std::move(body)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitDestructorDecl(*this);
            }

            std::string toString() const override
            {
                return "destructor { ... }";
            }
        };

        // =========================================================================
        // Enum Declaration / تصريح التعداد
        // =========================================================================

        /**
         * @brief (AR) عضو التعداد — يدعم التعدادات البسيطة والجبرية (ADT)
         *        (EN) Enum member — supports simple enums and Algebraic Data Types (ADT)
         *
         * @details
         * (AR) أنواع الأعضاء:
         *      - وحدة (Unit): بدون بيانات — مثل: نقطة
         *      - صف (Tuple): مع بيانات مرتبة — مثل: دائرة(نصف_القطر)
         *
         * (EN) Variant kinds:
         *      - Unit: no data — e.g., نقطة
         *      - Tuple: positional data — e.g., دائرة(نصف_القطر)
         *
         * @example
         *   تعداد شكل
         *       دائرة(نصف_القطر)           # Tuple variant
         *       مستطيل(عرض، ارتفاع)        # Tuple variant
         *       نقطة                        # Unit variant
         *   نهاية
         */
        struct EnumMember
        {
            std::string name;                ///< (AR) اسم العضو / (EN) Member name
            ExprPtr value;                   ///< (AR) قيمة العضو (اختيارية) / (EN) Member value (optional)
            std::vector<std::string> fields; ///< (AR) أسماء حقول الحمولة (ADT) / (EN) Payload field names (ADT)
            std::vector<std::string> fieldTypes; ///< (AR) أنواع حقول الحمولة — موازٍ لـ fields ("" = غير مُصنَّف/ديناميكيّ) / (EN) Payload field types — parallel to fields ("" = untyped)

            /**
             * @brief (AR) باني عضو تعداد بسيط (بدون بيانات أو مع قيمة فقط)
             *        (EN) Constructor for simple enum member (no data or value only)
             */
            EnumMember(const std::string &n, ExprPtr v = nullptr)
                : name(n), value(std::move(v)) {}

            /**
             * @brief (AR) باني عضو تعداد جبري (ADT) — مع حمولة موضعيّة غير مُصنَّفة
             *        (EN) Constructor for ADT enum member — untyped positional payload
             * @param n اسم العضو / Member name
             * @param fieldNames أسماء الحقول / Field names
             */
            EnumMember(const std::string &n, std::vector<std::string> fieldNames)
                : name(n), value(nullptr), fields(std::move(fieldNames)),
                  fieldTypes(this->fields.size()) {}

            /**
             * @brief (AR) باني عضو تعداد جبري (ADT) — مع حمولة موضعيّة مُصنَّفة (النوع قبل الاسم)
             *        (EN) Constructor for ADT enum member — typed positional payload (type before name)
             * @param n اسم العضو / Member name
             * @param fieldNames أسماء الحقول / Field names
             * @param fieldTypeNames أنواع الحقول الموازية / Parallel field type names
             */
            EnumMember(const std::string &n, std::vector<std::string> fieldNames,
                       std::vector<std::string> fieldTypeNames)
                : name(n), value(nullptr), fields(std::move(fieldNames)),
                  fieldTypes(std::move(fieldTypeNames)) {}

            /**
             * @brief (AR) هل هذا عضو يحمل بيانات (variant مع حقول)؟
             *        (EN) Does this member carry data (variant with fields)?
             */
            bool hasData() const { return !fields.empty(); }

            // Copy constructor deleted (contains unique_ptr)
            EnumMember(const EnumMember &) = delete;
            EnumMember &operator=(const EnumMember &) = delete;

            // Move constructor and assignment
            EnumMember(EnumMember &&) = default;
            EnumMember &operator=(EnumMember &&) = default;
        };

        /**
         * @brief (AR) عقدة تصريح التعداد — يدعم التعدادات البسيطة والجبرية (ADT)
         *        (EN) Enum declaration node — supports simple enums and ADT
         *
         * @example أمثلة / Examples:
         *   تعداد اللون           # تعداد بسيط
         *       أحمر، أخضر، أزرق
         *   نهاية
         *
         *   تعداد شكل             # تعداد جبري (ADT)
         *       دائرة(نصف_القطر)
         *       مستطيل(عرض، ارتفاع)
         *       نقطة
         *   نهاية
         * - enum Status { OK = 0, ERROR = 1 }
         */
        class EnumDecl : public Statement
        {
        public:
            std::string name;                ///< (AR) اسم التعداد / (EN) Enum name
            std::vector<EnumMember> members; ///< (AR) أعضاء التعداد / (EN) Enum members
            bool isExported;                 ///< (AR) مصدّر؟ / (EN) Is exported?

            /**
             * @brief Constructor / البناء
             */
            EnumDecl(const std::string &name, std::vector<EnumMember> members,
                     bool exported = false,
                     const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), members(std::move(members)),
                  isExported(exported) {}

            /**
             * @brief (AR) هل يحتوي التعداد على أعضاء جبرية (ADT) مع بيانات؟
             *        (EN) Does the enum contain ADT members with data?
             */
            bool isADT() const
            {
                for (const auto &m : members)
                {
                    if (m.hasData())
                        return true;
                }
                return false;
            }

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitEnumDecl(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // (AR) نظام الوحدات - الاستيراد والتصدير / (EN) Module System
        // =========================================================================

        /**
         * @note (AR) تم نقل عُقد نظام الوحدات إلى ملف منفصل: module_nodes.h
         *       (EN) Module system nodes moved to separate file: module_nodes.h
         *
         * @details
         * (AR) الأصناف الجديدة في module_nodes.h:
         *      - ImportStmt: استيراد كامل (استورد وحدة [كـ اسم])
         *      - FromImportStmt: استيراد انتقائي (من وحدة استورد رمز)
         *      - ExportDecl: تصدير (صدّر تصريح)
         *      - ImportItem: عنصر مستورد مع اسم مستعار
         *
         * (EN) New classes in module_nodes.h:
         *      - ImportStmt: Full import
         *      - FromImportStmt: Selective import
         *      - ExportDecl: Export declaration
         *      - ImportItem: Imported item with alias
         *
         * @see module_nodes.h
         */

        // (AR) ExportStmt القديم محفوظ للتوافق / (EN) Legacy ExportStmt for compatibility
        class ExportStmt : public Statement
        {
        public:
            StmtPtr declaration;

            ExportStmt(StmtPtr decl, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), declaration(std::move(decl)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitExportStmt(*this);
            }

            std::string toString() const override
            {
                return "export " + (declaration ? declaration->toString() : "");
            }
        };

        // =========================================================================
        // Template Type Parameter / معامل نوع القالب
        // =========================================================================

        /**
         * @brief Template type parameter / معامل نوع القالب
         *
         * Represents a type parameter in template declarations.
         * يمثل معامل نوع في تصريحات القوالب.
         *
         * @example Examples / أمثلة:
         * - T (simple type parameter)
         * - T: Comparable (constrained type parameter)
         * - T: Comparable + Hashable (multiple constraints)
         * - نوع ت (معامل نوع بسيط)
         * - ت: قابل_للمقارنة (معامل نوع مقيد)
         * - ت: قابل_للمقارنة + قابل_للتجزئة (قيود متعددة)
         */
        struct TypeParameter
        {
            std::string name;                     ///< Parameter name / اسم المعامل (e.g., "T", "ت")
            std::string constraint;               ///< First constraint (backward compat) / القيد الأول
            std::vector<std::string> constraints; ///< All constraints / جميع القيود (e.g., ["قابل_للمقارنة", "قابل_للتجزئة"])
            ExprPtr defaultType;                  ///< Default type (optional) / النوع الافتراضي

            // ==========================================================================
            // (AR) [Phase 4 — Monomorphization] دعم const-generics:
            //      معامل القالب يمكن أن يكون قيمة ثابتة بدلاً من نوع.
            //      مثال: قالب<نوع T، ثابت رقم N> دالة... (N معامل ثابت من نوع رقم)
            //      عند instantiation: f<عشري، 4>(...) — 4 قيمة ثابتة تُستبدل في جسم القالب
            // (EN) [Phase 4 — Monomorphization] const-generics support:
            //      A template parameter may be a compile-time constant value instead of a type.
            //      Example: template<typename T, const int N> func... (N is a const param of type int)
            //      At instantiation: f<float, 4>(...) — 4 is a literal substituted into template body
            // ==========================================================================
            bool isConst = false;      ///< (AR) معامل ثابت بدلاً من نوع / (EN) const-generic param
            std::string constTypeName; ///< (AR) اسم نوع الثابت (رقم/عشري/نص/منطقي) / (EN) const param type name
            ExprPtr defaultConstValue; ///< (AR) قيمة ثابتة افتراضية / (EN) optional default constant value

            TypeParameter(const std::string &n, const std::string &c = "",
                          ExprPtr def = nullptr)
                : name(n), constraint(c), defaultType(std::move(def))
            {
                if (!c.empty())
                    constraints.push_back(c);
            }

            TypeParameter(const std::string &n, const std::vector<std::string> &cs,
                          ExprPtr def = nullptr)
                : name(n), constraints(cs), defaultType(std::move(def))
            {
                if (!cs.empty())
                    constraint = cs[0];
            }

            // (AR) باني خاص بـ const-generic
            // (EN) Constructor for const-generic parameter
            static TypeParameter makeConst(const std::string &name,
                                           const std::string &typeName,
                                           ExprPtr defaultValue = nullptr)
            {
                TypeParameter p(name, std::string());
                p.isConst = true;
                p.constTypeName = typeName;
                p.defaultConstValue = std::move(defaultValue);
                return p;
            }

            // Move operations
            TypeParameter(TypeParameter &&) = default;
            TypeParameter &operator=(TypeParameter &&) = default;

            // Copy constructor
            TypeParameter(const TypeParameter &other)
                : name(other.name), constraint(other.constraint),
                  constraints(other.constraints), defaultType(nullptr),
                  isConst(other.isConst), constTypeName(other.constTypeName),
                  defaultConstValue(nullptr) {}
        };

        // =========================================================================
        // قيود where clause / Where Clause Types
        // =========================================================================

        /**
         * @brief عنصر قيد في جملة حيث / Single constraint item in where clause
         *
         * @example Examples / أمثلة:
         * - حيث ت: قابل_للمقارنة + قابل_للتجزئة
         * - حيث ت.عنصر: قابل_للطباعة       (Associated type constraint)
         * - حيث ت: قابل_للتحويل<م>         (Conditional constraint)
         */
        struct WhereConstraintItem
        {
            std::string typeName;                 ///< Type parameter name / اسم المعامل (e.g., "ت")
            std::string associatedPath;           ///< Associated type path / مسار نوع مرتبط (e.g., "عنصر") — optional
            std::vector<std::string> constraints; ///< Trait/type constraints / قيود السمات

            WhereConstraintItem() = default;
            WhereConstraintItem(const std::string &t, const std::vector<std::string> &cs)
                : typeName(t), constraints(cs) {}
            WhereConstraintItem(const std::string &t, const std::string &assocPath, const std::vector<std::string> &cs)
                : typeName(t), associatedPath(assocPath), constraints(cs) {}
        };

        /**
         * @brief جملة حيث / Where clause
         *
         * @example Examples / أمثلة:
         * - حيث ت: قابل_للمقارنة + قابل_للتجزئة، م: قابل_للطباعة
         * - حيث ت.عنصر == رقم
         */
        struct WhereClause
        {
            std::vector<WhereConstraintItem> items;
            bool empty() const { return items.empty(); }
        };

        /**
         * @brief نوع مرتبط / Associated type declaration
         *
         * @example Examples / أمثلة:
         * - سمة محتوى { نوع عنصر نهاية }   → AssociatedTypeDecl{"عنصر", {}}
         * - نوع عنصر: قابل_للمقارنة         → AssociatedTypeDecl{"عنصر", ["قابل_للمقارنة"]}
         */
        struct AssociatedTypeDecl
        {
            std::string name;                     ///< Associated type name / اسم النوع المرتبط
            std::vector<std::string> constraints; ///< Optional constraints / قيود اختيارية
            std::string defaultType;              ///< Default concrete type / نوع افتراضي

            AssociatedTypeDecl() = default;
            AssociatedTypeDecl(const std::string &n, const std::vector<std::string> &cs = {},
                               const std::string &def = "")
                : name(n), constraints(cs), defaultType(def) {}
        };

        // =========================================================================
        // Template Function Declaration / تصريح دالة قالب
        // =========================================================================

        /**
         * @brief Template function declaration / تصريح دالة قالب
         *
         * Represents a generic/template function declaration.
         * يمثل تصريح دالة عامة (قالب).
         *
         * @example Examples / أمثلة:
         * - قالب<نوع ت> دالة أكبر(ت أ، ت ب) ت { ... }
         * - template<typename T> func max(a: T, b: T) -> T { ... }
         * - قالب<نوع ت، نوع م> دالة تبديل(إشارة ت أ، إشارة م ب) { ... }
         */
        class TemplateFunctionDecl : public Statement
        {
        public:
            std::vector<TypeParameter> typeParameters; ///< Type parameters / معاملات الأنواع
            WhereClause whereClause;                   ///< Where clause / جملة حيث (اختيارية)
            std::string name;                          ///< Function name / اسم الدالة
            std::vector<Parameter> parameters;         ///< Function parameters / معاملات الدالة
            Types::SadTypeKind returnType;                 ///< Return type / نوع الإرجاع
            StmtPtr body;                              ///< Function body / جسم الدالة
            bool isExported;                           ///< Is exported? / مصدّر؟

            /**
             * @brief Constructor / البناء
             */
            TemplateFunctionDecl(std::vector<TypeParameter> typeParams,
                                 const std::string &name,
                                 std::vector<Parameter> params,
                                 Types::SadTypeKind retType,
                                 StmtPtr body,
                                 bool exported = false,
                                 const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), typeParameters(std::move(typeParams)),
                  name(name), parameters(std::move(params)),
                  returnType(retType), body(std::move(body)), isExported(exported) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitTemplateFunctionDecl(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // Template Class Declaration / تصريح صنف قالب
        // =========================================================================

        /**
         * @brief Template class declaration / تصريح صنف قالب
         *
         * Represents a generic/template class declaration.
         * يمثل تصريح صنف عام (قالب).
         *
         * @example Examples / أمثلة:
         * - قالب<نوع ت> صنف صندوق { ... نهاية }
         * - template<typename T> class Box { ... }
         * - قالب<نوع ك، نوع ق> صنف قاموس { ... نهاية }
         */
        class TemplateClassDecl : public Statement
        {
        public:
            std::vector<TypeParameter> typeParameters; ///< Type parameters / معاملات الأنواع
            WhereClause whereClause;                   ///< Where clause / جملة حيث (اختيارية)
            std::string name;                          ///< Class name / اسم الصنف
            std::vector<std::string> superclasses;     ///< Base class names / أسماء الأصناف الأساسية
            StmtList members;                          ///< Class members / أعضاء الصنف
            bool isExported;                           ///< Is exported? / مصدّر؟

            /**
             * @brief Constructor / البناء
             */
            TemplateClassDecl(std::vector<TypeParameter> typeParams,
                              const std::string &name,
                              const std::vector<std::string> &bases,
                              StmtList members,
                              bool exported = false,
                              const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), typeParameters(std::move(typeParams)),
                  name(name), superclasses(bases),
                  members(std::move(members)), isExported(exported) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitTemplateClassDecl(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // Template Instantiation Expression / تعبير تنفيذ القالب
        // =========================================================================

        /**
         * @brief Template instantiation expression / تعبير تنفيذ القالب
         *
         * Represents instantiation of a template with concrete types.
         * يمثل تنفيذ قالب بأنواع محددة.
         *
         * @example Examples / أمثلة:
         * - صندوق<رقم> ص = جديد صندوق<رقم>(42)
         * - أكبر<نص>(أ، ب)
         * - Box<int> b = new Box<int>(42)
         * - max<string>(a, b)
         */
        class TemplateInstantiation : public Expression
        {
        public:
            std::string templateName;                   ///< Template name / اسم القالب
            std::vector<Types::SadTypeKind> typeArguments;  ///< Type arguments / وسائط الأنواع
            std::vector<std::string> typeArgumentNames; ///< Type argument names / أسماء وسائط الأنواع (e.g., "رقم", "مركبة")

            // ==========================================================================
            // (AR) [Phase 4 — Monomorphization] دعم const-generics:
            //      وسائط ثابتة موازية للوسائط النوعية. كل عنصر يقابل معامل
            //      isConst في تعريف القالب. نُخزّنها كتعابير لتُقيَّم وقت
            //      التخصيص (instantiation) ثم تُستبدل بقيم حرفية في جسم القالب.
            //      مثال: f<عشري، 4> → typeArguments=[Float], constArguments=[4]
            // (EN) [Phase 4] const-generic arguments parallel to type arguments.
            //      Each entry corresponds to an isConst parameter in the template
            //      declaration. Stored as expressions for compile-time evaluation
            //      during monomorphization, then substituted as literals in the
            //      template body.
            //      Example: f<float, 4> → typeArguments=[Float], constArguments=[4]
            // ==========================================================================
            std::vector<ExprPtr> constArguments; ///< Const-generic arg values / قيم الوسائط الثابتة
            std::vector<int> argumentKindOrder;  ///< 0=type, 1=const — preserves original order

            /**
             * @brief Constructor / البناء
             */
            TemplateInstantiation(const std::string &name,
                                  std::vector<Types::SadTypeKind> typeArgs,
                                  const Lexer::Position &pos = Lexer::Position())
                : Expression(pos), templateName(name), typeArguments(std::move(typeArgs)) {}

            TemplateInstantiation(const std::string &name,
                                  std::vector<Types::SadTypeKind> typeArgs,
                                  std::vector<std::string> typeArgNames,
                                  const Lexer::Position &pos = Lexer::Position())
                : Expression(pos), templateName(name), typeArguments(std::move(typeArgs)),
                  typeArgumentNames(std::move(typeArgNames)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitTemplateInstantiation(*this);
            }

            Types::SadTypePtr getType() const override
            {
                // (AR) القالب يعيد نوع غير محدد حتى يتم instantiate
                // (EN) Template returns unknown type until instantiated
                return nullptr;
            }

            std::string toString() const override;
        };

        // =========================================================================
        // Namespace Declaration / تصريح فضاء الأسماء
        // =========================================================================

        /**
         * @brief Namespace declaration / تصريح فضاء الأسماء
         *
         * Represents a namespace for organizing code.
         * يمثل فضاء أسماء لتنظيم الكود.
         *
         * @example Examples / أمثلة:
         * - فضاء رياضيات ... نهاية_فضاء
         * - namespace math { ... }
         */
        class NamespaceDecl : public Statement
        {
        public:
            std::string name; ///< Namespace name / اسم الفضاء
            StmtList members; ///< Namespace members / أعضاء الفضاء

            /**
             * @brief Constructor / البناء
             */
            NamespaceDecl(const std::string &name, StmtList members,
                          const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), members(std::move(members)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitNamespaceDecl(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // Operator Overload Declaration / تصريح تحميل العامل
        // =========================================================================

        /**
         * @brief Operator overload declaration / تصريح تحميل العامل
         *
         * Represents an operator overload in a class.
         * يمثل تحميل عامل في صنف.
         *
         * @example Examples / أمثلة:
         * - عامل +(كسر آخر) كسر { ... }
         * - operator +(other: Fraction) -> Fraction { ... }
         */
        class OperatorDecl : public Statement
        {
        public:
            std::string operatorSymbol;        ///< Operator symbol / رمز العامل (+, -, *, etc.)
            std::vector<Parameter> parameters; ///< Parameters / المعاملات
            Types::SadTypeKind returnType;         ///< Return type / نوع الإرجاع
            StmtPtr body;                      ///< Operator body / جسم العامل
            AccessModifier access;             ///< Access modifier / معدّل الوصول

            /**
             * @brief Constructor / البناء
             */
            OperatorDecl(const std::string &op, std::vector<Parameter> params,
                         Types::SadTypeKind retType, StmtPtr body,
                         AccessModifier access = AccessModifier::PUBLIC,
                         const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), operatorSymbol(op), parameters(std::move(params)),
                  returnType(retType), body(std::move(body)), access(access) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitOperatorDecl(*this);
            }

            std::string toString() const override;
        };

    } // namespace AST
} // namespace Sad

// =========================================================================
// (AR) عقد نظام النحلة (BeeOS) — إضافات المرحلة 9
// (EN) BeeOS AST Nodes — Phase 9 additions
// =========================================================================

namespace Sad
{
    namespace AST
    {

        // =========================================================================
        // Struct Declaration / تصريح بنية
        // =========================================================================

        /**
         * @brief Struct declaration / تصريح بنية
         *
         * Represents a plain data structure (no inheritance, no vtable).
         * يمثل بنية بيانات بسيطة (بدون وراثة، بدون جدول دوال افتراضية).
         *
         * @example Examples / أمثلة:
         * - بنية نقطة { عشري س، عشري ي }
         * - بنية<ن> صندوق { ن قيمة }
         * - struct Point { x: f64, y: f64 }
         */
        struct StructField
        {
            std::string name;     ///< Field name / اسم الحقل
            Types::SadTypeKind type;  ///< Field type / نوع الحقل
            ExprPtr defaultValue; ///< Default value (optional) / القيمة الافتراضية
            bool isPublic;        ///< Is public? / عام؟

            // ──────────────────────────────────────────────────────────────
            // (AR) اسمُ الصنفِ حين يكون نوعُ الحقلِ صنفًا — وإلّا فارغ.
            //      كان `type` وحدَه هو كلَّ ما يُحفَظ، و`SadTypeKind::Class`
            //      نوعٌ **بلا هُويّة**: فحقلٌ صنفيٌّ في `بنية` يفقد اسمَ صنفِه
            //      عند التحليل، فلا يبقى ما يُنشَأ منه. وقِيس أثرُ ذلك:
            //      «بنية علبة { بسيط جزء }» كانت تُنتِج حقلًا اسمُه **بسيط**
            //      لأنّ المحلّلَ أخذ لفظَ النوعِ اسمًا للحقل، ثمّ ضاع «جزء».
            // (EN) Class name when the field's type is a class; empty otherwise.
            //      SadTypeKind::Class carries no identity, so a class-typed
            //      struct field lost its class at parse time. Measured: the
            //      field ended up named after its own type word.
            // ──────────────────────────────────────────────────────────────
            std::string typeName;

            StructField(const std::string &n, Types::SadTypeKind t,
                        ExprPtr def = nullptr, bool pub = true,
                        const std::string &typeNameIn = std::string())
                : name(n), type(t), defaultValue(std::move(def)), isPublic(pub),
                  typeName(typeNameIn) {}

            StructField(StructField &&) = default;
            StructField &operator=(StructField &&) = default;
            StructField(const StructField &other)
                : name(other.name), type(other.type), defaultValue(nullptr),
                  isPublic(other.isPublic), typeName(other.typeName) {}
        };

        class StructDecl : public Statement
        {
        public:
            std::string name;                          ///< Struct name / اسم البنية
            std::vector<StructField> fields;           ///< Struct fields / حقول البنية
            std::vector<TypeParameter> typeParameters; ///< Generic params / معاملات الأنواع
            std::vector<std::string> lifetimeParams;   ///< Lifetime parameters <'أ, 'ب> / معاملات العمر
            bool isPacked;                             ///< Packed? / محزومة؟
            bool isExported;                           ///< Exported? / مصدّرة؟
            bool isCRepr = false;                      ///< C-ABI layout (@تمثيل_سي)? / تخطيط C-متوافق (بلا ترويسة)؟ [RFC #53 F2-أ]
            StmtList methods;                          ///< Impl methods / الدوال المنفذة

            StructDecl(const std::string &name,
                       std::vector<StructField> fields,
                       std::vector<TypeParameter> typeParams = {},
                       bool packed = false,
                       bool exported = false,
                       const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), fields(std::move(fields)),
                  typeParameters(std::move(typeParams)),
                  isPacked(packed), isExported(exported) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitStructDecl(*this);
            }

            std::string toString() const override
            {
                return "struct " + name;
            }
        };

        // =========================================================================
        // Trait Declaration / تصريح سمة
        // =========================================================================

        /**
         * @brief Trait declaration / تصريح سمة
         *
         * Represents a trait (interface with optional default implementations).
         * يمثل سمة (واجهة مع تنفيذات افتراضية اختيارية).
         *
         * @example Examples / أمثلة:
         * - سمة قابل_للعرض { دالة اعرض(هذا) نص }
         * - سمة<ن> مقارن { دالة قارن(هذا، آخر: ن) رقم }
         * - trait Display { fn display(&self) -> String }
         */
        struct TraitMethod
        {
            std::string name;              ///< Method name / اسم الدالة
            std::vector<Parameter> params; ///< Parameters / المعاملات
            Types::SadTypeKind returnType;     ///< Return type / نوع الإرجاع
            // (AR) [Phase 5e] اسم نوع الإرجاع للأصناف المُعرَّفة من المستخدم.
            //      غير فارغ عندما يكون نوع الإرجاع صنفاً مثل: "نقطة"، "شخص".
            // (EN) [Phase 5e] Return type name for user-defined classes.
            //      Non-empty when return type is a class, e.g. "نقطة", "شخص".
            std::string returnTypeName;

            /// (AR) التنفيذ الافتراضي — shared_ptr للسماح بالمشاركة مع الأصناف المنفذة
            /// (EN) Default implementation — shared_ptr to allow sharing with implementing classes
            std::shared_ptr<Statement> defaultImpl;

            TraitMethod(const std::string &n, std::vector<Parameter> p,
                        Types::SadTypeKind ret, StmtPtr impl = nullptr)
                : name(n), params(std::move(p)), returnType(ret),
                  defaultImpl(std::move(impl)) {}

            TraitMethod(TraitMethod &&) = default;
            TraitMethod &operator=(TraitMethod &&) = default;
        };

        class TraitDecl : public Statement
        {
        public:
            std::string name;                          ///< Trait name / اسم السمة
            std::vector<TraitMethod> methods;          ///< Trait methods / دوال السمة
            std::vector<TypeParameter> typeParameters; ///< Generic params / معاملات الأنواع
            std::vector<std::string> superTraits;      ///< Required traits / السمات المطلوبة
            bool isExported;                           ///< Exported? / مصدّرة؟

            TraitDecl(const std::string &name,
                      std::vector<TraitMethod> methods,
                      std::vector<TypeParameter> typeParams = {},
                      std::vector<std::string> supers = {},
                      bool exported = false,
                      const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), methods(std::move(methods)),
                  typeParameters(std::move(typeParams)),
                  superTraits(std::move(supers)), isExported(exported) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitTraitDecl(*this);
            }

            std::string toString() const override
            {
                return "trait " + name;
            }
        };

        // =========================================================================
        // Impl Declaration / تصريح تنفيذ
        // =========================================================================

        /**
         * @brief Impl block declaration / تصريح كتلة التنفيذ
         *
         * Implements a trait for a type, or adds methods to a struct.
         * ينفّذ سمة لنوع، أو يضيف دوال لبنية.
         *
         * @example Examples / أمثلة:
         * - نفّذ قابل_للعرض لـ نقطة { دالة اعرض(هذا) نص { ... } }
         * - نفّذ نقطة { دالة المسافة(هذا) عشري { ... } }
         * - impl Display for Point { fn display(&self) -> String { ... } }
         */
        class ImplDecl : public Statement
        {
        public:
            std::string traitName;                     ///< Trait name (empty if inherent impl) / اسم السمة
            std::string targetType;                    ///< Target type name / اسم النوع المستهدف
            StmtList methods;                          ///< Implemented methods / الدوال المنفذة
            std::vector<TypeParameter> typeParameters; ///< Generic params / معاملات الأنواع

            ImplDecl(const std::string &trait, const std::string &target,
                     StmtList methods,
                     std::vector<TypeParameter> typeParams = {},
                     const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), traitName(trait), targetType(target),
                  methods(std::move(methods)), typeParameters(std::move(typeParams)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitImplDecl(*this);
            }

            std::string toString() const override
            {
                if (traitName.empty())
                    return "impl " + targetType;
                return "impl " + traitName + " for " + targetType;
            }
        };

        // =========================================================================
        // Test Declaration / تصريح اختبار
        // =========================================================================

        /**
         * @brief Test function declaration / تصريح دالة اختبار
         *
         * Represents a test case that can be run by the test framework.
         * يمثل حالة اختبار يمكن تشغيلها بإطار الاختبار.
         *
         * @example Examples / أمثلة:
         * - اختبر("الجمع يعمل") { تأكد(1 + 1 == 2) }
         * - test("addition works") { assert(1 + 1 == 2) }
         */
        class TestDecl : public Statement
        {
        public:
            std::string testName; ///< Test name / اسم الاختبار
            StmtPtr body;         ///< Test body / جسم الاختبار
            bool shouldFail;      ///< Expected to fail? / متوقع أن يفشل؟

            // (AR) إعدادات اختبار الخصائص (Property Testing)
            // (EN) Property test configuration
            bool isPropertyTest = false; ///< هل هذا اختبار خصائص؟
            int iterations = 0;          ///< عدد التكرارات (0 = اختبار عادي)
            int seed = -1;               ///< بذرة العشوائية (-1 = تلقائي)
            int maxSize = 100;           ///< الحد الأقصى لحجم القيم

            TestDecl(const std::string &name, StmtPtr body,
                     bool shouldFail = false,
                     const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), testName(name), body(std::move(body)),
                  shouldFail(shouldFail) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitTestDecl(*this);
            }

            std::string toString() const override
            {
                return "test \"" + testName + "\"";
            }
        };

        // =========================================================================
        // Atomic Declaration / تصريح ذرّي
        // =========================================================================

        /**
         * @brief Atomic variable declaration / تصريح متغير ذرّي
         *
         * @example Examples / أمثلة:
         * - ذرّي<رقم> عداد = 0
         * - atomic<int> counter = 0
         */
        class AtomicDecl : public Statement
        {
        public:
            std::string name;         ///< Variable name / اسم المتغير
            Types::SadTypeKind innerType; ///< Inner type / النوع الداخلي
            ExprPtr initialValue;     ///< Initial value / القيمة الأولية

            AtomicDecl(const std::string &name, Types::SadTypeKind type,
                       ExprPtr init = nullptr,
                       const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), innerType(type),
                  initialValue(std::move(init)) {}

            void accept(ASTVisitor &visitor) override
            {
                // (AR) ذرّي يُعامل كمتغير عادي مؤقتاً حتى يتم دعم التزامن
                // (EN) Atomic treated as regular variable until concurrency is fully supported
                VarDeclStmt varDecl(name, innerType, nullptr, false, position);
                varDecl.initializer = std::move(initialValue);
                visitor.visitVarDeclStmt(varDecl);
                initialValue = std::move(varDecl.initializer);
            }

            std::string toString() const override
            {
                return "atomic " + name;
            }
        };

        // =========================================================================
        // Extension Declaration / تصريح امتداد
        // =========================================================================

        /**
         * @brief (AR) تصريح كتلة امتداد — إضافة دوال جديدة لنوع موجود بدون تعديله
         * @brief (EN) Extension block declaration — add new methods to existing type without modifying it
         *
         * (AR) طرق الامتداد تسمح بإضافة دوال جديدة لأصناف وبنى موجودة بإعادة استخدام
         *      نفس البنية التحتية لـ ImplDecl (إضافة دوال لصنف موجود).
         *      الفرق: لا تتطلب سمة — تضيف دوال مباشرة للنوع.
         *
         * (EN) Extension methods allow adding new functions to existing classes/structs
         *      by reusing the ImplDecl infrastructure (adding methods to existing class).
         *      Difference: doesn't require a trait — adds methods directly to the type.
         *
         * @example
         * @code{.ص}
         * امتداد نص
         *     دالة عدّ_الكلمات(هذا)
         *         ارجع هذا.قسّم(" ").طول()
         *     نهاية
         * نهاية
         * @endcode
         */
        class ExtensionDecl : public Statement
        {
        public:
            std::string targetType;                    ///< (AR) اسم النوع المُمتد / (EN) Target type name
            StmtList methods;                          ///< (AR) الدوال المُضافة / (EN) Added methods
            std::vector<TypeParameter> typeParameters; ///< (AR) معاملات الأنواع (للقوالب) / (EN) Type parameters (generics)

            ExtensionDecl(const std::string &target,
                          StmtList methods,
                          std::vector<TypeParameter> typeParams = {},
                          const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), targetType(target),
                  methods(std::move(methods)), typeParameters(std::move(typeParams)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitExtensionDecl(*this);
            }

            std::string toString() const override
            {
                return "extension " + targetType;
            }
        };

        // =========================================================================
        // Macro Declaration / تصريح ماكرو
        // =========================================================================

        /**
         * @brief (AR) تصريح ماكرو — تعريف نمط قابل للتوسيع وقت التحليل أو التنفيذ
         * @brief (EN) Macro declaration — define a pattern expandable at parse/run time
         *
         * (AR) الماكرو يعمل كدالة مُضمّنة تُنفّذ في نطاق المُستدعي (وليس نطاق جديد).
         *      هذا يسمح بالتعديل المباشر على متغيرات المُستدعي.
         *      الماكرو يُستدعى بأسلوب: اسم!(معاملات)
         *
         * (EN) Macros work as inline functions executed in the caller's scope (not a new scope).
         *      This allows direct modification of the caller's variables.
         *      Macros are invoked with: name!(arguments)
         *
         * @example
         * @code{.ص}
         * ماكرو تبديل(أ، ب)
         *     متغير _م = أ
         *     أ = ب
         *     ب = _م
         * نهاية
         *
         * تبديل!(س، ص)
         * @endcode
         */
        class MacroDecl : public Statement
        {
        public:
            std::string name;                ///< (AR) اسم الماكرو / (EN) Macro name
            std::vector<std::string> params; ///< (AR) أسماء المعاملات / (EN) Parameter names
            StmtPtr body;                    ///< (AR) جسم الماكرو / (EN) Macro body
            bool isVariadic;                 ///< (AR) هل يقبل معاملات متغيرة / (EN) Is variadic

            MacroDecl(const std::string &name,
                      std::vector<std::string> params,
                      StmtPtr body,
                      const Lexer::Position &pos = Lexer::Position(),
                      bool isVariadic = false)
                : Statement(pos), name(name),
                  params(std::move(params)), body(std::move(body)),
                  isVariadic(isVariadic) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitMacroDecl(*this);
            }

            std::string toString() const override
            {
                return "macro " + name + "!";
            }
        };

        // =========================================================================
        // Type Alias Declaration / تصريح اسم مستعار للنوع
        // =========================================================================

        /**
         * @brief (AR) تصريح اسم مستعار للنوع — نوع اسم = نوع_هدف
         *        يُنشئ اسماً بديلاً لنوع أو صنف أو تعداد موجود.
         *        مثال: نوع عدد = رقم
         *        مثال: نوع قائمة = مصفوفة
         *        مثال: نوع مستخدم = شخص
         * @brief (EN) Type alias declaration — type name = target_type
         *        Creates an alias for an existing type, class, or enum.
         */
        class TypeAliasDecl : public Statement
        {
        public:
            std::string name; ///< (AR) اسم المستعار / (EN) Alias name
            ExprPtr target;   ///< (AR) التعبير الهدف (اسم نوع أو صنف) / (EN) Target expression (type or class name)

            TypeAliasDecl(const std::string &name, ExprPtr target,
                          const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), target(std::move(target)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitTypeAliasDecl(*this);
            }

            std::string toString() const override
            {
                return "نوع " + name + " = " + (target ? target->toString() : "?");
            }
        };

        // =========================================================================
        // (AR) تفكيك الصف — تعيين عناصر صف إلى متغيرات فردية
        // (EN) Tuple destructuring — assign tuple elements to individual variables
        // =========================================================================

        /// (AR) عقدة تفكيك الصف: متغير (أ، ب، ج) = تعبير
        /// (EN) Tuple destructure node: var (a, b, c) = expression
        class TupleDestructureStmt : public Statement
        {
        public:
            std::vector<std::string> names; ///< (AR) أسماء المتغيرات / (EN) Variable names
            bool isConst;                   ///< (AR) ثابت؟ / (EN) Is constant?
            ExprPtr initializer;            ///< (AR) تعبير الصف / (EN) Tuple expression

            TupleDestructureStmt(const std::vector<std::string> &names, bool isConst,
                                 ExprPtr init, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), names(names), isConst(isConst), initializer(std::move(init)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitTupleDestructureStmt(*this);
            }

            std::string toString() const override
            {
                std::string result = isConst ? "ثابت (" : "متغير (";
                for (size_t i = 0; i < names.size(); ++i)
                {
                    if (i > 0)
                        result += "، ";
                    result += names[i];
                }
                result += ") = " + (initializer ? initializer->toString() : "?");
                return result;
            }
        };

    } // namespace AST
} // namespace Sad

#endif // SAD_AST_DECLARATIONS_H
