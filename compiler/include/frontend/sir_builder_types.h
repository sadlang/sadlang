// ======================================================================
// sir_builder_types.h - أنواعُ حالةِ بناءِ SIR / SIR builder state types
// ======================================================================
// (AR) 🔑 هذه الأنواعُ كانت تسكنُ `sir_builder.h`، بينما يخزّنُها
//      `sir_builder_context.h` **بالقيمة** داخلَ `std::unordered_map` /
//      `std::vector` معتمِدًا على تصريحٍ أماميٍّ وحدَه. وذلك سيّئُ التشكيلِ
//      بذاتِه (IFNDR): `std::pair<const K, T>` يلزمُه `sizeof(T)`. وlibstdc++
//      13 يؤجّلُ التنميطَ فيمرُّ، وlibstdc++ 11 (ubuntu-22.04، وهو صفُّ
//      الإصدارِ) يُنمّطُ عندَ التصريحِ فيسقطُ بـ«‘second’ has incomplete type».
//      فالعلاجُ إحضارُ التعريفِ **قبلَ** موضعِ التخزين، لا إخفاءُ العَرَض.
// (EN) These types used to live in sir_builder.h while sir_builder_context.h
//      stored them BY VALUE in unordered_map/vector behind a forward
//      declaration alone — IFNDR: std::pair<const K, T> needs sizeof(T).
//      libstdc++ 13 defers instantiation and passes; libstdc++ 11 (the
//      release runner) instantiates at declaration and fails. The remedy is
//      to make the definition precede the storage, not to hide the symptom.
// ======================================================================

#pragma once

