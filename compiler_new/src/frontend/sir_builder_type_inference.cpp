// ============================================================================
// sir_builder_type_inference.cpp
// ============================================================================
#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <limits>
#include <functional>
#include <set>
namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // ????????????????????????????????????????????????????????????
            // (AR) ????? ????? ????? � for lambda param type inference
            //      ??????? ??? ????? namespace ????? ????? MSVC ?? static locals ?? lambdas
            // (EN) Method name tables � for lambda param type inference
            //      Defined at namespace level to avoid MSVC issues with static locals in lambdas
            // ????????????????????????????????????????????????????????????
            static const std::set<std::string> kStringMethodNames = {
                "\xD8\xB7\xD9\x88\xD9\x84",                                                  // ???
                "\xD9\x8A\xD8\xAD\xD8\xAA\xD9\x88\xD9\x8A",                                  // ?????
                "\xD9\x8A\xD8\xA8\xD8\xAF\xD8\xA3_\xD8\xA8\xD9\x80",                         // ????_??
                "\xD9\x8A\xD9\x86\xD8\xAA\xD9\x87\xD9\x8A_\xD8\xA8\xD9\x80",                 // ?????_??
                "\xD8\xAA\xD9\x82\xD8\xB3\xD9\x8A\xD9\x85",                                  // ?????
                "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xA8\xD8\xAF\xD9\x84",                          // ??????
                "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xB1\xD8\xA7\xD8\xAC",                  // ???????
                "\xD9\x82\xD8\xB5_\xD8\xA3\xD8\xB7\xD8\xB1\xD8\xA7\xD9\x81",                 // ??_?????
                "\xD8\xAA\xD8\xAD\xD9\x88\xD9\x8A\xD9\x84_\xD9\x83\xD8\xA8\xD9\x8A\xD8\xB1", // ?????_????
                "\xD8\xAA\xD8\xAD\xD9\x88\xD9\x8A\xD9\x84_\xD8\xB5\xD8\xBA\xD9\x8A\xD8\xB1", // ?????_????
            };
            static const std::set<std::string> kArrayMethodNames = {
                "\xD8\xA7\xD8\xB6\xD9\x81",                                          // ???
                "\xD8\xA7\xD8\xAD\xD8\xB0\xD9\x81_\xD8\xA7\xD8\xAE\xD9\x8A\xD8\xB1", // ????_????
                "\xD8\xB1\xD8\xAA\xD8\xA8",                                          // ???
                "\xD8\xB9\xD9\x83\xD8\xB3",                                          // ???
                "\xD8\xAE\xD8\xB1\xD9\x8A\xD8\xB7\xD8\xA9",                          // ?????
                "\xD8\xB1\xD8\xB4\xD8\xAD",                                          // ???
                "\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xB2\xD9\x84",                          // ?????
                "\xD9\x84\xD9\x83\xD9\x84",                                          // ???
                "\xD8\xA7\xD9\x88\xD9\x84",                                          // ???
                "\xD8\xA2\xD8\xAE\xD8\xB1",                                          // ???
                "\xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA\xD8\xA9",                          // ?????
                "\xD8\xA7\xD9\x84\xD8\xB7\xD9\x88\xD9\x84",                          // ?????
                "\xD8\xB4\xD8\xB1\xD9\x8A\xD8\xAD\xD8\xA9",                          // ?????
            };

            // (AR) ??? ????????? ????? ?? ????? (??????? ???????? ?????????)
            // (EN) Collect free variables in an expression (for closure capture detection)
            // ============================================================================
            void SIRBuilder::collectFreeVarsExpr(Sad::AST::Expression *expr,
                                                 const std::set<std::string> &boundNames,
                                                 std::set<std::string> &freeVars)
            {
                if (!expr)
                    return;

                // (AR) ????? - ??? ?? ??? ????????? ??? ???
                // (EN) Variable - if not bound, it's free
                if (auto *var = dynamic_cast<Sad::AST::VariableExpr *>(expr))
                {
                    if (boundNames.find(var->name) == boundNames.end())
                    {
                        // (AR) ???? ?? ????? ?? ?????? ???????
                        // (EN) Check if it exists in the outer scope
                        auto *varOpt = lookupVariable(var->name);
                        if (varOpt)
                        {
                            freeVars.insert(var->name);
                        }
                    }
                    return;
                }

                // (AR) ????? ??????
                // (EN) Binary operation
                if (auto *bin = dynamic_cast<Sad::AST::BinaryExpr *>(expr))
                {
                    collectFreeVarsExpr(bin->left.get(), boundNames, freeVars);
                    collectFreeVarsExpr(bin->right.get(), boundNames, freeVars);
                    return;
                }

                // (AR) ????? ??????
                // (EN) Unary operation
                if (auto *un = dynamic_cast<Sad::AST::UnaryExpr *>(expr))
                {
                    collectFreeVarsExpr(un->operand.get(), boundNames, freeVars);
                    return;
                }

                // (AR) ??????? ????
                // (EN) Function call
                if (auto *call = dynamic_cast<Sad::AST::CallExpr *>(expr))
                {
                    collectFreeVarsExpr(call->callee.get(), boundNames, freeVars);
                    for (auto &arg : call->arguments)
                    {
                        collectFreeVarsExpr(arg.get(), boundNames, freeVars);
                    }
                    return;
                }

                // (AR) ???? ???
                // (EN) Member access
                if (auto *mem = dynamic_cast<Sad::AST::MemberAccessExpr *>(expr))
                {
                    collectFreeVarsExpr(mem->object.get(), boundNames, freeVars);
                    return;
                }

                // (AR) ???? ????
                // (EN) Index access
                if (auto *idx = dynamic_cast<Sad::AST::IndexExpr *>(expr))
                {
                    collectFreeVarsExpr(idx->object.get(), boundNames, freeVars);
                    collectFreeVarsExpr(idx->index.get(), boundNames, freeVars);
                    return;
                }

                // (AR) ????? ???? ?????
                // (EN) Ternary conditional
                if (auto *tern = dynamic_cast<Sad::AST::TernaryExpr *>(expr))
                {
                    collectFreeVarsExpr(tern->condition.get(), boundNames, freeVars);
                    collectFreeVarsExpr(tern->trueExpr.get(), boundNames, freeVars);
                    collectFreeVarsExpr(tern->falseExpr.get(), boundNames, freeVars);
                    return;
                }

                // (AR) ??????
                // (EN) Array literal
                if (auto *arr = dynamic_cast<Sad::AST::ArrayExpr *>(expr))
                {
                    for (auto &el : arr->elements)
                    {
                        collectFreeVarsExpr(el.get(), boundNames, freeVars);
                    }
                    return;
                }

                // ================================================================
                // (AR) [إصلاح الإغلاقات المتداخلة] لامدا داخل لامدا:
                //      نحلل المتغيرات الحرة في اللامدا المتداخلة
                //      ثم نُضيف منها ما ليس مربوطاً في نطاقنا الحالي
                //      هذا يضمن "النقل العابر" (transitive capture):
                //      إذا لامدا داخلية تستخدم متغيراً من جد-الدالة،
                //      فاللامدا الوسطية يجب أن تلتقطه أيضاً وتمرره.
                //      مثال: دالة(أ) → لامدا(ب) → لامدا(ج) → أ*ب+ج
                //      اللامدا الوسطية تحتاج التقاط `أ` لتمريره للداخلية.
                // (EN) [Fix nested closures] Lambda inside lambda:
                //      Analyze the nested lambda's free variables
                //      then add those not bound in our current scope
                //      This ensures "transitive capture":
                //      If an inner lambda uses a var from grandparent function,
                //      the middle lambda must also capture and forward it.
                // ================================================================
                if (auto *nestedLambda = dynamic_cast<Sad::AST::LambdaExpr *>(expr))
                {
                    // (AR) جمع معاملات اللامدا المتداخلة كأسماء مربوطة
                    // (EN) Collect nested lambda's params as bound names
                    std::set<std::string> nestedBound;
                    for (const auto &param : nestedLambda->parameters)
                    {
                        nestedBound.insert(param.name);
                    }

                    // (AR) جمع المتغيرات الحرة في جسم اللامدا المتداخلة
                    // (EN) Collect free vars from nested lambda's body
                    std::set<std::string> nestedFreeVars;
                    if (nestedLambda->body)
                    {
                        collectFreeVarsExpr(nestedLambda->body.get(), nestedBound, nestedFreeVars);
                    }
                    if (nestedLambda->blockBody)
                    {
                        std::set<std::string> nestedBoundCopy = nestedBound;
                        collectFreeVarsStmt(nestedLambda->blockBody.get(), nestedBoundCopy, nestedFreeVars);
                    }

                    // (AR) المتغيرات الحرة في اللامدا المتداخلة التي ليست مربوطة عندنا
                    //      تصبح متغيرات حرة لنا أيضاً (نقل عابر)
                    // (EN) Nested lambda's free vars that are not bound in OUR scope
                    //      become free vars for us too (transitive capture)
                    for (const auto &nfv : nestedFreeVars)
                    {
                        if (boundNames.find(nfv) == boundNames.end())
                        {
                            auto *varOpt = lookupVariable(nfv);
                            if (varOpt)
                            {
                                freeVars.insert(nfv);
                            }
                        }
                    }
                    return;
                }

                // (AR) ????? ??????? (?????? ????? ??????) - ???? ??????? ???
                // (EN) Literals (numbers, strings, booleans) - not free variables
                // No action needed for NumberLiteral, StringLiteral, BoolLiteral, NullLiteral

                // ================================================================
                // (AR) [إصلاح شامل] MethodCallExpr — استدعاء طريقة على كائن
                //      مثال: ق.أرسل("مرحبا") — الكائن `ق` قد يكون متغيراً حراً
                //      يجب زيارة الكائن والوسائط لاكتشاف المتغيرات الحرة
                // (EN) [Comprehensive fix] MethodCallExpr — method call on object
                //      Example: ch.send("hello") — object `ch` may be a free variable
                //      Must visit object and arguments to discover free variables
                // ================================================================
                if (auto *methodCall = dynamic_cast<Sad::AST::MethodCallExpr *>(expr))
                {
                    collectFreeVarsExpr(methodCall->object.get(), boundNames, freeVars);
                    for (auto &arg : methodCall->arguments)
                    {
                        collectFreeVarsExpr(arg.get(), boundNames, freeVars);
                    }
                    return;
                }

                // ================================================================
                // (AR) AssignExpr — تعبير إسناد (متغير = قيمة)
                //      مهم: لا نزور تعبيرات الإسناد لاكتشاف المتغيرات الحرة.
                //      السبب: saveContext() لا تحفظ/تستعيد scope stack، لذا الدوال المستقلة
                //      تصل للمتغيرات الخارجية عبر scope stack المشترك بشكل صحيح.
                //      زيارة الإسناد تكشف المتغير عبر VariableExpr في الجانب الأيمن
                //      (مثل: عداد + 1) → يحوّل الدالة لإغلاق → التقاط بالقيمة → كسر الدلالات.
                //      MethodCallExpr والتعبيرات الأخرى تُكتشف من مواقعها الأصلية (ExprStmt).
                // (EN) AssignExpr — assignment expression (variable = value)
                //      Important: We do NOT visit assignment expressions for free var detection.
                //      Reason: saveContext() doesn't save/restore scope stack, so standalone
                //      functions access outer variables correctly through the shared scope stack.
                //      Visiting the RHS detects vars via VariableExpr (e.g. counter + 1)
                //      → converts function to closure → capture by value → breaks semantics.
                //      MethodCallExpr and other expressions are detected from their own sites.
                // ================================================================
                // AssignExpr — intentionally not handled (see above)
            }

            // ============================================================================
            // (AR) ??? ????????? ????? ?? ???? (??????)
            // (EN) Collect free variables in a statement (recursive)
            // ============================================================================
            void SIRBuilder::collectFreeVarsStmt(Sad::AST::Statement *stmt,
                                                 std::set<std::string> &boundNames,
                                                 std::set<std::string> &freeVars)
            {
                if (!stmt)
                    return;

                // (AR) ???? ???
                // (EN) Block statement
                if (auto *block = dynamic_cast<Sad::AST::BlockStmt *>(stmt))
                {
                    for (auto &s : block->statements)
                    {
                        collectFreeVarsStmt(s.get(), boundNames, freeVars);
                    }
                    return;
                }

                // (AR) ????? ?????
                // (EN) Expression statement
                if (auto *exprStmt = dynamic_cast<Sad::AST::ExprStmt *>(stmt))
                {
                    if (exprStmt->expression)
                    {
                        collectFreeVarsExpr(exprStmt->expression.get(), boundNames, freeVars);
                    }
                    return;
                }

                // (AR) ????? ?????
                // (EN) Variable declaration
                if (auto *varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(stmt))
                {
                    if (varDecl->initializer)
                    {
                        collectFreeVarsExpr(varDecl->initializer.get(), boundNames, freeVars);
                    }
                    boundNames.insert(varDecl->name);
                    return;
                }

                // (AR) ?????
                // (EN) Return
                if (auto *ret = dynamic_cast<Sad::AST::ReturnStmt *>(stmt))
                {
                    if (ret->value)
                    {
                        collectFreeVarsExpr(ret->value.get(), boundNames, freeVars);
                    }
                    return;
                }

                // (AR) ???
                // (EN) If statement
                if (auto *ifStmt = dynamic_cast<Sad::AST::IfStmt *>(stmt))
                {
                    collectFreeVarsExpr(ifStmt->condition.get(), boundNames, freeVars);
                    if (ifStmt->thenBranch)
                    {
                        collectFreeVarsStmt(ifStmt->thenBranch.get(), boundNames, freeVars);
                    }
                    if (ifStmt->elseBranch)
                    {
                        collectFreeVarsStmt(ifStmt->elseBranch.get(), boundNames, freeVars);
                    }
                    return;
                }

                // (AR) ???? while
                // (EN) While loop
                if (auto *whileStmt = dynamic_cast<Sad::AST::WhileStmt *>(stmt))
                {
                    collectFreeVarsExpr(whileStmt->condition.get(), boundNames, freeVars);
                    if (whileStmt->body)
                    {
                        collectFreeVarsStmt(whileStmt->body.get(), boundNames, freeVars);
                    }
                    return;
                }

                // (AR) ???? for-each
                // (EN) For-each loop
                if (auto *forStmt = dynamic_cast<Sad::AST::ForRangeStmt *>(stmt))
                {
                    collectFreeVarsExpr(forStmt->iterable.get(), boundNames, freeVars);
                    boundNames.insert(forStmt->variable);
                    if (forStmt->body)
                    {
                        collectFreeVarsStmt(forStmt->body.get(), boundNames, freeVars);
                    }
                    return;
                }

                // ================================================================
                // (AR) [إصلاح شامل] TryStmt — حاول/امسك/أخيراً
                //      يجب زيارة كتلة المحاولة وجميع بنود الالتقاط وكتلة أخيراً
                // (EN) [Comprehensive fix] TryStmt — try/catch/finally
                //      Must visit try block, all catch clauses, and finally block
                // ================================================================
                if (auto *tryStmt = dynamic_cast<Sad::AST::TryStmt *>(stmt))
                {
                    if (tryStmt->tryBlock)
                        collectFreeVarsStmt(tryStmt->tryBlock.get(), boundNames, freeVars);
                    for (auto &catchClause : tryStmt->catchClauses)
                    {
                        // (AR) متغير الاستثناء يصبح مربوطاً داخل catch
                        // (EN) Exception variable becomes bound inside catch
                        std::set<std::string> catchBound = boundNames;
                        if (!catchClause.exceptionVar.empty())
                            catchBound.insert(catchClause.exceptionVar);
                        if (catchClause.body)
                            collectFreeVarsStmt(catchClause.body.get(), catchBound, freeVars);
                    }
                    if (tryStmt->finallyBlock)
                        collectFreeVarsStmt(tryStmt->finallyBlock.get(), boundNames, freeVars);
                    return;
                }

                // ================================================================
                // (AR) [إصلاح شامل] RaiseStmt — ارمي (throw)
                //      التعبير المرمي قد يحتوي متغيرات حرة
                // (EN) [Comprehensive fix] RaiseStmt — throw
                //      The thrown expression may contain free variables
                // ================================================================
                if (auto *raiseStmt = dynamic_cast<Sad::AST::RaiseStmt *>(stmt))
                {
                    if (raiseStmt->exception)
                        collectFreeVarsExpr(raiseStmt->exception.get(), boundNames, freeVars);
                    return;
                }

                // ================================================================
                // (AR) [إصلاح شامل] DeferStmt — أجّل
                //      الجملة المؤجلة قد تحتوي متغيرات حرة
                // (EN) [Comprehensive fix] DeferStmt — defer
                //      The deferred statement may contain free variables
                // ================================================================
                if (auto *deferStmt = dynamic_cast<Sad::AST::DeferStmt *>(stmt))
                {
                    if (deferStmt->body)
                        collectFreeVarsStmt(deferStmt->body.get(), boundNames, freeVars);
                    return;
                }

                // ================================================================
                // (AR) [إصلاح شامل] GoStmt — أطلق
                //      التعبير أو كتلة الكود قد تحتوي متغيرات حرة
                // (EN) [Comprehensive fix] GoStmt — go
                //      The expression or block body may contain free variables
                // ================================================================
                if (auto *goStmt = dynamic_cast<Sad::AST::GoStmt *>(stmt))
                {
                    if (goStmt->expression)
                        collectFreeVarsExpr(goStmt->expression.get(), boundNames, freeVars);
                    if (goStmt->blockBody)
                        collectFreeVarsStmt(goStmt->blockBody.get(), boundNames, freeVars);
                    return;
                }

                // ================================================================
                // (AR) [إصلاح شامل] MatchStmt — طابق
                //      التعبير المُطابق وأجسام الحالات قد تحتوي متغيرات حرة
                // (EN) [Comprehensive fix] MatchStmt — match
                //      The matched expression and case bodies may contain free variables
                // ================================================================
                if (auto *matchStmt = dynamic_cast<Sad::AST::MatchStmt *>(stmt))
                {
                    if (matchStmt->value)
                        collectFreeVarsExpr(matchStmt->value.get(), boundNames, freeVars);
                    for (auto &matchCase : matchStmt->cases)
                    {
                        for (auto &bodyStmt : matchCase.body)
                        {
                            collectFreeVarsStmt(bodyStmt.get(), boundNames, freeVars);
                        }
                    }
                    return;
                }
            }

            // ============================================================================
            // inferExprType - ??????? ??? SIR ?? ????? AST ???? ???? SIR
            // ============================================================================
            // (AR) ??????? ?? ????? ??????? ??????? (??????? 1.7) ??????
            //      ????? ??????? ?? ????? ?????????
            // (EN) Used in type inference phase (Phase 1.7) to determine
            //      argument types at call sites
            // ============================================================================
            SadTypeKind SIRBuilder::inferExprType(const Sad::AST::Expression *expr)
            {
                if (!expr)
                    return SadTypeKind::Integer;

                // (AR) ???? � ?????? ????? ?? ??? ?????
                // (EN) Literal � infer type from token type
                if (auto *lit = dynamic_cast<const Sad::AST::LiteralExpr *>(expr))
                {
                    auto tokenType = lit->token.getType();
                    if (tokenType == Lexer::TokenType::STRING_LITERAL)
                        return SadTypeKind::String;
                    if (tokenType == Lexer::TokenType::NUMBER_INTEGER)
                        return SadTypeKind::Integer;
                    if (tokenType == Lexer::TokenType::NUMBER_DOUBLE)
                        return SadTypeKind::Float;
                    if (tokenType == Lexer::TokenType::LITERAL_TRUE ||
                        tokenType == Lexer::TokenType::LITERAL_FALSE)
                        return SadTypeKind::Boolean;
                    if (tokenType == Lexer::TokenType::LITERAL_NULL)
                        return SadTypeKind::Integer;
                    return SadTypeKind::Integer;
                }

                // (AR) ????? � ???? ?? ???????? ??????? (?? ?????)
                // (EN) Variable � look up in current scopes (if available)
                if (auto *var = dynamic_cast<const Sad::AST::VariableExpr *>(expr))
                {
                    // (AR) ??? ?? ???????? ???????
                    // (EN) Look up in current scopes
                    for (auto scopeIt = scopeStack_.rbegin(); scopeIt != scopeStack_.rend(); ++scopeIt)
                    {
                        auto it = scopeIt->find(var->name);
                        if (it != scopeIt->end())
                        {
                            return it->second.type;
                        }
                    }
                    // (AR) ??? ?? ??????? ?????? ??????? ????? ??? ????? ????????? (Phase 1.7)
                    // (EN) Look up in current scanning function's parameters (Phase 1.7)
                    if (!currentScanFuncName_.empty())
                    {
                        auto funcIt = functionTable_.find(currentScanFuncName_);
                        if (funcIt != functionTable_.end())
                        {
                            for (const auto &param : funcIt->second.parameters)
                            {
                                if (param.name == var->name)
                                {
                                    return param.type;
                                }
                            }
                        }
                    }
                    // (AR) ??? ?? ????????? ?????? (???????)
                    // (EN) Look up in global variables (constants)
                    if (module_)
                    {
                        auto gv = module_->getGlobalVariable(var->name);
                        if (gv)
                        {
                            return gv->type;
                        }
                    }
                    // (AR) ??? DataType ?? AST
                    // (EN) Check AST DataType
                    auto dtype = var->getDataType();
                    if (dtype == Sad::Data::DataType::STRING)
                        return SadTypeKind::String;
                    if (dtype == Sad::Data::DataType::FLOAT)
                        return SadTypeKind::Float;
                    if (dtype == Sad::Data::DataType::BOOLEAN)
                        return SadTypeKind::Boolean;
                    return SadTypeKind::Integer;
                }

                // (AR) ????? ????? � ??? ??? ??? ??????? ????? ??????? ?? (??? ????)
                // (EN) Binary expr � if either side is string, result is string (concatenation)
                if (auto *bin = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
                {
                    SadTypeKind leftType = inferExprType(bin->left.get());
                    SadTypeKind rightType = inferExprType(bin->right.get());
                    // (AR) ?????? `/` ????? ???? ?????? (??? ??????? ?????)
                    // (EN) Division `/` always produces float (per language spec)
                    if (bin->op == Sad::Lexer::TokenType::OP_DIVIDE)
                        return SadTypeKind::Float;
                    if (leftType == SadTypeKind::String || rightType == SadTypeKind::String)
                        return SadTypeKind::String;
                    if (leftType == SadTypeKind::Float || rightType == SadTypeKind::Float)
                        return SadTypeKind::Float;
                    // (AR) ?????? ???????? ????? BOOL
                    // (EN) Comparison ops return BOOL
                    switch (bin->op)
                    {
                    case Sad::Lexer::TokenType::OP_EQUAL:
                    case Sad::Lexer::TokenType::OP_NOT_EQUAL:
                    case Sad::Lexer::TokenType::OP_LESS:
                    case Sad::Lexer::TokenType::OP_GREATER:
                    case Sad::Lexer::TokenType::OP_LESS_EQUAL:
                    case Sad::Lexer::TokenType::OP_GREATER_EQUAL:
                    case Sad::Lexer::TokenType::OP_AND:
                    case Sad::Lexer::TokenType::OP_OR:
                        return SadTypeKind::Boolean;
                    default:
                        break;
                    }
                    return SadTypeKind::Integer;
                }

                // (AR) ?????? ?????
                // (EN) Array literal
                if (dynamic_cast<const Sad::AST::ArrayExpr *>(expr))
                    return SadTypeKind::Array;

                // (AR) ????? ???? ????
                // (EN) New object
                if (dynamic_cast<const Sad::AST::NewExpr *>(expr))
                    return SadTypeKind::Struct;

                // (AR) ??????? ???? � ???? ?? ??? ??????? ?? functionTable_
                // (EN) Function call � look up return type in functionTable_
                if (auto *call = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                {
                    if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(call->callee.get()))
                    {
                        auto it = functionTable_.find(varExpr->name);
                        if (it != functionTable_.end())
                        {
                            return it->second.returnType;
                        }
                    }
                    return SadTypeKind::Integer;
                }

                // (AR) ????? ?????
                // (EN) Ternary expression
                if (auto *tern = dynamic_cast<const Sad::AST::TernaryExpr *>(expr))
                {
                    SadTypeKind trueType = inferExprType(tern->trueExpr.get());
                    SadTypeKind falseType = inferExprType(tern->falseExpr.get());
                    if (trueType == SadTypeKind::String || falseType == SadTypeKind::String)
                        return SadTypeKind::String;
                    if (trueType == SadTypeKind::Float || falseType == SadTypeKind::Float)
                        return SadTypeKind::Float;
                    return trueType;
                }

                // (AR) ????? � ??? ?????? ????????
                // (EN) Assignment � type of assigned value
                if (auto *assign = dynamic_cast<const Sad::AST::AssignExpr *>(expr))
                {
                    return inferExprType(assign->value.get());
                }

                // (AR) ????? ?????
                // (EN) Unary expression
                if (auto *unary = dynamic_cast<const Sad::AST::UnaryExpr *>(expr))
                {
                    if (unary->op == Sad::Lexer::TokenType::OP_NOT)
                        return SadTypeKind::Boolean;
                    return inferExprType(unary->operand.get());
                }

                // (AR) ??? DataType ?? ???????
                // (EN) Check DataType from expression
                auto dtype = expr->getDataType();
                if (dtype == Sad::Data::DataType::STRING)
                    return SadTypeKind::String;
                if (dtype == Sad::Data::DataType::FLOAT)
                    return SadTypeKind::Float;
                if (dtype == Sad::Data::DataType::BOOLEAN)
                    return SadTypeKind::Boolean;
                if (dtype == Sad::Data::DataType::ARRAY)
                    return SadTypeKind::Array;
                if (dtype == Sad::Data::DataType::MAP)
                    return SadTypeKind::Map;

                // (AR) ??????? � I64
                // (EN) Default � I64
                return SadTypeKind::Integer;
            }

            // ============================================================================
            // scanCallSitesInExpr - ??? ?????? ????????? ????? ?? ????????? ??????
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
