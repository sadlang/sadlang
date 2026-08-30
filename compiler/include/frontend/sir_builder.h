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
#include "builders/template_builder.h"
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
            // (AR) فاصلُ فضاءِ أسماءِ المواقعِ الداخليّة (ISSUE-110)
            //      كلُّ موقعٍ يُصدرُه المولّدُ لحسابِه — عدّادُ حلقةٍ · فهرسُ تعدادٍ ·
            //      رصّةُ «أجّل» · مواقعُ الالتقاط — يجبُ أن يقعَ في فضاءِ أسماءٍ
            //      **لا يستطيعُ المستخدمُ تهجئتَه**. وإلّا وقعَ تصريحٌ بالاسمِ عينِه
            //      في الموقعِ نفسِه، فتختطفُ الـALLOC الثانيةُ خريطةَ الأسماءِ
            //      ⇒ حلقةٌ لا نهائيّةٌ أو انهيارُ ذاكرةٍ أو تباعُدٌ صامت.
            //      🔑 اللاحقةُ الرقميّةُ **لا تكفي**: «عدّاد_5» تهجئةٌ مشروعةٌ تمامًا.
            //      و«#» وحدَه آمنٌ لأنّه في لغةِ ص **بادئُ تعليق**، فيلتقطُه المُعجَمُ
            //      قبلَ scanIdentifier ولا يقبلُه الأخيرُ (حروفٌ وأرقامٌ و«_» فقط)
            //      ⇒ يستحيلُ وقوعُه داخلَ معرِّفٍ يكتبُه المستخدم. وLLVM يقتبسُ
            //      مثلَ هذا الاسمِ (%"%ي#5") فيقبلُه سليمًا.
            // (EN) Internal slot namespace separator (ISSUE-110). Every slot the builder
            //      emits for its own bookkeeping must live in a namespace the USER CANNOT
            //      SPELL; otherwise a same-named declaration hijacks the codegen name map
            //      ⇒ infinite loop, SIGSEGV, or silent divergence. A numeric suffix is NOT
            //      enough (`counter_5` is a legal identifier). Only `#` is safe: it starts
            //      a comment in Sad, so the lexer consumes it before scanIdentifier, which
            //      accepts only alphanumerics and `_`. LLVM quotes such names and accepts
            //      them.
            //      🔑 الثابتُ نفسُه في `sir_constants.h` كي تبلغَه الخلفيّاتُ بلا
            //      جرِّ الباني؛ ويُستحضَرُ هنا بالاسمِ لا يُهجَّأُ ثانيةً.
            //      🔑 The constant itself lives in sir_constants.h so backends can reach
            //      it without dragging the builder; imported here by name, never re-spelled.
            // ======================================================================
            using Sad::Compiler::kSlotNamespaceSeparator;

            // ======================================================================
            // (AR) معاملُ بيئةِ الإغلاقِ وموقعُه. الاسمُ الثابتُ بلا لاحقةٍ أشدُّ
            //      عرضةً للتصادمِ لا أقلَّ. والاسمانِ مقترنانِ عمدًا: الموقعُ مشتقٌّ
            //      من المعامل، فلو هُجِّئَ كلٌّ منهما وحدَه لانفصلا صامتَين.
            // (EN) Closure-environment parameter and its slot. Deliberately coupled: the
            //      slot derives from the parameter, so hand-spelling each invites silent
            //      drift between producer and matcher.
            //      🔑 والفاصلُ يُشتقُّ هنا من الثابتِ أعلاه لا يُهجَّأُ ثانيةً، وإلّا
            //      نقلَ تغييرُ الفاصلِ المواقعَ كلَّها وأبقى اسمَ البيئةِ خلفَه
            //      ⇒ انشقاقُ فضاءِ الأسماءِ صامتًا. وهما دالّتانِ لا متغيّرَينِ
            //      لأنّ الساكنَ المحلّيَّ يُهيَّأُ عندَ أوّلِ نداءٍ فينجو من
            //      عشوائيّةِ ترتيبِ التهيئةِ بينَ وحداتِ الترجمة.
            //      🔑 The separator is DERIVED here, never re-spelled: otherwise changing
            //      it would move every slot and leave the environment name behind — a
            //      silent namespace split. Functions, not variables: a function-local
            //      static initializes on first call, immune to cross-TU init order.
            // ======================================================================
            inline const std::string &environmentParameterName()
            {
                static const std::string name = std::string(kSlotNamespaceSeparator) + "env";
                return name;
            }

            inline const std::string &environmentSlotName()
            {
                static const std::string name = "%" + environmentParameterName();
                return name;
            }

            // ======================================================================
            // (AR) بُناةُ أسماءِ مواقعِ المتغيّراتِ الملتقَطة. المُنتِجُ والمقارِنُ
            //      بالبادئةِ في ملفَّينِ متباعدَين ⇒ تهجئةٌ واحدةٌ هنا لا غير.
            // (EN) Capture-slot name builders. Producer and prefix matcher live in
            //      different files; one spelling, here.
            // ======================================================================
            inline std::string makeCaptureSlotPrefix(const std::string &variableName)
            {
                return std::string("%") + kSlotNamespaceSeparator + "cap" +
                       kSlotNamespaceSeparator + variableName + kSlotNamespaceSeparator;
            }

            inline std::string makeCaptureSlotName(const std::string &variableName, size_t captureIndex)
            {
                return makeCaptureSlotPrefix(variableName) + std::to_string(captureIndex);
            }

            inline std::string makeDeferCaptureSlotName(const std::string &variableName, size_t captureIndex)
            {
                return std::string("%") + kSlotNamespaceSeparator + "defer_cap" +
                       kSlotNamespaceSeparator + variableName + kSlotNamespaceSeparator +
                       std::to_string(captureIndex);
            }

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
                friend class TemplateBuilder;

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

                // ════════════════════════════════════════════════════════════
                // (AR) 🔑 إطارُ التأجيلِ لدالّةٍ واحدة — معينٌ واحدٌ لمسارَي البناء.
                //      الدالّةُ المتداخلةُ تُبنى بمسارَين: بلا التقاطاتٍ عبر
                //      `buildFunction`، ومعها عبر بناءِ إغلاقٍ يدويٍّ في
                //      `statement_main.cpp`. وكانت الآلةُ في الأوّلِ وحدَه فكان
                //      `أجّل` يُسقَط صامتًا في الثاني — بلا تعليمةٍ وبلا تشخيص.
                //      كلُّ مسارٍ يبني جسمَ دالّةٍ يلزمه النداءان معًا.
                // (EN) 🔑 Per-function defer frame — one source for both build paths.
                //      A nested function is built two ways: without captures via
                //      buildFunction, with captures via the hand-rolled closure build
                //      in statement_main.cpp. The machinery lived only in the first, so
                //      `defer` was dropped silently in the second — no instruction, no
                //      diagnostic. Every path that builds a function body owes both calls.
                // ════════════════════════════════════════════════════════════
                struct DeferFrame
                {
                    bool active = false;
                    std::shared_ptr<SIRBasicBlock> cleanupBlock;
                };

                DeferFrame emitDeferFrameBegin(const Sad::AST::Statement *body);
                // (AR) 🔑 الكتلةُ **بالقيمة** لا بمرجع: التابعُ يُعيد إسنادَ
                //      `currentBlock_` في أثنائه، فمُنادٍ يُمرِّر `currentBlock_`
                //      نفسَه يجعل المُعامِلَ اسمًا ثانيًا للعضوِ المتغيّر فتُسنَد
                //      الكتلةُ الميّتةُ إلى نفسِها ويُبتلَع تفريغُ المؤجَّلات صامتًا.
                //      النسخُ يجعل ذلك مستحيلًا على كلِّ مُنادٍ لا على المنتبِه وحدَه.
                // (EN) 🔑 The block is taken BY VALUE, not by reference: this method
                //      reassigns `currentBlock_` while running, so a caller passing
                //      `currentBlock_` itself would alias the mutating member — the
                //      dead block gets assigned to itself and the defer flush is
                //      swallowed silently. A copy makes that impossible for every
                //      caller, not only the one who noticed.
                void emitDeferFrameEnd(const DeferFrame &frame,
                                       std::shared_ptr<SIRBasicBlock> bodyContinuationBlock);

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
                void buildTemplateFunction(Sad::AST::TemplateFunctionDecl *templateDecl)
                {
                    templates_->buildTemplateFunction(templateDecl);
                }

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
                std::string instantiateTemplate(const std::string &templateName, const std::vector<SadTypeKind> &typeArguments)
                {
                    return templates_->instantiateTemplate(templateName, typeArguments);
                }

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
                std::string instantiateTemplate(const std::string &templateName, const std::vector<SadTypeKind> &typeArguments, const std::vector<SIROperand> &constArguments)
                {
                    return templates_->instantiateTemplate(templateName, typeArguments, constArguments);
                }

                /**
                 * @brief (AR) بناء متغير عام
                 * @brief (EN) Build global variable
                 *
                 * @param varDecl (AR) تصريح المتغير / (EN) Variable declaration
                 */
                void buildGlobalVariable(AST::VariableDeclNode *varDecl);
                /// (AR) طيُّ مُهيِّئٍ حرفيٍّ مركّبٍ لثابتٍ عامّ (سالبٌ، جمعُ حرفيّتَين، وصلُ نصَّين).
                ///      `false` إن لم يكن التعبيرُ حرفيًّا بحتًا — وهي الحالُ التي كانت تُسقَط بصمت.
                /// (EN) Fold a composite literal initializer for a module global; false when the
                ///      expression is not purely literal — the case that used to be dropped silently.
                bool tryFoldLiteralInitializer(Sad::AST::Expression *expr, std::string &out,
                                               SadTypeKind &outKind);

                /**
                 * @brief (AR) بناء صنف
                 * @brief (EN) Build class
                 *
                 * @param classDecl (AR) تصريح الصنف / (EN) Class declaration
                 */
                void buildClass(Sad::AST::ClassDecl *classDecl)
                {
                    classes_->buildClass(classDecl);
                }

                /**
                 * @brief (AR) بناء سمة/واجهة
                 * @brief (EN) Build trait/interface
                 *
                 * @param traitDecl (AR) تصريح السمة / (EN) Trait declaration
                 */
                void buildTrait(Sad::AST::TraitDecl *traitDecl)
                {
                    classes_->buildTrait(traitDecl);
                }

                /**
                 * @brief (AR) بناء كتلة تنفيذ سمة
                 * @brief (EN) Build impl block
                 *
                 * @param implDecl (AR) تصريح التنفيذ / (EN) Impl declaration
                 */
                void buildImpl(Sad::AST::ImplDecl *implDecl)
                {
                    classes_->buildImpl(implDecl);
                }

                // ==================================================================
                // (AR) بناء جمل الاستيراد / (EN) Building Import Statements
                // ==================================================================

                /**
                 * @brief (AR) بناء جملة استيراد كاملة: استورد وحدة
                 * @brief (EN) Build full import statement: import module
                 *
                 * @param importStmt (AR) عقدة الاستيراد / (EN) Import statement node
                 */
                void buildImportStmt(Sad::AST::ImportStmt *importStmt)
                {
                    templates_->buildImportStmt(importStmt);
                }

                /**
                 * @brief (AR) بناء جملة استيراد انتقائي: من وحدة استورد ...
                 * @brief (EN) Build selective import statement: from module import ...
                 *
                 * @param fromImportStmt (AR) عقدة الاستيراد الانتقائي / (EN) From-import statement node
                 */
                void buildFromImportStmt(Sad::AST::FromImportStmt *fromImportStmt)
                {
                    templates_->buildFromImportStmt(fromImportStmt);
                }

                /**
                 * @brief (AR) تعيين مسار الملف الحالي (لحل مسارات الاستيراد النسبية)
                 * @brief (EN) Set current file path (for resolving relative import paths)
                 *
                 * @param filePath (AR) مسار الملف / (EN) File path
                 */
                void setCurrentFilePath(const std::string &filePath)
                {
                    templates_->setCurrentFilePath(filePath);
                }

                /**
                 * @brief (AR) تعيين وضع الوحدة — تخطي إنشاء __sad_main ودالة main wrapper
                 * @brief (EN) Set module mode — skip __sad_main and main wrapper generation
                 *
                 * @param mode (AR) صحيح لتفعيل وضع الوحدة / (EN) true to enable module mode
                 */
                void setModuleMode(bool mode) { moduleMode_ = mode; }
                bool isModuleMode() const { return moduleMode_; }

                /**
                 * @brief (AR) وضع الترجمة الحرّة (--freestanding) — تستشيره بوّابة استيراد
                 *        المدمجات (لا مكتبة قياسيّة تُستورد في هذا الوضع)
                 * @brief (EN) Freestanding mode — consulted by the builtin import gate
                 */
                void setFreestanding(bool mode) { freestandingMode_ = mode; }
                bool isFreestandingMode() const { return freestandingMode_; }

                // (AR) [موجة الجسر الموسوم] تفعيلُ الجسرِ **علمٌ مستقلٌّ** عن شكلِ SIR
                //      الحرِّ: المسارُ الأصليُّ (--خلفية-أصلية) يبني بشكلِ الحرِّ
                //      (freestanding_sir_shape = حرّ ∨ أصليّ) لكنّ أهدافَه المستضافةَ
                //      (لينكس ELF) تُريدُ الجسرَ — والحرُّ الخامُّ وحدَه (نواةُ النحلة/
                //      UEFI) يُعطِّلُه (ميزانيّةُ الحافة). البوّابةُ على شكلِ SIR كانت
                //      تُطفئُ الجسرَ في المسارِ الأصليِّ كلِّه — مقيس: نداءُ dynproto
                //      لم يُوسَمْ قطّ فسلكَ البروتوكولَ الخامَّ وانهار.
                // (EN) [Tagged-bridge wave] Bridge enablement is a flag SEPARATE from
                //      the freestanding SIR shape: the native path builds with the
                //      freestanding shape (freestanding_sir_shape = raw ∨ native) yet
                //      its hosted targets (Linux ELF) want the bridge — only RAW
                //      freestanding (nahla/UEFI kernels) disables it (edge budget).
                //      Gating on the SIR shape silently switched the bridge off for
                //      the whole native path — measured: the dynproto site was never
                //      marked, took the raw protocol, and crashed.
                void setTaggedBridgeEnabled(bool enabled) { taggedBridgeEnabled_ = enabled; }
                bool isTaggedBridgeEnabled() const { return taggedBridgeEnabled_; }

                // ==================================================================
                // (AR) [موجة الجسر الموسوم] توليدُ جسرِ البروتوكولِ الموسومِ دالّةَ SIR:
                //      `__dynbr_<هدف>` بمعاملاتِ Any وعائدِ Any — جسمُها نداءٌ للهدفِ
                //      (تكييفُ الوسائطِ عملُ خفضِ CALL في كلِّ خلفيّة) ثمّ إرجاعُ
                //      نتيجتِه (تغليفُها عملُ خفضِ RET). بهذا تحصلُ **كلُّ** الخلفيّاتِ
                //      على الجسرِ عبرَ أنبوبِها العاديِّ بلا توليدِ آلةٍ يدويٍّ لكلٍّ —
                //      خلفيّةُ LLVM تجدُها بالاسمِ في emitClosureCreate فتُعيدُ
                //      استعمالَها بدلَ توليدِها، والأصليّةُ تربطُها بخانةِ [16].
                //      لا تُولَّدُ في الوضعِ الحرِّ (ميزانيّةُ الحافة) ولا لمولِّدٍ
                //      (جسرُ المولِّدِ حارسُ رفعٍ تصنعه خلفيّةُ LLVM بنفسِها).
                // (EN) [Tagged-bridge wave] Synthesize the tagged-protocol bridge as a
                //      SIR function: `__dynbr_<target>` with Any params and an Any
                //      return — its body is a CALL to the target (argument coercion
                //      is each backend's CALL lowering) then a RET of its result
                //      (boxing is each backend's RET lowering). Every backend thus
                //      gets the bridge through its normal pipeline with no per-arch
                //      hand-written machine code — the LLVM backend finds it by name
                //      in emitClosureCreate and reuses it, and the native backend
                //      wires it into slot [16]. Not synthesized in freestanding
                //      mode (edge budget) nor for generators (the generator bridge
                //      is a raise guard the LLVM backend builds itself).
                // ==================================================================
                void emitDynBridgeFunction(const std::string &targetName,
                                           size_t userArity,
                                           bool targetTakesEnv,
                                           SadTypeKind targetRet)
                {
                    if (!taggedBridgeEnabled_ || !module_)
                        return;
                    const std::string bridgeName =
                        std::string(Sad::Compiler::kClosureDynBridgePrefix) + targetName;
                    if (module_->getFunction(bridgeName))
                        return;
                    auto bridgeFn =
                        std::make_shared<SIRFunction>(bridgeName, SadTypeKind::Any);
                    for (size_t i = 0; i < userArity; ++i)
                        bridgeFn->addParameter(SIRParameter(
                            std::string(Sad::Compiler::kClosureDynBridgeArgPrefix) + std::to_string(i), SadTypeKind::Any));
                    bridgeFn->addParameter(
                        SIRParameter(Sad::Compiler::kClosureDynBridgeEnvParamName, SadTypeKind::Integer));
                    auto bridgeEntry = std::make_shared<SIRBasicBlock>(
                        Sad::Compiler::kEntryBlockName);

                    SIRInstruction bridgeCall(SIROpcode::CALL);
                    const bool targetReturnsValue =
                        targetRet != SadTypeKind::Void;
                    const std::string bridgeResultReg = Sad::Compiler::kClosureDynBridgeResultReg;
                    if (targetReturnsValue)
                        bridgeCall.result =
                            SIROperand::Register(bridgeResultReg, targetRet);
                    bridgeCall.operands.push_back(SIROperand::Function(targetName));
                    for (size_t i = 0; i < userArity; ++i)
                        bridgeCall.operands.push_back(SIROperand::Register(
                            std::string("%") + Sad::Compiler::kClosureDynBridgeArgPrefix + std::to_string(i), SadTypeKind::Any));
                    if (targetTakesEnv)
                        bridgeCall.operands.push_back(SIROperand::Register(
                            std::string("%") + Sad::Compiler::kClosureDynBridgeEnvParamName, SadTypeKind::Integer));
                    bridgeEntry->addInstruction(bridgeCall);

                    SIRInstruction bridgeRet(SIROpcode::RET);
                    if (targetReturnsValue)
                        bridgeRet.operands.push_back(
                            SIROperand::Register(bridgeResultReg, targetRet));
                    else
                    {
                        // (AR) هدفٌ فراغيُّ العائد: يُعادُ الغيابُ ببذرةِ العدمِ موسومةً
                        //      Null — المفسّرُ يطبعُ «لاشيء» لهذه الحالةِ (مقيس).
                        // (EN) A void-returning target: return absence as the null
                        //      sentinel tagged Null — the interpreter prints
                        //      «لاشيء» for this case (measured).
                        SIROperand voidSentinel =
                            SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel);
                        voidSentinel.dataType = SadTypeKind::Null;
                        bridgeRet.operands.push_back(voidSentinel);
                    }
                    bridgeEntry->addInstruction(bridgeRet);

                    bridgeFn->addBasicBlock(bridgeEntry);
                    module_->addFunction(bridgeFn);
                }

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
                {
                    statements_->buildStatement(stmt);
                }

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
                {
                    statements_->buildIfStatement(ifStmt);
                }

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
                {
                    statements_->buildMatchStatement(matchStmt);
                }

                /**
                 * @brief (AR) بناء حلقة while
                 * @brief (EN) Build while loop
                 *
                 * @param whileLoop (AR) حلقة while / (EN) While loop
                 */
                void buildWhileLoop(Sad::AST::WhileStmt *whileLoop)
                {
                    statements_->buildWhileLoop(whileLoop);
                }

                /**
                 * @brief (AR) بناء حلقة for
                 * @brief (EN) Build for loop
                 *
                 * @param forLoop (AR) حلقة for / (EN) For loop
                 */
                void buildForLoop(Sad::AST::ForStmt *forLoop)
                {
                    statements_->buildForLoop(forLoop);
                }

                /**
                 * @brief (AR) بناء حلقة for-range (لكل في)
                 * @brief (EN) Build for-range loop (foreach)
                 *
                 * @param forRange (AR) حلقة لكل في / (EN) For-range loop
                 */
                void buildForRangeLoop(Sad::AST::ForRangeStmt *forRange)
                {
                    statements_->buildForRangeLoop(forRange);
                }

                /**
                 * @brief (AR) بناء جملة return
                 * @brief (EN) Build return statement
                 *
                 * @param retStmt (AR) جملة return / (EN) Return statement
                 */
                void buildReturnStatement(Sad::AST::ReturnStmt *retStmt)
                {
                    statements_->buildReturnStatement(retStmt);
                }

                /**
                 * @brief (AR) بناء جملة break
                 * @brief (EN) Build break statement
                 *
                 * @param breakStmt (AR) جملة break / (EN) Break statement
                 */
                void buildBreakStatement(Sad::AST::BreakStmt *breakStmt)
                {
                    statements_->buildBreakStatement(breakStmt);
                }

                /**
                 * @brief (AR) بناء جملة continue
                 * @brief (EN) Build continue statement
                 *
                 * @param continueStmt (AR) جملة continue / (EN) Continue statement
                 */
                void buildContinueStatement(Sad::AST::ContinueStmt *continueStmt)
                {
                    statements_->buildContinueStatement(continueStmt);
                }

                /**
                 * @brief (AR) بناء إسناد متغير
                 * @brief (EN) Build variable assignment
                 *
                 * @param assignment (AR) جملة الإسناد / (EN) Assignment statement
                 */
                void buildAssignment(Sad::AST::AssignExpr *assignment)
                {
                    statements_->buildAssignment(assignment);
                }

                /**
                 * @brief (AR) بناء إغلاق مخفي لجسم defer من جملة خام
                 * @brief (EN) Build a hidden closure for a defer body from a raw statement
                 *
                 * @param stmt (AR) الجملة المؤجلة / (EN) Deferred statement body
                 * @return (AR) الإغلاق الناتج / (EN) Resulting closure value
                 */
                BuildResult buildDeferredClosure(Sad::AST::Statement *stmt)
                {
                    return statements_->buildDeferredClosure(stmt);
                }

                /**
                 * @brief (AR) تنفيذ كل الإغلاقات المؤجلة للدالة الحالية بترتيب LIFO مرة واحدة فقط
                 * @brief (EN) Execute all deferred closures for the current function in LIFO order exactly once
                 */
                void emitRunDeferredClosures()
                {
                    statements_->emitRunDeferredClosures();
                }

                /**
                 * @brief (AR) إزالة معالج تنظيف الدالة الحالي إذا كان مفعلاً
                 * @brief (EN) Pop the current function cleanup handler if active
                 */
                void emitPopFunctionCleanupHandler()
                {
                    statements_->emitPopFunctionCleanupHandler();
                }

                /**
                 * @brief (AR) بناء تصريح متغير محلي
                 * @brief (EN) Build local variable declaration
                 *
                 * @param varDecl (AR) تصريح المتغير / (EN) Variable declaration
                 */
                void buildLocalVariable(Sad::AST::VarDeclStmt *varDecl)
                {
                    statements_->buildLocalVariable(varDecl);
                }

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
                {
                    return expressions_->buildExpression(expr);
                }

                // (AR) [طبقة طبيعي] مندوب النوع السطحيّ الضحل — يفوّض لباني التعابير.
                //      يُستعمَل في مسار الطباعة (call_main) لاشتقاق إشارة الوسيط من شجرته
                //      كمرآةٍ لـresolveStaticType بالمفسّر (خطوة ٤).
                // (EN) [طبيعي layer] Shallow surface-type delegate — forwards to the expression
                //      builder. Used by the print path (call_main) to derive an argument's sign from
                //      its AST, mirroring the interpreter's resolveStaticType (Step 4).
                Sad::Types::SadTypeKind resolveSurfaceType(const Sad::AST::Expression *expr)
                {
                    return expressions_->resolveSurfaceType(expr);
                }

                /**
                 * @brief (AR) بناء عملية ثنائية (+ - * / % < > == && ||)
                 * @brief (EN) Build binary operation (+ - * / % < > == && ||)
                 *
                 * @param binOp (AR) عملية ثنائية / (EN) Binary operation
                 * @return (AR) نتيجة العملية / (EN) Operation result
                 */
                BuildResult buildBinaryOp(Sad::AST::BinaryExpr *binOp)
                {
                    return expressions_->buildBinaryOp(binOp);
                }

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
                {
                    return expressions_->buildShortCircuitLogical(binOp);
                }

                /**
                 * @brief (AR) بناء عملية أحادية (- !)
                 * @brief (EN) Build unary operation (- !)
                 *
                 * @param unOp (AR) عملية أحادية / (EN) Unary operation
                 * @return (AR) نتيجة العملية / (EN) Operation result
                 */
                BuildResult buildUnaryOp(Sad::AST::UnaryExpr *unOp)
                {
                    return expressions_->buildUnaryOp(unOp);
                }

                // ════════════════════════════════════════════════════════════
                // (AR) 🔑 التحويلُ الضمنيُّ إلى منطقيّ: `كائن` ⇒ `كائن.منطقي()`.
                //      كان منسوخًا حرفيًّا في شرطِ `إذا` وشرطِ الحلقةِ فقط، وغائبًا
                //      عن النفيِ الأحاديِّ وعن طرفَي `و`/`أو`. والنسخُ هو العلّة: كلُّ
                //      موضعٍ يُضاف يبدأ ناقصًا، ولا حارسَ يكشف نقصَه لأنّ الناتج
                //      يُترجَم ويعمل — يقارن **مؤشِّرَ الكائنِ** بصفرٍ فيكون «صحيحًا»
                //      دائمًا. فـ`ليس م2` على مجموعةٍ فارغةٍ كانت تُجيب «خطأ» حيث
                //      يجيب المفسّرُ «صحيح»، بلا رسالةٍ ولا رمزِ خطأ.
                //      يُعيد true إن حُوِّل فعلًا، وfalse إن لم يكن كائنًا أو لم
                //      يُعرِّف صنفُه العامل — وحينئذٍ تبقى القيمةُ كما هي.
                // (EN) Implicit object→bool conversion: `obj` ⇒ `obj.منطقي()`.
                //      It was copy-pasted into the `if` and loop conditions only, and
                //      missing from unary NOT and from both operands of `and`/`or`.
                //      The copying IS the defect: every new site starts incomplete, and
                //      no guard catches it because the result still compiles and runs —
                //      it compares the OBJECT POINTER against zero, which is always
                //      true. So `not m2` on an empty set answered false where the
                //      interpreter answered true, with no message and no error code.
                //      Returns true if a conversion was emitted; false leaves the value
                //      untouched (not an object, or its class defines no such operator).
                // ════════════════════════════════════════════════════════════
                bool coerceObjectToBool(BuildResult &value)
                {
                    std::string className = value.className;
                    if (className.empty() && !value.registerName.empty())
                    {
                        auto it = classInstanceTypes_.find(value.registerName);
                        if (it != classInstanceTypes_.end())
                            className = it->second;
                    }
                    if (className.empty() || !currentBlock_)
                        return false;

                    // (AR) بحثٌ في سلسلة الوراثة — الصنفُ قد يرث العامل
                    // (EN) Search the inheritance chain — the operator may be inherited
                    std::string searchClass = className;
                    bool found = false;
                    while (!searchClass.empty())
                    {
                        if (functionTable_.find(searchClass + kOpToBoolQualifiedSuffix) != functionTable_.end())
                        {
                            found = true;
                            break;
                        }
                        auto classInfo = module_ ? module_->getClass(searchClass) : nullptr;
                        if (classInfo && !classInfo->parentClass.empty())
                            searchClass = classInfo->parentClass;
                        else
                            break;
                    }
                    if (!found)
                        return false;

                    // ════════════════════════════════════════════════════════
                    // (AR) 🔑 المستقبِلُ العدميُّ يُفحَص قبل الإرسال. `متغير م = مجموعة(…)`
                    //      ثمّ `م = لاشيء` يترك الاسمَ مربوطًا بالصنفِ في خريطةٍ مسطّحةٍ
                    //      بلا نطاق، فيُرسَل `__op_tobool__` على قيمةِ العدم: يُحوَّل
                    //      العددُ مؤشِّرًا ويُقرأ منه جدولُ الدوالِّ ⇒ **انهيار**.
                    //      مقيسٌ ٢٠٢٦-٠٨-٢٠: `إذا (م)` و`ليس م` كلاهما rc=139 بينما
                    //      يُجيب المفسّرُ «خطأ» و«صحيح». والعطبُ **سابقٌ** لتوحيدِ
                    //      التحويل: كان في موضعَي الشرطِ قبلَه؛ ونشرَه التوحيدُ إلى
                    //      خمسةٍ — ولأنّه صار في تابعٍ واحدٍ يُسَدُّ مرّةً لخمستِها.
                    //      والعدمُ كاذبٌ بلا نداء، وهو ما يفعله المفسّر.
                    //      والدمجُ بخانةِ مكدّسٍ لا بـPHI: قيمةٌ مُعرَّفةٌ في فرعٍ لا
                    //      تسود كتلةَ الالتقاء — نفسُ نمطِ `buildShortCircuitLogical`.
                    // (EN) Null-receiver check before dispatch. `var m = Set(…)` then
                    //      `m = null` leaves the name bound to its class in a flat,
                    //      scope-less map, so `__op_tobool__` is dispatched on the null
                    //      value: the integer is turned into a pointer and a vtable is
                    //      read out of it ⇒ CRASH. Measured 2026-08-20: `if (m)` and
                    //      `not m` both rc=139 while the interpreter answers false/true.
                    //      The defect PRE-DATES the coercion unification — it lived at the
                    //      two condition sites; unifying spread it to five, and because it
                    //      now lives in one helper it is sealed once for all five.
                    //      Null is falsy without a call, matching the interpreter.
                    //      Merged through a stack slot, not a PHI: a value defined inside a
                    //      branch does not dominate the merge block — the same idiom
                    //      buildShortCircuitLogical uses.
                    // ════════════════════════════════════════════════════════
                    const std::string callLabel = newLabel("tobool_call");
                    const std::string mergeLabel = newLabel("tobool_merge");
                    auto callBlock = createBasicBlock(callLabel);
                    auto mergeBlock = createBasicBlock(mergeLabel);
                    if (currentFunction_)
                    {
                        currentFunction_->addBasicBlock(callBlock);
                        currentFunction_->addBasicBlock(mergeBlock);
                    }

                    const std::string slotReg = newTempRegister();
                    SIRInstruction allocSlot(SIROpcode::ALLOC);
                    allocSlot.result = SIROperand::Register(slotReg, SadTypeKind::Boolean);
                    currentBlock_->addInstruction(allocSlot);

                    SIRInstruction storeDefault(SIROpcode::STORE);
                    storeDefault.operands.push_back(SIROperand::ConstantBool(false));
                    storeDefault.operands.push_back(SIROperand::Register(slotReg, SadTypeKind::Boolean));
                    currentBlock_->addInstruction(storeDefault);

                    // (AR) غيرُ عدميٍّ = لا يساوي حارسَ العدم ولا الصفر (مؤشِّرٌ خام غيرُ مهيَّأ).
                    // (EN) Non-null = neither the null sentinel nor zero (raw uninitialised pointer).
                    const std::string notSentinelReg = newTempRegister();
                    SIRInstruction neSentinel(SIROpcode::NE);
                    neSentinel.result = SIROperand::Register(notSentinelReg, SadTypeKind::Boolean);
                    neSentinel.operands.push_back(SIROperand::Register(value.registerName, value.type));
                    neSentinel.operands.push_back(SIROperand::ConstantI64(kSadNullSentinel));
                    currentBlock_->addInstruction(neSentinel);

                    const std::string notZeroReg = newTempRegister();
                    SIRInstruction neZero(SIROpcode::NE);
                    neZero.result = SIROperand::Register(notZeroReg, SadTypeKind::Boolean);
                    neZero.operands.push_back(SIROperand::Register(value.registerName, value.type));
                    neZero.operands.push_back(SIROperand::ConstantI64(0));
                    currentBlock_->addInstruction(neZero);

                    const std::string liveReg = newTempRegister();
                    SIRInstruction bothLive(SIROpcode::AND);
                    bothLive.result = SIROperand::Register(liveReg, SadTypeKind::Boolean);
                    bothLive.operands.push_back(SIROperand::Register(notSentinelReg, SadTypeKind::Boolean));
                    bothLive.operands.push_back(SIROperand::Register(notZeroReg, SadTypeKind::Boolean));
                    currentBlock_->addInstruction(bothLive);

                    currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(liveReg, SadTypeKind::Boolean),
                        SIROperand::Label(callLabel),
                        SIROperand::Label(mergeLabel)));

                    currentBlock_ = callBlock;
                    std::string boolReg = newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::OBJECT_CALL;
                    callInst.result = SIROperand::Register(boolReg, SadTypeKind::Boolean);
                    callInst.operands.push_back(SIROperand::Register(value.registerName, value.type));
                    callInst.operands.push_back(SIROperand::ConstantString(kOpToBoolName));
                    currentBlock_->addInstruction(callInst);

                    SIRInstruction storeCalled(SIROpcode::STORE);
                    storeCalled.operands.push_back(SIROperand::Register(boolReg, SadTypeKind::Boolean));
                    storeCalled.operands.push_back(SIROperand::Register(slotReg, SadTypeKind::Boolean));
                    currentBlock_->addInstruction(storeCalled);
                    currentBlock_->addInstruction(
                        SIRInstruction::Branch(SIROperand::Label(mergeLabel)));

                    currentBlock_ = mergeBlock;
                    const std::string mergedReg = newTempRegister();
                    SIRInstruction loadMerged(SIROpcode::LOAD);
                    loadMerged.result = SIROperand::Register(mergedReg, SadTypeKind::Boolean);
                    loadMerged.operands.push_back(SIROperand::Register(slotReg, SadTypeKind::Boolean));
                    currentBlock_->addInstruction(loadMerged);

                    value = BuildResult(mergedReg, SadTypeKind::Boolean);
                    return true;
                }

                /**
                 * @brief (AR) بناء استدعاء دالة
                 * @brief (EN) Build function call
                 *
                 * @param call (AR) استدعاء دالة / (EN) Function call
                 * @return (AR) قيمة الإرجاع / (EN) Return value
                 */
                BuildResult buildFunctionCall(Sad::AST::CallExpr *call)
                {
                    return calls_->buildFunctionCall(call);
                }

                // ── دوال مساعدة مستخرجة من buildFunctionCall (CW-05, CW-03) ──
                // ── Helper methods extracted from buildFunctionCall (CW-05, CW-03) ──

                /**
                 * @brief (AR) معالجة توسيع استدعاء الماكرو — يُرجع nullopt إذا لم يكن ماكرو
                 * @brief (EN) Handle macro call expansion — returns nullopt if not a macro call
                 */
                std::optional<BuildResult> buildMacroCallExpansion(Sad::AST::CallExpr *call, const std::string &funcName)
                {
                    return calls_->buildMacroCallExpansion(call, funcName);
                }

                /**
                 * @brief (AR) معالجة استدعاء الكائن القابل للاستدعاء operator() — يُرجع nullopt إذا لم ينطبق
                 * @brief (EN) Handle callable object operator() invocation — returns nullopt if not applicable
                 */
                std::optional<BuildResult> buildCallableObjectInvoke(Sad::AST::CallExpr *call, const std::string &funcName)
                {
                    return calls_->buildCallableObjectInvoke(call, funcName);
                }

                /**
                 * @brief (AR) معالجة استدعاء باني الأب أساس/الأساس/super — يُرجع nullopt إذا لم ينطبق
                 * @brief (EN) Handle super constructor call أساس/الأساس/super — returns nullopt if not applicable
                 */
                std::optional<BuildResult> buildSuperConstructorCall(Sad::AST::CallExpr *call, const std::string &funcName)
                {
                    return calls_->buildSuperConstructorCall(call, funcName);
                }

                /**
                 * @brief (AR) بناء قائمة الوسائط لاستدعاء دالة عادي
                 * @brief (EN) Build argument list for regular function call
                 * @param outArgOperands (AR) الوسائط بصيغة SIROperand / (EN) Arguments as SIROperand
                 * @param outArgResults  (AR) نتائج تقييم الوسائط / (EN) Argument evaluation results
                 * @return (AR) true = نجح / false = فشل (خطأ مسجّل في errors_)
                 * @return (EN) true = success / false = failure (error recorded in errors_)
                 */
                bool buildCallArgumentsList(Sad::AST::CallExpr *call, std::vector<SIROperand> &outArgOperands, std::vector<BuildResult> &outArgResults)
                {
                    return calls_->buildCallArgumentsList(call, outArgOperands, outArgResults);
                }

                /**
                 * @brief (AR) تعبئة قيم الوسائط الافتراضية الناقصة
                 * @brief (EN) Fill in missing default argument values
                 */
                void fillDefaultCallArguments(Sad::AST::CallExpr *call, const std::string &funcName, std::vector<SIROperand> &argOperands, std::vector<BuildResult> &argResults)
                {
                    calls_->fillDefaultCallArguments(call, funcName, argOperands, argResults);
                }

                /**
                 * @brief (AR) معالجة استدعاء دالة مدمجة أساسية
                 * @brief (EN) Handle core builtin function call (type conv, print, math, string, array, file)
                 */
                std::optional<BuildResult> buildBuiltinCallCore(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands, bool isSyntaxDesugared = false)
                {
                    return builtins_->buildBuiltinCallCore(funcName, isUserDefinedFunction, argResults, argOperands, isSyntaxDesugared);
                }

                /**
                 * @brief (AR) معالجة استدعاءات SIMD للمتجهات (Phase 3)
                 *        متجه() للبناء، متجه_بث، متجه_جمع_عرضي، متجه_جذر، متجه_مطلق،
                 *        متجه_أصغر، متجه_أكبر، متجه_ضرب_جمع (FMA)، متجه_جداء_قياسي، إلخ.
                 * @brief (EN) Handle SIMD vector builtin calls (Phase 3)
                 *        vector(), splat, hsum, sqrt, abs, min, max, fma, dot, etc.
                 */
                std::optional<BuildResult> buildBuiltinCallSimd(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinCallSimd(funcName, isUserDefinedFunction, argResults, argOperands);
                }

                /**
                 * @brief (AR) معالجة استدعاء دالة مدمجة للنظام
                 * @brief (EN) Handle system builtin function call (hardware, GPIO, timer, atomic, async, security)
                 */
                std::optional<BuildResult> buildBuiltinCallSystem(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinCallSystem(funcName, isUserDefinedFunction, argResults, argOperands);
                }

                /**
                 * @brief (AR) معالجة استدعاء دالة مدمجة للشبكة
                 *        مقابس TCP/UDP، عميل HTTP، خادم HTTP، أدوات الشبكة، العناوين
                 * @brief (EN) Handle network builtin function call
                 *        TCP/UDP sockets, HTTP client, HTTP server, network utilities, addresses
                 */
                std::optional<BuildResult> buildBuiltinCallNetwork(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinCallNetwork(funcName, isUserDefinedFunction, argResults, argOperands);
                }

                /**
                 * @brief (AR) معالجة استدعاء دالة مدمجة من وحدة «عمليات»
                 *        شغل_برنامجا · انتظر_عملية · انبوب · اغلق · افتح_وصفا
                 * @brief (EN) Handle a builtin call from the «عمليات» (processes) module
                 */
                std::optional<BuildResult> buildBuiltinCallProcesses(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinCallProcesses(funcName, isUserDefinedFunction, argResults, argOperands);
                }

                /**
                 * @brief (AR) دالة مساعدة: بناء تعليمة SIR لدالة شبكة
                 * @brief (EN) Helper: build SIR instruction for a network function
                 * @param opcode (AR) رمز العملية / (EN) SIR opcode
                 * @param argOperands (AR) المعاملات / (EN) Operands
                 * @param returnType (AR) نوع القيمة المُرجعة / (EN) Return type
                 * @param comment (AR) تعليق التعليمة / (EN) Instruction comment
                 */
                BuildResult buildNetworkBuiltinInstruction(SIROpcode opcode, std::vector<SIROperand> &argOperands, SadTypeKind returnType, const char *comment)
                {
                    return builtins_->buildNetworkBuiltinInstruction(opcode, argOperands, returnType, comment);
                }

                /**
                 * @brief (AR) بناء إنشاء كائن جديد
                 * @brief (EN) Build new object creation
                 *
                 * @param newExpr (AR) تعبير جديد / (EN) New expression
                 * @return (AR) مؤشر للكائن / (EN) Pointer to object
                 */
                BuildResult buildNewObject(Sad::AST::NewExpr *expr)
                {
                    return expressions_->buildNewObject(expr);
                }

                /**
                 * @brief (AR) بناء الوصول لعضو في كائن
                 * @brief (EN) Build member access
                 *
                 * @param memberExpr (AR) تعبير الوصول للعضو / (EN) Member access expression
                 * @return (AR) قيمة العضو / (EN) Member value
                 */
                BuildResult buildMemberAccess(Sad::AST::MemberAccessExpr *expr)
                {
                    return expressions_->buildMemberAccess(expr);
                }

                /**
                 * @brief (AR) بناء استدعاء طريقة على كائن
                 * @brief (EN) Build method call on object
                 *
                 * @param methodCallExpr (AR) تعبير استدعاء الطريقة / (EN) Method call expression
                 * @return (AR) نتيجة الاستدعاء / (EN) Call result
                 */
                BuildResult buildMethodCall(Sad::AST::MethodCallExpr *methodCallExpr)
                {
                    return calls_->buildMethodCall(methodCallExpr);
                }

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
                {
                    return methodCalls_->buildChannelMethodCall(expr, objResult);
                }

                std::optional<BuildResult> buildMutexMethodCall(AST::MethodCallExpr *expr, const BuildResult &objResult)
                {
                    return methodCalls_->buildMutexMethodCall(expr, objResult);
                }

                std::optional<BuildResult> buildFutureMethodCall(AST::MethodCallExpr *expr, const BuildResult &objResult)
                {
                    return methodCalls_->buildFutureMethodCall(expr, objResult);
                }

                std::optional<BuildResult> buildWaitGroupMethodCall(AST::MethodCallExpr *expr, const BuildResult &objResult)
                {
                    return methodCalls_->buildWaitGroupMethodCall(expr, objResult);
                }

                std::optional<BuildResult> buildArrayBasicMethodCall(const BuildResult &objResult,
                                                                     const std::string &methodName, const std::vector<SIROperand> &args)
                {
                    return methodCalls_->buildArrayBasicMethodCall(objResult, methodName, args);
                }

                std::optional<BuildResult> buildArrayHigherOrderMethodCall(const BuildResult &objResult,
                                                                           const std::string &methodName, const std::vector<SIROperand> &args,
                                                                           const std::string &closureLambdaName, SadTypeKind closureRetType)
                {
                    return methodCalls_->buildArrayHigherOrderMethodCall(objResult, methodName, args, closureLambdaName, closureRetType);
                }

                std::optional<BuildResult> buildStringBuiltinMethodCall(const BuildResult &objResult,
                                                                        const std::string &methodName, const std::vector<SIROperand> &args)
                {
                    return methodCalls_->buildStringBuiltinMethodCall(objResult, methodName, args);
                }

                std::optional<BuildResult> buildMapBuiltinMethodCall(const BuildResult &objResult,
                                                                     const std::string &methodName, const std::vector<SIROperand> &args)
                {
                    return methodCalls_->buildMapBuiltinMethodCall(objResult, methodName, args);
                }

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
                {
                    return builtins_->buildBuiltinMathCall(funcName, argResults, argOperands);
                }

                /**
                 * @brief (AR) بناء دوال النصوص والمصفوفات المضمنة (طول_نص/تقسيم/إضافة_عنصر/ترتيب...)
                 * @brief (EN) Build string & array builtin calls (string_length/split/append/sort...)
                 */
                std::optional<BuildResult> buildBuiltinStringArrayCall(const std::string &funcName, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinStringArrayCall(funcName, argResults, argOperands);
                }

                /**
                 * @brief (AR) بناء دوال الملفات والأدوات وفحص الأنواع (اقرأ_ملف/عشوائي/هو_رقم...)
                 * @brief (EN) Build file I/O, utility, and type-checking builtins
                 */
                std::optional<BuildResult> buildBuiltinIOUtilsCall(const std::string &funcName, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinIOUtilsCall(funcName, argResults, argOperands);
                }

                /**
                 * @brief (AR) بناء شرط نمط match — يُرجع اسم سجل الشرط
                 * @brief (EN) Build match pattern condition — returns condition register name
                 * @param pattern النمط AST
                 * @param matchValueReg سجل القيمة المُطابقة
                 * @param matchValueType نوع القيمة المُطابقة
                 * @param caseIndex فهرس الحالة (للتشخيص)
                 * @param deferredExtractions مخرج: حقول ADT المؤجل استخراجها
                 * @param failLabel (ISSUE-067) هدف القفز عند فشل المطابقة — يُمكّن قصر
                 *        الدائرة للأنماط المركّبة المتداخلة (قائمة/بنية داخل قائمة/بنية).
                 *        فارغ ⇒ المسار المسطّح التقليديّ (لا تفريع).
                 */
                std::string buildMatchPatternCondition(
                    const AST::PatternNode *pattern,
                    const std::string &matchValueReg,
                    SadTypeKind matchValueType,
                    size_t caseIndex,
                    std::vector<MatchDeferredField> &deferredExtractions,
                    const std::string &failLabel = "",
                    SadTypeKind matchValueElementType = SadTypeKind::Void);

                /**
                 * @brief (AR) مُطابِق أنماط قاصر الدائرة (ISSUE-067) — يعالج الأنماط
                 *        المركّبة المتداخلة بأمان: يفحص البنية (طول/نوع) ويتفرّع إلى
                 *        failLabel عند الفشل، ثمّ يستخرج الأبناء (بعد التحقّق ⇒ آمن من
                 *        تجاوز الحدود) ويربط المتغيّرات، متعاوِدًا للأنماط المركّبة.
                 *        عند النجاح يترك currentBlock_ على كتلة استمرار.
                 * @brief (EN) Short-circuiting nested pattern matcher (ISSUE-067).
                 */
                void emitPatternMatchShortCircuit(
                    const AST::PatternNode *pattern,
                    const std::string &valueReg,
                    SadTypeKind valueType,
                    const std::string &failLabel,
                    SadTypeKind valueElementType = SadTypeKind::Void);

                /**
                 * @brief (AR) هل يحتوي النمط على ابنٍ مركّب (قائمة/بنية/نطاق/تعداد/بدائل/ربط)
                 *        يتطلّب مسار قصر الدائرة؟ / (EN) Does pattern have a composite child?
                 */
                static bool patternHasCompositeChild(const AST::PatternNode *pattern);

                /**
                 * @brief (AR) بناء باني صنف — مستخرج من buildClass (CW-05)
                 * @brief (EN) Build class constructor — extracted from buildClass
                 */
                void buildClassConstructor(Sad::AST::ClassDecl *classDecl, std::shared_ptr<SIRClass> sirClass, Sad::AST::ConstructorDecl *ctorDecl)
                {
                    classes_->buildClassConstructor(classDecl, sirClass, ctorDecl);
                }

                // (AR) بناء طريقة صنف/بنية بسياق الصنف الكامل — ISSUE-060
                // (EN) Build a class/struct method with full class context — ISSUE-060
                void buildClassMethod(Sad::AST::ClassDecl *classDecl, std::shared_ptr<SIRClass> sirClass, Sad::AST::MethodDecl *methodDecl)
                {
                    classes_->buildClassMethod(classDecl, sirClass, methodDecl);
                }

                /**
                 * @brief (AR) بناء عامل زائد في صنف — مستخرج من buildClass (CW-05)
                 * @brief (EN) Build class operator overload — extracted from buildClass
                 */
                void buildClassOperator(Sad::AST::ClassDecl *classDecl, std::shared_ptr<SIRClass> sirClass, Sad::AST::OperatorDecl *operatorDecl)
                {
                    classes_->buildClassOperator(classDecl, sirClass, operatorDecl);
                }

                /**
                 * @brief (AR) بناء وصول لمتغير
                 * @brief (EN) Build variable access
                 *
                 * @param var (AR) متغير / (EN) Variable
                 * @return (AR) قيمة المتغير / (EN) Variable value
                 */
                BuildResult buildVariableAccess(Sad::AST::VariableExpr *varNode)
                {
                    return expressions_->buildVariableAccess(varNode);
                }

                /**
                 * @brief (AR) بناء ثابت (رقم، نص، منطقي)
                 * @brief (EN) Build literal (number, string, boolean)
                 *
                 * @param literal (AR) ثابت / (EN) Literal
                 * @return (AR) القيمة الثابتة / (EN) Constant value
                 */
                BuildResult buildLiteral(Sad::AST::LiteralExpr *literal)
                {
                    return expressions_->buildLiteral(literal);
                }

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
                {
                    return expressions_->buildExprMember(expr);
                }

                /// @brief (AR) بناء تعبير MemberAssignExpr / (EN) Build MemberAssignExpr
                BuildResult buildExprMemberAssign(Sad::AST::MemberAssignExpr *expr)
                {
                    return expressions_->buildExprMemberAssign(expr);
                }

                /// @brief (AR) بناء تعبير TernaryExpr / (EN) Build TernaryExpr
                BuildResult buildExprTernary(Sad::AST::TernaryExpr *expr)
                {
                    return expressions_->buildExprTernary(expr);
                }

                /// @brief (AR) بناء تعبير IndexExpr / (EN) Build IndexExpr
                BuildResult buildExprIndex(Sad::AST::IndexExpr *expr)
                {
                    return expressions_->buildExprIndex(expr);
                }

                /// @brief (AR) بناء تعبير IndexAssignExpr / (EN) Build IndexAssignExpr
                BuildResult buildExprIndexAssign(Sad::AST::IndexAssignExpr *expr)
                {
                    return expressions_->buildExprIndexAssign(expr);
                }

                /// @brief (AR) بناء تعبير ArrayExpr / (EN) Build ArrayExpr
                BuildResult buildExprArray(Sad::AST::ArrayExpr *expr)
                {
                    return expressions_->buildExprArray(expr);
                }

                /// @brief (AR) بناء تعبير TupleExpr / (EN) Build TupleExpr
                BuildResult buildExprTuple(Sad::AST::TupleExpr *expr)
                {
                    return expressions_->buildExprTuple(expr);
                }

                /// @brief (AR) بناء تعبير MapExpr / (EN) Build MapExpr
                BuildResult buildExprMap(Sad::AST::MapExpr *expr)
                {
                    return expressions_->buildExprMap(expr);
                }

                /// @brief (AR) بناء تعبير WalrusExpr / (EN) Build WalrusExpr
                BuildResult buildExprWalrus(Sad::AST::WalrusExpr *expr)
                {
                    return expressions_->buildExprWalrus(expr);
                }

                /// @brief (AR) بناء تعبير LambdaExpr / (EN) Build LambdaExpr
                BuildResult buildExprLambda(Sad::AST::LambdaExpr *expr)
                {
                    return expressions_->buildExprLambda(expr);
                }

                /// @brief (AR) بناء تعبير RangeExpr / (EN) Build RangeExpr
                BuildResult buildExprRange(Sad::AST::RangeExpr *expr)
                {
                    return expressions_->buildExprRange(expr);
                }

                /// @brief (AR) بناء تعبير ListComprehensionExpr / (EN) Build ListComprehensionExpr
                BuildResult buildExprListComp(Sad::AST::ListComprehensionExpr *expr)
                {
                    return expressions_->buildExprListComp(expr);
                }

                /// @brief (AR) بناء تعبير DictComprehensionExpr / (EN) Build DictComprehensionExpr
                BuildResult buildExprDictComp(Sad::AST::DictComprehensionExpr *expr)
                {
                    return expressions_->buildExprDictComp(expr);
                }

                /// @brief (AR) بناء تعبير SetComprehensionExpr / (EN) Build SetComprehensionExpr
                BuildResult buildExprSetComp(Sad::AST::SetComprehensionExpr *expr)
                {
                    return expressions_->buildExprSetComp(expr);
                }

                /// @brief (AR) بناء تعبير InlineAsmExpr / (EN) Build InlineAsmExpr
                BuildResult buildExprInlineAsm(Sad::AST::InlineAsmExpr *expr)
                {
                    return expressions_->buildExprInlineAsm(expr);
                }

                /// @brief (AR) بناء تعبير SizeofExpr / (EN) Build SizeofExpr
                BuildResult buildExprSizeof(Sad::AST::SizeofExpr *expr)
                {
                    return expressions_->buildExprSizeof(expr);
                }

                /// @brief (AR) بناء تعبير AtomicExpr / (EN) Build AtomicExpr
                BuildResult buildExprAtomic(Sad::AST::AtomicExpr *expr)
                {
                    return expressions_->buildExprAtomic(expr);
                }

                /// @brief (AR) بناء تعبير OptionalChainExpr / (EN) Build OptionalChainExpr
                BuildResult buildExprOptionalChain(Sad::AST::OptionalChainExpr *expr)
                {
                    return expressions_->buildExprOptionalChain(expr);
                }

                /// @brief (AR) بناء تعبير NullCoalesceExpr / (EN) Build NullCoalesceExpr
                BuildResult buildExprNullCoalesce(Sad::AST::NullCoalesceExpr *expr)
                {
                    return expressions_->buildExprNullCoalesce(expr);
                }

                /// @brief (AR) بناء تعبير SliceExpr / (EN) Build SliceExpr
                BuildResult buildExprSlice(Sad::AST::SliceExpr *expr)
                {
                    return expressions_->buildExprSlice(expr);
                }

                /// @brief (AR) بناء تعبير ErrorPropagateExpr / (EN) Build ErrorPropagateExpr
                BuildResult buildExprErrorPropagate(Sad::AST::ErrorPropagateExpr *expr)
                {
                    return expressions_->buildExprErrorPropagate(expr);
                }

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

                /**
                 * @brief (AR) هل «اسم_فضاء.رمز» تأهيلٌ صحيحٌ لوحدةٍ مستوردة؟
                 * @brief (EN) Is `namespaceName.symbolName` a valid module qualification?
                 *
                 * (AR) حاسمٌ **واحدٌ** يستعمله كلُّ من يفكّ التأهيل (النداء، العضو،
                 *      الاستنتاج) فلا تتباعد شروطُهم — وقد تباعدت فعلًا ففات الحارسُ
                 *      موضعًا من ثلاثة. يشترط ثلاثةً معًا:
                 *      ١. أن يكون الاسمُ فضاءَ وحدةٍ في **هذا الملفّ** (moduleNamespaces_).
                 *      ٢. ألّا يكون متغيّرًا حقيقيًّا — المتغيّرُ يفوز دائمًا فلا يُختطف
                 *         نداءُ كائنٍ يصادف اسمُه اسمَ وحدة.
                 *      ٣. أن يكون الرمزُ **مِن تلك الوحدة** لا من الجدولِ العامّ.
                 *      وإن صدَقت الثلاثةُ وكان الرمزُ نفسُه مُصدَّرًا من وحدةٍ أخرى
                 *      مستوردةٍ كذلك، فالتأهيلُ **مُبهَم**: المصرّفُ يُسطّح فينهار
                 *      الرمزان إلى واحدٍ ويفوز آخِرُهما، فيُملأ `ambiguityDiagnostic`
                 *      ويُرفض الحلُّ — تشخيصٌ صريحٌ خيرٌ من قيمةٍ خاطئةٍ صامتة.
                 * (EN) ONE decider used by every un-qualifier (call, member, inference) so
                 *      their conditions cannot drift — they already had, and the guard was
                 *      missing in one of three places. Requires all of: (1) the name is a
                 *      module namespace in THIS file; (2) it is not a real variable — a
                 *      variable always wins; (3) the symbol belongs to THAT module, not merely
                 *      to the global table. If it does, but the same symbol is also exported by
                 *      another imported module, the qualification is AMBIGUOUS: flattening
                 *      collapses both into one symbol and the last wins, so
                 *      `ambiguityDiagnostic` is filled and resolution refused — an explicit
                 *      diagnostic beats a silently wrong value.
                 */
                bool isModuleQualifiedSymbol(const std::string &namespaceName,
                                             const std::string &symbolName,
                                             std::string &ambiguityDiagnostic);

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

                /**
                 * @brief (AR) (اللبنة 3.17) يُبلِّغ SEM024 (تصادم اسم رمز @رمز مُصدَّر)
                 *        مبنيًّا من كتالوج الأخطاء (مصدر الحقيقة — لا نصّ يدويّ) ويدفعه إلى
                 *        errors_ ليُفشِل البناء عبر hasErrors(). يُستدعى من مساري الدالّة
                 *        (sir_builder_functions) والمتغيّر (sir_builder_module).
                 * @brief (EN) (Brick 3.17) Reports SEM024 (@رمز exported-symbol collision)
                 *        built from the error catalog (SoT — no hand-written text) and pushes
                 *        it to errors_ so hasErrors() fails the build. Called from both the
                 *        function path and the variable path.
                 */
                void reportDuplicateExportSymbol(const std::string &symbol);

                /**
                 * @brief (AR) هل توجد تحذيرات؟ (غير قاتلة — لا تُفشِل البناء)
                 * @brief (EN) Has warnings? (non-fatal — do not fail the build)
                 */
                bool hasWarnings() const { return !warnings_.empty(); }

                /**
                 * @brief (AR) الحصول على قائمة التحذيرات
                 * @brief (EN) Get list of warnings
                 */
                const std::vector<std::string> &getWarnings() const { return warnings_; }

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

                // ==================================================================
                // (AR) Phase 6 — Step 8 (الأخيرة): بنّاء القوالب والاستيراد والاستنتاج
                // (EN) Phase 6 — Step 8 (final): templates/imports/inference builder
                // ==================================================================
                std::unique_ptr<TemplateBuilder> templates_;

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
                SadTypeKind astTypeToSIRType(const Sad::Types::SadTypeKind &astType);

                /**
                 * @brief (AR) نوعُ **خانةِ** تصريحٍ — يفكّ `T؟` ويختار تمثيلَ العدمِ له
                 * @brief (EN) The STORAGE kind of a declaration — unwraps `T?` and picks
                 *             its null representation
                 *
                 * (AR) 🔑 سلطةٌ واحدةٌ لمحوِ `Optional`. كان المحوُ مكرَّرًا في ستّةِ مواضع:
                 *      المتغيّرُ المحلّيّ · نوعُ الإرجاع · المعامِلُ (مرّتان) ·
                 *      **والعامُّ في مسارَيه** — والأخيرانِ كانا **بلا معالجةٍ للـOptional
                 *      أصلًا**، فيسقط `منطقي عدمي` في المستوى الأعلى إلى الفرعِ الافتراضيِّ
                 *      في `astTypeToSIRType`. وقد قِيس ذلك: بعد إصلاحِ الأربعةِ وحدَها
                 *      صحّ المحلّيُّ والمعامِلُ وبقي العامُّ يطبع `1` مكانَ `لاشيء` —
                 *      لأنّ نطاقَ الأسماءِ صار يقول `Any` بينما `SIRGlobalVariable`
                 *      ما زال يقول `Integer`، فاختلف الطرفانِ على خانةٍ واحدة.
                 *      فوجب أن تكون **دالّةً واحدةً** لا نمطًا منسوخًا: النمطُ المنسوخُ هو
                 *      عينُ ما ترك مسارَ العامِّ خارجَ الإصلاحِ سنةً كاملة.
                 * (EN) 🔑 ONE authority for Optional erasure. It used to be duplicated across
                 *      six sites: local var · return type · parameter (twice) · AND the global
                 *      in both of its paths — the last two handled `Optional` NOT AT ALL, so a
                 *      top-level `bool?` fell through to the default arm of `astTypeToSIRType`.
                 *      Measured: fixing only the four made locals and parameters correct while
                 *      the global still printed `1` instead of `null`, because the scope said
                 *      `Any` while `SIRGlobalVariable` still said `Integer` — two owners
                 *      disagreeing about one slot. Hence a FUNCTION, not a copied pattern: the
                 *      copied pattern is exactly what left the global path out of the fix.
                 *
                 * 🔴 (AR) **ولماذا `fallbackKind` لا نوعَ مباشرًا**: أوّلُ صيغةٍ من هذه
                 *      الدالّةِ كانت تُرجِع `astTypeToSIRType(declaredKind)` لغيرِ `T؟`،
                 *      فاستُدعيت **بلا شرطٍ** في مواضعِ التجاوز — فدهست ما استُنتِج قبلها.
                 *      وقِيس الثمنُ فورًا: نوعُ إرجاعِ الدالّةِ غيرِ المُصرَّحِ يُستنتَج من
                 *      الجسمِ (`inferReturnTypeFromBody`)، فصار يُدهَس بـ`Integer` ⇒
                 *      `ارجع "ممتاز"` تطبع **مؤشّرًا خامًّا** `140699353424075`.
                 *      أخفقت به ثلاثةُ اختباراتِ سلوكٍ لا علاقةَ لها بالعدميّة.
                 *      فصار العقدُ صريحًا: **لا يُغيَّر شيءٌ إلّا لـ`T؟`**؛ وما عداه
                 *      يُرجَعُ `fallbackKind` كما هو. ولا يُترَك ذلك لانضباطِ موضعِ النداء:
                 *      الشرطُ المنسوخُ في ستّةِ مواضعَ هو ما أوقعَ العطبَ أوّلَ مرّة.
                 * 🔴 (EN) Why `fallbackKind` and not a direct kind: the first version returned
                 *      `astTypeToSIRType(declaredKind)` for non-optionals, so it was called
                 *      UNCONDITIONALLY at override sites and clobbered previously inferred
                 *      types. Measured immediately: an unannotated return type is inferred from
                 *      the body, and the clobber forced it to `Integer`, so `return "ممتاز"`
                 *      printed a raw pointer. Three unrelated behaviour tests failed. The
                 *      contract is now explicit — nothing changes except for `T?`, everything
                 *      else returns `fallbackKind` untouched — rather than relying on call-site
                 *      discipline, since the copied condition is what caused the defect.
                 *
                 * @param declaredKind  (AR) نوعُ التصريحِ كما بناه المحلّلُ النحويّ
                 * @param declaredSadType (AR) النوعُ المُسبَّكُ (يحمل الداخليَّ لـ`T؟`)؛ قد يكون فارغًا
                 * @param fallbackKind  (AR) ما يُرجَع حين لا يكون التصريحُ `T؟` — يُمرَّرُ إليه
                 *                      النوعُ القائمُ في موضعِ النداء (مُستنتَجًا كان أو مُصرَّحًا)
                 */
                SadTypeKind resolveDeclaredStorageKind(const Sad::Types::SadTypeKind &declaredKind,
                                                       const Sad::Types::SadType *declaredSadType,
                                                       SadTypeKind fallbackKind);

                /**
                 * @brief (AR) نوعُ خانةٍ صُرِّحت بلا نوعٍ وبلا تهيئة — بابُ ISSUE-138 الواحد
                 * @brief (EN) Storage kind of a typeless, initializer-less slot — the single
                 *        ISSUE-138 door
                 *
                 * 🔑 (AR) `astTypeToSIRType` تُرجِع `Integer` عن `Unknown` بتعليقٍ يقول إنّه
                 *      **نائبٌ يستبدله استنتاجُ الأنواعِ لاحقًا**. والفرضُ صحيحٌ حيثما وُجِد
                 *      مُهيِّئٌ يُستنتَج منه، ويسقط حين لا يوجد: فلا شيءَ يستبدل النائبَ،
                 *      فيخرج «رقم» جوابًا نهائيًّا عن خانةٍ لا نوعَ لها.
                 *
                 * 🔴 (AR) ولمَ بابٌ واحدٌ لا شرطٌ يُنسَخ: نوعُ خانةِ التصريحِ يُحسَب في
                 *      **ثلاثةِ مواضعَ منفصلة** (المحلّيُّ · التسجيلُ المسبَقُ للعوامّ ·
                 *      `buildGlobalVariable`). وقِيس ما يحدث حين يُطبَّق الشرطُ في موضعٍ
                 *      واحد: صُحِّح المحلّيُّ وحدَه فصار العامُّ يُخصَّص `i64` بينما يُعلَن
                 *      نوعُه `Any` ⇒ **انهيارُ مترجِمٍ داخليٌّ** (`PRINT_ANY_RAW_I64`) في
                 *      أبسطِ برنامجٍ ممكن: «متغير ك» ثمّ «اطبع_سطر(ك)». فالتوزيعُ على
                 *      المواضعِ ليس أسلوبًا بل عطبٌ مؤجَّل.
                 *
                 * @param declaredKind   (AR) نوعُ التصريحِ كما بناه المحلّلُ النحويّ
                 * @param hasInitializer (AR) أللتصريحِ مُهيِّئٌ يُستنتَج منه النوع؟
                 * @param resolvedKind   (AR) النوعُ المحسوبُ في موضعِ النداء — يُرجَع كما هو
                 *                       ما لم تنطبق الحالة
                 */
                static SadTypeKind resolveBareSlotStorageKind(
                    const Sad::Types::SadTypeKind &declaredKind,
                    bool hasInitializer,
                    SadTypeKind resolvedKind);

                /**
                 * @brief (AR) تحويل نوع AST إلى SadTypePtr (النظام الموحد)
                 * @brief (EN) Convert AST DataType to SadTypePtr (unified type system)
                 * @param astType نوع AST / AST DataType
                 * @return مؤشر SadType الموحد / Unified SadType pointer
                 */
                Sad::Types::SadTypePtr astTypeToSadType(const Sad::Types::SadTypeKind &astType);

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
                SadTypeKind inferReturnTypeFromBody(const Sad::AST::Statement *body, const Sad::AST::FunctionDecl *funcDecl = nullptr)
                {
                    return templates_->inferReturnTypeFromBody(body, funcDecl);
                }

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
                SadTypeKind inferExprType(const Sad::AST::Expression *expr)
                {
                    return templates_->inferExprType(expr);
                }

                /**
                 * @brief (AR) مسح مواقع الاستدعاء لاستنتاج أنواع المعاملات غير المحددة
                 * @brief (EN) Scan call sites to infer types for untyped parameters
                 * @param program البرنامج الكامل / Full program AST
                 *
                 * يمسح جميع CallExpr في البرنامج ويحدّث functionTable_ عندما
                 * يكون المعامل I64 (من Unknown) والوسيط الفعلي STRING/F64/BOOL
                 */
                void inferParamTypesFromCallSites(Sad::AST::StmtList *program)
                {
                    templates_->inferParamTypesFromCallSites(program);
                }

                /**
                 * @brief (AR) تسجيل تواقيع دوالّ قائمةِ عباراتٍ مسبقًا (المرحلة الأولى)
                 * @brief (EN) Pre-register a statement list's function signatures (Phase 1)
                 * @param program قائمةُ العبارات / Statement list
                 *
                 * (AR) تمريرةٌ **واحدة** يشترك فيها ملفُّ الدخول (buildProgram) والوحداتُ
                 *      المستوردة (buildImportStmt). حصرُها سابقًا في مسار الدخول جعل
                 *      النداءَ الأماميّ داخل وحدةٍ مستوردة يفشل — انظر التعليل الكامل
                 *      عند التعريف في sir_builder_module.cpp‏ (#333).
                 * (EN) One pass shared by the entry file and by imported modules; see the
                 *      full rationale at the definition in sir_builder_module.cpp (#333).
                 */
                void preRegisterFunctionSignatures(Sad::AST::StmtList *program);

                /**
                 * @brief (AR) تسجيل تواقيع دوالّ الوحدات المستوردة مسبقًا قبل الطور 1.7
                 * @brief (EN) Pre-register imported module function signatures before Phase 1.7
                 * @param program البرنامج الكامل / Full program AST
                 */
                void preRegisterImportedSignatures(Sad::AST::StmtList *program)
                {
                    templates_->preRegisterImportedSignatures(program);
                }

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
                void inferLambdaParamFromExpr(const Sad::AST::Expression *expr, const std::set<std::string> &paramNames, std::unordered_map<std::string, SadTypeKind> &result)
                {
                    templates_->inferLambdaParamFromExpr(expr, paramNames, result);
                }

                /**
                 * @brief (AR) تحليل جملة لاستنتاج أنواع معاملات اللامدا من الاستخدام
                 * @brief (EN) Analyze statement to infer lambda param types from usage
                 */
                void inferLambdaParamFromStmt(const Sad::AST::Statement *stmt, const std::set<std::string> &paramNames, std::unordered_map<std::string, SadTypeKind> &result)
                {
                    templates_->inferLambdaParamFromStmt(stmt, paramNames, result);
                }

                /**
                 * @brief (AR) مسح تعاودي للجمل للبحث عن استدعاءات الدوال
                 * @brief (EN) Recursively scan statements for function calls
                 */
                void scanCallSitesInStmt(const Sad::AST::Statement *stmt)
                {
                    templates_->scanCallSitesInStmt(stmt);
                }

                /**
                 * @brief (AR) مسح تعاودي للتعبيرات للبحث عن استدعاءات الدوال
                 * @brief (EN) Recursively scan expressions for function calls
                 */
                void scanCallSitesInExpr(const Sad::AST::Expression *expr)
                {
                    templates_->scanCallSitesInExpr(expr);
                }

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
                void collectFreeVarsExpr(Sad::AST::Expression *expr, const std::set<std::string> &boundNames, std::set<std::string> &freeVars)
                {
                    templates_->collectFreeVarsExpr(expr, boundNames, freeVars);
                }

                /**
                 * @brief (AR) جمع المتغيرات الحرة في جملة (تعاودي)
                 * @brief (EN) Collect free variables in a statement (recursive)
                 */
                void collectFreeVarsStmt(Sad::AST::Statement *stmt, std::set<std::string> &boundNames, std::set<std::string> &freeVars)
                {
                    templates_->collectFreeVarsStmt(stmt, boundNames, freeVars);
                }

                // (AR) ملاحظة Phase 6: حُذفت convertType (إعلان بلا تنفيذ ولا استخدام)
                // (EN) Phase 6 note: removed convertType (declared but never defined or used)

                // ── دوال فرعية لتقسيم buildBuiltinCallSystem ──
                std::optional<BuildResult> buildBuiltinSystem_Embedded(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinSystem_Embedded(funcName, isUserDefinedFunction, argResults, argOperands);
                }
                std::optional<BuildResult> buildBuiltinSystem_Security(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinSystem_Security(funcName, isUserDefinedFunction, argResults, argOperands);
                }
                std::optional<BuildResult> buildBuiltinSystem_Crypto(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinSystem_Crypto(funcName, isUserDefinedFunction, argResults, argOperands);
                }

                std::optional<BuildResult> buildBuiltinSystem_FFI(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinSystem_FFI(funcName, isUserDefinedFunction, argResults, argOperands);
                }

                std::optional<BuildResult> buildBuiltinSystem_Async(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinSystem_Async(funcName, isUserDefinedFunction, argResults, argOperands);
                }
                std::optional<BuildResult> buildBuiltinSystem_OsCore(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinSystem_OsCore(funcName, isUserDefinedFunction, argResults, argOperands);
                }
                std::optional<BuildResult> buildBuiltinSystem_OsHardware(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinSystem_OsHardware(funcName, isUserDefinedFunction, argResults, argOperands);
                }
                std::optional<BuildResult> buildBuiltinSystem_OsSystem(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinSystem_OsSystem(funcName, isUserDefinedFunction, argResults, argOperands);
                }
                std::optional<BuildResult> buildBuiltinSystem_Uefi(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinSystem_Uefi(funcName, isUserDefinedFunction, argResults, argOperands);
                }
                std::optional<BuildResult> buildBuiltinSystem_UI(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands)
                {
                    return builtins_->buildBuiltinSystem_UI(funcName, isUserDefinedFunction, argResults, argOperands);
                }

                // ── دوال فرعية لتقسيم buildStatement ──
                bool buildStatement_Exceptions(Sad::AST::Statement *stmt)
                {
                    return statements_->buildStatement_Exceptions(stmt);
                }
                bool buildStatement_Generators(Sad::AST::Statement *stmt)
                {
                    return statements_->buildStatement_Generators(stmt);
                }
                bool buildStatement_Types(Sad::AST::Statement *stmt)
                {
                    return statements_->buildStatement_Types(stmt);
                }

                bool buildStatement_Advanced(Sad::AST::Statement *stmt)
                {
                    return statements_->buildStatement_Advanced(stmt);
                }

                // ── دوال مساعدة مستخرجة من buildStatement_Advanced (CW-05, CW-03) ──
                /**
                 * @brief (AR) معالجة TestDecl — اختبار عادي واختبار خصائص
                 * @brief (EN) Handle TestDecl — normal test and property-based test
                 */
                bool buildStatement_Test(Sad::AST::Statement *stmt)
                {
                    return statements_->buildStatement_Test(stmt);
                }

                /**
                 * @brief (AR) معالجة PropertyDecl — getter/setter
                 * @brief (EN) Handle PropertyDecl — getter/setter lowering
                 */
                bool buildStatement_Property(Sad::AST::Statement *stmt)
                {
                    return statements_->buildStatement_Property(stmt);
                }

                /**
                 * @brief (AR) معالجة GoStmt — goroutine (أطلق)
                 * @brief (EN) Handle GoStmt — concurrent goroutine spawn
                 */
                bool buildStatement_Go(Sad::AST::Statement *stmt)
                {
                    return statements_->buildStatement_Go(stmt);
                }

                /**
                 * @brief (AR) معالجة SelectStmt — اختر من قنوات متعددة
                 * @brief (EN) Handle SelectStmt — channel select multiplexer
                 */
                bool buildStatement_Select(Sad::AST::Statement *stmt)
                {
                    return statements_->buildStatement_Select(stmt);
                }

                /**
                 * @brief (AR) معالجة ExtensionDecl — امتداد صنف موجود
                 * @brief (EN) Handle ExtensionDecl — extension methods for existing class
                 */
                bool buildStatement_Extension(Sad::AST::Statement *stmt)
                {
                    return statements_->buildStatement_Extension(stmt);
                }

                /**
                 * @brief (AR) معالجة TypeAliasDecl + ReExportStmt — أسماء مستعارة وإعادة تصدير
                 * @brief (EN) Handle TypeAliasDecl + ReExportStmt — type aliases and re-exports
                 */
                bool buildStatement_TypesAndReExport(AST::Statement *stmt);
            };

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