#include "sir_module.h"
#include "sir_types.h"
// (AR) عقدُ AST التي تُلقَّبُ في كتلةِ الألقابِ أدناه، وTokenType خلفَ
//      BinaryOperator/UnaryOperator. كانت تصلُ إلى الألقابِ عبرَ ترتيبِ
//      الإدراجِ في sir_builder.h — وهو سندٌ لا يصمدُ لنقلِ الكتلة.
// (EN) The AST nodes aliased below, plus TokenType behind Binary/UnaryOperator.
//      They used to arrive through sir_builder.h include order — a support that
//      does not survive moving the block.
#include "ast_node.h"
#include "expressions.h"
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "module_nodes.h"
#include "pattern_nodes.h"
#include "advanced_expr_nodes.h"
#include "property_nodes.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
                // (② rfcs#46) اسم دالّةٍ مسمّاة مُمرَّرة كمرجع (`.عند_السحب(معالِج)`): خلافًا
                //   للامدا لا يملأ closureLambdaName. نحمله كي تبصمه إضافةُ الحدث في
                //   comment=«lambda:الاسم»، فتقرأ الخلفيّةُ أريّةَ المعالِج وتبني بنية «حدث».
                std::string funcRefName; ///< (② rfcs#46) اسم دالّة المرجع المسمّاة (لبصمة معالِج الحدث)
                bool isGeneratorFuncRef = false; ///< (AR) مرجع دالّة مولّدة (يُصدِر CONSUME عند الاستدعاء غير المباشر) / (EN) Reference to a generator function (emits CONSUME on indirect call)

                // (AR) 🔑 العرضُ المُعلَنُ — نظيرُ `VariableInfo::declaredSurfaceType`
                //      على مستوى التعبير. `type` أعلاه نوعُ **خانة**: «الخيارُ ب»
                //      يُنزِلُ كلَّ عرضٍ دونَ ٦٤ إلى i64 فيُجيبُ `Integer` عن `رقم8`
                //      كما عن `رقم`. و`نوع()` سؤالٌ دلاليٌّ فيلزمُه المُعلَن.
                //      ولا يُرفَعُ إلى `type`: ذاك تغييرُ تمثيلٍ يطالُ كلَّ قارئ.
                // (EN) The declared width — the expression-level peer of
                //      VariableInfo::declaredSurfaceType. `type` above is a STORAGE kind
                //      (Option B lowers every sub-64 width to i64), and نوع() asks a
                //      semantic question. Never lifted into `type`: that is a
                //      representation change affecting every reader.
                SadTypeKind declaredSurfaceType = SadTypeKind::Unknown;


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
                /// (AR) نوع الحمولة المُستنتَج (ISSUE-076، المسار أ′) — يُحسَب عند إنشاء
                ///      الحقل المؤجَّل من `adtEnumTable_[enum].variant.fieldTypeAt(idx)` حيث
                ///      تتوفّر الحالة؛ يُمرَّر لـ`ENUM_GET_PAYLOAD` ليُستخرَج بالنوع الحقيقيّ
                ///      (عشريّ/نصّ) بدل تصليب Integer. Unknown ⇒ مسار Integer القديم (تراجُع آمن).
                /// (EN) Inferred payload type (ISSUE-076, path A′) — computed when the deferred
                ///      field is created from `adtEnumTable_[enum].variant.fieldTypeAt(idx)` where
                ///      the variant is known; passed to `ENUM_GET_PAYLOAD` to extract with the real
                ///      type (float/string) instead of erased Integer. Unknown ⇒ old Integer path.
                SadTypeKind fieldType = SadTypeKind::Unknown;
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
                // (AR) [طبقة طبيعي — الخطوة ٥] النوع السطحيّ المُصرَّح **صراحةً** (طبيعي/بايت…)
                //      من تعليق `متغير <نوع>` أو نوع المعامل — لا المُستنتَج. `type` أعلاه قد
                //      يُرقّى بالاستنتاج (متغيّر مُسنَد من نداء طبيعي) بينما هذا يبقى Unknown ما لم
                //      يُصرَّح صراحةً. يُستعمَل حصرًا لقرار إشارة المقارنة (يُرآي resolveStaticType
                //      بالمفسّر) كي لا يختلف المساران على المُستنتَج. Unknown = لا نوع سطحيّ صريح.
                // (EN) [طبيعي layer — Step 5] The EXPLICITLY-declared surface type (طبيعي/بايت…)
                //      from a `var <type>` annotation or a parameter type — NOT inferred. `type`
                //      above may be promoted by inference (a var assigned from a طبيعي call) while
                //      this stays Unknown unless explicitly declared. Used ONLY for the comparison
                //      signedness decision (mirrors the interpreter's resolveStaticType) so the two
                //      tracks never disagree on inferred values. Unknown = no explicit surface type.
                SadTypeKind declaredSurfaceType = SadTypeKind::Unknown;
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
                // (AR) موقعُ تحكُّمٍ بحلقة: عدّادُ `لكل ي من … الى …` يملكُ موقعَه
                //      شرطُ الحلقةِ لا التصريحُ الذي يحملُ الاسمَ نفسَه. من دونِ هذه
                //      الرايةِ يجتازُ العدّادُ كلَّ شروطِ إعادةِ استعمالِ الموقع
                //      (محلّيٌّ · متغيّرٌ · عددٌ · `%<اسم>`) فيكتبُ التصريحُ الداخليُّ
                //      في موقعِ التحكّمِ ويُفسدُ عدَّ الدورات.
                // (EN) Loop-control storage: a range-loop counter's slot is owned by the
                //      loop condition, not by a same-named declaration inside the body.
                //      Without this flag the counter satisfies every slot-reuse condition
                //      (local, mutable, integer, `%<name>`), so an inner declaration would
                //      write into the control slot and corrupt the iteration count.
                // ================================================================
                bool isLoopControl = false; ///< (AR) عدّاد حلقة مدى؟ / (EN) Is a range-loop counter?

                // ================================================================
                // (AR) اسم دالة اللامدا المرتبطة (إذا كان المتغير يحمل بنية إغلاق)
                //      يُستخدم في Step 3.5 لتحديد نوع إرجاع CLOSURE_CALL
                //      بدون هذا: نوع الإرجاع يكون Integer افتراضياً → Boolean يضيع
                // (EN) Associated lambda function name (if variable holds closure)
                //      Used in Step 3.5 to determine CLOSURE_CALL return type
                // ================================================================
                std::string closureLambdaName; ///< (AR) اسم __lambda_X المرتبط / (EN) Associated __lambda_X name

                // (AR) هل المتغيّر مرجعُ دالّةٍ مولّدة (مثل `متغير د = عد` حيث عد مولّد)؟
                //      يُستعمل عند الاستدعاء غير المباشر `د()` لإصدار GENERATOR_CONSUME
                //      (وإلّا يبقى المقبض خامًا فينهار التكرار). يطابق المفسّر الذي يجمع
                //      قيم المولّد في مصفوفة عند الاستدعاء.
                // (EN) Does this variable hold a reference to a generator function (e.g.
                //      `var d = count` where count is a generator)? Used at the indirect
                //      call `d()` to emit GENERATOR_CONSUME (else the raw handle leaks and
                //      iteration crashes). Matches the interpreter which collects the
                //      generator's yields into an array at call time.
                bool isGeneratorFuncRef = false;

                // (AR) [موجة ABI المغاليق] أصلُ مرجعِ الدالّةِ المسمّاةِ: «متغير د = اسم_دالة»
                //      يحمل الاسمَ هنا، فينزعُ buildFunctionCall الوساطةَ (نداءٌ مباشرٌ لا
                //      CLOSURE_CALL) متى ثبتَ أنّ الاسمَ لا يُعادُ إسنادُه في البرنامج كلِّه
                //      (scanAssignedNames_ — برهانٌ غيرُ حسّاسٍ للتدفّق، فحلقةٌ تُعيدُ الإسنادَ
                //      تُسمِّمُ كلَّ المواقع). النداءُ المباشرُ يرثُ آلاتِ الوسمِ كلَّها —
                //      ترقيةَ العائدِ المصرَّحِ «رقم» الموسومةَ وتوسيعَ المعاملِ — التي يفقدُها
                //      عقدُ المغاليقِ i64 (وهو عقدُ UEFI بحرفِه فلا يُمَسّ).
                // (EN) [Closure-ABI wave] Provenance of a named function reference:
                //      «متغير د = funcName» carries the name here, and buildFunctionCall
                //      devirtualizes (direct CALL, not CLOSURE_CALL) once the name is proven
                //      never reassigned anywhere (scanAssignedNames_ — flow-INsensitive, so a
                //      loop that reassigns poisons every site). The direct call inherits the
                //      full tagging machinery — declared-«رقم» dyn return promotion and param
                //      widening — which the i64 closure ABI (UEFI's contract verbatim,
                //      untouchable) cannot carry.
                std::string funcRefProvenance;

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
                // ════════════════════════════════════════════════════════════════
                // (AR) 🔑 عرضُ الإرجاعِ **المُعلَنُ** — `returnType` أعلاه خانةُ تخزينٍ
                //      («الخيارُ ب» يُنزِلُ كلَّ عرضٍ دونَ ٦٤ إلى i64) فسؤالُه عن العرضِ
                //      يُجيبُ «رقم» دائمًا. والدوالُّ الحرّةُ تجدُ التصريحَ في `astDecl`،
                //      أمّا الطرائقُ فلا `astDecl` لها — فالحقلُ هنا هو طريقُها الوحيد.
                //      يقرؤه اليومَ رافعُ مُعامِلِ `نوع()` وحدَه؛ ولا يُرفَعُ إلى
                //      `returnType` لأنّ ذاك تغييرُ تمثيلٍ يطالُ كلَّ قارئٍ للنداء.
                // (EN) DECLARED return width. returnType above is a STORAGE kind (Option B
                //      lowers every sub-64 width to i64), so asking it for a width always
                //      answers «رقم». Free functions recover the declaration from astDecl;
                //      methods have no astDecl, so this field is their only path. Read today
                //      only by the نوع() operand lift; never lifted into returnType.
                // ════════════════════════════════════════════════════════════════
                SadTypeKind declaredReturnSurfaceType = SadTypeKind::Unknown;
                std::vector<SIRParameter> parameters;     ///< (AR) المعاملات / (EN) Parameters
                // (AR) فهارس المعاملات التي نوعُها **افتراضٌ** لا تصريحُ مستخدمٍ (المرحلة
                //      1.3 تسجّل Unknown رقمًا فيستحيل تمييزُهما من النوع وحده) — توسيعُ
                //      النداءِ الأماميِّ يقتصر عليها كي لا يدهس تصريحًا صريحًا (رصد
                //      مراجعة الجودة). فارغةٌ = لا معلومة (سلوك متحفظ: لا توسيع).
                // (EN) Indices of parameters whose kind is a DEFAULT, not a user
                //      declaration (Phase 1.3 lowers Unknown to Integer, so the kind
                //      alone cannot tell) — forward-call widening is limited to these
                //      so it never clobbers an explicit declaration (quality-review
                //      finding). Empty = no info (conservative: no widening).
                std::vector<bool> paramDefaulted;         ///< (AR) معامل بنوع افتراضي؟ / (EN) defaulted param kind?
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
                // (AR) نوع عنصر المصفوفة المُرجعة — Any إن أرجعت الدالّةُ مصفوفةً مختلطةً
                //      موسومةً زمنَ التشغيل. يضعه موقعُ الاستدعاء على نتيجته فتُقرأ الفهرسةُ
                //      اللاحقةُ موسومةً لا عدديًّا. (نظير returnClassName لكن لعنصر المصفوفة.)
                // (EN) Returned array element type — Any if the function returns a mixed,
                //      runtime-tagged array. The call site stamps it on the result so a later
                //      index reads the slot tagged, not as an integer. (Sibling of
                //      returnClassName but for the array element.)
                // ================================================================
                SadTypeKind returnElementType = SadTypeKind::Void; ///< (AR) نوع عنصر المصفوفة المُرجعة / (EN) Returned array element type

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
                // (AR) عمق «حاول» اللفظيّ عند دخول الحلقة — ليحسب «قف/أكمل» عددَ «حاول»
                //      التي يخرج منها (الحاجز ٧: خفض __sad_try_active على الخروج غير المحلّيّ).
                // (EN) Lexical «try» depth at loop entry — lets break/continue compute how many
                //      «try»s they exit (Barrier 7: decrement __sad_try_active on non-local exit).
                int tryDepthAtEntry = 0;
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

                /// (AR) أنواع حقول الحمولة — مُستنتَجة من نوع وسيط الباني وقت الإنشاء (ISSUE-076).
                ///      موازٍ لـ`fields` بالطول؛ Unknown حتّى يُسجَّل من موقع إنشاءٍ فعليّ. يُمكّن
                ///      استخراج الحمولة بنوعها الحقيقيّ (عشريّ/نصّ/منطقيّ) بدل تصليب Integer
                ///      المطموس — نظير النموذج الديناميّ للمفسّر. لا وسم MSB ولا malloc.
                /// (EN) Payload field types — inferred from the constructor argument type at
                ///      construction (ISSUE-076). Parallel to `fields`; Unknown until registered
                ///      from an actual construction site. Enables extracting the payload with its
                ///      real type (float/string/bool) instead of the erased hardcoded Integer —
                ///      mirroring the interpreter's dynamic model. No MSB tag, no malloc.
                std::vector<SadTypeKind> fieldTypes;

                /// (AR) هل هذه حالة بدون بيانات (Unit)؟
                /// (EN) Is this a unit variant (no data)?
                bool isUnit() const { return fields.empty(); }

                /// (AR) نوع الحقل بالفهرس (Unknown إن لم يُسجَّل أو خارج المدى)
                /// (EN) Field type by index (Unknown if unregistered or out of range)
                SadTypeKind fieldTypeAt(size_t idx) const
                {
                    return idx < fieldTypes.size() ? fieldTypes[idx] : SadTypeKind::Unknown;
                }

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

                /// (AR) نوع الحقل بالاسم — من أوّل حالة تحتويه (يوازي findFieldIndex).
                ///      يُستخدم للوصول المباشر `ش.نق` ليُستخرَج بنوعه الحقيقيّ (ISSUE-076، أ′).
                ///      Unknown إن لم يُوجَد أو لم يُسجَّل نوعه من موقع إنشاء.
                /// (EN) Field type by name — from the first variant containing it (mirrors
                ///      findFieldIndex). Used for direct access `s.r` to extract with the real
                ///      type (ISSUE-076, A′). Unknown if not found or type unregistered.
                SadTypeKind findFieldType(const std::string &fieldName) const
                {
                    for (const auto &v : variants)
                    {
                        int idx = v.findFieldIndex(fieldName);
                        if (idx >= 0)
                            return v.fieldTypeAt(static_cast<size_t>(idx));
                    }
                    return SadTypeKind::Unknown;
                }
            };

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
