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
#include <unordered_map>
#include <unordered_set>

namespace Sad
{
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

                // (AR) خريطة أسماء المتغيرات → أسماء الأصناف التي هي كائنات منها
                // (EN) Variable names → class names they are instances of
                std::unordered_map<std::string, std::string> classInstanceTypes_;

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

                // (AR) قائمة التحذيرات (غير قاتلة — تُطبَع ولا تمنع البناء). قناة منفصلة عن
                //      errors_ كي لا يُخلَط تحذيرٌ بخطأ فيفشل البناء خطأً أو ينجح خطأً (RFC:
                //      فصل التحذيرات عن الأخطاء في بانِي SIR).
                // (EN) Warning list (non-fatal — printed, does not block the build). A channel
                //      distinct from errors_ so a warning is never conflated with an error.
                std::vector<std::string> warnings_;

                // (AR) محلل الوحدات للاستيراد / (EN) Module resolver
                std::unique_ptr<Modules::ModuleResolver> moduleResolver_;

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
