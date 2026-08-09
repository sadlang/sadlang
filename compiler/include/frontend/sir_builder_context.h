// ======================================================================
// sir_builder_context.h - حالة بناء SIR المشتركة / Shared SIR Builder State
// ======================================================================
// الوصف بالعربية:
//   يحتوي هذا الملف على struct SIRBuilderContext الذي يجمع كل حالة
//   مُنشئ SIR (الوحدة، الدالة الحالية، النطاقات، الجداول، إلخ) في موقع
//   واحد. SIRBuilder يرث من هذا الـstruct (Phase 6 — F-06) كخطوة أولى
//   نحو فصل المُنشئ إلى builders فرعية تتشارك السياق.
//
// English Description:
//   This file contains struct SIRBuilderContext which gathers all the
//   state of the SIR builder (module, current function, scopes, tables,
//   etc.) in one place. SIRBuilder inherits from this struct (Phase 6 —
//   F-06) as a first step towards splitting the builder into sub-builders
//   sharing the context.
//
// المؤلف / Author: Sad Compiler Team — Phase 6 Step 1
// التاريخ / Date: 2025-12
// ======================================================================

#pragma once

#include "sir_module.h"
#include "sir_instruction.h"
#include "sir_types.h"
#include "module_resolver.h"
#include "ast_node.h"
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "module_nodes.h"

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace Sad
{
    // (AR) [GAP 4] إعلانٌ مسبقٌ لعقدة اللامدا (تُستعمل مؤشّرًا في خرائط المسح المُسبَق)
    // (EN) [GAP 4] Forward declaration of the lambda node (used as a pointer key in the pre-pass maps)
    namespace AST
    {
        class LambdaExpr;
    }
    namespace Compiler
    {
        namespace SIR
        {
            // (AR) إعلانات مسبقة للأنواع المعرّفة في sir_builder.h
            // (EN) Forward declarations for types defined in sir_builder.h
            struct VariableInfo;
            struct FunctionInfo;
            struct LoopContext;
            struct GenericScope;
            struct ADTEnumInfo;

            /**
             * @brief (AR) السياق المشترك لبناء SIR — يجمع كل حقول الحالة
             * @brief (EN) Shared SIR build context — gathers all state fields
             *
             * @details
             * (AR) Phase 6 — Step 1: نقل حرفي لجميع الحقول الخاصة من
             *      SIRBuilder إلى هذا الـstruct. SIRBuilder يرث منه عبر
             *      `class SIRBuilder : public SIRBuilderContext` لذا تبقى
             *      جميع الـ67 ملف cpp تعمل بدون تغيير (الوصول للحقول
             *      مباشر عبر this->field_ أو field_).
             *
             * (EN) Phase 6 — Step 1: literal move of all private state
             *      fields from SIRBuilder into this struct. SIRBuilder
             *      inherits via `class SIRBuilder : public SIRBuilderContext`
             *      so all 67 cpp files keep working without modification
             *      (direct field access via this->field_ or field_).
             */
            struct SIRBuilderContext
            {
                // ──────────────────────────────────────────────────────────
                // (AR) بنية سياق finally — تضمن تشغيل finally حتى عند ارجع
                //      داخل try/catch
                // (EN) Finally context — ensures finally runs even on return
                //      inside try/catch
                // ──────────────────────────────────────────────────────────
                struct FinallyContext
                {
                    std::string finallyLabel; ///< (AR) تسمية كتلة finally / (EN) Label of finally block
                    std::string exitLabel;    ///< (AR) تسمية كتلة الخروج / (EN) Label of exit block
                    std::string retValI64Reg; ///< (AR) alloca i64 لقيم الأرقام/البوليين / (EN) alloca i64 for int/bool returns
                    std::string retValPtrReg; ///< (AR) alloca ptr للنصوص/الكائنات / (EN) alloca ptr for string/object returns
                    std::string retTypeReg;   ///< (AR) alloca i64 لنوع القيمة / (EN) alloca i64 for return type tag
                    std::string hasReturnReg; ///< (AR) alloca i64 علامة "تم ارجع" / (EN) alloca i64 "did return" flag
                };

                // ──────────────────────────────────────────────────────────
                // (AR) حالة سياق البناء — تجمع المتغيرات التي تُحفظ وتُستعاد
                //      عند الدخول/الخروج من بناء دالة أو lambda أو defer
                // (EN) Builder context state — saves/restores variables when
                //      entering/exiting function, lambda, or defer building
                // ──────────────────────────────────────────────────────────
                struct SIRBuilderContextState
                {
                    std::shared_ptr<SIRFunction> function;            ///< (AR) الدالة الحالية / (EN) Current function
                    std::shared_ptr<SIRBasicBlock> block;             ///< (AR) الكتلة الحالية / (EN) Current block
                    std::vector<Sad::AST::Statement *> deferredStatements; ///< (AR) جمل التأجيل / (EN) Deferred stmts
                    std::string deferStackReg;                        ///< (AR) سجل مكدس التأجيل / (EN) Defer stack reg
                    std::string deferExecutedFlagReg;                 ///< (AR) علامة تنفيذ التأجيل / (EN) Defer executed flag
                    bool cleanupHandlerActive = false;                ///< (AR) معالج التنظيف نشط / (EN) Cleanup handler active
                    std::vector<FinallyContext> finallyStack;         ///< (AR) مكدس finally / (EN) Finally stack
                    std::string className;                            ///< (AR) اسم الصنف الحالي / (EN) Current class name
                };

                // ──────────────────────────────────────────────────────────
                // (AR) معلومات عضو فضاء أسماء (namespace member)
                // (EN) Namespace member info
                // ──────────────────────────────────────────────────────────
                struct NamespaceMemberInfo
                {
                    std::string kind;                        ///< "var" | "func" | "class"
                    std::string sirName;                     ///< (AR) الاسم في SIR / (EN) Name in SIR
                    SadTypeKind type = SadTypeKind::Integer; ///< (AR) نوع العضو / (EN) Member type
                };

                // ──────────────────────────────────────────────────────────
                // (AR) معلومات التقاط متغير في إغلاق
                // (EN) Closure variable capture info
                // ──────────────────────────────────────────────────────────
                struct CaptureInfo
                {
                    std::string varName;      ///< (AR) اسم المتغير الملتقط / (EN) Captured variable name
                    std::string registerName; ///< (AR) اسم السجل في النطاق الخارجي / (EN) Register name in outer scope
                    SadTypeKind type;         ///< (AR) نوع المتغير / (EN) Variable type
                    // (AR) [طبقة طبيعي64 — الخطوة ٧/٥] النوع السطحيّ المُصرَّح صراحةً (طبيعي64/بايت)
                    //      يُنقَل عبر الالتقاط كي يبقى قرار الإشارة (%،//،مقارنة) على المتغيّر
                    //      المُلتقَط مطابقًا للمفسّر. بدونه: طبيعي64 مُلتقَط يُرى Integer ⇒ موقَّع
                    //      (رصده أميليا: `1` مفسّر مقابل `-1` مترجم في إغلاق/دالّة متداخلة/أجّل).
                    // (EN) [طبيعي64 layer — Step 7/5] Explicitly-declared surface type (طبيعي64/Byte)
                    //      carried through capture so the signedness decision (%,//,comparison) on the
                    //      captured variable stays identical to the interpreter. Without it a captured
                    //      طبيعي64 is seen as Integer ⇒ signed (found by Amelia: `1` interp vs `-1`
                    //      compiler in a closure/nested-function/defer).
                    SadTypeKind declaredSurfaceType = SadTypeKind::Unknown;
                    // (AR) [GAP 1] نوعُ عنصر المصفوفة المُلتقَطة يُنقَل عبر الالتقاط. مصفوفةٌ
                    //      مختلطة/متجانسة غيرُ صحيحةٍ يكون عنصرُها Any، فتُقرأ فهرستُها داخل
                    //      جسم اللامدا موسومةً زمنَ التشغيل لا عدديًّا (نظير declaredSurfaceType
                    //      لكن لعنصر المصفوفة لا لإشارة العدد). بدونه: مصفوفةٌ مختلطةٌ مُلتقَطة
                    //      تُقرأ Int ⇒ قمامة (رصده الاختبار: `نصّ` مفسّر مقابل مؤشّرٍ مترجم).
                    // (EN) [GAP 1] Element type of a captured array carried through capture. A
                    //      mixed / homogeneous-non-int array has element Any, so its indexing
                    //      inside the lambda body reads runtime-tagged, not as int (mirrors
                    //      declaredSurfaceType but for the array element, not integer signedness).
                    SadTypeKind elementType = SadTypeKind::Void;
                };

                // ──────────────────────────────────────────────────────────
                // (AR) نتائج SIR المُجمَّعة من وحدة مستوردة (ذاكرة مخبئية)
                // (EN) Compiled SIR artifacts from imported module (cache)
                // ──────────────────────────────────────────────────────────
                struct ModuleSIRArtifacts
                {
                    std::vector<std::shared_ptr<SIRFunction>> functions;         ///< (AR) الدوال / (EN) Functions
                    std::vector<std::shared_ptr<SIRGlobalVariable>> globals;     ///< (AR) المتغيرات العامة / (EN) Globals
                    std::vector<std::shared_ptr<SIRClass>> classes;              ///< (AR) الأصناف / (EN) Classes
                    std::unordered_map<std::string, FunctionInfo> functionTable; ///< (AR) جدول الدوال / (EN) Function table
                    std::vector<std::string> exportedSymbols;                    ///< (AR) الرموز المُصدَّرة / (EN) Exported symbols
                };

                // ==================================================================
                // الحقول / State Fields
                // ==================================================================

                std::shared_ptr<SIRModule> module_;            ///< (AR) الوحدة الحالية / (EN) Current module
                std::shared_ptr<SIRFunction> currentFunction_; ///< (AR) الدالة الحالية / (EN) Current function
                std::shared_ptr<SIRBasicBlock> currentBlock_;  ///< (AR) الكتلة الحالية / (EN) Current block

                // (AR) مكدس التأجيل (defer) للدالة الحالية
                // (EN) Deferred statements stack for current function
                std::vector<Sad::AST::Statement *> deferredStatements_;

                // (AR) حالة defer وقت التشغيل / (EN) Runtime defer state
                std::string currentDeferStackReg_;
                std::string currentDeferExecutedFlagReg_;
                bool currentFunctionCleanupHandlerActive_ = false;

                // (AR) مكدس سياق finally / (EN) Finally context stack
                std::vector<FinallyContext> finallyStack_;

                // (AR) اسم الصنف الحالي أثناء بناء دوال الصنف
                // (EN) Current class name during method building
                std::string currentClassName_;

                // (AR) اسم الدالة الحالية أثناء مسح مواقع الاستدعاء
                // (EN) Current function name during call site scanning
                std::string currentScanFuncName_;

                // (AR) صنفُ معامِلِ دالّةٍ حرّة، مفتاحُه مركَّب: (اسمُ الدالّة، اسمُ المعامِل).
                //
                //      موضعُه هنا — في السياقِ المشترَك — لا في `TemplateBuilder`، لأنّ له
                //      قارئَينِ لا قارئًا واحدًا: المسحُ يُرقّي **خانةَ** المعامِل، والموزِّعُ
                //      يبثُّ **النداء**. وحينَ يحسمُ كلٌّ منهما المالكَ على حِدَة يختلفان،
                //      فتُرقّى الخانةُ بتوقيعِ صنفٍ ويُبَثُّ النداءُ إلى صنفٍ آخرَ ⇒ إجهاضُ
                //      LLVM: «Calling a function with a bad signature». فالحسمُ واحدٌ
                //      يُكتَبُ مرّةً ويُقرَأُ مرّتَين.
                //
                //      والمفتاحُ مركَّبٌ لأنّ خرائطَ الأسماءِ العاريةِ مسطَّحةٌ على البرنامجِ
                //      كلِّه، فيتصادمُ معامِلُ دالّةٍ مع متغيّرٍ محلّيٍّ يحملُ اسمَه في دالّةٍ
                //      أخرى. و`nullptr` قيمةً ليست غيابًا بل **تعارُضٌ محسوم**: موقعانِ
                //      يُمرّرانِ صنفَينِ مختلفَينِ ⇒ لا ترقيةَ ولا توجيه، فيبقى السلوكُ كما كان.
                // (EN) A free function's parameter class, under a composite key:
                //      (function name, parameter name).
                //
                //      It lives here — in the shared context — rather than in `TemplateBuilder`
                //      because it has two readers, not one: the scan promotes the parameter
                //      **slot**, the dispatcher emits the **call**. When each resolves the owner
                //      separately they disagree, so the slot is promoted with one class's
                //      signature while the call is emitted to another ⇒ an LLVM assertion:
                //      "Calling a function with a bad signature". One resolution, written once
                //      and read twice.
                //
                //      The key is composite because bare-name maps are flat over the whole
                //      program, so a parameter collides with a same-named local in another
                //      function. A `nullptr` value is not absence but a **settled conflict**: two
                //      sites pass different classes ⇒ neither promotion nor redirection, so the
                //      behaviour stays exactly as it was.
                std::map<std::pair<std::string, std::string>, std::string> paramClassBindings_;

                // (AR) خريطة اسم المتغيّر → نوع عنصر مصفوفته الحرفيّة أثناء المسح
                //      المُسبَق. تُملأ عند تصريح متغيّرٍ بمُهيّئٍ مصفوفةٍ مختلطةٍ قياسيّةٍ
                //      (⇒ Any)، وتُقرأ في استنتاج نوع المعامل حين يُمرَّر المتغيّرُ نفسُه
                //      وسيطًا (GAP 3b: توسيع وسيط المتغيّر إلى Any).
                // (EN) Variable name → its literal array element-type during the pre-pass
                //      scan. Populated when a variable is declared with a scalar-heterogeneous
                //      array initializer (⇒ Any); read in parameter-type inference when that
                //      same variable is passed as an argument (GAP 3b: variable-arg widening).
                std::unordered_map<std::string, SadTypeKind> scanVarElementType_;

                // (AR) خريطة «اسمُ الدالّة#اسمُ المتغيّر» → نوعُ مُهيّئه القياسيّ أثناء
                //      المسح المُسبَق (الطور 1.7). علّتُها: `inferExprType` لمتغيّرٍ محلّيٍّ
                //      كان يسقط إلى `Integer` الافتراضيّ — فالنطاقاتُ لم تُبنَ بعد وليس
                //      المتغيّرُ معامِلًا ولا عالميًّا. فدالّةٌ لا يُنادى معامِلُها إلّا
                //      بمتغيّرٍ محلّيٍّ نصّيٍّ يُجمَّد معامِلُها عددًا، فتُقارَن نصوصُه
                //      بالعناوين (`الحرف >= "0"` خطأٌ دائمًا) — وهي علّةُ فشلِ مكتبةِ
                //      جيسون مصرَّفةً بعد إصلاح ز.١٣ نفسِه.
                //      نُسجّل القياسيَّ وحدَه (نصّ/عشريّ/منطقيّ/بايت)؛ المصفوفاتُ لها
                //      scanVarElementType_ بدلالتها الخاصّة، والصحيحُ هو الافتراضُ أصلًا.
                // (EN) «funcName#varName» → its scalar initializer type during the Phase-1.7
                //      pre-pass. Why: inferExprType for a local variable fell through to the
                //      default `Integer` — scopes are not built yet, and the variable is
                //      neither a parameter nor a global. A function whose parameter is only
                //      ever called with a local string variable therefore froze that
                //      parameter as an integer, so its string comparisons ran on addresses
                //      (`الحرف >= "0"` always false) — the reason the JSON library still
                //      failed compiled after the ز.١٣ fix itself was correct.
                //      Only scalars are recorded (String/Float/Boolean/Byte); arrays have
                //      scanVarElementType_ with its own meaning, and Integer is the default.
                std::unordered_map<std::string, SadTypeKind> scanLocalVarType_;

                // (AR) [GAP 4] خريطتا المسح المُسبَق للامدات:
                //   (١) scanLambdaVar_: اسمُ المتغيّر (مُنطاقًا باسم الدالّة) → مؤشّرُ
                //       عقدة اللامدا التي هو مربوطٌ بها. تُملأ عند تصريح متغيّرٍ بمُهيّئٍ
                //       لامدا، وتُستعمل عند موقع النداء لإيجاد عقدة اللامدا من اسم المتغيّر.
                //   (٢) scanLambdaParamAny_: مؤشّرُ عقدة اللامدا → مجموعةُ فهارس معاملاتٍ
                //       يجب توسيعُها إلى Any (لأنّها استُقبلت مصفوفةً مختلطةً/متجانسةً غيرَ
                //       صحيحةٍ في موقع نداء). تُقرأ في buildExprLambda قبل بناء الجسم كي
                //       يُبنى الجسمُ عالمًا أنّ فهرسةَ المعامل موسومةٌ زمنَ التشغيل.
                //   المفتاحُ مؤشّرُ عقدة AST مستقرٌّ بين تمريرات المسح وبناء الجسم.
                // (EN) [GAP 4] Two lambda pre-pass maps:
                //   (1) scanLambdaVar_: variable name (scoped by function) → the LambdaExpr
                //       node it is bound to. Populated at a lambda-initialized VarDecl; used
                //       at a call site to find the lambda node from the callee variable name.
                //   (2) scanLambdaParamAny_: LambdaExpr node → set of parameter indices to
                //       widen to Any (because they received a mixed / homogeneous-non-int
                //       array at a call site). Read in buildExprLambda before the body is
                //       built, so the body knows the param's index reads runtime-tagged.
                //   The AST-node pointer key is stable across scan passes and body build.
                std::unordered_map<std::string, const Sad::AST::LambdaExpr *> scanLambdaVar_;
                std::unordered_map<const Sad::AST::LambdaExpr *, std::unordered_set<size_t>> scanLambdaParamAny_;

                // (AR) خريطة أسماء المتغيرات → أسماء الأصناف التي هي كائنات منها
                // (EN) Variable names → class names they are instances of
                std::unordered_map<std::string, std::string> classInstanceTypes_;

                // ────────────────────────────────────────────────────────────
                // (AR) استنتاج نوع المرميّ لكلّ «حاول» فعّال — لربط متغيّر «امسك»
                //      بنوعه الساكن الصحيح (كائن/رقم/نص) فيطابق نوع() المفسّرَ ويعمل
                //      وصولُ الحقل. تُدفَع خانةٌ عند دخول «حاول»، وتُسجّل كلُّ «ارمي»
                //      لفظيّة نوعَها في الخانة العليا، ثمّ تُلتقَط وتُنبثَق بعد بناء الجسم.
                // (EN) Per-active-«try» thrown-type inference — to bind the «catch» variable
                //      with its correct static type (object/number/string) so نوع() matches
                //      the interpreter and field access works. A slot is pushed on «try»
                //      entry; each lexical «throw» records its type into the top slot; the
                //      slot is captured and popped after the try body is built.
                struct ThrownInfo
                {
                    SadTypeKind kind = SadTypeKind::Void; ///< (AR) نوع القيمة المرميّة / (EN) thrown value kind
                    std::string className;                ///< (AR) اسم الصنف إن كانت كائنًا / (EN) class name if object
                    bool sawThrow = false;                ///< (AR) هل رُصدت أيّ «ارمي»؟ / (EN) any throw seen?
                    bool mixed = false;                   ///< (AR) أنواع متعدّدة ⇒ لا استنتاج / (EN) multiple types ⇒ no inference
                };
                std::vector<ThrownInfo> tryThrownStack_;

                // (AR) عمق «حاول» اللفظيّ الحاليّ أثناء التوليد (الحاجز ٧). يُزاد عند بناء
                //      جسم «حاول» ويُنقَص بعده. تستعمله جُمَل الخروج غير المحلّيّ (ارجع/قف/
                //      أكمل) لتبعث __sad_try_exit بعدد «حاول» التي تخرج منها، فلا يتسرّب
                //      __sad_try_active (تسرّبه يجعل حاجزَ القسمة يقفز لإطارٍ ميّت ⇒ انهيار).
                // (EN) Current lexical «try» depth during codegen (Barrier 7). Incremented while
                //      building a «try» body, decremented after. Non-local-exit statements
                //      (return/break/continue) use it to emit __sad_try_exit for each «try» they
                //      leave, so __sad_try_active never leaks (a leak makes the division guard
                //      longjmp into a dead frame ⇒ crash).
                int currentTryDepth_ = 0;

                // (AR) خريطة أنواع الأصناف للمعاملات (مُستنتجة من call sites)
                // (EN) Class type map for function parameters (inferred from call sites)
                std::unordered_map<std::string, std::unordered_map<std::string, std::string>> paramClassTypes_;

                int nextTempRegister_ = 0;  ///< (AR) رقم السجل المؤقت التالي / (EN) Next temp register
                int nextLabel_ = 0;         ///< (AR) رقم التسمية التالية / (EN) Next label number
                int currentScopeLevel_ = 0; ///< (AR) مستوى النطاق الحالي / (EN) Current scope level

                // (AR) مكدس النطاقات / (EN) Scope stack
                std::vector<std::unordered_map<std::string, VariableInfo>> scopeStack_;

                // (AR) جدول الدوال / (EN) Function table
                std::unordered_map<std::string, FunctionInfo> functionTable_;

                // (AR) جدول الأصناف / (EN) Class table
                std::unordered_map<std::string, std::shared_ptr<SIRClass>> classTable_;

                // (AR) القيم الافتراضية لحقول البنى: "اسم البنية" → [(اسم الحقل، تعبير القيمة الافتراضية)]
                //      تُملأ عند معالجة StructDecl وتُستهلَك في buildNewObject لتهيئة الحقول (ISSUE-036).
                //      المؤشّر خام لأن AST::Expression مملوكة عبر unique_ptr في شجرة AST الحيّة طوال البناء.
                // (EN) Struct field default values: structName → [(fieldName, defaultExpr)]
                //      Filled when processing StructDecl, consumed in buildNewObject to init fields (ISSUE-036).
                //      Raw pointer because AST::Expression is owned via unique_ptr in the live AST during build.
                std::unordered_map<std::string,
                                   std::vector<std::pair<std::string, Sad::AST::Expression *>>>
                    structFieldDefaults_;

                // (AR) الحقول الساكنة: "صنف.حقل" → نوع
                // (EN) Static fields: "class.field" → type
                std::unordered_map<std::string, SadTypeKind> staticFields_;

                // (AR) ISSUE-058: نوع الحقل المُستنتَج من إسناداته عبر البرنامج
                //      (اسم الحقل → نوع بدائيّ) — يُملأ بمسحٍ مسبق في buildModule، ويستشيره
                //      باني الصنف للحقول غير المُصرَّحة بلا مُهيّئ (بدل افتراض Pointer الذي
                //      ينهار عند تخزين عدد). القيمة Unknown تعني «متضارب/غير بدائيّ ⇒ لا تستنتج».
                // (EN) ISSUE-058: field type inferred from its assignments across the program
                //      (fieldName → primitive type) — populated by a pre-scan in buildModule and
                //      consulted by the class builder for untyped/uninitialized fields (instead of
                //      the Pointer default that crashes when an integer is stored). Value Unknown
                //      means "ambiguous/non-primitive ⇒ do not infer".
                std::unordered_map<std::string, SadTypeKind> inferredFieldTypes_;

                // (AR) الدوال الساكنة: "صنف.دالة"
                // (EN) Static methods: "class.method"
                std::unordered_set<std::string> staticMethods_;

                // (AR) أسماء الدوال المُستخدمة كمراجع (function references)
                // (EN) Function names used as references
                std::unordered_set<std::string> funcRefNames_;

                // (AR) خريطة أعضاء الفضاءات
                // (EN) Namespace members map
                std::unordered_map<std::string, std::unordered_map<std::string, NamespaceMemberInfo>> namespaceMembers_;

                // (AR) جدول التعدادات الجبرية (ADT)
                // (EN) ADT enum table
                std::unordered_map<std::string, ADTEnumInfo> adtEnumTable_;

                // (AR) ثوابت أعضاء التعداد النصّيّة: "التعداد.العضو" → القيمة النصّيّة.
                //   المسار البسيط للتعداد يفرض i64، فأعضاء القيم النصّيّة (مثل
                //   «تعداد ألوان { أحمر = "أحمر" }») تُخزَّن هنا ويعيدها وصولُ العضو
                //   كثابتٍ نصّيّ — تكافؤًا مع المفسّر الذي يشرّف القيمة النصّيّة.
                // (EN) String-valued enum member constants: "Enum.Member" → string.
                //   The simple-enum path forces i64; string-valued members are stored
                //   here and returned as string constants by member access — matching
                //   the interpreter, which honors the explicit string value.
                std::unordered_map<std::string, std::string> enumStringConstants_;

                // (AR) مكدس سياق الحلقات / (EN) Loop context stack
                std::vector<LoopContext> loopStack_;

                // (AR) أسماء مستعارة لدوال Lambda
                // (EN) Lambda aliases
                std::unordered_map<std::string, std::string> lambdaAliases_;

                // (AR) خريطة أنواع القنوات
                // (EN) Channel element type map
                std::unordered_map<std::string, SadTypeKind> channelTypeMap_;

                // (AR) التقاطات الإغلاقات
                // (EN) Closure captures
                std::unordered_map<std::string, std::vector<CaptureInfo>> closureCaptures_;

                // (AR) [ISSUE-053] راية: عند تحليل المتغيّرات الحرّة للامدا (لا لـdefer)
                //      نلتقط أيضًا المتغيّر المُسنَد إليه (هدف الإسناد) كمتغيّر حرّ، كي تدوم حالة
                //      الإغلاق المُعيد للامدا تُعدّل محلّيًّا (مصنع عدّاد). يجب أن تبقى false لـdefer
                //      لأنّ defer يُعدّل المتغيّر الخارجيّ عبر المكدّس المشترك لا بالالتقاط بالقيمة.
                // (EN) [ISSUE-053] Flag: while analyzing a lambda's free vars (NOT defer) we also
                //      capture the assignment target var as free, so a closure returning a lambda
                //      that mutates a local (counter factory) keeps its state. Must stay false for
                //      defer, which mutates the outer var via the shared scope stack, not by value.
                bool captureAssignTargetsInClosure_ = false;

                // (AR) مكدس نطاقات الأنواع العامة
                // (EN) Generic scopes stack
                std::vector<GenericScope> genericScopeStack_;

                // (AR) خريطة قوالب الدوال
                // (EN) Template function map
                std::unordered_map<std::string, Sad::AST::TemplateFunctionDecl *> templateFunctions_;

                // (AR) خريطة قوالب الأصناف
                // (EN) Template class map
                std::unordered_map<std::string, Sad::AST::TemplateClassDecl *> templateClasses_;

                // (AR) التنفيذات الافتراضية للسمات
                // (EN) Trait default implementations
                std::unordered_map<std::string, Sad::AST::TraitDecl *> traitDefaultImpls_;

                // (AR) الدوال المُنشأة من القوالب
                // (EN) Instantiated template functions
                std::unordered_map<std::string, std::shared_ptr<SIRFunction>> instantiatedTemplates_;

                // (AR) خريطة الماكروز
                // (EN) Macros map
                std::unordered_map<std::string, Sad::AST::MacroDecl *> macros_;

                // (AR) قائمة الأخطاء (قاتلة — تمنع إنتاج الثنائيّ) / (EN) Error list (fatal)
                std::vector<std::string> errors_;

                // (AR) (اللبنة 3.17) أسماء رموز @رمز المُصدَّرة لتعريفات ذات جسم، مفصولةً
                //      حسب النوع (دالّة/متغيّر) ليتقاطع كلّ مسارٍ مع الآخر. حارس تصادم:
                //      تعريفان باسم رابط واحد يتصادمان صامتًا في الخلفيّة — دالّتان تُدمَجان
                //      (الثاني يُفقَد)، ودالّة/متغيّر يُعيد LLVM تسمية أحدهما (@اسم.1) فيَضيع
                //      عقد الـABI ⇒ نرفض التكرار بخطأ (SEM024). الفصل بمجموعتين يُبقي تصادم
                //      متغيّر/متغيّر لحارسه القائم (SEM022 خلفيًّا) بلا تغيير، ويرفع تصادم
                //      دالّة/دالّة ودالّة/متغيّر إلى SEM024 مستقلًّا عن ترتيب المعالجة.
                // (EN) (Brick 3.17) @رمز exported symbol names for defined (bodied) entities,
                //      split by kind (function/variable) so each path cross-checks the other.
                //      Collision guard (SEM024): functions merge silently; a function sharing a
                //      variable's name makes LLVM rename one (@name.1), losing the ABI contract.
                //      Two sets keep variable/variable collisions on their existing guard (SEM022,
                //      backend) untouched, and raise function/function + function/variable to
                //      SEM024 order-independently.
                std::unordered_set<std::string> exportedFunctionLinkNames_;
                std::unordered_set<std::string> exportedVarLinkNames_;

                // (AR) علَمٌ: هل الذراع الحاليّ ميتٌ ساكنًا؟ يضبطه failAlways داخل الدائرة
                //      القصيرة (SC) عند فشلٍ بنيويّ غير مشروط، فيُصفَّر قبل كلّ ذراعٍ في
                //      buildMatchStatement. يقرأه ربطُ المتغيّرات الصوريّ: لا نربط قيمةً
                //      صوريّة (صفر) إلّا للذراع الميت ساكنًا — وإلّا لأخفينا ثغرةً صامتة
                //      في ذراعٍ حيٍّ فقد ربط حمولته (نقد Amelia: تعداد داخل قائمة بلا حارس).
                // (EN) Flag: is the current arm statically dead? Set by failAlways inside the
                //      short-circuit (SC) on an unconditional structural fail; reset before each
                //      arm in buildMatchStatement. Read by the dummy variable-binding pass: bind
                //      a dummy (zero) ONLY for a statically-dead arm — otherwise we'd mask a
                //      silent divergence in a LIVE arm that lost its payload binding.
                bool matchArmStaticallyDead_ = false;

                // (AR) [أ-م٥] اسم تعداد قيمة المطابقة الحاليّة (className المتتبَّع للقيمة
                //      المُطابَقة). يضبطه buildMatchStatement قبل حلقة الأذرع ويُستعيده بعدها
                //      (آمنٌ للتداخل). يقرأه نمطُ المتغيّر في buildMatchPatternCondition ليميّز
                //      المعاملَ الوحدويّ العاري «عندما فراغ:» (فحص وسمٍ ENUM_IS_VARIANT بلا ربط)
                //      عن الرباط الشامل الحقيقيّ «عندما س:» على قيمةٍ عاديّة (يبقى شاملًا).
                // (EN) [A-M5] Enum name of the current match value (its tracked className). Set
                //      by buildMatchStatement before the arm loop and restored after it (nesting-
                //      safe). Read by the variable pattern in buildMatchPatternCondition to tell a
                //      bare unit variant «when None:» (ENUM_IS_VARIANT tag check, no binding) from
                //      a genuine catch-all binding «when x:» on an ordinary value (stays catch-all).
                std::string currentMatchEnumName_;

                // (AR) قائمة التحذيرات (غير قاتلة — تُطبَع ولا تمنع البناء). قناة منفصلة عن
                //      errors_ كي لا يُخلَط تحذيرٌ بخطأ فيفشل البناء خطأً أو ينجح خطأً (RFC:
                //      فصل التحذيرات عن الأخطاء في بانِي SIR).
                // (EN) Warning list (non-fatal — printed, does not block the build). A channel
                //      distinct from errors_ so a warning is never conflated with an error.
                std::vector<std::string> warnings_;

                // (AR) محلل الوحدات للاستيراد / (EN) Module resolver
                std::unique_ptr<Modules::ModuleResolver> moduleResolver_;

                // (AR) أشجار AST للوحدات المستوردة (متعدّيًا) — يملؤها
                //      preRegisterImportedSignatures قبل الطور 1.7 كي يمسحها
                //      inferParamTypesFromCallSites أيضًا، فيُستنتَج انتشارُ الأنواع
                //      المتعدّي عبر حدود الوحدات (رسالة⇒تحية). مؤشّرات معارة إلى
                //      خبيئة ModuleResolver (تبقى حيّةً طوال الترجمة). تُمسح لكلّ وحدة.
                // (EN) Imported module ASTs (transitive) — populated by
                //      preRegisterImportedSignatures before Phase 1.7 so
                //      inferParamTypesFromCallSites scans them too, enabling
                //      transitive type propagation across module boundaries
                //      (رسالة⇒تحية). Borrowed pointers into the ModuleResolver cache
                //      (alive for the whole compilation). Cleared per module.
                std::vector<Sad::AST::StmtList *> importedModuleBodies_;

                // (AR) أسماء الدوالّ التي بذرها preRegisterImportedSignatures كتواقيعَ
                //      مستوردةٍ (sirFunction=nullptr) قبل الطور 1.7. حارس التخطّي في
                //      buildFromImportStmt يبني هذه وحدها إن لم تُبنَ بعد، ولا يمسّ
                //      دلالةَ الدوالّ المحلّيّة المسجَّلة في الطور 1. تُمسح لكلّ وحدة.
                // (EN) Names seeded by preRegisterImportedSignatures as imported
                //      signatures (sirFunction=nullptr) before Phase 1.7. The skip-guard
                //      in buildFromImportStmt builds only these if not yet built, without
                //      touching the semantics of local functions registered in Phase 1.
                //      Cleared per module.
                std::unordered_set<std::string> preRegisteredImportNames_;

                // (AR) الأسماء المستعارة «كـ» ⇐ الاسم الأصليّ. المفسّر ينفّذ «كـ» بربط
                //      الرمز باسمه الجديد؛ وكان المصرّف يبني الرمزَ باسمه الأصليّ ويُسقط
                //      المستعارَ صامتًا ⇒ «استدعاء دالة غير معرّفة» (ISSUE-085/088).
                //      🔑 مُنطَّقةٌ بالملفّ المُسجِّل: تُملأ من عبارات ملفٍّ واحدٍ عُليا
                //      (collectFileImportAliases)، ويُبدّلها ImportAliasScope بخريطةِ
                //      الوحدةِ قبل بناء أجسامها ويستعيدها بعده. «كـ» ربطُ اسمٍ محلّيٌّ
                //      للملفّ الذي كتبه، فلو شُورِكت خريطةٌ واحدةٌ للبرنامج لتسرّبت
                //      استعارةُ الوحدة إلى مستعمِلها فاختطفت مدمَجًا يصادف الاسمَ
                //      (اطبع_سطر مثلًا) ⇒ كودٌ خاطئٌ صامت.
                // (EN) Alias («as») ⇒ original name. The interpreter implements it by
                //      binding the symbol to its new name; the compiler used to build the
                //      symbol under its original name and drop the alias silently.
                //      🔑 Scoped to the recording file: filled from ONE file's top-level
                //      statements (collectFileImportAliases) and swapped for the module's
                //      own map by ImportAliasScope while its bodies are built. Sharing one
                //      program-wide map leaked a module's alias into its consumer and
                //      hijacked a builtin sharing the name ⇒ silent miscompile.
                std::unordered_map<std::string, std::string> importAliases_;

                // (AR) مفاتيحُ النطاق العامّ التي بذَرها احتياطيُّ الاستعارة في
                //      scopeStack_.front(). الخريطةُ وحدَها لا تكفي للتنطيق: التخبئةُ
                //      تُبقي الربطَ في النطاق العامّ **الدائم** فيعبُر حدَّ الوحدة كما
                //      كانت الخريطةُ تعبُره — فيَقبل المصرّفُ اسمًا لم يُستورَد، وأخطرُ
                //      منه أن تقرأ وحدةٌ شقيقةٌ قيمةَ أختِها بالاسم نفسِه صامتةً.
                //      فيُمسَك ما بُذر ليُمحى عند مغادرة نطاقه — ما بذَره الاحتياطيُّ
                //      وحدَه، لا كلُّ مفتاحٍ يصادف الاسمَ (وإلّا مُحي عامٌّ مشروع).
                // (EN) Global-scope keys seeded by the alias fallback into
                //      scopeStack_.front(). Scoping the map alone is not enough: the cache
                //      keeps the binding in the PERMANENT global scope, so it crosses the
                //      module boundary just as the map used to — the compiler then accepts
                //      a never-imported name, and worse, a sibling module silently reads
                //      its sibling's value under the same name. Only what the fallback
                //      itself seeded is erased on scope exit, never a legitimate global.
                std::unordered_set<std::string> aliasSeededGlobals_;

                // (AR) فضاءاتُ أسماءِ الوحدات: الاسمُ الفعّالُ لـ«استورد م [كـ ر]» ⇐ اسمُ
                //      الوحدة الكامل. المفسّرُ يجعل هذه الصيغةَ **فضاءَ أسماءٍ حصرًا**
                //      (`ر.جمع(…)` تعمل والمسطَّحةُ `جمع(…)` تُخطئ SEM004)، والمصرّفُ كان
                //      **يُسطّح حصرًا** فيفشل المؤهَّلُ بـ«Undefined variable 'ر'» —
                //      تباعُدٌ متعاكسٌ تامٌّ لا يعمل معه أيُّ برنامجٍ في المحرّكَين
                //      (ISSUE-090). فتُقيَّد هنا ليقبل المصرّفُ الصيغةَ المؤهَّلةَ كذلك،
                //      حالًّا إيّاها إلى الرمز المسطَّح الذي يبنيه أصلًا.
                //      🔑 مُنطَّقةٌ بالملفّ كـ importAliases_ وللسبب نفسِه: «استورد م»
                //      في وحدةٍ لا يُتيح فضاءَها لمستعمِلها.
                // (EN) Module namespaces: the effective name of `استورد م [كـ ر]` ⇒ the full
                //      module name. The interpreter makes this form a namespace EXCLUSIVELY
                //      (`ر.جمع(…)` works, flat `جمع(…)` raises SEM004) while the compiler
                //      flattened EXCLUSIVELY, failing the qualified form with "Undefined
                //      variable 'ر'" — a fully inverted divergence under which no program
                //      using this form ran on both engines (ISSUE-090). Recorded here so the
                //      compiler also accepts the qualified form, resolving it to the flat
                //      symbol it already builds.
                //      🔑 File-scoped like importAliases_ and for the same reason: an
                //      `استورد م` inside a module does not expose m's namespace to its user.
                std::unordered_map<std::string, std::string> moduleNamespaces_;

                // (AR) رموزُ كلِّ وحدةٍ المُصدَّرة، بمفتاحِ اسمِ الوحدةِ الكامل. لازمةٌ
                //      لأنّ الخريطةَ أعلاه تقول «ر وحدةٌ» ولا تقول «أيُّ رمزٍ فيها»:
                //      فبدونها كان النداءُ المؤهَّلُ يُحَلّ من جدولِ الدوالِّ **العامّ**
                //      فيقبل رمزًا من وحدةٍ أخرى لم تُؤهَّل به، وأخطرُ منه أنّ وحدتَين
                //      تُصدّران الاسمَ نفسَه تنهاران إلى رمزٍ مسطَّحٍ واحدٍ فيفوز آخِرُهما
                //      صامتًا (`فضاء_أول.أعط()` و`فضاء_ثان.أعط()` تُرجعان القيمةَ عينَها).
                //      وهو ما كشفته مراجعةُ أميليا: انحدارٌ من فشلٍ صريحٍ إلى **قيمةٍ
                //      خاطئةٍ صامتة** في عينِ الحالةِ التي وُجد فضاءُ الأسماءِ من أجلها.
                //      غيرُ مُنطَّقةٍ بالملفّ عمدًا: «ما تُصدّره الوحدةُ م» حقيقةٌ عنها
                //      لا ربطٌ محلّيٌّ لمن استوردها.
                // (EN) Each module's exported symbols, keyed by full module name. Required
                //      because the map above says "ر is a module" but not "which symbols it
                //      holds": without it a qualified call resolved against the GLOBAL function
                //      table, accepting a symbol from a different module, and — worse — two
                //      modules exporting the same name collapse to one flat symbol so the last
                //      one silently wins. Deliberately NOT file-scoped: "what module m exports"
                //      is a fact about m, not a local binding of whoever imported it.
                std::unordered_map<std::string, std::set<std::string>> moduleExportedSymbols_;

                // (AR) الوحداتُ التي يجري إتْباعُ إعادةِ تصديرٍ إليها الآن — قاطعُ الدور.
                //      انظر buildReExportStmt: بدونه تنهار وحدتان تُعيد كلٌّ تصديرَ
                //      الأخرى بانهيارِ مكدّسٍ بلا تشخيص.
                // (EN) Modules whose re-export is currently being followed — the cycle
                //      breaker. See buildReExportStmt: without it two mutually re-exporting
                //      modules die by stack overflow with no diagnostic.
                std::unordered_set<std::string> reExportsInFlight_;

                // (AR) أنواعُ ثوابتِ/متغيّراتِ الوحدات المستوردة العُليا، مبذورةً قبل
                //      الطور 1.7. عِلّتُها: عوالمُ الوحدة لا تُبعَث في module_ إلّا في
                //      الطور 2، فيرى inferExprType اسمَ ثابتٍ نصّيٍّ في موقعِ نداءٍ
                //      داخلَ الوحدة مجهولًا ⇒ Integer الافتراضيّ ⇒ يبقى المعاملُ عددًا
                //      ⇒ تُطوى مقارنتُه بنصٍّ ثابتًا (اختصارُ المقارنة الصارمة) ⇒
                //      «فشل بناء شرط إذا». تُمسح لكلّ وحدة.
                // (EN) Types of imported modules' top-level consts/vars, seeded before
                //      Phase 1.7. Module globals are only emitted into module_ in Phase 2,
                //      so inferExprType sees a string const's name at an intra-module call
                //      site as unknown ⇒ default Integer ⇒ the parameter stays numeric ⇒
                //      its comparison against a string folds to a constant (strict-compare
                //      shortcut) ⇒ "Failed to build if condition". Cleared per module.
                std::unordered_map<std::string, SadTypeKind> pendingGlobalTypes_;

                // (AR) مسار الملف الحالي / (EN) Current file path
                std::string currentFilePath_;

                // (AR) وضع الوحدة (تخطي __sad_main)
                // (EN) Module mode (skip __sad_main)
                bool moduleMode_ = false;

                // (AR) وضع الترجمة الحرّة (--freestanding) — تستشيره الواجهة الأماميّة
                //      في بوّابة استيراد المدمجات (لا مكتبة قياسيّة تُستورد في هذا الوضع)
                // (EN) Freestanding mode (--freestanding) — consulted by the frontend
                //      builtin import gate (no stdlib to import in this mode)
                bool freestandingMode_ = false;

                // (AR) الوحدات التي تمت معالجتها / (EN) Processed modules
                std::unordered_set<std::string> processedModules_;

                // (AR) ذاكرة مخبئية للوحدات المُجمَّعة
                // (EN) Compiled module cache
                std::unordered_map<std::string, ModuleSIRArtifacts> moduleCache_;

                // ==================================================================
                // helpers — حفظ/استعادة الحالة / Save/Restore Helpers
                // ==================================================================

                /**
                 * @brief (AR) حفظ حالة السياق الحالية وإعادة تعيين المتغيرات لنطاق جديد
                 * @brief (EN) Save current context state and reset for a new scope
                 */
                SIRBuilderContextState saveContext()
                {
                    SIRBuilderContextState state;
                    state.function = currentFunction_;
                    state.block = currentBlock_;
                    state.deferredStatements = std::move(deferredStatements_);
                    state.deferStackReg = currentDeferStackReg_;
                    state.deferExecutedFlagReg = currentDeferExecutedFlagReg_;
                    state.cleanupHandlerActive = currentFunctionCleanupHandlerActive_;
                    state.finallyStack = std::move(finallyStack_);
                    state.className = currentClassName_;

                    // (AR) إعادة تعيين الحالة لنطاق جديد نظيف
                    // (EN) Reset state for a clean new scope
                    deferredStatements_.clear();
                    currentDeferStackReg_.clear();
                    currentDeferExecutedFlagReg_.clear();
                    currentFunctionCleanupHandlerActive_ = false;
                    finallyStack_.clear();

                    return state;
                }

                /**
                 * @brief (AR) استعادة حالة السياق من حالة محفوظة سابقاً
                 * @brief (EN) Restore context state from a previously saved state
                 */
                void restoreContext(SIRBuilderContextState &&state)
                {
                    currentFunction_ = state.function;
                    currentBlock_ = state.block;
                    deferredStatements_ = std::move(state.deferredStatements);
                    currentDeferStackReg_ = state.deferStackReg;
                    currentDeferExecutedFlagReg_ = state.deferExecutedFlagReg;
                    currentFunctionCleanupHandlerActive_ = state.cleanupHandlerActive;
                    finallyStack_ = std::move(state.finallyStack);
                    currentClassName_ = state.className;
                }
            };

        } // namespace SIR
    }     // namespace Compiler
} // namespace Sad
