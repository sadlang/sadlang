// ============================================================================
// sir_builder_type_inference.cpp
// ============================================================================
#include <string>
#include "sir_builder.h"
#include "builders/template_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include "builtin_registry.h"
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
            // ────────────────────────────────────────────────────────────
            // (AR) جداول أسماء الطرق — لاستنتاج أنواع معاملات اللامدا
            //      معرَّفة على مستوى namespace لتجنب مشاكل MSVC مع static locals في lambdas
            // (EN) Method name tables — for lambda param type inference
            //      Defined at namespace level to avoid MSVC issues with static locals in lambdas
            // ────────────────────────────────────────────────────────────
            static const std::set<std::string> kStringMethodNames = {
                "\xD8\xB7\xD9\x88\xD9\x84",                                                  // طول
                "\xD9\x8A\xD8\xAD\xD8\xAA\xD9\x88\xD9\x8A",                                  // يحتوي
                "\xD9\x8A\xD8\xA8\xD8\xAF\xD8\xA3_\xD8\xA8\xD9\x80",                         // يبدأ_بـ
                "\xD9\x8A\xD9\x86\xD8\xAA\xD9\x87\xD9\x8A_\xD8\xA8\xD9\x80",                 // ينتهي_بـ
                "\xD8\xAA\xD9\x82\xD8\xB3\xD9\x8A\xD9\x85",                                  // تقسيم
                "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xA8\xD8\xAF\xD9\x84",                          // استبدل
                "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xB1\xD8\xA7\xD8\xAC",                  // استخراج
                "\xD9\x82\xD8\xB5_\xD8\xA3\xD8\xB7\xD8\xB1\xD8\xA7\xD9\x81",                 // قص_أطراف
                "\xD8\xAA\xD8\xAD\xD9\x88\xD9\x8A\xD9\x84_\xD9\x83\xD8\xA8\xD9\x8A\xD8\xB1", // تحويل_كبير
                "\xD8\xAA\xD8\xAD\xD9\x88\xD9\x8A\xD9\x84_\xD8\xB5\xD8\xBA\xD9\x8A\xD8\xB1", // تحويل_صغير
            };
            static const std::set<std::string> kArrayMethodNames = {
                "\xD8\xA7\xD8\xB6\xD9\x81",                                          // اضف
                "\xD8\xA7\xD8\xAD\xD8\xB0\xD9\x81_\xD8\xA7\xD8\xAE\xD9\x8A\xD8\xB1", // احذف_اخير
                "\xD8\xB1\xD8\xAA\xD8\xA8",                                          // رتب
                "\xD8\xB9\xD9\x83\xD8\xB3",                                          // عكس
                "\xD8\xAE\xD8\xB1\xD9\x8A\xD8\xB7\xD8\xA9",                          // خريطة
                "\xD8\xB1\xD8\xB4\xD8\xAD",                                          // رشح
                "\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xB2\xD9\x84",                          // اختزل
                "\xD9\x84\xD9\x83\xD9\x84",                                          // لكل
                "\xD8\xA7\xD9\x88\xD9\x84",                                          // اول
                "\xD8\xA2\xD8\xAE\xD8\xB1",                                          // آخر
                "\xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA\xD8\xA9",                          // فارغة
                "\xD8\xA7\xD9\x84\xD8\xB7\xD9\x88\xD9\x84",                          // الطول
                "\xD8\xB4\xD8\xB1\xD9\x8A\xD8\xAD\xD8\xA9",                          // شريحة
            };

            // (AR) جمع المتغيرات الحرة في تعبير (لاكتشاف التقاط الإغلاقات)
            // (EN) Collect free variables in an expression (for closure capture detection)
            // ============================================================================
            void TemplateBuilder::collectFreeVarsExpr(Sad::AST::Expression *expr,
                                                 const std::set<std::string> &boundNames,
                                                 std::set<std::string> &freeVars)
            {
                if (!expr)
                    return;

                // (AR) متغير - إذا لم يكن مربوطاً فهو حر
                // (EN) Variable - if not bound, it's free
                if (auto *var = dynamic_cast<Sad::AST::VariableExpr *>(expr))
                {
                    if (boundNames.find(var->name) == boundNames.end())
                    {
                        // (AR) تحقق من وجوده في النطاق الخارجي
                        // (EN) Check if it exists in the outer scope
                        auto *varOpt = b_.lookupVariable(var->name);
                        if (varOpt)
                        {
                            freeVars.insert(var->name);
                        }
                    }
                    return;
                }

                // (AR) عملية ثنائية
                // (EN) Binary operation
                if (auto *bin = dynamic_cast<Sad::AST::BinaryExpr *>(expr))
                {
                    collectFreeVarsExpr(bin->left.get(), boundNames, freeVars);
                    collectFreeVarsExpr(bin->right.get(), boundNames, freeVars);
                    return;
                }

                // (AR) عملية أحادية
                // (EN) Unary operation
                if (auto *un = dynamic_cast<Sad::AST::UnaryExpr *>(expr))
                {
                    collectFreeVarsExpr(un->operand.get(), boundNames, freeVars);
                    return;
                }

                // (AR) استدعاء دالة
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

                // (AR) وصول عضو
                // (EN) Member access
                if (auto *mem = dynamic_cast<Sad::AST::MemberAccessExpr *>(expr))
                {
                    collectFreeVarsExpr(mem->object.get(), boundNames, freeVars);
                    return;
                }

                // (AR) وصول فهرس
                // (EN) Index access
                if (auto *idx = dynamic_cast<Sad::AST::IndexExpr *>(expr))
                {
                    collectFreeVarsExpr(idx->object.get(), boundNames, freeVars);
                    collectFreeVarsExpr(idx->index.get(), boundNames, freeVars);
                    return;
                }

                // (AR) تعبير شرطي ثلاثي
                // (EN) Ternary conditional
                if (auto *tern = dynamic_cast<Sad::AST::TernaryExpr *>(expr))
                {
                    collectFreeVarsExpr(tern->condition.get(), boundNames, freeVars);
                    collectFreeVarsExpr(tern->trueExpr.get(), boundNames, freeVars);
                    collectFreeVarsExpr(tern->falseExpr.get(), boundNames, freeVars);
                    return;
                }

                // (AR) مصفوفة
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
                            auto *varOpt = b_.lookupVariable(nfv);
                            if (varOpt)
                            {
                                freeVars.insert(nfv);
                            }
                        }
                    }
                    return;
                }

                // (AR) القيم الحرفية (أعداد، نصوص، منطقية) - ليست متغيرات حرة
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
                //      السبب: b_.saveContext() لا تحفظ/تستعيد scope stack، لذا الدوال المستقلة
                //      تصل للمتغيرات الخارجية عبر scope stack المشترك بشكل صحيح.
                //      زيارة الإسناد تكشف المتغير عبر VariableExpr في الجانب الأيمن
                //      (مثل: عداد + 1) → يحوّل الدالة لإغلاق → التقاط بالقيمة → كسر الدلالات.
                //      MethodCallExpr والتعبيرات الأخرى تُكتشف من مواقعها الأصلية (ExprStmt).
                // (EN) AssignExpr — assignment expression (variable = value)
                //      Important: We do NOT visit assignment expressions for free var detection.
                //      Reason: b_.saveContext() doesn't save/restore scope stack, so standalone
                //      functions access outer variables correctly through the shared scope stack.
                //      Visiting the RHS detects vars via VariableExpr (e.g. counter + 1)
                //      → converts function to closure → capture by value → breaks semantics.
                //      MethodCallExpr and other expressions are detected from their own sites.
                // ================================================================
                // (AR) [ISSUE-053] تحديث: نلتقط المتغيّر المُسنَد إليه أيضًا. الإغلاق الذي يُعيد
                //      لامدا تُعدّل متغيّرًا محلّيًّا (مصنع عدّاد) كان يفقد حالته بين النداءات لأنّ ق
                //      لم يُكتشف متغيّرًا حرًّا (env=0، التقاط مفقود). بالتقاطه تُخزَّن قيمته في env
                //      عند CLOSURE_CREATE، وتُكتب تعديلاتُه عبر ENV_STORE إلى env الكومة الدائم في
                //      كائن الإغلاق ⇒ تدوم الحالة. (نلتقط الاسم المُسنَد إليه + نمسح الجانب الأيمن.)
                // (EN) [ISSUE-053] Update: also capture the assigned-to variable. A closure that
                //      returns a lambda mutating an outer local (counter factory) lost its state
                //      across calls because the LHS var was never detected as free (env=0). Capturing
                //      it stores its value in env at CLOSURE_CREATE and writes mutations back via
                //      ENV_STORE into the closure object's persistent heap env ⇒ state survives.
                if (auto *asg = dynamic_cast<Sad::AST::AssignExpr *>(expr))
                {
                    // (AR) مسار defer (الراية مخفوضة): لا نزور تعبير الإسناد إطلاقًا — هذا هو
                    //      السلوك الأصليّ. defer يُعدّل المتغيّر الخارجيّ عبر المكدّس المشترك لا
                    //      بالالتقاط بالقيمة؛ لو زرنا الجانب الأيمن (سجل + "ج") لالتُقط سجل بالقيمة
                    //      وكُسر defer (انهيار/ناتج خاطئ).
                    // (EN) defer path (flag down): do NOT visit the assignment at all — this is the
                    //      original behavior. defer mutates the outer var via the shared scope stack,
                    //      not by value; visiting the RHS (سجل + "ج") would capture سجل by value and
                    //      break defer (crash / wrong output).
                    if (!b_.captureAssignTargetsInClosure_)
                        return;

                    // (AR) مسار اللامدا (الراية مرفوعة): نلتقط هدف الإسناد + نزور الجانب الأيمن،
                    //      كي تدوم حالة الإغلاق المُعيد للامدا تُعدّل متغيّرًا محلّيًّا (مصنع عدّاد).
                    // (EN) lambda path (flag up): capture the assignment target + visit the RHS, so a
                    //      closure returning a lambda mutating a local (counter factory) keeps state.
                    if (boundNames.find(asg->name) == boundNames.end())
                    {
                        auto *varOpt = b_.lookupVariable(asg->name);
                        if (varOpt)
                            freeVars.insert(asg->name);
                    }
                    collectFreeVarsExpr(asg->value.get(), boundNames, freeVars);
                    return;
                }
            }

            // ============================================================================
            // (AR) جمع المتغيرات الحرة في جملة (تعاودي)
            // (EN) Collect free variables in a statement (recursive)
            // ============================================================================
            void TemplateBuilder::collectFreeVarsStmt(Sad::AST::Statement *stmt,
                                                 std::set<std::string> &boundNames,
                                                 std::set<std::string> &freeVars)
            {
                if (!stmt)
                    return;

                // (AR) جملة كتلة
                // (EN) Block statement
                if (auto *block = dynamic_cast<Sad::AST::BlockStmt *>(stmt))
                {
                    for (auto &s : block->statements)
                    {
                        collectFreeVarsStmt(s.get(), boundNames, freeVars);
                    }
                    return;
                }

                // (AR) جملة تعبيرية
                // (EN) Expression statement
                if (auto *exprStmt = dynamic_cast<Sad::AST::ExprStmt *>(stmt))
                {
                    if (exprStmt->expression)
                    {
                        collectFreeVarsExpr(exprStmt->expression.get(), boundNames, freeVars);
                    }
                    return;
                }

                // (AR) تصريح متغير
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

                // (AR) إرجاع
                // (EN) Return
                if (auto *ret = dynamic_cast<Sad::AST::ReturnStmt *>(stmt))
                {
                    if (ret->value)
                    {
                        collectFreeVarsExpr(ret->value.get(), boundNames, freeVars);
                    }
                    return;
                }

                // (AR) شرط
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

                // (AR) حلقة while
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

                // (AR) حلقة for-each
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

                // ================================================================
                // (AR) SwitchStmt — حالة: التعبير المُختبَر وأجسام الفروع والافتراضيّ
                //      قد تحوي متغيّرات حرّة (كان مفقوداً — نظير MatchStmt أعلاه).
                // (EN) SwitchStmt — case: scrutinee + case bodies + default may contain
                //      free variables (was missing — mirror of MatchStmt above).
                // ================================================================
                if (auto *switchStmt = dynamic_cast<Sad::AST::SwitchStmt *>(stmt))
                {
                    if (switchStmt->expression)
                        collectFreeVarsExpr(switchStmt->expression.get(), boundNames, freeVars);
                    for (auto &caseBranch : switchStmt->cases)
                    {
                        if (caseBranch.value)
                            collectFreeVarsExpr(caseBranch.value.get(), boundNames, freeVars);
                        if (caseBranch.body)
                            collectFreeVarsStmt(caseBranch.body.get(), boundNames, freeVars);
                    }
                    if (switchStmt->defaultCase)
                        collectFreeVarsStmt(switchStmt->defaultCase.get(), boundNames, freeVars);
                    return;
                }
            }

            // ============================================================================
            // inferExprType - استنتاج نوع SIR من تعبير AST داخل باني SIR
            // ============================================================================
            // (AR) تُستخدم في مرحلة استدلال الأنواع (المرحلة 1.7) لتحديد
            //      أنواع الوسائط في مواقع الاستدعاء
            // (EN) Used in type inference phase (Phase 1.7) to determine
            //      argument types at call sites
            // ============================================================================
            SadTypeKind TemplateBuilder::inferExprType(const Sad::AST::Expression *expr)
            {
                if (!expr)
                    return SadTypeKind::Integer;

                // (AR) قيمة حرفية — استنتاج النوع من نوع الرمز
                // (EN) Literal — infer type from token type
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

                // (AR) متغير — ابحث في النطاقات الحالية (إن وُجدت)
                // (EN) Variable — look up in current scopes (if available)
                if (auto *var = dynamic_cast<const Sad::AST::VariableExpr *>(expr))
                {
                    // (AR) ابحث في النطاقات الحالية
                    // (EN) Look up in current scopes
                    for (auto scopeIt = b_.scopeStack_.rbegin(); scopeIt != b_.scopeStack_.rend(); ++scopeIt)
                    {
                        auto it = scopeIt->find(var->name);
                        if (it != scopeIt->end())
                        {
                            return it->second.type;
                        }
                    }
                    // (AR) ابحث في معاملات الدالة الجاري مسحها ضمن مرحلة الاستدلال (Phase 1.7)
                    // (EN) Look up in current scanning function's parameters (Phase 1.7)
                    if (!b_.currentScanFuncName_.empty())
                    {
                        auto funcIt = b_.functionTable_.find(b_.currentScanFuncName_);
                        if (funcIt != b_.functionTable_.end())
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
                    // (AR) ابحث في المتغيرات العامة (الثوابت)
                    // (EN) Look up in global variables (constants)
                    if (b_.module_)
                    {
                        auto gv = b_.module_->getGlobalVariable(var->name);
                        if (gv)
                        {
                            return gv->type;
                        }
                    }
                    // (AR) ثوابتُ الوحدات المستوردة العُليا (الطور 1.7): لم تُبعَث عوالمُها
                    //      في module_ بعد (الطور 2 يفعل)، فنقرأ النوعَ من البذرة.
                    // (EN) Imported modules' top-level consts (Phase 1.7): their globals are
                    //      not emitted into module_ yet (Phase 2 does that) — read the seed.
                    {
                        auto pit = b_.pendingGlobalTypes_.find(var->name);
                        if (pit != b_.pendingGlobalTypes_.end())
                            return pit->second;
                    }
                    // (AR) [ز.٢٠] متغيّرٌ محلّيٌّ في الدالّة الجاري مسحُها (الطور 1.7):
                    //      نطاقاتُه لم تُبنَ بعد، فنقرأ نوعَ مُهيّئه المسجَّلَ في المسح.
                    //      يسبق فحصَ DataType لأنّ الأخيرَ يسقط إلى Integer لكلّ متغيّرٍ
                    //      بلا نوعٍ مصرَّح — وهو السقوطُ الذي كان يجمّد المعامِلَ عددًا.
                    // (EN) [ز.٢٠] A local variable of the function being scanned (Phase 1.7):
                    //      its scopes are not built yet, so read the initializer type recorded
                    //      during the scan. Placed before the DataType check, which otherwise
                    //      falls through to Integer for every untyped variable — the very
                    //      fall-through that froze the callee's parameter as an integer.
                    //
                    //      🔴 **مُعطَّلٌ عمدًا — لا تُفعّله بلا سدِّ ز.٢١ أوّلًا.**
                    //      تفعيلُه يصلح الشاهدَ (`هو_رقم(الحرف)` داخل الدوالّ) لكنّه يُسقط
                    //      توليدَ LLVM على «نداءٌ بتوقيعٍ فاسد» في مكتبة جيسون: ترقيةُ
                    //      المعامل إلى نصٍّ تلتصق (منطقُ التضارب في تحديث المعامل)، ومعامِلٌ
                    //      متعدّدُ الأنواع (يتلقّى نصًّا ومصفوفةً وعددًا) لا مسارَ مصالحةٍ
                    //      له إلّا التوسيعُ إلى Any — وهو غيرُ مُنفَّذ. فالسدُّ الصحيحُ ز.٢١
                    //      (توسيعُ المعامل المشترَك إلى Any) ثمّ تفعيلُ هذا.
                    // (EN) 🔴 DELIBERATELY DISABLED — do not enable before closing ز.٢١.
                    //      Enabling it fixes the witness but crashes LLVM codegen with
                    //      "Calling a function with a bad signature!" on the JSON library: the
                    //      String promotion sticks, and a polymorphic parameter (receiving a
                    //      string, an array and an integer) has no reconciliation path other
                    //      than widening to Any, which is not implemented. Close ز.٢١ first.
                    // {
                    //     auto lit = b_.scanLocalVarType_.find(b_.currentScanFuncName_ + "#" + var->name);
                    //     if (lit != b_.scanLocalVarType_.end())
                    //         return lit->second;
                    // }

                    // (AR) افحص DataType من AST
                    // (EN) Check AST DataType
                    auto dtype = var->getTypeKind();
                    if (dtype == Sad::Types::SadTypeKind::String)
                        return SadTypeKind::String;
                    if (dtype == Sad::Types::SadTypeKind::Float)
                        return SadTypeKind::Float;
                    if (dtype == Sad::Types::SadTypeKind::Boolean)
                        return SadTypeKind::Boolean;
                    return SadTypeKind::Integer;
                }

                // (AR) تعبير ثنائي — إذا كان أحد الطرفين نصاً فالنتيجة نص (دمج نصي)
                // (EN) Binary expr — if either side is string, result is string (concatenation)
                if (auto *bin = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
                {
                    SadTypeKind leftType = inferExprType(bin->left.get());
                    SadTypeKind rightType = inferExprType(bin->right.get());
                    // (AR) ISSUE-063: دلالة المفسّر للقسمة `/`: معاملٌ عشريّ ⇒ عشريّ؛
                    //      صحيح/صحيح ⇒ يتقرّر زمنَ التشغيل ⇒ نوعٌ ديناميّ (Any).
                    // (EN) ISSUE-063: interpreter `/` semantics: a float operand ⇒ Float;
                    //      int/int ⇒ runtime-dependent ⇒ dynamic (Any).
                    if (bin->op == Sad::Lexer::TokenType::OP_DIVIDE)
                        return (leftType == SadTypeKind::Float || rightType == SadTypeKind::Float)
                                   ? SadTypeKind::Float
                                   : SadTypeKind::Any;
                    if (leftType == SadTypeKind::String || rightType == SadTypeKind::String)
                        return SadTypeKind::String;
                    if (leftType == SadTypeKind::Float || rightType == SadTypeKind::Float)
                        return SadTypeKind::Float;
                    // (AR) عمليات المقارنة تُرجع BOOL
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

                // (AR) مصفوفة حرفية
                // (EN) Array literal
                if (dynamic_cast<const Sad::AST::ArrayExpr *>(expr))
                    return SadTypeKind::Array;

                // (AR) كائن جديد
                // (EN) New object
                if (dynamic_cast<const Sad::AST::NewExpr *>(expr))
                    return SadTypeKind::Struct;

                // ================================================================
                // (AR) نداء طريقة (معدّل انسيابيّ) على عنصر ⇒ يُرجع العنصر نفسه
                //      (Pointer) للتسلسل. بدون هذا: متغيّر عامّ يُخزَّن كنتيجة
                //      `عرض = نص_عنصر(..).حجم(..)` لا يُتتبَّع كـPointer عبر الدوال.
                //      ⚠ قيدٌ معروف: مثيلُ صنفِ مستخدمٍ كائنُه Pointer وله طريقةٌ ذات
                //        إرجاعٍ غير Pointer يُستنتَج Pointer خطأً هنا؛ نادر ومقبول
                //        (المعدّلات الانسيابيّة على العناصر هي الحالة السائدة).
                // (EN) Method call (fluent modifier) on a widget returns the widget
                //      itself (Pointer) for chaining — keeps global widget vars typed.
                //      ⚠ Known limit: a user-class instance (Pointer) with a non-Pointer
                //        returning method is mis-inferred as Pointer here; rare, accepted.
                // ================================================================
                // (AR) نداءٌ مؤهَّلٌ بفضاءِ وحدة («ح.تحية(…)»): نوعُه نوعُ عائدِ الرمزِ
                //      المسطَّح، إذ المصرّفُ يُسطّح رموزَ الوحدة (ISSUE-090). يسبق فحصَ
                //      المؤشّر أدناه كي لا يُخلَط فضاءُ وحدةٍ بعنصرِ واجهةٍ انسيابيّ.
                // (EN) Module-qualified call (`ح.تحية(…)`): its type is the flattened symbol's
                //      return type, since the compiler flattens module symbols (ISSUE-090).
                //      Placed before the pointer check below so a module namespace is never
                //      confused with a fluent widget.
                if (auto *qualifiedCall = dynamic_cast<const Sad::AST::MethodCallExpr *>(expr))
                {
                    if (auto *moduleVar =
                            dynamic_cast<const Sad::AST::VariableExpr *>(qualifiedCall->object.get()))
                    {
                        // (AR) الحاسمُ الواحدُ نفسُه لا شرطٌ مُعادٌ هنا: كان هذا الموضعُ
                        //      يفتقد حارسَ المتغيّرِ الحقيقيّ فيصف كائنًا يصادف اسمُه
                        //      اسمَ وحدةٍ بنوعِ عائدِ رمزٍ لا يخصّه.
                        // (EN) The same single decider rather than a re-stated condition: this
                        //      site was missing the real-variable guard, typing an object whose
                        //      name merely matched a module with an unrelated return type.
                        // (AR) التشخيصُ يُهمَل هنا عمدًا: الاستنتاجُ يصف نوعًا ولا
                        //      يُبلّغ أخطاءً؛ ومسارُ البناءِ يُشخّص الإبهامَ في موضعه.
                        // (EN) The diagnostic is deliberately dropped here: inference describes
                        //      a type, it does not report errors; the build path diagnoses the
                        //      ambiguity at its own site.
                        std::string ignoredAmbiguityDiagnostic;
                        if (b_.isModuleQualifiedSymbol(moduleVar->name, qualifiedCall->methodName,
                                                       ignoredAmbiguityDiagnostic))
                        {
                            auto flatIt = b_.functionTable_.find(qualifiedCall->methodName);
                            if (flatIt != b_.functionTable_.end())
                                return flatIt->second.returnType;
                        }
                    }
                }

                if (auto *mcall = dynamic_cast<const Sad::AST::MethodCallExpr *>(expr))
                    if (inferExprType(mcall->object.get()) == SadTypeKind::Pointer)
                        return SadTypeKind::Pointer;

                // (AR) استدعاء دالة — ابحث عن نوع الإرجاع في b_.functionTable_
                // (EN) Function call — look up return type in b_.functionTable_
                if (auto *call = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                {
                    if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(call->callee.get()))
                    {
                        // ================================================================
                        // (AR) الدوال المضمنة التي تُنشئ أنواعاً خاصة — يجب معالجتها قبل
                        //      البحث في b_.functionTable_ لأنها غير مسجّلة فيها.
                        //      بدون هذا: varType = Integer → objResult.type = Integer
                        //      → b_.buildArrayBasicMethodCall يُولِّد BUILTIN_ARRAY_CONTAINS
                        //      بدلاً من __sad_map_has → type mismatch في LLVM IR
                        // (EN) Builtin constructors that create special types — must be handled
                        //      before b_.functionTable_ lookup since they are not registered there.
                        //      Without this: global var gets Integer type → method dispatch breaks.
                        // ================================================================
                        const auto &fname = varExpr->name;
                        // خريطة() / map() → Map
                        if (fname == "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9" || fname == "map")
                            return SadTypeKind::Map;
                        // مصفوفة() / array() → Array
                        if (fname == "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9" || fname == "array")
                            return SadTypeKind::Array;

                        // (AR) دالّة المستخدم أوّلًا: إن ظلَّل المستخدم اسمَ مصنع عنصر
                        //      بدالّةٍ خاصّة كسبت الأسبقيّة — مطابقةً لترتيب مسار استنتاج
                        //      نوع الإرجاع من الجسم (template_infer_return.cpp) فلا يتباعد
                        //      المساران على الاسم نفسه.
                        // (EN) User function first: a user-defined function shadowing a
                        //      widget-factory name wins — mirrors the return-body path's
                        //      order so both inference paths agree on the same name.
                        auto it = b_.functionTable_.find(fname);
                        if (it != b_.functionTable_.end())
                        {
                            return it->second.returnType;
                        }

                        // ================================================================
                        // (AR) مصانع عناصر الواجهة (UIWidgets) تُرجع «كائن» ⇒ مقبض عنصر
                        //      (Pointer). نشتقّ ذلك من سجلّ المُدمَجات (returnType) لا من
                        //      قائمة أسماء مثبّتة، فيمرّ المتغيّر العامّ عبر مسار SET_PROP
                        //      الانسيابيّ (.محتوى/.لون…) بدل نداء دالّة غير معرَّفة.
                        // (EN) UI widget factories return «كائن» ⇒ widget handle (Pointer).
                        //      Derived from the builtin registry's returnType so a global
                        //      widget var is tracked as Pointer across function boundaries.
                        // ================================================================
                        if (const auto *meta = Sad::Builtins::findBuiltinMeta(fname))
                            if (TemplateBuilder::builtinReturnsToSIRKind(meta->returnType) == SadTypeKind::Pointer)
                                return SadTypeKind::Pointer;
                    }
                    return SadTypeKind::Integer;
                }

                // (AR) تعبير ثلاثي
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

                // (AR) إسناد — نوع القيمة المسندة
                // (EN) Assignment — type of assigned value
                if (auto *assign = dynamic_cast<const Sad::AST::AssignExpr *>(expr))
                {
                    return inferExprType(assign->value.get());
                }

                // (AR) تعبير أحادي
                // (EN) Unary expression
                if (auto *unary = dynamic_cast<const Sad::AST::UnaryExpr *>(expr))
                {
                    if (unary->op == Sad::Lexer::TokenType::OP_NOT)
                        return SadTypeKind::Boolean;
                    return inferExprType(unary->operand.get());
                }

                // (AR) ISSUE-076/084 (ب″): وصولٌ مباشرٌ لحقل ADT (X.حقل حيث «حقل» اسمُ حقلٍ في
                //      حالةٍ ضمن adtEnumTable_) ⇒ حمولةٌ ديناميّةٌ موسومة ⇒ Any، فيُرقّى معامِلُ
                //      دالةٍ تتلقّاه إلى Any (يُوسَم عند الاستدعاء ويُفكّ عند الاستهلاك) ⇒ يُصلح
                //      `ط(ش.نق)`. حتّى لو تصادف اسمُ حقلِ صنفٍ مع حقلِ ADT فالتوسيم يُدوِّر كلَّ
                //      نوعٍ صحيحًا (صحيح↔10 · عشريّ↔صندوق · نصّ↔00 · منطقيّ↔11) فلا انحدار.
                // (EN) ISSUE-076/084 (ب″): direct ADT field access (X.f where «f» is a field of a
                //      variant in adtEnumTable_) ⇒ a tagged dynamic payload ⇒ Any, so a function
                //      receiving it has its param widened to Any (tagged at the call, decoded at the
                //      consumer) ⇒ fixes `ط(ش.نق)`. Even if a class field name collides with an ADT
                //      field, tagging round-trips every type correctly (int↔10 · float↔box · string↔
                //      00 · bool↔11) — no regression.
                {
                    std::string memberName;
                    if (auto *mem = dynamic_cast<const Sad::AST::MemberExpr *>(expr))
                        memberName = mem->member;
                    else if (auto *ma = dynamic_cast<const Sad::AST::MemberAccessExpr *>(expr))
                        memberName = ma->memberName;
                    if (!memberName.empty())
                    {
                        for (const auto &kv : b_.adtEnumTable_)
                            if (kv.second.findFieldIndex(memberName) >= 0)
                                return SadTypeKind::Any;
                    }
                }

                // (AR) افحص DataType من التعبير
                // (EN) Check DataType from expression
                auto dtype = expr->getTypeKind();
                if (dtype == Sad::Types::SadTypeKind::String)
                    return SadTypeKind::String;
                if (dtype == Sad::Types::SadTypeKind::Float)
                    return SadTypeKind::Float;
                if (dtype == Sad::Types::SadTypeKind::Boolean)
                    return SadTypeKind::Boolean;
                if (dtype == Sad::Types::SadTypeKind::Array)
                    return SadTypeKind::Array;
                if (dtype == Sad::Types::SadTypeKind::Map)
                    return SadTypeKind::Map;

                // (AR) الافتراضي — I64
                // (EN) Default — I64
                return SadTypeKind::Integer;
            }

            // ============================================================================
            // scanCallSitesInExpr - مسح مواقع الاستدعاء داخل التعبيرات لاستدلال أنواع الوسائط
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
