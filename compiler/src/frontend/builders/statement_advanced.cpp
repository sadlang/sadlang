// ============================================================================
// sir_builder_stmt_advanced.cpp
// (AR) Advanced: test, atomic, property, macro, goroutine, select, extension
// (EN) Advanced statement types: testing, concurrency, metaprogramming
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            bool StatementBuilder::buildStatement_Advanced(AST::Statement *stmt)
            {
                // ================================================================
                // (AR) مرحلة 1: اختبارات (TestDecl) — مستخرجة إلى sir_builder_stmt_test.cpp
                // (EN) Phase 1: Test declarations extracted to sir_builder_stmt_test.cpp
                // ================================================================
                if (buildStatement_Test(stmt))
                    return true;

                // ================================================================
                // (AR) مرحلة 2: الخصائص (PropertyDecl) — مستخرجة إلى sir_builder_stmt_property.cpp
                // (EN) Phase 2: Property declarations extracted to sir_builder_stmt_property.cpp
                // ================================================================
                if (buildStatement_Property(stmt))
                    return true;

                // ========================================================================
                // (AR) AtomicDecl - متغير ذري (declarations.h:AtomicDecl)
                // (EN) Atomic variable declaration — lowered to regular variable with atomic hint
                // ========================================================================
                if (auto atomicDecl = dynamic_cast<Sad::AST::AtomicDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found AtomicDecl: " << atomicDecl->name << std::endl;
#endif

                    // (AR) تحويل النوع الداخلي إلى SadTypeKind
                    // (EN) Convert inner type to SadTypeKind
                    SadTypeKind varType = b_.astTypeToSIRType(atomicDecl->innerType);

                    // (AR) تخصيص مكان في الذاكرة
                    // (EN) Allocate memory
                    std::string reg = b_.newTempRegister();
                    if (b_.currentBlock_)
                    {
                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::ALLOC;
                        allocInst.result = SIROperand::Register(reg, varType);
                        b_.currentBlock_->addInstruction(allocInst);
                    }

                    // (AR) تهيئة القيمة إن وُجدت
                    // (EN) Initialize value if present
                    if (atomicDecl->initialValue)
                    {
                        auto initResult = b_.buildExpression(atomicDecl->initialValue.get());
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(SIRInstruction::Store(
                                SIROperand::Register(reg, varType),
                                SIROperand::Register(initResult.registerName, initResult.type)));
                        }
                    }

                    // (AR) تسجيل المتغير في النطاق الحالي
                    // (EN) Register variable in current scope
                    VariableInfo varInfo;
                    varInfo.name = atomicDecl->name;
                    varInfo.type = varType;
                    varInfo.registerName = reg;
                    varInfo.isGlobal = false;
                    varInfo.isMutable = true;
                    b_.addVariable(varInfo);
                    return true;
                }

                // ========================================================================
                // (AR) UnsafeBlockStmt - @غير_آمن ... نهاية
                // (EN) Unsafe block statement - @غير_آمن ... نهاية
                // ========================================================================
                if (auto unsafeStmt = dynamic_cast<Sad::AST::UnsafeBlockStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found UnsafeBlockStmt with "
                              << unsafeStmt->body.size() << " statements" << std::endl;
#endif

                    // (AR) تنفيذ جميع الجمل داخل الكتلة غير الآمنة
                    // (EN) Execute all statements inside the unsafe block
                    for (const auto &innerStmt : unsafeStmt->body)
                    {
                        if (innerStmt)
                        {
                            buildStatement(innerStmt.get());
                        }
                    }
                    return true;
                }

                // ========================================================================
                // (AR) ComptimeBlockStmt - @وقت_الترجمة ... نهاية
                // (EN) Compile-time block statement - @وقت_الترجمة ... نهاية
                // ========================================================================
                if (auto comptimeStmt = dynamic_cast<Sad::AST::ComptimeBlockStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ComptimeBlockStmt with "
                              << comptimeStmt->body.size() << " statements" << std::endl;
#endif

                    // (AR) في وقت الترجمة، نقوم فقط بمعالجة الجمل (يمكن توسيعها لاحقاً)
                    // (EN) At compile time, we just process the statements (can be extended later)
                    for (const auto &innerStmt : comptimeStmt->body)
                    {
                        if (innerStmt)
                        {
                            buildStatement(innerStmt.get());
                        }
                    }
                    return true;
                }

                // ========================================================================
                // (AR) VolatileVarDeclStmt - @متطاير متغير
                // (EN) Volatile variable declaration - @متطاير متغير
                // ========================================================================
                if (auto volatileDecl = dynamic_cast<Sad::AST::VolatileVarDeclStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found VolatileVarDeclStmt" << std::endl;
#endif

                    // (AR) استخراج VarDeclStmt من declaration
                    // (EN) Extract VarDeclStmt from declaration
                    if (volatileDecl->declaration)
                    {
                        if (auto varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(volatileDecl->declaration.get()))
                        {
                            // (AR) بناء المتغير العادي مع تعليق volatile
                            // (EN) Build normal variable with volatile comment
                            buildLocalVariable(varDecl);
                        }
                        else
                        {
                            // (AR) إذا لم يكن VarDeclStmt، نعالج كجملة عادية
                            // (EN) If not VarDeclStmt, process as normal statement
                            buildStatement(volatileDecl->declaration.get());
                        }
                    }
                    return true;
                }

                // ========================================================================
                // (AR) MacroDecl - تصريح ماكرو: ماكرو اسم(معاملات) ... نهاية
                //      نُسجّل الماكرو في خريطة b_.macros_ فقط — يُوسَّع عند الاستدعاء
                //      (مشابه لـ StatementExecutor::visitMacroDecl في المفسر)
                // (EN) MacroDecl - macro declaration: macro name(params) ... end
                //      We register the macro in b_.macros_ map only — expanded at call site
                //      (mirrors StatementExecutor::visitMacroDecl in interpreter)
                // ========================================================================
                if (auto macroDecl = dynamic_cast<Sad::AST::MacroDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Registering macro: '" << macroDecl->name
                              << "' with " << macroDecl->params.size() << " params"
                              << (macroDecl->isVariadic ? " (variadic)" : "") << std::endl;
#endif
                    // (AR) تخزين مؤشر الماكرو (AST يملك الذاكرة)
                    // (EN) Store macro pointer (AST owns memory)
                    b_.macros_[macroDecl->name] = macroDecl;
                    return true;
                }

                // ================================================================
                // (AR) مرحلة 3: التزامن — مستخرجة إلى sir_builder_stmt_concurrency.cpp
                // (EN) Phase 3: Concurrency extracted to sir_builder_stmt_concurrency.cpp
                // ================================================================
                if (buildStatement_Go(stmt))
                    return true;
                if (buildStatement_Select(stmt))
                    return true;

                // ================================================================
                // (AR) مرحلة 4: الامتداد — مستخرجة إلى sir_builder_stmt_extension.cpp
                // (EN) Phase 4: Extension extracted to sir_builder_stmt_extension.cpp
                // ================================================================
                if (buildStatement_Extension(stmt))
                    return true;

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad