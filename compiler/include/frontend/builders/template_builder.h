// ======================================================================
// template_builder.h - بنّاء القوالب والاستيراد والاستنتاج
// ======================================================================
// (AR) Phase 6 — Step 8 (الأخيرة): صنف منفصل يجمع 15 method:
//      6 templates/import (buildTemplateFunction, instantiateTemplate x2,
//      setCurrentFilePath, buildImportStmt, buildFromImportStmt)
//      + 1 inferReturnTypeFromBody
//      + 5 lambda inference (scanCallSitesIn{Expr,Stmt}, inferLambdaParamFrom{Expr,Stmt}, inferParamTypesFromCallSites)
//      + 3 type inference (collectFreeVars{Expr,Stmt}, inferExprType).
// (EN) Phase 6 — Step 8 (final): standalone class for templates, imports,
//      and inference (15 methods).
// ======================================================================

#pragma once

#include "../sir_types.h"
#include "../sir_instruction.h"

#include <memory>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// (AR) StmtList هو using alias في ast_node.h — نضمنه بدلاً من forward-decl
// (EN) StmtList is a using alias in ast_node.h — include it instead of forward-decl
#include "ast_node.h"

namespace Sad
{
    namespace AST
    {
        class Expression;
        class Statement;
        class FunctionDecl;
        class TemplateFunctionDecl;
        class ImportStmt;
        class FromImportStmt;
    }
}

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            class SIRBuilder;
            struct BuildResult;

            /**
             * @brief (AR) بنّاء القوالب والاستيراد والاستنتاج (15 method)
             * @brief (EN) Templates, imports & inference builder (15 methods)
             */
            class TemplateBuilder
            {
            public:
                explicit TemplateBuilder(SIRBuilder &b) : b_(b) {}

                void buildTemplateFunction(Sad::AST::TemplateFunctionDecl *templateDecl);

                void setCurrentFilePath(const std::string &filePath);

                void buildImportStmt(Sad::AST::ImportStmt *importStmt);

                void buildFromImportStmt(Sad::AST::FromImportStmt *fromImportStmt);

                /**
                 * @brief (AR) إتْباعُ إعادةِ تصديرٍ إلى وحدةِ مصدرِها
                 * @brief (EN) Follow a re-export through to its source module
                 *
                 * (AR) «صدّر * من م» في وحدةٍ وسيطةٍ يُتيح رموزَ م لمستورِدِ الوسيطة.
                 *      وكان الطوران يمرّان على `ReExportStmt` دون التقاطها، فيقف
                 *      المصرّفُ عند الوسيطةِ فلا يجد الرمزَ ويقع على مدمَجٍ يصادف
                 *      الاسمَ (ISSUE-089). ولمّا كان المصرّفُ يُسطّح، فإتْباعُها
                 *      **مطابقٌ دلاليًّا** لاستيرادٍ انتقائيٍّ من م — فتُمرَّر إليه.
                 *      و«صدّر *» المجرّدةُ (مسارٌ فارغ) لا وحدةَ مصدرٍ لها فهي لا-عمليّة.
                 * (EN) `صدّر * من م` in an intermediate module exposes m's symbols to the
                 *      intermediate's importer. Both passes walked past `ReExportStmt`, so the
                 *      compiler stopped at the intermediate, missed the symbol and fell onto a
                 *      builtin of the same name (ISSUE-089). Since the compiler flattens,
                 *      following it is semantically identical to a selective import from m, so
                 *      it is delegated there. Bare `صدّر *` (empty path) has no source module
                 *      and is a no-op.
                 */
                void buildReExportStmt(Sad::AST::ReExportStmt *reExportStmt);

                SadTypeKind inferReturnTypeFromBody(const Sad::AST::Statement *body, const Sad::AST::FunctionDecl *funcDecl);

                void scanCallSitesInExpr(const Sad::AST::Expression *expr);

                void scanCallSitesInStmt(const Sad::AST::Statement *stmt);

                void inferLambdaParamFromExpr(const Sad::AST::Expression *expr, const std::set<std::string> &paramNames, std::unordered_map<std::string, SadTypeKind> &result);

                void inferLambdaParamFromStmt(const Sad::AST::Statement *stmt, const std::set<std::string> &paramNames, std::unordered_map<std::string, SadTypeKind> &result);

                void inferParamTypesFromCallSites(Sad::AST::StmtList *program);

                // (AR) تسجيل تواقيع دوالّ الوحدات المستوردة انتقائيًّا مسبقًا في
                //      functionTable_ قبل الطور 1.7، كي يقدر inferParamTypesFromCallSites
                //      على استنتاج أنواع معاملاتها غير المصرَّحة من مواقع النداء في
                //      الوحدة الرئيسة (وإلّا بُنيت بأنواع Integer افتراضيّة فتُطبع مشوَّهة).
                // (EN) Pre-register selectively-imported module function signatures in
                //      functionTable_ before Phase 1.7, so inferParamTypesFromCallSites
                //      can infer their undeclared parameter types from the main module's
                //      call sites (otherwise built with default Integer params → garbled).
                void preRegisterImportedSignatures(Sad::AST::StmtList *program);

                /**
                 * @brief (AR) جمعُ رُبُطِ الاستيرادِ المحلّيّة من عبارات ملفٍّ واحدٍ عُليا
                 * @brief (EN) Collect ONE file's local import bindings from its top level
                 *
                 * (AR) رَبطان لا رَبطٌ واحد: الأسماءُ المستعارةُ «كـ» للرموز، وفضاءاتُ
                 *      أسماءِ الوحدات لـ«استورد م [كـ ر]» (ISSUE-090). كلاهما محلّيٌّ
                 *      للملفّ فيُجمعان معًا ويُنطَّقان معًا.
                 * (EN) Two bindings, not one: symbol «as» aliases, and module namespaces for
                 *      `استورد م [كـ ر]` (ISSUE-090). Both are file-local, so they are
                 *      collected together and scoped together.
                 *
                 * (AR) تمسح العباراتِ العُليا للملفّ المُمرَّر — «من م استورد س كـ ص»
                 *      و«استورد س كـ ص من م» و«صدّر س كـ ص من م» — فتُقيّد ص ⇐ س في
                 *      الخريطة المُمرَّرة. لا تنحدر إلى الوحدات المستوردة عمدًا:
                 *      الاستعارةُ ربطُ اسمٍ **خاصٌّ بالملفّ الذي كتبه**. ولذلك تُستدعى
                 *      مرّةً لملفّ الجذر ومرّةً لكلّ وحدةٍ تُبنى أجسامُها، والخريطةُ
                 *      النشطةُ تُبدَّل بينهما بـImportAliasScope؛ فلو شُورِكت خريطةٌ
                 *      واحدةٌ لتسرّبت استعارةُ الوحدة إلى مستعمِلها فاختطفت أسماءً لم
                 *      يستوردها (مدمَجاتٍ مثلًا) بلا أيّ تشخيص.
                 * (EN) Scans the given file's top-level statements only and records
                 *      alias ⇒ original into the supplied map. It deliberately does not
                 *      descend into imported modules: an alias is a binding private to the
                 *      file that wrote it. It is therefore called once for the root file and
                 *      once per module whose bodies are built, with the active map swapped by
                 *      ImportAliasScope; sharing a single map would leak a module's alias
                 *      into its consumer and hijack names it never imported (builtins
                 *      among them) with no diagnostic at all.
                 */
                void collectFileImportBindings(
                    const Sad::AST::StmtList &fileStatements,
                    std::unordered_map<std::string, std::string> &aliases,
                    std::unordered_map<std::string, std::string> &moduleNamespaces);

                // ════════════════════════════════════════════════════════════════
                // (AR) الإغلاقُ التعدّيُّ للتصريحات الخاصّة في وحدةٍ مستورَدة: أسماءُ
                //      الدوالّ غيرِ المُصدَّرة التي تصلها الرموزُ المطلوبةُ مباشرةً أو
                //      عبر وسائط. الوحدةُ وحدةُ ترجمة، فالمُصدَّرُ المطلوبُ يحتاج
                //      مساعِديه الخاصّين؛ لكنّ بناءَ **كلِّ** خاصٍّ يُظلّل المدمَجات
                //      (وحدةٌ فيها «طول» خاصّةٌ تختطف مدمَجَ «طول» في الملفّ المستورِد).
                //      فيُبنى منها ما يُحتاج فقط.
                // (EN) Transitive closure of a module's private declarations: the names of
                //      non-exported functions reachable from the requested symbols. A module
                //      is a compilation unit, so a requested export needs its private
                //      helpers; but building EVERY private one shadows builtins (a module
                //      with a private «طول» hijacks the builtin in the importing file).
                //      Only what is actually reachable is built.
                // ════════════════════════════════════════════════════════════════
                std::set<std::string> computeImportedPrivateClosure(
                    const std::vector<std::unique_ptr<Sad::AST::Statement>> &moduleAst,
                    const std::set<std::string> &requestedSymbols,
                    bool isWildcard);

                void collectFreeVarsExpr(Sad::AST::Expression *expr, const std::set<std::string> &boundNames, std::set<std::string> &freeVars);

                void collectFreeVarsStmt(Sad::AST::Statement *stmt, std::set<std::string> &boundNames, std::set<std::string> &freeVars);

                // ════════════════════════════════════════════════════════════════
                // (AR) 🔑 نوعُ ناتجِ `/` — معينٌ **واحد**. كانت الحقيقةُ منسوخةً ثلاثًا:
                //      الباني (`expression_binary_op.cpp`)، و`inferExprType`،
                //      و`inferReturnTypeFromBody`. غُيِّرت الأولى إلى القسمةِ الصحيحةِ
                //      فبقيت الأخريان تقولان `Any`، فأعلنَ استنتاجُ العائدِ أنّ الطريقةَ
                //      تُرجِع `%SadDyn` بينما جسمُها يُرجِع `i64` خامًّا ⇒ يفكُّ النداءُ
                //      صندوقًا لا وجودَ له ⇒ **انهيار** (مقيسٌ: صنفٌ فيه `ارجع 12 / 2`
                //      ثمّ `"م:" + م.مساحة()` ⇒ rc=139؛ ويعمل بـ`ارجع 6` وبـ`/ 2.0`).
                //      ولأنّ نسختَين تُصلَحان وتبقى ثالثة، جُمِعن في معينٍ واحد.
                //      الدلالة: عشريٌّ في طرفٍ ⇒ عشريّ · «أي»/مجهولٌ ⇒ ديناميّ (نوعُه
                //      حقيقةُ تشغيل) · وما عداهما ⇒ صحيحٌ باقتطاعٍ نحو الصفر كما في C.
                // (EN) 🔑 Result kind of `/` — a SINGLE source. The fact used to be copied
                //      three times: the builder, inferExprType, and inferReturnTypeFromBody.
                //      The first was changed to integer division while the other two kept
                //      answering Any, so return inference declared a method as returning
                //      %SadDyn while its body returned a raw i64 — the call site unboxed a
                //      non-box and crashed. Two of three could be fixed and the third left
                //      behind, so they are now one function.
                // ════════════════════════════════════════════════════════════════
                static SadTypeKind divisionResultKind(SadTypeKind left, SadTypeKind right)
                {
                    if (left == SadTypeKind::Float || right == SadTypeKind::Float)
                        return SadTypeKind::Float;
                    if (left == SadTypeKind::Any || right == SadTypeKind::Any ||
                        left == SadTypeKind::Unknown || right == SadTypeKind::Unknown)
                        return SadTypeKind::Any;
                    return SadTypeKind::Integer;
                }

                // (AR) 🔑 نوعُ ناتجِ القراءةِ بالقوسِ `و[ف]` — معينٌ **واحد** على نمطِ
                //      `divisionResultKind` أعلاه. كانت الحقيقةُ منسوخةً مرّتين:
                //      ذراعُ IndexExpr في `inferExprType` وذراعُها التوأمُ داخلَ
                //      `inferReturnTypeFromBody` — والنسختان بدأتا تفترقان (الثانيةُ
                //      وحدَها تقرأ نوعَ العنصرِ المسجَّل). الدلالةُ تتبعُ **التمثيلَ
                //      المبعوث**: نوعُ العنصرِ متى عُلِم يقينًا، وفهرسةُ نصٍّ نصٌّ
                //      (محرفٌ)، وإلّا «أي» — العنصرُ يُقرأ موسومًا زمنَ التشغيل
                //      (mget_dyn / القراءةُ الموسومةُ للمصفوفات).
                // (EN) 🔑 Result kind of a bracket read `c[i]` — a SINGLE source, the
                //      divisionResultKind pattern. The fact lived in two copies (the
                //      IndexExpr arm of inferExprType and its twin inside
                //      inferReturnTypeFromBody) that had already started to drift.
                //      The answer follows the EMITTED representation: the element
                //      type when certainly known, String for string indexing (one
                //      character), otherwise Any — the element is read tagged at
                //      runtime.
                static SadTypeKind bracketReadResultKind(SadTypeKind objectType,
                                                         SadTypeKind knownElementType)
                {
                    if (knownElementType != SadTypeKind::Void &&
                        knownElementType != SadTypeKind::Unknown)
                        return knownElementType;
                    if (objectType == SadTypeKind::String)
                        return SadTypeKind::String;
                    return SadTypeKind::Any;
                }

                SadTypeKind inferExprType(const Sad::AST::Expression *expr);

                // (AR) تحويل نوع إرجاع مدمجة من مصدر الحقيقة (BuiltinMeta::returnType)
                //      إلى SadTypeKind — مركزيّ يستهلكه مساراً الاستنتاج (الجسم + التعبير)
                //      كي يبقيا مفتاحيْن على الحقل نفسه (returnType) لا على حقلين مختلفين.
                // (EN) Central SoT-returnType → SadTypeKind mapping shared by both
                //      inference paths (return-body & expr) so they key on one field.
                static SadTypeKind builtinReturnsToSIRKind(std::string_view soTReturns);

                std::string instantiateTemplate(const std::string &templateName, const std::vector<SadTypeKind> &typeArguments);

                std::string instantiateTemplate(const std::string &templateName, const std::vector<SadTypeKind> &typeArguments, const std::vector<SIROperand> &constArguments);

            private:
                // ══════════════════════════════════════════════════════════════
                // (AR) خريطتا أعضاءِ الأصناف لاستنتاجِ أنواعِ المعاملاتِ من مواقعِ
                //      النداء. الاستنتاجُ كان يقفُ عندَ الدوالِّ الحرّة، فمعامِلُ
                //      العضوِ غيرُ المُصرَّحِ يبقى Integer الافتراضيَّ ⇒ يُقرأُ النصُّ
                //      عنوانًا ويُطبَعُ رقمٌ مكانَه. وكان الدمجُ السطريُّ يستُرُ ذلك
                //      بتمريرِ الوسيطِ بنوعِه في موقعِ النداء؛ فلمّا مُنِعَ دمجُ
                //      الأعضاءِ (خاناتُ حقولِها نسبةً إلى إطارِ الكائن) ظهرَ العيبُ.
                //      والتصحيحُ يُكتَبُ في الشجرةِ مباشرةً لأنّ بانِيَ الصنفِ يقرأُ
                //      أنواعَ معاملاتِه منها لا من `functionTable_`.
                // (EN) Class-member maps for call-site parameter type inference. Inference
                //      stopped at free functions, so an undeclared member parameter kept the
                //      default Integer ⇒ a string was read as an address and a number printed
                //      in its place. Inlining used to mask this by passing the argument with
                //      its own type at the call site; forbidding member inlining (a member's
                //      field slots are relative to the object frame) surfaced it. The
                //      correction is written into the AST because the class builder reads its
                //      parameter types from there, not from `functionTable_`.
                // ══════════════════════════════════════════════════════════════
                std::unordered_map<std::string, Sad::AST::ClassDecl *> scanClassByName_;
                std::unordered_map<std::string, Sad::AST::ClassDecl *> scanClassOfVariable_;

                Sad::AST::ClassDecl *scanEnclosingClass_ = nullptr;

                static Sad::AST::ClassDecl *asClassDecl(Sad::AST::Statement *stmt);
                Sad::AST::MethodDecl *findMethodInHierarchy(Sad::AST::ClassDecl *owner,
                                                            const std::string &methodName);
                Sad::AST::ClassDecl *resolveExprClass(const Sad::AST::Expression *expr);
                void bindFunctionParamsToArgumentClasses(
                    const std::string &functionName,
                    const std::vector<std::unique_ptr<Sad::AST::Expr>> &arguments);

                // (AR) أنواعُ الوسائطِ التي بلغت كلَّ خانةِ معامِلِ عضوٍ، مجموعةً عبرَ
                //      مواقعِ النداءِ كلِّها. والترقيةُ تُطبَّقُ **بعدَ** المسحِ لا أثناءَه:
                //      فترقيةٌ من موقعٍ واحدٍ تُفسِدُ الخانةَ إن كان موقعٌ آخرُ يُمرِّرُ
                //      عدمًا أو نوعًا مغايرًا — والمقيسُ انهيارٌ (SIGSEGV) حين رُقّيت
                //      خانةٌ إلى نصٍّ ثمّ بلغَها `لاشيء` من نداءٍ آخر.
                // (EN) The argument kinds that reached each member parameter slot, gathered
                //      across every call site. Promotion is applied **after** the scan, not
                //      during it: promoting from a single site corrupts the slot when another
                //      site passes null or a different kind — measured as a SIGSEGV once a slot
                //      was promoted to string and then reached by `null` from another call.
                std::map<Sad::AST::Parameter *, std::set<int>> scanMemberArgKinds_;

                // (AR) نظيرُها للدوالِّ الحرّة: اسمُ الدالّة ⇒ رقمُ الخانة ⇒ أنواعُ الوسائط
                //      المسجَّلةُ من مواقعِ النداء. الأعضاءُ كان لهم إجماعٌ والحرّةُ لا،
                //      فسياستُها «الموقعُ الأوّل يفوز» — وهي تُثبِّت خانةً نصًّا ثمّ يصلها
                //      وسيطٌ معلَّبٌ فتُقارَن مؤشِّرَ نصٍّ: انهيارٌ، أو جوابٌ خاطئٌ صامتٌ حين
                //      لا تنهار. مرتَّبةٌ (map/set) لا مبعثرة: ترتيبُ التطبيقِ يجب أن يكون
                //      حتميًّا وإلّا تغيّر مخرَجُ الترجمةِ بلا تغيُّرِ مصدر.
                // (EN) Free-function counterpart: function name ⇒ slot index ⇒ the argument
                //      kinds recorded at call sites. Members had unanimity, free functions did
                //      not — their policy was "first call site wins", which pins a slot to e.g.
                //      String and then a boxed argument is compared as a string pointer: a
                //      crash, or a silent wrong answer when it does not crash. Ordered
                //      containers on purpose: application order must be deterministic.
                std::map<std::string, std::map<size_t, std::set<int>>> scanFreeArgKinds_;

                // (AR) ونظيرُها لأسماءِ الأصناف: خانةٌ يصلها كائنٌ في موقعٍ وعددٌ في آخر
                //      لا يجوز أن تحتفظ بربطِها بالصنف، وإلّا وُجِّهت مقارنتُها إلى
                //      `__op_eq__` فمُرِّر العددُ مكانَ المستقبِل. والنصُّ الفارغُ يعني
                //      «وسيطٌ ليس كائنًا» وهو مخالِفٌ يمنع الإجماع، لا غيابٌ يُغفَل.
                // (EN) Class-name counterpart: a slot fed an object at one site and a number
                //      at another must not keep its class binding, or its comparison is routed
                //      to `__op_eq__` and the number is passed where the receiver belongs. An
                //      empty string means "argument is not an object" — a disagreeing entry
                //      that blocks unanimity, not an absence to be skipped.
                std::map<std::string, std::map<size_t, std::set<std::string>>> scanFreeArgClasses_;

                /// (AR) يُسجّل أنواعَ وسائطِ موقعِ نداءٍ لخاناتِ معامِلاتِ عضو
                /// (EN) Records one call site's argument kinds against a member's parameter slots
                void recordMemberParamArgs(std::vector<Sad::AST::Parameter> &params,
                                           const std::vector<std::unique_ptr<Sad::AST::Expr>> &arguments,
                                           bool paramsBelongToConstructor);

                /// (AR) يُطبّق الترقياتِ التي اتّفقت عليها مواقعُ النداءِ كلُّها
                /// (EN) Applies the promotions every call site agreed on
                void applyAgreedMemberParamTypes();
                void applyAgreedFreeParamTypes();
                // (AR) [موجة الجسر الموسوم] توسيعُ الخاناتِ الرقميّةِ/غيرِ المصرَّحةِ
                //      للدوالِّ الهاربةِ مرجعًا (scanEscapedFuncs_) إلى Any — مواقعُها
                //      عبرَ المرجعِ غيرُ مرئيّةٍ للمسح.
                // (EN) [Tagged-bridge wave] Widen the numeric/undeclared slots of
                //      functions escaping as references (scanEscapedFuncs_) to Any —
                //      their call sites through the reference are invisible to the scan.
                void applyEscapedFuncParamWidening();

                /// (AR) يُرقّي معاملاتِ عضوٍ بلغَه نداءٌ — بانيًا كان أو طريقة
                /// (EN) Refines a called member's parameters — constructor or method
                void refineCalledMember(const Sad::AST::Expression *objectExpr,
                                        const std::string &methodName,
                                        const std::vector<std::unique_ptr<Sad::AST::Expr>> &arguments);
                // (AR) جسمُ الاستيرادِ الانتقائيِّ الواحد: يستهلكه `من م استورد …`
                //      و`صدّر … من م` معًا. استُخرج بالحقولِ الثلاثةِ لا بالعقدة كي
                //      لا يُصطنَع `FromImportStmt` مؤقّتٌ لإعادةِ التصدير — فعمرُه
                //      يصير قيدًا خفيًّا على ما يجوز للجسم أن يحتفظ به.
                // (EN) The single selective-import body, shared by `من م استورد …` and
                //      `صدّر … من م`. Extracted over the three fields rather than the node
                //      so no temporary FromImportStmt is fabricated for a re-export — its
                //      lifetime would become a hidden constraint on what the body may retain.
                void buildSelectiveImportFrom(const std::vector<std::string> &modulePath,
                                              const std::vector<Sad::AST::ImportItem> &items,
                                              bool isWildcard);

                // (AR) ما تُتيحه الوحدةُ للاستيراد، متعدّيًا عبر «صدّر * من …». يلزم
                //      حلُّ وحدةِ المصدرِ فلذلك هو طريقةٌ لا دالّةٌ حرّة. `visited`
                //      يقطع الدورَ ويمنع العملَ المكرّر.
                // (EN) What a module makes importable, transitively through `صدّر * من …`.
                //      Resolving the source module is required, hence a method rather than a
                //      free function. `visited` cuts cycles and avoids repeated work.
                void collectModuleExportedNames(const Sad::AST::StmtList &moduleStatements,
                                                const std::string &moduleFilePath,
                                                std::set<std::string> &exportedNames,
                                                std::set<std::string> &visitedModulePaths);

                SIRBuilder &b_;
            };

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
