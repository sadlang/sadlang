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
                SIRBuilder &b_;
            };

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
