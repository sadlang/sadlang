#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

/**
 * @file main_function_rule.cpp
 * @brief تنفيذ قاعدة الدالة الرئيسية المشتركة (SEM018)
 *        Implementation of the shared main-function rule (SEM018)
 *
 * (AR) منقول حرفيًّا من منطق interpreter_core (كان محصورًا في المفسّر) إلى مكوّن
 *      مشترك يستهلكه المحرّكان — إنهاءً لتباعد المحرّكات من مصدره.
 * (EN) Lifted verbatim from interpreter_core (previously interpreter-only) into a
 *      shared component consumed by both engines — ending the engine divergence.
 */

#include "program_rules/main_function_rule.h"

// (AR) عقد الاشتقاق المطلوبة للتمييز البنيويّ (dynamic_cast). module_nodes.h صريحٌ
//      لأنّ الدالة تميّز Import/FromImport/ExportDecl/ExportStmt (لا تعتمد جرًّا عابرًا).
// (EN) Derived node headers needed for the structural discrimination. module_nodes.h is
//      explicit since the function discriminates Import/Export nodes (no transitive reliance).
#include "declarations.h"
#include "statements.h"
#include "module_nodes.h"

namespace Sad
{
    namespace Semantic
    {
        MainRuleResult checkMainFunctionRule(
            const std::vector<std::unique_ptr<AST::Statement>> &program)
        {
            MainRuleResult result;

            // (AR) المرحلة 1: هل عُرِّفت دالة رئيسية؟ (علم isMainFunction يضبطه المحلّل)
            // (EN) Phase 1: does the program define a main function?
            for (const auto &stmt : program)
            {
                if (auto *funcDecl = dynamic_cast<AST::FunctionDecl *>(stmt.get()))
                {
                    if (funcDecl->isMainFunction)
                    {
                        result.hasMain = true;
                        break;
                    }
                }
            }

            // (AR) بلا دالة رئيسية: الكود العلويّ برنامجٌ يُنفَّذ تلقائيًّا — لا قاعدة.
            // (EN) No main: top-level code is the program (auto-run) — rule N/A.
            if (!result.hasMain)
                return result;

            // (AR) المرحلة 2: عند وجود رئيسية، امنع أيّ جملة تنفيذية علويّة. يُسمح فقط
            //      بالتصريحات، والمتغيّرات العامّة، وجُمل الوحدات (استيراد/تصدير).
            // (EN) Phase 2: with main present, forbid any top-level executable stmt.
            for (const auto &stmt : program)
            {
                const bool isDeclaration =
                    dynamic_cast<AST::FunctionDecl *>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::ClassDecl *>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::EnumDecl *>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::TemplateFunctionDecl *>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::TemplateClassDecl *>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::NamespaceDecl *>(stmt.get()) != nullptr;

                const bool isGlobalVar =
                    dynamic_cast<AST::VarDeclStmt *>(stmt.get()) != nullptr;

                const bool isModuleStmt =
                    dynamic_cast<AST::ImportStmt *>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::FromImportStmt *>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::ExportDecl *>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::ExportStmt *>(stmt.get()) != nullptr;

                if (!isDeclaration && !isGlobalVar && !isModuleStmt)
                {
                    // (AR) جملة تنفيذية علويّة عند وجود رئيسية — انتهاك SEM018.
                    // (EN) Top-level executable statement with main present — SEM018.
                    result.ok = false;
                    if (stmt)
                    {
                        result.line = stmt->position.line;
                        result.column = stmt->position.column;
                    }
                    return result;
                }
            }

            return result;
        }

    } // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
