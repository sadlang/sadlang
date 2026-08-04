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
