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
#include "sir_constants.h"
#include "sir_builder_context.h"
#include "ast_node.h"
#include "expressions.h"
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "pattern_nodes.h"
#include "advanced_expr_nodes.h"
#include "property_nodes.h"
#include "builders/method_call_builder.h"
#include "builders/builtin_builder.h"
#include "builders/call_builder.h"
#include "builders/class_builder.h"
#include "builders/statement_builder.h"
#include "builders/expression_builder.h"
#include <memory>
#include <string>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <optional>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ======================================================================
            // AST Type Aliases - توافق مع الأسماء القديمة
            // AST Type Compatibility Aliases
            // ======================================================================

            // Map old AST node names to new ones for backward compatibility
            // ربط أسماء AST القديمة بالجديدة للتوافق
            namespace AST
            {
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
                using ForRangeStmt = Sad::AST::ForRangeStmt; // (AR) حلقة لكل في / (EN) for-each loop
                using ReturnStmt = Sad::AST::ReturnStmt;
                using BreakStmt = Sad::AST::BreakStmt;
                using ContinueStmt = Sad::AST::ContinueStmt;
                using ExprStmt = Sad::AST::ExprStmt;
                using VarDeclStmt = Sad::AST::VarDeclStmt;
                using AssignExpr = Sad::AST::AssignExpr;
                using FieldDecl = Sad::AST::FieldDecl;
                using MethodDecl = Sad::AST::MethodDecl;
                using FunctionDecl = Sad::AST::FunctionDecl;
                using ClassDecl = Sad::AST::ClassDecl;
                using TemplateFunctionDecl = Sad::AST::TemplateFunctionDecl; // (AR) دالة قالب / (EN) Template function
                using TemplateClassDecl = Sad::AST::TemplateClassDecl;       // (AR) صنف قالب / (EN) Template class
                using NewExpr = Sad::AST::NewExpr;
                using MemberAccessExpr = Sad::AST::MemberAccessExpr;
                using MethodCallExpr = Sad::AST::MethodCallExpr;
                using ThisExpr = Sad::AST::ThisExpr;

                // (AR) عقد نظام الاستيراد والتصدير / (EN) Import/Export system nodes
                using ImportStmt = Sad::AST::ImportStmt;
                using FromImportStmt = Sad::AST::FromImportStmt;
                using ExportDecl = Sad::AST::ExportDecl;
                using ExportStmt = Sad::AST::ExportStmt;
                using TraitDecl = Sad::AST::TraitDecl;
                using ImplDecl = Sad::AST::ImplDecl;
                using BlockStmt = Sad::AST::BlockStmt;
                using MemberAssignExpr = Sad::AST::MemberAssignExpr;
                using VariableExpr = Sad::AST::VariableExpr;
                using LiteralExpr = Sad::AST::LiteralExpr;

                // Operator types - enums not classes
                using BinaryOperator = Sad::Lexer::TokenType;
                using UnaryOperator = Sad::Lexer::TokenType;

                // (AR) أنواع التعابير المستخدمة في buildExpression — مطلوبة للتصريحات
                // (EN) Expression types used in buildExpression — required for declarations
                using MemberExpr = Sad::AST::MemberExpr;
                using TernaryExpr = Sad::AST::TernaryExpr;
                using IndexExpr = Sad::AST::IndexExpr;
                using IndexAssignExpr = Sad::AST::IndexAssignExpr;
                using ArrayExpr = Sad::AST::ArrayExpr;
                using TupleExpr = Sad::AST::TupleExpr;
                using MapExpr = Sad::AST::MapExpr;
                using WalrusExpr = Sad::AST::WalrusExpr;
                using LambdaExpr = Sad::AST::LambdaExpr;
                using RangeExpr = Sad::AST::RangeExpr;
                using ListComprehensionExpr = Sad::AST::ListComprehensionExpr;
                using DictComprehensionExpr = Sad::AST::DictComprehensionExpr;
                using SetComprehensionExpr = Sad::AST::SetComprehensionExpr;
                using GeneratorExpr = Sad::AST::GeneratorExpr;
                using InlineAsmExpr = Sad::AST::InlineAsmExpr;
                using SizeofExpr = Sad::AST::SizeofExpr;
                using AtomicExpr = Sad::AST::AtomicExpr;
                using OptionalChainExpr = Sad::AST::OptionalChainExpr;
                using NullCoalesceExpr = Sad::AST::NullCoalesceExpr;
                using SliceExpr = Sad::AST::SliceExpr;
                using ErrorPropagateExpr = Sad::AST::ErrorPropagateExpr;

                // (AR) عقد الأنماط لمطابقة match / (EN) Pattern nodes for match statement
                using PatternNode = Sad::AST::Pattern;
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
            struct BuildResult
            {
                std::string registerName;  ///< (AR) اسم السجل الافتراضي / (EN) Virtual register name
                SadTypeKind type;          ///< (AR) نوع البيانات / (EN) Data type
                bool isConstant = false;   ///< (AR) هل قيمة ثابتة؟ / (EN) Is constant value?
                bool isParameter = false;  ///< (AR) هل قيمة من معامل دالة؟ / (EN) Is value from function parameter?
                std::string constantValue; ///< (AR) القيمة الثابتة (إن وجدت) / (EN) Constant value (if any)

                // (AR) معلومات إضافية للأنواع المركبة / Additional info for composite types
                SadTypeKind elementType;       ///< (AR) نوع عنصر المصفوفة / (EN) Array element type
                std::string elementClassName;  ///< (AR) اسم صنف عنصر المصفوفة إذا كان كائناً / (EN) Array element class name when element is object
                std::string className;         ///< (AR) اسم الصنف للكائنات / (EN) Class name for objects
                bool isFieldAccess = false;    ///< (AR) نتيجة وصول لحقل OOP (لا تحتاج LOAD إضافي) / (EN) OOP field access result (no extra LOAD needed)
                bool isDirectValue = false;    ///< (AR) قيمة مباشرة (نتيجة CALL/CLOSURE_CALL) لا تحتاج LOAD إضافي / (EN) Direct value (CALL/CLOSURE_CALL result) — no extra LOAD needed
                std::string closureLambdaName; ///< (AR) اسم __lambda_X المرتبط عند إسناد لامدا / (EN) Associated __lambda_X name for lambda assignment

                /**
                 * @brief (AR) منشئ افتراضي
                 * @brief (EN) Default constructor
                 */
                BuildResult() : type(SadTypeKind::Void), elementType(SadTypeKind::Void) {}

                /**
                 * @brief (AR) منشئ بسجل ونوع
                 * @brief (EN) Constructor with register and type
                 */
                BuildResult(const std::string &reg, SadTypeKind t)
                    : registerName(reg), type(t), isConstant(false), elementType(SadTypeKind::Void) {}

                /**
                 * @brief (AR) منشئ لقيمة ثابتة
                 * @brief (EN) Constructor for constant value
                 */
                BuildResult(const std::string &value, SadTypeKind t, bool isConst)
                    : constantValue(value), type(t), isConstant(isConst), elementType(SadTypeKind::Void) {}
            };

            /**
             * @brief (AR) بنية تأجيل استخراج حقول التعداد الجبري (ADT) في match
             * @brief (EN) Deferred ADT field extraction info for match patterns
             */
            struct MatchDeferredField
            {
                std::string varName;   ///< (AR) اسم المتغير المراد ربطه / (EN) Variable name to bind
                size_t fieldIndex;     ///< (AR) فهرس الحقل / (EN) Field index in variant
                std::string fieldName; ///< (AR) اسم الحقل الأصلي / (EN) Original field name
                std::string enumName;  ///< (AR) اسم التعداد / (EN) Enum name for lookup
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
            struct VariableInfo
            {
                std::string name;             ///< (AR) الاسم / (EN) Name
                SadTypeKind type;             ///< (AR) النوع / (EN) Type
                std::string registerName;     ///< (AR) اسم السجل / (EN) Register name
                bool isGlobal;                ///< (AR) متغير عام؟ / (EN) Is global?
                bool isMutable;               ///< (AR) قابل للتعديل؟ / (EN) Is mutable?
                bool isVolatile;              ///< (AR) متطاير؟ / (EN) Is volatile?
                bool isParameter = false;     ///< (AR) هل معامل دالة؟ / (EN) Is function parameter?
                int scopeLevel;               ///< (AR) مستوى النطاق / (EN) Scope level
                std::string className;        ///< (AR) اسم الصنف (مثلاً __channel__ للقنوات) / (EN) Class name (e.g. __channel__ for channels)
                SadTypeKind elementType;      ///< (AR) نوع عنصر المصفوفة — يُستخدم لحلقات foreach / (EN) Array element type — used for foreach loops
                std::string elementClassName; ///< (AR) اسم صنف عنصر المصفوفة عند كون العناصر كائنات / (EN) Array element class name when elements are objects

                // ================================================================
                // (AR) [Fix #51] معلومات الالتقاط — تُستخدم عندما يكون المتغير ملتقطاً في إغلاق
                //      isCaptured: هل هذا متغير ملتقط من نطاق خارجي؟
                //      captureIndex: فهرس المتغير في مصفوفة البيئة env[]
                //      envRegister: اسم سجل __env الذي يحمل مؤشر المصفوفة
                // (EN) [Fix #51] Capture info — used when variable is captured in a closure
                //      isCaptured: is this variable captured from outer scope?
                //      captureIndex: index in the environment array env[]
                //      envRegister: register name of __env holding the array pointer
                // ================================================================
                bool isCaptured = false; ///< (AR) هل ملتقط في إغلاق؟ / (EN) Is captured in closure?
                int captureIndex = -1;   ///< (AR) فهرس في مصفوفة env / (EN) Index in env array
                std::string envRegister; ///< (AR) سجل __env / (EN) __env register name

                // ================================================================
                // (AR) اسم دالة اللامدا المرتبطة (إذا كان المتغير يحمل بنية إغلاق)
                //      يُستخدم في Step 3.5 لتحديد نوع إرجاع CLOSURE_CALL
                //      بدون هذا: نوع الإرجاع يكون Integer افتراضياً → Boolean يضيع
                // (EN) Associated lambda function name (if variable holds closure)
                //      Used in Step 3.5 to determine CLOSURE_CALL return type
                // ================================================================
                std::string closureLambdaName; ///< (AR) اسم __lambda_X المرتبط / (EN) Associated __lambda_X name

                /**
                 * @brief (AR) منشئ افتراضي
                 * @brief (EN) Default constructor
                 */
                VariableInfo() : type(SadTypeKind::Void), isGlobal(false), isMutable(true), isVolatile(false), isParameter(false), scopeLevel(0), elementType(SadTypeKind::Void) {}
            };

            /**
             * @brief (AR) معلومات الدالة للجدول
             * @brief (EN) Function information for lookup table
             */
            struct FunctionInfo
            {
                std::string name;                         ///< (AR) الاسم / (EN) Name
                SadTypeKind returnType;                   ///< (AR) نوع الإرجاع / (EN) Return type
                std::vector<SIRParameter> parameters;     ///< (AR) المعاملات / (EN) Parameters
                std::shared_ptr<SIRFunction> sirFunction; ///< (AR) مؤشر لدالة SIR / (EN) Pointer to SIR function
                bool isGenerator = false;                 ///< (AR) دالة مولّد / (EN) Generator function

                // ================================================================
                // (AR) اسم الصنف المُرجع — يُستخدم عندما تُرجع الدالة كائناً (جديد ClassName)
                //      لتتبع النوع الفعلي للكائن عبر استدعاءات الدوال.
                //      مثال: دالة اصنع_نقطة() ترجع جديد نقطة(1,2) → returnClassName = "نقطة"
                // (EN) Return class name — used when function returns an object (new ClassName)
                //      to track the actual object type across function calls.
                //      Example: function makePoint() returns new Point(1,2) → returnClassName = "Point"
                // ================================================================
                std::string returnClassName; ///< (AR) اسم الصنف المُرجع / (EN) Return class name

                // ================================================================
                // (AR) [Fix #52] اسم اللامدا المُرجعة — يُستخدم عندما تُرجع الدالة إغلاقاً
                //      مثال: دالة صانع() ارجع لامدا()... → returnLambdaName = "__lambda_0"
                //      هذا يسمح بتتبع نوع الإرجاع الصحيح عند استدعاء الإغلاق عبر متغير
                // (EN) [Fix #52] Returned lambda name — used when function returns a closure
                //      Example: func factory() return lambda()... → returnLambdaName = "__lambda_0"
                //      Enables correct return type tracking when calling closure via variable
                // ================================================================
                std::string returnLambdaName; ///< (AR) اسم اللامدا المُرجعة / (EN) Returned lambda name

                // ================================================================
                // (AR) مرجع إلى تعريف الدالة في AST — يُستخدم لمعالجة القيم الافتراضية
                //      عند استدعاء الدالة بوسائط أقل من عدد المعاملات
                // (EN) Reference to function AST declaration — used to process default
                //      parameter values when calling with fewer arguments than parameters
                // ================================================================
                AST::FunctionDeclNode *astDecl = nullptr; ///< (AR) تعريف AST / (EN) AST declaration

                /**
                 * @brief (AR) منشئ افتراضي
                 * @brief (EN) Default constructor
                 */
                FunctionInfo() : returnType(SadTypeKind::Void) {}
            };

            /**
             * @brief (AR) سياق الحلقة - لمعالجة break/continue
             * @brief (EN) Loop context - for handling break/continue
             */
            struct LoopContext
            {
                std::string continueLabel; ///< (AR) تسمية continue / (EN) Continue label
                std::string breakLabel;    ///< (AR) تسمية break / (EN) Break label
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
            struct TypeParameter
            {
                std::string name;           ///< (AR) اسم معامل النوع (مثل T, U, K, V) / (EN) Type parameter name (e.g., T, U, K, V)
                SadTypeKind constraintType; ///< (AR) قيد النوع (اختياري) / (EN) Type constraint (optional)
                bool hasConstraint;         ///< (AR) هل يوجد قيد؟ / (EN) Has constraint?

                /**
                 * @brief (AR) منشئ افتراضي
                 * @brief (EN) Default constructor
                 */
                TypeParameter() : constraintType(SadTypeKind::Void), hasConstraint(false) {}

                /**
                 * @brief (AR) منشئ باسم
                 * @brief (EN) Constructor with name
                 */
                TypeParameter(const std::string &n)
                    : name(n), constraintType(SadTypeKind::Void), hasConstraint(false) {}
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
            struct GenericScope
            {
                std::vector<TypeParameter> typeParameters;                      ///< (AR) معاملات الأنواع / (EN) Type parameters
                std::unordered_map<std::string, SadTypeKind> typeSubstitutions; ///< (AR) الاستبدالات الحالية / (EN) Current substitutions

                // ==========================================================================
                // (AR) [Phase 4 — Monomorphization] استبدالات const-generic:
                //      عند بناء جسم قالب يحتوي على معامل ثابت N، نحفظ هنا
                //      الـSIROperand المقابل (Constant). buildVariableAccess
                //      يفحص هذا الجدول قبل البحث في النطاقات العادية، وعند
                //      التطابق ينتج فوراً BuildResult ثابت بدون load.
                // (EN) [Phase 4] const-generic substitutions: when building a
                //      template body that uses a const param N, we store the
                //      corresponding SIROperand (Constant) here. buildVariableAccess
                //      checks this table before scope lookup and, on match,
                //      produces a constant BuildResult immediately (no load).
                // ==========================================================================
                std::unordered_map<std::string, SIROperand> constSubstitutions;

                /**
                 * @brief (AR) التحقق من وجود معامل نوع
                 * @brief (EN) Check if type parameter exists
                 */
                bool hasTypeParameter(const std::string &name) const
                {
                    for (const auto &param : typeParameters)
                    {
                        if (param.name == name)
                            return true;
                    }
                    return false;
                }

                /**
                 * @brief (AR) الحصول على النوع المستبدل
                 * @brief (EN) Get substituted type
                 */
                SadTypeKind getSubstitutedType(const std::string &name) const
                {
                    auto it = typeSubstitutions.find(name);
                    return (it != typeSubstitutions.end()) ? it->second : SadTypeKind::Void;
                }
            };

            // ======================================================================
            // (AR) معلومات التعداد الجبري (ADT) / (EN) ADT Enum Type Info
            // ======================================================================

            /**
             * @brief (AR) معلومات حالة (variant) في تعداد جبري
             * @brief (EN) Variant information in an ADT enum
             *
             * @details
             * (AR) كل حالة لها مميّز (tag) وقائمة بأسماء حقولها
             *      الحالة بدون حقول = Unit variant (مثل نقطة)
             *      الحالة مع حقول = Tuple variant (مثل دائرة(نصف_القطر))
             * (EN) Each variant has a discriminant (tag) and list of field names
             *      Variant without fields = Unit variant (e.g., Point)
             *      Variant with fields = Tuple variant (e.g., Circle(radius))
             */
            struct ADTVariantInfo
            {
                std::string name;                ///< (AR) اسم الحالة / (EN) Variant name
                int64_t tag;                     ///< (AR) المميّز (discriminant) / (EN) Discriminant tag
                std::vector<std::string> fields; ///< (AR) أسماء الحقول / (EN) Field names

                /// (AR) هل هذه حالة بدون بيانات (Unit)؟
                /// (EN) Is this a unit variant (no data)?
                bool isUnit() const { return fields.empty(); }

                /// (AR) عدد الحقول
                /// (EN) Number of fields
                size_t fieldCount() const { return fields.size(); }

                /// (AR) البحث عن فهرس حقل بالاسم — يُرجع -1 إذا لم يُوجد
                /// (EN) Find field index by name — returns -1 if not found
                int findFieldIndex(const std::string &fieldName) const
                {
                    for (size_t i = 0; i < fields.size(); ++i)
                    {
                        if (fields[i] == fieldName)
                            return static_cast<int>(i);
                    }
                    return -1;
                }
            };

            /**
             * @brief (AR) معلومات تعداد جبري كامل (ADT Enum)
             * @brief (EN) Complete ADT enum type information
             *
             * @details
             * (AR) يحتفظ بكل المعلومات اللازمة لترجمة التعداد الجبري:
             *      - الاسم
             *      - قائمة الحالات (variants) مع مميّزاتها وحقولها
             *      - أكبر عدد حقول عبر جميع الحالات (لحساب حجم البنية)
             *      - اسم بنية SIR التي تمثل هذا التعداد
             *
             * (EN) Holds all information needed to compile an ADT enum:
             *      - Name
             *      - List of variants with tags and fields
             *      - Maximum field count across all variants (for struct size)
             *      - Name of SIR struct representing this enum
             *
             * @example
             * تعداد شكل
             *     دائرة(نصف_القطر)            ← tag=0, fields=["نصف_القطر"]
             *     مستطيل(عرض، ارتفاع)         ← tag=1, fields=["عرض", "ارتفاع"]
             *     نقطة                         ← tag=2, fields=[]
             * نهاية
             *
             * → maxFieldCount = 2 (مستطيل)
             * → SIR struct: { __tag: I64, __f0: PTR, __f1: PTR }
             */
            struct ADTEnumInfo
            {
                std::string name;                     ///< (AR) اسم التعداد / (EN) Enum name
                std::vector<ADTVariantInfo> variants; ///< (AR) قائمة الحالات / (EN) List of variants
                size_t maxFieldCount = 0;             ///< (AR) أكبر عدد حقول / (EN) Max field count
                std::string structName;               ///< (AR) اسم بنية SIR / (EN) SIR struct name

                /// (AR) البحث عن حالة بالاسم
                /// (EN) Find variant by name
                const ADTVariantInfo *findVariant(const std::string &variantName) const
                {
                    for (const auto &v : variants)
                    {
                        if (v.name == variantName)
                            return &v;
                    }
                    return nullptr;
                }

                /// (AR) البحث عن فهرس حقل بالاسم عبر جميع الحالات
                ///      يُرجع أول حالة تحتوي على هذا الحقل مع فهرسه
                ///      يُستخدم لدعم الوصول بالاسم: ش.نصف_القطر
                /// (EN) Find field index by name across all variants
                ///      Returns first variant containing this field with its index
                ///      Used to support named access: s.radius
                int findFieldIndex(const std::string &fieldName) const
                {
                    for (const auto &v : variants)
                    {
                        int idx = v.findFieldIndex(fieldName);
                        if (idx >= 0)
                            return idx;
                    }
                    return -1;
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
            class SIRBuilder : public SIRBuilderContext
            {
                // ==================================================================
                // (AR) أصدقاء — sub-builders تحتاج وصولاً للحقول والـhelpers الخاصة
                // (EN) Friends — sub-builders need access to private fields/helpers
                //      أُضيفت في Phase 6 (Steps 2/3/4 وما بعدها)
                // ==================================================================
                friend class MethodCallBuilder;
                friend class BuiltinBuilder;
                friend class CallBuilder;
                friend class ClassBuilder;
                friend class StatementBuilder;
                friend class ExpressionBuilder;

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
                SIRBuilder(const SIRBuilder &) = delete;
                SIRBuilder &operator=(const SIRBuilder &) = delete;

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
                std::shared_ptr<SIRModule> buildModule(AST::ProgramNode *program);

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
                void buildFunction(AST::FunctionDeclNode *funcDecl);

                /**
                 * @brief (AR) بناء دالة قالب (حفظها للإنشاء لاحقاً)
                 * @brief (EN) Build template function (store for later instantiation)
                 *
                 * @param templateDecl (AR) تصريح دالة قالب / (EN) Template function declaration
                 *
                 * @details
                 * (AR) يحفظ الدالة القالب في جدول القوالب
                 * سيتم إنشاء نسخ منها عند الاستدعاء مع أنواع محددة
                 * (EN) Stores template function in template table
                 * Will instantiate copies when called with specific types
                 */
                void buildTemplateFunction(AST::TemplateFunctionDecl *templateDecl);

                /**
                 * @brief (AR) إنشاء نسخة محددة من دالة قالب
                 * @brief (EN) Instantiate specific version of template function
                 *
                 * @param templateName (AR) اسم القالب / (EN) Template name
                 * @param typeArguments (AR) الأنواع المحددة / (EN) Specified types
                 * @return (AR) اسم الدالة المُنشأة / (EN) Instantiated function name
                 *
                 * @details
                 * (AR) ينشئ نسخة من دالة القالب باستبدال معاملات الأنواع بأنواع محددة
                 * مثل: أكبر<رقم> ينشئ أكبر_i64
                 * (EN) Creates copy of template function with type parameters substituted
                 * Example: max<int> creates max_i64
                 */
                std::string instantiateTemplate(const std::string &templateName,
                                                const std::vector<SadTypeKind> &typeArguments);

                /**
                 * @brief (AR) إنشاء قالب مع وسائط أنواع ووسائط ثوابت (const-generics).
                 * @brief (EN) Instantiate a template with both type and const-generic args.
                 *
                 * @details
                 * (AR) [Phase 4 — Monomorphization] التوقيع الكامل:
                 *      - typeArguments: قائمة الأنواع المُمرَّرة لمعاملات typename
                 *      - constArguments: قائمة قيم Constant لمعاملات `ثابت`
                 *      اسم النسخة المُولَّد يدمج الاثنين: f_i64_4 لـ f<رقم,4>.
                 *      التخصصات تُحفظ في cache عبر هذا الاسم لتجنّب التكرار.
                 *
                 * (EN) [Phase 4] Full signature: typeArguments for `typename` params,
                 *      constArguments (each must be SIROperandType::CONSTANT) for
                 *      `const` params. Generated name combines both (e.g. f_i64_4).
                 */
                std::string instantiateTemplate(const std::string &templateName,
                                                const std::vector<SadTypeKind> &typeArguments,
                                                const std::vector<SIROperand> &constArguments);

                /**
                 * @brief (AR) بناء متغير عام
                 * @brief (EN) Build global variable
                 *
                 * @param varDecl (AR) تصريح المتغير / (EN) Variable declaration
                 */
                void buildGlobalVariable(AST::VariableDeclNode *varDecl);

                /**
                 * @brief (AR) بناء صنف
                 * @brief (EN) Build class
                 *
                 * @param classDecl (AR) تصريح الصنف / (EN) Class declaration
                 */
                void buildClass(Sad::AST::ClassDecl *classDecl)
                { classes_->buildClass(classDecl); }


                /**
                 * @brief (AR) بناء سمة/واجهة
                 * @brief (EN) Build trait/interface
                 *
                 * @param traitDecl (AR) تصريح السمة / (EN) Trait declaration
                 */
                void buildTrait(Sad::AST::TraitDecl *traitDecl)
                { classes_->buildTrait(traitDecl); }


                /**
                 * @brief (AR) بناء كتلة تنفيذ سمة
                 * @brief (EN) Build impl block
                 *
                 * @param implDecl (AR) تصريح التنفيذ / (EN) Impl declaration
                 */
                void buildImpl(Sad::AST::ImplDecl *implDecl)
                { classes_->buildImpl(implDecl); }


                // ==================================================================
                // (AR) بناء جمل الاستيراد / (EN) Building Import Statements
                // ==================================================================

                /**
                 * @brief (AR) بناء جملة استيراد كاملة: استورد وحدة
                 * @brief (EN) Build full import statement: import module
                 *
                 * @param importStmt (AR) عقدة الاستيراد / (EN) Import statement node
                 */
                void buildImportStmt(AST::ImportStmt *importStmt);

                /**
                 * @brief (AR) بناء جملة استيراد انتقائي: من وحدة استورد ...
                 * @brief (EN) Build selective import statement: from module import ...
                 *
                 * @param fromImportStmt (AR) عقدة الاستيراد الانتقائي / (EN) From-import statement node
                 */
                void buildFromImportStmt(AST::FromImportStmt *fromImportStmt);

                /**
                 * @brief (AR) تعيين مسار الملف الحالي (لحل مسارات الاستيراد النسبية)
                 * @brief (EN) Set current file path (for resolving relative import paths)
                 *
                 * @param filePath (AR) مسار الملف / (EN) File path
                 */
                void setCurrentFilePath(const std::string &filePath);

                /**
                 * @brief (AR) تعيين وضع الوحدة — تخطي إنشاء __sad_main ودالة main wrapper
                 * @brief (EN) Set module mode — skip __sad_main and main wrapper generation
                 *
                 * @param mode (AR) صحيح لتفعيل وضع الوحدة / (EN) true to enable module mode
                 */
                void setModuleMode(bool mode) { moduleMode_ = mode; }

                // ==================================================================
                // بناء الجمل / Building Statements
                // ==================================================================

                /**
                 * @brief (AR) بناء جملة - موزع للأنواع المختلفة
                 * @brief (EN) Build statement - dispatcher for different types
                 *
                 * @param stmt (AR) عقدة الجملة / (EN) Statement node
                 */
                void buildStatement(Sad::AST::Statement *stmt)
                { statements_->buildStatement(stmt); }


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
                void buildIfStatement(Sad::AST::IfStmt *ifStmt)
                { statements_->buildIfStatement(ifStmt); }


                /**
                 * @brief (AR) بناء جملة match (مطابقة أنماط)
                 * @brief (EN) Build match statement (pattern matching)
                 *
                 * @param matchStmt (AR) جملة match / (EN) Match statement
                 *
                 * @details
                 * (AR) يحول match إلى سلسلة BR_COND باستخدام SIR الموجود
                 * (EN) Lowers match to chain of BR_COND using existing SIR
                 */
                void buildMatchStatement(Sad::AST::MatchStmt *matchStmt)
                { statements_->buildMatchStatement(matchStmt); }


                /**
                 * @brief (AR) بناء حلقة while
                 * @brief (EN) Build while loop
                 *
                 * @param whileLoop (AR) حلقة while / (EN) While loop
                 */
                void buildWhileLoop(Sad::AST::WhileStmt *whileLoop)
                { statements_->buildWhileLoop(whileLoop); }


                /**
                 * @brief (AR) بناء حلقة for
                 * @brief (EN) Build for loop
                 *
                 * @param forLoop (AR) حلقة for / (EN) For loop
                 */
                void buildForLoop(Sad::AST::ForStmt *forLoop)
                { statements_->buildForLoop(forLoop); }


                /**
                 * @brief (AR) بناء حلقة for-range (لكل في)
                 * @brief (EN) Build for-range loop (foreach)
                 *
                 * @param forRange (AR) حلقة لكل في / (EN) For-range loop
                 */
                void buildForRangeLoop(Sad::AST::ForRangeStmt *forRange)
                { statements_->buildForRangeLoop(forRange); }


                /**
                 * @brief (AR) بناء جملة return
                 * @brief (EN) Build return statement
                 *
                 * @param retStmt (AR) جملة return / (EN) Return statement
                 */
                void buildReturnStatement(Sad::AST::ReturnStmt *retStmt)
                { statements_->buildReturnStatement(retStmt); }


                /**
                 * @brief (AR) بناء جملة break
                 * @brief (EN) Build break statement
                 *
                 * @param breakStmt (AR) جملة break / (EN) Break statement
                 */
                void buildBreakStatement(Sad::AST::BreakStmt *breakStmt)
                { statements_->buildBreakStatement(breakStmt); }


                /**
                 * @brief (AR) بناء جملة continue
                 * @brief (EN) Build continue statement
                 *
                 * @param continueStmt (AR) جملة continue / (EN) Continue statement
                 */
                void buildContinueStatement(Sad::AST::ContinueStmt *continueStmt)
                { statements_->buildContinueStatement(continueStmt); }


                /**
                 * @brief (AR) بناء إسناد متغير
                 * @brief (EN) Build variable assignment
                 *
                 * @param assignment (AR) جملة الإسناد / (EN) Assignment statement
                 */
                void buildAssignment(Sad::AST::AssignExpr *assignment)
                { statements_->buildAssignment(assignment); }


                /**
                 * @brief (AR) بناء إغلاق مخفي لجسم defer من جملة خام
                 * @brief (EN) Build a hidden closure for a defer body from a raw statement
                 *
                 * @param stmt (AR) الجملة المؤجلة / (EN) Deferred statement body
                 * @return (AR) الإغلاق الناتج / (EN) Resulting closure value
                 */
                BuildResult buildDeferredClosure(Sad::AST::Statement *stmt)
                { return statements_->buildDeferredClosure(stmt); }


                /**
                 * @brief (AR) تنفيذ كل الإغلاقات المؤجلة للدالة الحالية بترتيب LIFO مرة واحدة فقط
                 * @brief (EN) Execute all deferred closures for the current function in LIFO order exactly once
                 */
                void emitRunDeferredClosures()
                { statements_->emitRunDeferredClosures(); }


                /**
                 * @brief (AR) إزالة معالج تنظيف الدالة الحالي إذا كان مفعلاً
                 * @brief (EN) Pop the current function cleanup handler if active
                 */
                void emitPopFunctionCleanupHandler()
                { statements_->emitPopFunctionCleanupHandler(); }


                /**
                 * @brief (AR) بناء تصريح متغير محلي
                 * @brief (EN) Build local variable declaration
                 *
                 * @param varDecl (AR) تصريح المتغير / (EN) Variable declaration
                 */
                void buildLocalVariable(Sad::AST::VarDeclStmt *varDecl)
                { statements_->buildLocalVariable(varDecl); }


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
                BuildResult buildExpression(Sad::AST::Expression *expr)
                { return expressions_->buildExpression(expr); }


                /**
                 * @brief (AR) بناء عملية ثنائية (+ - * / % < > == && ||)
                 * @brief (EN) Build binary operation (+ - * / % < > == && ||)
                 *
                 * @param binOp (AR) عملية ثنائية / (EN) Binary operation
                 * @return (AR) نتيجة العملية / (EN) Operation result
                 */
                BuildResult buildBinaryOp(Sad::AST::BinaryExpr *binOp)
                { return expressions_->buildBinaryOp(binOp); }


                /**
                 * @brief (AR) بناء تقييم كسول (short-circuit) للعوامل المنطقية و/أو
                 * @brief (EN) Build short-circuit evaluation for logical AND/OR operators
                 *
                 * @details
                 * (AR) يُنشئ كتل SIR (basic blocks) لتقييم الطرف الأيسر أولاً،
                 *      ثم تفرع مشروط: الطرف الأيمن يُقيَّم فقط إذا لزم الأمر.
                 *      - AND: إذا كان الأيسر false → النتيجة false بدون تقييم الأيمن
                 *      - OR: إذا كان الأيسر true → النتيجة true بدون تقييم الأيمن
                 * (EN) Creates SIR basic blocks to evaluate left first, then
                 *      conditionally branches: right is only evaluated when needed.
                 *      - AND: if left is false → result is false without evaluating right
                 *      - OR: if left is true → result is true without evaluating right
                 *
                 * @param binOp (AR) عملية ثنائية منطقية (&&/||) / (EN) Logical binary operation (&&/||)
                 * @return (AR) نتيجة العملية المنطقية / (EN) Logical operation result
                 */
                BuildResult buildShortCircuitLogical(Sad::AST::BinaryExpr *binOp)
                { return expressions_->buildShortCircuitLogical(binOp); }


                /**
                 * @brief (AR) بناء عملية أحادية (- !)
                 * @brief (EN) Build unary operation (- !)
                 *
                 * @param unOp (AR) عملية أحادية / (EN) Unary operation
                 * @return (AR) نتيجة العملية / (EN) Operation result
                 */
                BuildResult buildUnaryOp(Sad::AST::UnaryExpr *unOp)
                { return expressions_->buildUnaryOp(unOp); }


                /**
                 * @brief (AR) بناء استدعاء دالة
                 * @brief (EN) Build function call
                 *
                 * @param call (AR) استدعاء دالة / (EN) Function call
                 * @return (AR) قيمة الإرجاع / (EN) Return value
                 */
                BuildResult buildFunctionCall(Sad::AST::CallExpr *call)
                { return calls_->buildFunctionCall(call); }


                // ── دوال مساعدة مستخرجة من buildFunctionCall (CW-05, CW-03) ──
                // ── Helper methods extracted from buildFunctionCall (CW-05, CW-03) ──

                /**
                 * @brief (AR) معالجة توسيع استدعاء الماكرو — يُرجع nullopt إذا لم يكن ماكرو
                 * @brief (EN) Handle macro call expansion — returns nullopt if not a macro call
                 */
                std::optional<BuildResult> buildMacroCallExpansion(Sad::AST::CallExpr *call, const std::string &funcName)
                { return calls_->buildMacroCallExpansion(call, funcName); }


                /**
                 * @brief (AR) معالجة استدعاء الكائن القابل للاستدعاء operator() — يُرجع nullopt إذا لم ينطبق
                 * @brief (EN) Handle callable object operator() invocation — returns nullopt if not applicable
                 */
                std::optional<BuildResult> buildCallableObjectInvoke(Sad::AST::CallExpr *call, const std::string &funcName)
                { return calls_->buildCallableObjectInvoke(call, funcName); }


                /**
                 * @brief (AR) معالجة استدعاء باني الأب أساس/الأساس/super — يُرجع nullopt إذا لم ينطبق
                 * @brief (EN) Handle super constructor call أساس/الأساس/super — returns nullopt if not applicable
                 */
                std::optional<BuildResult> buildSuperConstructorCall(Sad::AST::CallExpr *call, const std::string &funcName)
                { return calls_->buildSuperConstructorCall(call, funcName); }


                /**
                 * @brief (AR) بناء قائمة الوسائط لاستدعاء دالة عادي
                 * @brief (EN) Build argument list for regular function call
                 * @param outArgOperands (AR) الوسائط بصيغة SIROperand / (EN) Arguments as SIROperand
                 * @param outArgResults  (AR) نتائج تقييم الوسائط / (EN) Argument evaluation results
                 * @return (AR) true = نجح / false = فشل (خطأ مسجّل في errors_)
                 * @return (EN) true = success / false = failure (error recorded in errors_)
                 */
                bool buildCallArgumentsList(Sad::AST::CallExpr *call, std::vector<SIROperand> &outArgOperands, std::vector<BuildResult> &outArgResults)
                { return calls_->buildCallArgumentsList(call, outArgOperands, outArgResults); }


                /**
                 * @brief (AR) تعبئة قيم الوسائط الافتراضية الناقصة
                 * @brief (EN) Fill in missing default argument values
                 */
                void fillDefaultCallArguments(Sad::AST::CallExpr *call, const std::string &funcName, std::vector<SIROperand> &argOperands, std::vector<BuildResult> &argResults)
                { calls_->fillDefaultCallArguments(call, funcName, argOperands, argResults); }


                /**
                 * @brief (AR) معالجة استدعاء دالة مدمجة أساسية
                 * @brief (EN) Handle core builtin function call (type conv, print, math, string, array, file)
                 */
                std::optional<BuildResult> buildBuiltinCallCore(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinCallCore(funcName, isUserDefinedFunction, argResults, argOperands); }


                /**
                 * @brief (AR) معالجة استدعاءات SIMD للمتجهات (Phase 3)
                 *        متجه() للبناء، متجه_بث، متجه_جمع_عرضي، متجه_جذر، متجه_مطلق،
                 *        متجه_أصغر، متجه_أكبر، متجه_ضرب_جمع (FMA)، متجه_جداء_قياسي، إلخ.
                 * @brief (EN) Handle SIMD vector builtin calls (Phase 3)
                 *        vector(), splat, hsum, sqrt, abs, min, max, fma, dot, etc.
                 */
                std::optional<BuildResult> buildBuiltinCallSimd(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinCallSimd(funcName, isUserDefinedFunction, argResults, argOperands); }


                /**
                 * @brief (AR) معالجة استدعاء دالة مدمجة للنظام
                 * @brief (EN) Handle system builtin function call (hardware, GPIO, timer, atomic, async, security)
                 */
                std::optional<BuildResult> buildBuiltinCallSystem(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinCallSystem(funcName, isUserDefinedFunction, argResults, argOperands); }


                /**
                 * @brief (AR) معالجة استدعاء دالة مدمجة للشبكة
                 *        مقابس TCP/UDP، عميل HTTP، خادم HTTP، أدوات الشبكة، العناوين
                 * @brief (EN) Handle network builtin function call
                 *        TCP/UDP sockets, HTTP client, HTTP server, network utilities, addresses
                 */
                std::optional<BuildResult> buildBuiltinCallNetwork(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinCallNetwork(funcName, isUserDefinedFunction, argResults, argOperands); }


                /**
                 * @brief (AR) دالة مساعدة: بناء تعليمة SIR لدالة شبكة
                 * @brief (EN) Helper: build SIR instruction for a network function
                 * @param opcode (AR) رمز العملية / (EN) SIR opcode
                 * @param argOperands (AR) المعاملات / (EN) Operands
                 * @param returnType (AR) نوع القيمة المُرجعة / (EN) Return type
                 * @param comment (AR) تعليق التعليمة / (EN) Instruction comment
                 */
                BuildResult buildNetworkBuiltinInstruction(SIROpcode opcode, std::vector<SIROperand> &argOperands, SadTypeKind returnType, const char *comment)
                { return builtins_->buildNetworkBuiltinInstruction(opcode, argOperands, returnType, comment); }


                /**
                 * @brief (AR) بناء إنشاء كائن جديد
                 * @brief (EN) Build new object creation
                 *
                 * @param newExpr (AR) تعبير جديد / (EN) New expression
                 * @return (AR) مؤشر للكائن / (EN) Pointer to object
                 */
                BuildResult buildNewObject(Sad::AST::NewExpr *expr)
                { return expressions_->buildNewObject(expr); }


                /**
                 * @brief (AR) بناء الوصول لعضو في كائن
                 * @brief (EN) Build member access
                 *
                 * @param memberExpr (AR) تعبير الوصول للعضو / (EN) Member access expression
                 * @return (AR) قيمة العضو / (EN) Member value
                 */
                BuildResult buildMemberAccess(Sad::AST::MemberAccessExpr *expr)
                { return expressions_->buildMemberAccess(expr); }


                /**
                 * @brief (AR) بناء استدعاء طريقة على كائن
                 * @brief (EN) Build method call on object
                 *
                 * @param methodCallExpr (AR) تعبير استدعاء الطريقة / (EN) Method call expression
                 * @return (AR) نتيجة الاستدعاء / (EN) Call result
                 */
                BuildResult buildMethodCall(Sad::AST::MethodCallExpr *methodCallExpr)
                { return calls_->buildMethodCall(methodCallExpr); }


                // ================================================================
                // (AR) دوال مساعدة لـ buildMethodCall — مستخرجة وفق CW-05/CW-01
                //      كل مجموعة طرق في ملف مستقل لتسهيل الصيانة
                // (EN) Helper functions for buildMethodCall — extracted per CW-05/CW-01
                //      Each method group in a separate file for maintainability
                // ================================================================
                // ==================================================================
                // (AR) Phase 6 — Step 2: استدعاءات الطرق على الأنواع المدمجة
                //      نُقلت إلى class MethodCallBuilder. الـ8 methods أدناه
                //      هي wrappers inline تُفوّض إلى methodCalls_.
                // (EN) Phase 6 — Step 2: built-in type method calls were moved
                //      to class MethodCallBuilder. The 8 inline wrappers below
                //      delegate to methodCalls_.
                // ==================================================================

                std::optional<BuildResult> buildChannelMethodCall(AST::MethodCallExpr *expr, const BuildResult &objResult)
                { return methodCalls_->buildChannelMethodCall(expr, objResult); }

                std::optional<BuildResult> buildMutexMethodCall(AST::MethodCallExpr *expr, const BuildResult &objResult)
                { return methodCalls_->buildMutexMethodCall(expr, objResult); }

                std::optional<BuildResult> buildFutureMethodCall(AST::MethodCallExpr *expr, const BuildResult &objResult)
                { return methodCalls_->buildFutureMethodCall(expr, objResult); }

                std::optional<BuildResult> buildWaitGroupMethodCall(AST::MethodCallExpr *expr, const BuildResult &objResult)
                { return methodCalls_->buildWaitGroupMethodCall(expr, objResult); }

                std::optional<BuildResult> buildArrayBasicMethodCall(const BuildResult &objResult,
                                                                     const std::string &methodName, const std::vector<SIROperand> &args)
                { return methodCalls_->buildArrayBasicMethodCall(objResult, methodName, args); }

                std::optional<BuildResult> buildArrayHigherOrderMethodCall(const BuildResult &objResult,
                                                                           const std::string &methodName, const std::vector<SIROperand> &args,
                                                                           const std::string &closureLambdaName, SadTypeKind closureRetType)
                { return methodCalls_->buildArrayHigherOrderMethodCall(objResult, methodName, args, closureLambdaName, closureRetType); }

                std::optional<BuildResult> buildStringBuiltinMethodCall(const BuildResult &objResult,
                                                                        const std::string &methodName, const std::vector<SIROperand> &args)
                { return methodCalls_->buildStringBuiltinMethodCall(objResult, methodName, args); }

                std::optional<BuildResult> buildMapBuiltinMethodCall(const BuildResult &objResult,
                                                                     const std::string &methodName, const std::vector<SIROperand> &args)
                { return methodCalls_->buildMapBuiltinMethodCall(objResult, methodName, args); }


                // ================================================================
                // (AR) فحص استيراد الوحدات القياسية — توحيد سلوك المترجم مع المفسر
                // (EN) Stdlib module import checking — unify compiler behavior with interpreter
                // ================================================================

                /**
                 * @brief (AR) تحقق من استيراد وحدة قياسية (مع دعم الأسماء المستعارة)
                 * @brief (EN) Check if a stdlib module is imported (with alias resolution)
                 * @param moduleName اسم الوحدة الأساسي (مثال: "نصوص", "رياضيات", "أساسيات")
                 * @return صحيح إذا تم استيراد الوحدة أو أي من أسمائها المستعارة
                 */
                bool isStdlibModuleImported(const std::string &moduleName) const;

                /**
                 * @brief (AR) تحديد الوحدة المطلوبة لدالة مضمنة (وفق سلوك المفسر)
                 * @brief (EN) Determine required module for a builtin function (matching interpreter behavior)
                 * @param funcName اسم الدالة المضمنة
                 * @return اسم الوحدة المطلوبة، أو سلسلة فارغة إذا كانت أساسية (لا تحتاج استيراد)
                 */
                static std::string getRequiredModuleForBuiltin(const std::string &funcName);

                // ================================================================
                // (AR) دوال مساعدة لـ buildBuiltinCallCore — مستخرجة وفق CW-05
                // (EN) Helper functions for buildBuiltinCallCore — extracted per CW-05
                // ================================================================

                /**
                 * @brief (AR) بناء دوال الرياضيات المضمنة (جذر/أس/لوغاريتم/sin/cos/أكبر/أصغر...)
                 * @brief (EN) Build math builtin calls (sqrt/pow/log/sin/cos/max/min...)
                 */
                std::optional<BuildResult> buildBuiltinMathCall(const std::string &funcName, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinMathCall(funcName, argResults, argOperands); }


                /**
                 * @brief (AR) بناء دوال النصوص والمصفوفات المضمنة (طول_نص/تقسيم/إضافة_عنصر/ترتيب...)
                 * @brief (EN) Build string & array builtin calls (string_length/split/append/sort...)
                 */
                std::optional<BuildResult> buildBuiltinStringArrayCall(const std::string &funcName, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinStringArrayCall(funcName, argResults, argOperands); }


                /**
                 * @brief (AR) بناء دوال الملفات والأدوات وفحص الأنواع (اقرأ_ملف/عشوائي/هو_رقم...)
                 * @brief (EN) Build file I/O, utility, and type-checking builtins
                 */
                std::optional<BuildResult> buildBuiltinIOUtilsCall(const std::string &funcName, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinIOUtilsCall(funcName, argResults, argOperands); }


                /**
                 * @brief (AR) بناء شرط نمط match — يُرجع اسم سجل الشرط
                 * @brief (EN) Build match pattern condition — returns condition register name
                 * @param pattern النمط AST
                 * @param matchValueReg سجل القيمة المُطابقة
                 * @param matchValueType نوع القيمة المُطابقة
                 * @param caseIndex فهرس الحالة (للتشخيص)
                 * @param deferredExtractions مخرج: حقول ADT المؤجل استخراجها
                 */
                std::string buildMatchPatternCondition(
                    const AST::PatternNode *pattern,
                    const std::string &matchValueReg,
                    SadTypeKind matchValueType,
                    size_t caseIndex,
                    std::vector<MatchDeferredField> &deferredExtractions);

                /**
                 * @brief (AR) بناء باني صنف — مستخرج من buildClass (CW-05)
                 * @brief (EN) Build class constructor — extracted from buildClass
                 */
                void buildClassConstructor(Sad::AST::ClassDecl *classDecl, std::shared_ptr<SIRClass> sirClass, Sad::AST::ConstructorDecl *ctorDecl)
                { classes_->buildClassConstructor(classDecl, sirClass, ctorDecl); }


                /**
                 * @brief (AR) بناء عامل زائد في صنف — مستخرج من buildClass (CW-05)
                 * @brief (EN) Build class operator overload — extracted from buildClass
                 */
                void buildClassOperator(Sad::AST::ClassDecl *classDecl, std::shared_ptr<SIRClass> sirClass, Sad::AST::OperatorDecl *operatorDecl)
                { classes_->buildClassOperator(classDecl, sirClass, operatorDecl); }


                /**
                 * @brief (AR) بناء وصول لمتغير
                 * @brief (EN) Build variable access
                 *
                 * @param var (AR) متغير / (EN) Variable
                 * @return (AR) قيمة المتغير / (EN) Variable value
                 */
                BuildResult buildVariableAccess(Sad::AST::VariableExpr *varNode)
                { return expressions_->buildVariableAccess(varNode); }


                /**
                 * @brief (AR) بناء ثابت (رقم، نص، منطقي)
                 * @brief (EN) Build literal (number, string, boolean)
                 *
                 * @param literal (AR) ثابت / (EN) Literal
                 * @return (AR) القيمة الثابتة / (EN) Constant value
                 */
                BuildResult buildLiteral(Sad::AST::LiteralExpr *literal)
                { return expressions_->buildLiteral(literal); }


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
                BuildResult buildStringMethod(AST::MethodCallNode *methodCall);

                // ==================================================================
                // دوال بناء التعابير المستخرجة / Extracted Expression Builders
                // ==================================================================

                /// @brief (AR) بناء تعبير MemberExpr (وصول للعضو) / (EN) Build MemberExpr
                BuildResult buildExprMember(Sad::AST::MemberExpr *expr)
                { return expressions_->buildExprMember(expr); }


                /// @brief (AR) بناء تعبير MemberAssignExpr / (EN) Build MemberAssignExpr
                BuildResult buildExprMemberAssign(Sad::AST::MemberAssignExpr *expr)
                { return expressions_->buildExprMemberAssign(expr); }


                /// @brief (AR) بناء تعبير TernaryExpr / (EN) Build TernaryExpr
                BuildResult buildExprTernary(Sad::AST::TernaryExpr *expr)
                { return expressions_->buildExprTernary(expr); }


                /// @brief (AR) بناء تعبير IndexExpr / (EN) Build IndexExpr
                BuildResult buildExprIndex(Sad::AST::IndexExpr *expr)
                { return expressions_->buildExprIndex(expr); }


                /// @brief (AR) بناء تعبير IndexAssignExpr / (EN) Build IndexAssignExpr
                BuildResult buildExprIndexAssign(Sad::AST::IndexAssignExpr *expr)
                { return expressions_->buildExprIndexAssign(expr); }


                /// @brief (AR) بناء تعبير ArrayExpr / (EN) Build ArrayExpr
                BuildResult buildExprArray(Sad::AST::ArrayExpr *expr)
                { return expressions_->buildExprArray(expr); }


                /// @brief (AR) بناء تعبير TupleExpr / (EN) Build TupleExpr
                BuildResult buildExprTuple(Sad::AST::TupleExpr *expr)
                { return expressions_->buildExprTuple(expr); }


                /// @brief (AR) بناء تعبير MapExpr / (EN) Build MapExpr
                BuildResult buildExprMap(Sad::AST::MapExpr *expr)
                { return expressions_->buildExprMap(expr); }


                /// @brief (AR) بناء تعبير WalrusExpr / (EN) Build WalrusExpr
                BuildResult buildExprWalrus(Sad::AST::WalrusExpr *expr)
                { return expressions_->buildExprWalrus(expr); }


                /// @brief (AR) بناء تعبير LambdaExpr / (EN) Build LambdaExpr
                BuildResult buildExprLambda(Sad::AST::LambdaExpr *expr)
                { return expressions_->buildExprLambda(expr); }


                /// @brief (AR) بناء تعبير RangeExpr / (EN) Build RangeExpr
                BuildResult buildExprRange(Sad::AST::RangeExpr *expr)
                { return expressions_->buildExprRange(expr); }


                /// @brief (AR) بناء تعبير ListComprehensionExpr / (EN) Build ListComprehensionExpr
                BuildResult buildExprListComp(Sad::AST::ListComprehensionExpr *expr)
                { return expressions_->buildExprListComp(expr); }


                /// @brief (AR) بناء تعبير DictComprehensionExpr / (EN) Build DictComprehensionExpr
                BuildResult buildExprDictComp(Sad::AST::DictComprehensionExpr *expr)
                { return expressions_->buildExprDictComp(expr); }


                /// @brief (AR) بناء تعبير SetComprehensionExpr / (EN) Build SetComprehensionExpr
                BuildResult buildExprSetComp(Sad::AST::SetComprehensionExpr *expr)
                { return expressions_->buildExprSetComp(expr); }


                /// @brief (AR) بناء تعبير GeneratorExpr / (EN) Build GeneratorExpr
                BuildResult buildExprGenerator(Sad::AST::GeneratorExpr *expr)
                { return expressions_->buildExprGenerator(expr); }


                /// @brief (AR) بناء تعبير InlineAsmExpr / (EN) Build InlineAsmExpr
                BuildResult buildExprInlineAsm(Sad::AST::InlineAsmExpr *expr)
                { return expressions_->buildExprInlineAsm(expr); }


                /// @brief (AR) بناء تعبير SizeofExpr / (EN) Build SizeofExpr
                BuildResult buildExprSizeof(Sad::AST::SizeofExpr *expr)
                { return expressions_->buildExprSizeof(expr); }


                /// @brief (AR) بناء تعبير AtomicExpr / (EN) Build AtomicExpr
                BuildResult buildExprAtomic(Sad::AST::AtomicExpr *expr)
                { return expressions_->buildExprAtomic(expr); }


                /// @brief (AR) بناء تعبير OptionalChainExpr / (EN) Build OptionalChainExpr
                BuildResult buildExprOptionalChain(Sad::AST::OptionalChainExpr *expr)
                { return expressions_->buildExprOptionalChain(expr); }


                /// @brief (AR) بناء تعبير NullCoalesceExpr / (EN) Build NullCoalesceExpr
                BuildResult buildExprNullCoalesce(Sad::AST::NullCoalesceExpr *expr)
                { return expressions_->buildExprNullCoalesce(expr); }


                /// @brief (AR) بناء تعبير SliceExpr / (EN) Build SliceExpr
                BuildResult buildExprSlice(Sad::AST::SliceExpr *expr)
                { return expressions_->buildExprSlice(expr); }


                /// @brief (AR) بناء تعبير ErrorPropagateExpr / (EN) Build ErrorPropagateExpr
                BuildResult buildExprErrorPropagate(Sad::AST::ErrorPropagateExpr *expr)
                { return expressions_->buildExprErrorPropagate(expr); }


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
                std::string newLabel(const std::string &prefix = "L");

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
                std::shared_ptr<SIRBasicBlock> createBasicBlock(const std::string &name);

                // (AR) ملاحظة Phase 6: حُذفت setCurrentBlock/getCurrentBlock/addInstruction
                //      (إعلانات بلا تنفيذ ولا استخدام). الكود يصل للحقول مباشرة.
                // (EN) Phase 6 note: removed setCurrentBlock/getCurrentBlock/addInstruction
                //      (declarations without implementation or use). Code accesses fields directly.

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
                void addVariable(const VariableInfo &varInfo);

                /**
                 * @brief (AR) البحث عن متغير في النطاقات
                 * @brief (EN) Lookup variable in scopes
                 *
                 * @param name (AR) اسم المتغير / (EN) Variable name
                 * @return (AR) معلومات المتغير أو nullptr / (EN) Variable info or nullptr
                 */
                VariableInfo *lookupVariable(const std::string &name);

                // ==================================================================
                // إدارة الحلقات / Loop Management
                // ==================================================================

                /**
                 * @brief (AR) دخول سياق حلقة
                 * @brief (EN) Enter loop context
                 *
                 * @param ctx (AR) سياق الحلقة / (EN) Loop context
                 */
                void enterLoop(const LoopContext &ctx);

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
                LoopContext *getCurrentLoop();

                // ==================================================================
                // إدارة الأنواع العامة / Generic Types Management
                // ==================================================================

                /**
                 * @brief (AR) دخول نطاق أنواع عامة (عند تعريف دالة أو صنف عام)
                 * @brief (EN) Enter generic types scope (when defining generic function/class)
                 *
                 * @param typeParams (AR) قائمة معاملات الأنواع / (EN) Type parameters list
                 */
                void enterGenericScope(const std::vector<TypeParameter> &typeParams);

                /**
                 * @brief (AR) الخروج من نطاق الأنواع العامة
                 * @brief (EN) Exit generic types scope
                 */
                void exitGenericScope();

                // (AR) ملاحظة Phase 6: حُذفت setTypeSubstitution/resolveType/validateTypeParameters
                //      (إعلانات بلا تنفيذ ولا استخدام). أي كود يحتاجها يستخدم
                //      genericScopeStack_.back().typeSubstitutions[name] = type مباشرة.
                // (EN) Phase 6 note: removed setTypeSubstitution/resolveType/validateTypeParameters
                //      (unimplemented and unused). Code uses
                //      genericScopeStack_.back().typeSubstitutions[name] = type directly.

                // ==================================================================
                // معالجة الأخطاء / Error Handling
                // ==================================================================

                // (AR) ملاحظة Phase 6: حُذفت reportError(message) — الكود يضيف
                //      مباشرة إلى errors_.push_back(...) في 67 ملف.
                // (EN) Phase 6 note: removed reportError(message) — code uses
                //      errors_.push_back(...) directly across 67 files.

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
                const std::vector<std::string> &getErrors() const { return errors_; }

            private:
                // ==================================================================
                // الحالة الداخلية / Internal State
                // ==================================================================
                //
                // (AR) Phase 6 — Step 1: جميع حقول الحالة والـstructs الداخلية
                //      (FinallyContext, SIRBuilderContextState, NamespaceMemberInfo,
                //      CaptureInfo, ModuleSIRArtifacts) ودوال saveContext/restoreContext
                //      نُقلت إلى struct SIRBuilderContext في sir_builder_context.h.
                //      SIRBuilder الآن يرث منها (`class SIRBuilder : public SIRBuilderContext`)
                //      فيبقى الوصول المباشر للحقول من جميع ملفات sir_builder_*.cpp يعمل
                //      دون أي تغيير.
                // (EN) Phase 6 — Step 1: all state fields and inner structs were moved
                //      to struct SIRBuilderContext in sir_builder_context.h. SIRBuilder
                //      inherits from it; direct field access keeps working unchanged.
                // ==================================================================

                // ==================================================================
                // (AR) Phase 6 — Step 2: بنّاء استدعاءات الطرق المنفصل
                // (EN) Phase 6 — Step 2: separated method-call builder
                // ==================================================================
                std::unique_ptr<MethodCallBuilder> methodCalls_;

                // ==================================================================
                // (AR) Phase 6 — Step 3: بنّاء الدوال المدمجة المنفصل
                // (EN) Phase 6 — Step 3: separated builtin function builder
                // ==================================================================
                std::unique_ptr<BuiltinBuilder> builtins_;

                // ==================================================================
                // (AR) Phase 6 — Step 4: بنّاء استدعاءات الدوال المنفصل
                // (EN) Phase 6 — Step 4: separated function call builder
                // ==================================================================
                std::unique_ptr<CallBuilder> calls_;

                // ==================================================================
                // (AR) Phase 6 — Step 5: بنّاء الأصناف والسمات المنفصل
                // (EN) Phase 6 — Step 5: separated class/trait builder
                // ==================================================================
                std::unique_ptr<ClassBuilder> classes_;

                // ==================================================================
                // (AR) Phase 6 — Step 6: بنّاء العبارات المنفصل
                // (EN) Phase 6 — Step 6: separated statement builder
                // ==================================================================
                std::unique_ptr<StatementBuilder> statements_;

                // ==================================================================
                // (AR) Phase 6 — Step 7: بنّاء التعابير المنفصل
                // (EN) Phase 6 — Step 7: separated expression builder
                // ==================================================================
                std::unique_ptr<ExpressionBuilder> expressions_;

                /**
                 * @brief (AR) تجميع وحدة وحفظها في الذاكرة المخبئية
                 *        (EN) Compile module and save to cache
                 * @param fullModuleName (AR) الاسم الكامل / (EN) Full module name
                 * @param module (AR) الوحدة المحملة / (EN) Loaded module
                 * @return (AR) مؤشر لنتائج SIR المخبئية / (EN) Pointer to cached SIR artifacts
                 */
                ModuleSIRArtifacts *compileAndCacheModule(const std::string &fullModuleName,
                                                          Modules::Module *module);

                /**
                 * @brief (AR) دمج نتائج SIR المخبئية في الوحدة الحالية
                 *        (EN) Merge cached SIR artifacts into current module
                 * @param artifacts (AR) النتائج المخبئية / (EN) Cached artifacts
                 * @param filter (AR) مرشح الرموز (فارغ = كل شيء) / (EN) Symbol filter (empty = all)
                 */
                void mergeCachedArtifacts(const ModuleSIRArtifacts &artifacts,
                                          const std::unordered_set<std::string> &filter = {});

                // ==================================================================
                // دوال مساعدة خاصة / Private Helper Functions
                // ==================================================================

                /**
                 * @brief (AR) تحويل نوع AST إلى SadTypeKind
                 * @brief (EN) Convert AST Type to SadTypeKind
                 */
                SadTypeKind astTypeToSIRType(const Sad::Data::DataType &astType);

                /**
                 * @brief (AR) تحويل نوع AST إلى SadTypePtr (النظام الموحد)
                 * @brief (EN) Convert AST DataType to SadTypePtr (unified type system)
                 * @param astType نوع AST / AST DataType
                 * @return مؤشر SadType الموحد / Unified SadType pointer
                 */
                Sad::Types::SadTypePtr astTypeToSadType(const Sad::Data::DataType &astType);

                /**
                 * @brief (AR) استنتاج نوع الإرجاع من جسم الدالة مع تتبع أنواع المتغيرات
                 * @brief (EN) Infer return type from function body with local variable type tracking
                 * @param body جسم الدالة / Function body
                 * @param funcDecl تصريح الدالة (اختياري) لتتبع أنواع المعاملات / Function declaration (optional) for parameter type tracking
                 * @return نوع الإرجاع المُستنتج / Inferred return type
                 *
                 * هذه الدالة تمسح جسم الدالة تسلسلياً لتتبع أنواع المتغيرات المحلية
                 * ثم تجمع أنواع جميع عبارات الإرجاع وتوحّدها
                 */
                SadTypeKind inferReturnTypeFromBody(const Sad::AST::Statement *body,
                                                    const Sad::AST::FunctionDecl *funcDecl = nullptr);

                /**
                 * @brief (AR) فحص إذا كانت الجملة تحتوي return مع قيمة (تعاودي)
                 * @brief (EN) Check if statement contains return with value (recursive)
                 */
                bool hasReturnWithValue(const Sad::AST::Statement *stmt);

                /**
                 * @brief (AR) استنتاج نوع SIR من تعبير AST (بدون بناء SIR)
                 * @brief (EN) Infer SadTypeKind from AST expression (without building SIR)
                 * @param expr التعبير / Expression to analyze
                 * @return نوع SIR المُستنتج / Inferred SIR type
                 */
                SadTypeKind inferExprType(const Sad::AST::Expression *expr);

                /**
                 * @brief (AR) مسح مواقع الاستدعاء لاستنتاج أنواع المعاملات غير المحددة
                 * @brief (EN) Scan call sites to infer types for untyped parameters
                 * @param program البرنامج الكامل / Full program AST
                 *
                 * يمسح جميع CallExpr في البرنامج ويحدّث functionTable_ عندما
                 * يكون المعامل I64 (من DataType::UNKNOWN) والوسيط الفعلي STRING/F64/BOOL
                 */
                void inferParamTypesFromCallSites(AST::ProgramNode *program);

                /**
                 * @brief (AR) استنتاج أنواع معاملات اللامدا من تحليل جسمها وسياق الاستدعاء
                 * @brief (EN) Infer lambda parameter types from body analysis and call context
                 * @param lambdaExpr عقدة اللامدا في AST / Lambda AST node
                 * @param paramNames أسماء المعاملات / Parameter names set
                 * @return خريطة اسم_معامل → نوع_SIR / Map of param_name → SIR type
                 *
                 * (AR) تحلل جسم اللامدا (تعبير أو كتلة) لاستنتاج أنواع المعاملات:
                 *      - إذا استُخدم المعامل في عملية + مع نص → String
                 *      - إذا مُرر لدالة معروفة النوع → نوع معامل الدالة
                 *      - إذا استُخدم في طرق نصية (طول، يحتوي، ...) → String
                 *      - إذا استُخدم في عملية حسابية فقط → Integer (الافتراضي)
                 *      كما تفحص AST Parameter::type إذا حدده المبرمج صراحة
                 */
                std::unordered_map<std::string, SadTypeKind> inferLambdaParamTypes(
                    AST::LambdaExpr *lambdaExpr,
                    const std::set<std::string> &paramNames);

                /**
                 * @brief (AR) تحليل تعبير لاستنتاج أنواع معاملات اللامدا من الاستخدام
                 * @brief (EN) Analyze expression to infer lambda param types from usage
                 */
                void inferLambdaParamFromExpr(
                    const Sad::AST::Expression *expr,
                    const std::set<std::string> &paramNames,
                    std::unordered_map<std::string, SadTypeKind> &result);

                /**
                 * @brief (AR) تحليل جملة لاستنتاج أنواع معاملات اللامدا من الاستخدام
                 * @brief (EN) Analyze statement to infer lambda param types from usage
                 */
                void inferLambdaParamFromStmt(
                    const Sad::AST::Statement *stmt,
                    const std::set<std::string> &paramNames,
                    std::unordered_map<std::string, SadTypeKind> &result);

                /**
                 * @brief (AR) مسح تعاودي للجمل للبحث عن استدعاءات الدوال
                 * @brief (EN) Recursively scan statements for function calls
                 */
                void scanCallSitesInStmt(const Sad::AST::Statement *stmt);

                /**
                 * @brief (AR) مسح تعاودي للتعبيرات للبحث عن استدعاءات الدوال
                 * @brief (EN) Recursively scan expressions for function calls
                 */
                void scanCallSitesInExpr(const Sad::AST::Expression *expr);

                /**
                 * @brief (AR) تحويل عامل ثنائي AST إلى SIR opcode
                 * @brief (EN) Convert AST binary operator to SIR opcode
                 */
                SIROpcode binaryOpToOpcode(AST::BinaryOperator op, SadTypeKind leftType);

                /**
                 * @brief (AR) تحويل عامل أحادي AST إلى SIR opcode
                 * @brief (EN) Convert AST unary operator to SIR opcode
                 */
                SIROpcode unaryOpToOpcode(AST::UnaryOperator op, SadTypeKind operandType);

                // (AR) ملاحظة Phase 6: حُذفت areTypesCompatible (إعلان بلا تنفيذ ولا استخدام)
                // (EN) Phase 6 note: removed areTypesCompatible (declared but never defined or used)

                /**
                 * @brief (AR) جمع المتغيرات الحرة في تعبير (لاكتشاف التقاطات الإغلاقات)
                 * @brief (EN) Collect free variables in an expression (for closure capture detection)
                 */
                void collectFreeVarsExpr(Sad::AST::Expression *expr,
                                         const std::set<std::string> &boundNames,
                                         std::set<std::string> &freeVars);

                /**
                 * @brief (AR) جمع المتغيرات الحرة في جملة (تعاودي)
                 * @brief (EN) Collect free variables in a statement (recursive)
                 */
                void collectFreeVarsStmt(Sad::AST::Statement *stmt,
                                         std::set<std::string> &boundNames,
                                         std::set<std::string> &freeVars);

                // (AR) ملاحظة Phase 6: حُذفت convertType (إعلان بلا تنفيذ ولا استخدام)
                // (EN) Phase 6 note: removed convertType (declared but never defined or used)

                // ── دوال فرعية لتقسيم buildBuiltinCallSystem ──
                std::optional<BuildResult> buildBuiltinSystem_Embedded(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinSystem_Embedded(funcName, isUserDefinedFunction, argResults, argOperands); }
                std::optional<BuildResult> buildBuiltinSystem_Security(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinSystem_Security(funcName, isUserDefinedFunction, argResults, argOperands); }

                std::optional<BuildResult> buildBuiltinSystem_FFI(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinSystem_FFI(funcName, isUserDefinedFunction, argResults, argOperands); }

                std::optional<BuildResult> buildBuiltinSystem_Async(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinSystem_Async(funcName, isUserDefinedFunction, argResults, argOperands); }
                std::optional<BuildResult> buildBuiltinSystem_OsCore(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinSystem_OsCore(funcName, isUserDefinedFunction, argResults, argOperands); }
                std::optional<BuildResult> buildBuiltinSystem_OsHardware(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinSystem_OsHardware(funcName, isUserDefinedFunction, argResults, argOperands); }
                std::optional<BuildResult> buildBuiltinSystem_OsSystem(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinSystem_OsSystem(funcName, isUserDefinedFunction, argResults, argOperands); }
                std::optional<BuildResult> buildBuiltinSystem_Uefi(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinSystem_Uefi(funcName, isUserDefinedFunction, argResults, argOperands); }
                std::optional<BuildResult> buildBuiltinSystem_UI(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                { return builtins_->buildBuiltinSystem_UI(funcName, isUserDefinedFunction, argResults, argOperands); }


                // ── دوال فرعية لتقسيم buildStatement ──
                bool buildStatement_Exceptions(Sad::AST::Statement *stmt)
                { return statements_->buildStatement_Exceptions(stmt); }
                bool buildStatement_Generators(Sad::AST::Statement *stmt)
                { return statements_->buildStatement_Generators(stmt); }
                bool buildStatement_Types(Sad::AST::Statement *stmt)
                { return statements_->buildStatement_Types(stmt); }

                bool buildStatement_Advanced(Sad::AST::Statement *stmt)
                { return statements_->buildStatement_Advanced(stmt); }


                // ── دوال مساعدة مستخرجة من buildStatement_Advanced (CW-05, CW-03) ──
                /**
                 * @brief (AR) معالجة TestDecl — اختبار عادي واختبار خصائص
                 * @brief (EN) Handle TestDecl — normal test and property-based test
                 */
                bool buildStatement_Test(Sad::AST::Statement *stmt)
                { return statements_->buildStatement_Test(stmt); }


                /**
                 * @brief (AR) معالجة PropertyDecl — getter/setter
                 * @brief (EN) Handle PropertyDecl — getter/setter lowering
                 */
                bool buildStatement_Property(Sad::AST::Statement *stmt)
                { return statements_->buildStatement_Property(stmt); }


                /**
                 * @brief (AR) معالجة GoStmt — goroutine (أطلق)
                 * @brief (EN) Handle GoStmt — concurrent goroutine spawn
                 */
                bool buildStatement_Go(Sad::AST::Statement *stmt)
                { return statements_->buildStatement_Go(stmt); }


                /**
                 * @brief (AR) معالجة SelectStmt — اختر من قنوات متعددة
                 * @brief (EN) Handle SelectStmt — channel select multiplexer
                 */
                bool buildStatement_Select(Sad::AST::Statement *stmt)
                { return statements_->buildStatement_Select(stmt); }


                /**
                 * @brief (AR) معالجة ExtensionDecl — امتداد صنف موجود
                 * @brief (EN) Handle ExtensionDecl — extension methods for existing class
                 */
                bool buildStatement_Extension(Sad::AST::Statement *stmt)
                { return statements_->buildStatement_Extension(stmt); }


                /**
                 * @brief (AR) معالجة TypeAliasDecl + ReExportStmt — أسماء مستعارة وإعادة تصدير
                 * @brief (EN) Handle TypeAliasDecl + ReExportStmt — type aliases and re-exports
                 */
                bool buildStatement_TypesAndReExport(AST::Statement *stmt);
            };

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
