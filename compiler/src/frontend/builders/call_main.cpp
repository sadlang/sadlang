// ============================================================================
// sir_builder.cpp - بناء SIR من AST / SIR Builder from AST
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 5, 2026
// الإصدار / Version: 2.0 (إعادة كتابة كاملة من الصفر)
//
// ملاحظة هامة / Important Note:
// هذا الملف مُعاد كتابته بالكامل من الصفر بما يتوافق 100% مع:
// This file is completely rewritten from scratch to comply 100% with:
// - STRICT_CODING_RULES.md
// - sir_builder.h
// - sir_types.h
// - sir_instruction.h
// - sir_module.h
// - AST headers (ast_node.h, expressions.h, statements.h, declarations.h)
// ============================================================================

#include "sir_builder.h"
#include "builders/call_builder.h"
#include <string>
#include <cstdio>
#include <set>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <optional>
#include "bounds_checker.h" // (AR) فحص حدود موحَّد / (EN) unified bounds checking

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // emitClosureCallFromCallee - إصدار CLOSURE_CALL على مؤشّر إغلاق مبنيّ مسبقًا
            //                             Emit CLOSURE_CALL on an already-built closure ptr
            // ============================================================================
            // (AR) مُستخرَج من مساري IndexExpr وLambdaExpr (IIFE) في buildFunctionCall لإزالة
            //      التكرار: كلاهما يبني مؤشّر الإغلاق أوّلًا ثم يبني الوسائط ويُصدر CLOSURE_CALL
            //      بنوع نتيجة Integer. السلوك مطابق للنسختين السابقتين حرفًا بحرف.
            // (EN) Factored out of the IndexExpr and LambdaExpr (IIFE) paths in buildFunctionCall
            //      to remove duplication: both build the closure pointer first, then build
            //      arguments and emit CLOSURE_CALL with an Integer result type. Behavior is
            //      byte-for-byte identical to the two prior inline copies.
            BuildResult CallBuilder::emitClosureCallFromCallee(const BuildResult &closureResult,
                                                               Sad::AST::CallExpr *call,
                                                               const std::string &comment)
            {
                if (closureResult.registerName.empty())
                    return BuildResult();

                // (AR) بناء الوسائط
                // (EN) Build arguments
                std::vector<SIROperand> argOps;
                for (auto &arg : call->arguments)
                {
                    BuildResult argRes = b_.buildExpression(arg.get());
                    if (!argRes.registerName.empty())
                        argOps.push_back(SIROperand::Register(argRes.registerName, argRes.type));
                }

                // (AR) إصدار CLOSURE_CALL
                // (EN) Emit CLOSURE_CALL
                std::string resultReg = b_.newTempRegister();
                SIRInstruction closureCallInst;
                closureCallInst.opcode = SIROpcode::CLOSURE_CALL;
                closureCallInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                closureCallInst.operands.push_back(SIROperand::Register(closureResult.registerName, SadTypeKind::Function));
                for (const auto &argOp : argOps)
                {
                    closureCallInst.operands.push_back(argOp);
                }
                closureCallInst.comment = comment;
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(closureCallInst);

                return BuildResult(resultReg, SadTypeKind::Integer);
            }

            // ============================================================================
            // buildFunctionCall - بناء استدعاء دالة
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:464
            // التوقيع / Signature: BuildResult buildFunctionCall(AST::FunctionCallNode* call);
            //
            // المعاملات / Parameters:
            // - call: AST::FunctionCallNode* = Sad::AST::CallExpr* (sir_builder.h:68)
            //
            // CallExpr Members (expressions.h:276):
            // - callee: ExprPtr (line 278)
            // - arguments: ExprList (line 279)
            //
            // الدوال المضمنة / Builtin Functions (sir_types.h:178-187):
            // - طول() / length: STRING_LEN للنصوص، ARRAY_LEN للمصفوفات
            // - اطبع() / print: BUILTIN_PRINT
            // - إلى_رقم() / to_int: STRING_TO_I64
            // - إلى_نص() / to_string: I64_TO_STRING / F64_TO_STRING
            // ============================================================================
            BuildResult CallBuilder::buildFunctionCall(AST::FunctionCallNode *call)
            {
                if (!call)
                {
                    return BuildResult();
                }
#ifdef SIR_BUILDER_DEBUG
                std::cerr << "[SIR-DBG] buildFunctionCall entered" << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 1: استخراج اسم الدالة من callee
                // (EN) Step 1: Extract function name from callee
                // المصدر: expressions.h:290 - callee: ExprPtr
                // ========================================================================
                std::string funcName;

                // (AR) callee يمكن أن يكون VariableExpr (اسم دالة مباشر)
                // (EN) callee can be VariableExpr (direct function name)
                // المصدر: expressions.h:210-212 - VariableExpr::name
                if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr *>(call->callee.get()))
                {
                    funcName = varExpr->name;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: function name = '" << funcName << "'" << std::endl;
#endif
                }
                // ================================================================
                // (AR) [Phase 4 — Monomorphization] callee هو TemplateInstantiation
                //      (مثال: f<عشري، 4>(arg)) — نُنشئ النسخة المتخصصة فوراً
                //      مع كلٍّ من typeArguments و constArguments المُستخرجين من
                //      الحرفيات في AST، ثم نُسند funcName لاسم النسخة لإكمال
                //      مسار الاستدعاء العادي.
                // (EN) [Phase 4 — Monomorphization] callee is TemplateInstantiation
                //      (e.g. f<float, 4>(arg)) — instantiate the specialized
                //      version up-front with both typeArguments and the literal
                //      constArguments extracted from AST, then bind funcName to
                //      the specialization so the regular call path takes over.
                // ================================================================
                else if (auto tInst = dynamic_cast<Sad::AST::TemplateInstantiation *>(call->callee.get()))
                {
                    std::vector<SadTypeKind> typeArgs;
                    typeArgs.reserve(tInst->typeArguments.size());
                    for (const auto &t : tInst->typeArguments)
                        typeArgs.push_back(b_.astTypeToSIRType(t));

                    // (AR) تقييم وسائط الثوابت كـ SIROperand مباشرة من الحرفيات
                    // (EN) Lower const-generic args from AST literals into SIROperands
                    std::vector<SIROperand> constArgs;
                    constArgs.reserve(tInst->constArguments.size());
                    for (const auto &cExpr : tInst->constArguments)
                    {
                        auto *lit = dynamic_cast<Sad::AST::LiteralExpr *>(cExpr.get());
                        if (!lit)
                        {
                            b_.errors_.push_back("Error: const-generic argument must be a literal in '" + tInst->templateName + "'");
                            return BuildResult();
                        }
                        const auto &tok = lit->token;
                        using TT = Sad::Lexer::TokenType;
                        switch (tok.getType())
                        {
                        case TT::NUMBER_INTEGER:
                            constArgs.push_back(SIROperand::ConstantI64(std::stoll(tok.getValue())));
                            break;
                        case TT::NUMBER_DOUBLE:
                            constArgs.push_back(SIROperand::ConstantF64(std::stod(tok.getValue())));
                            break;
                        case TT::STRING_LITERAL:
                            constArgs.push_back(SIROperand::ConstantString(tok.getValue()));
                            break;
                        case TT::LITERAL_TRUE:
                            constArgs.push_back(SIROperand::ConstantBool(true));
                            break;
                        case TT::LITERAL_FALSE:
                            constArgs.push_back(SIROperand::ConstantBool(false));
                            break;
                        default:
                            b_.errors_.push_back("Error: unsupported const-generic literal kind in '" + tInst->templateName + "'");
                            return BuildResult();
                        }
                    }

                    std::string instName = b_.instantiateTemplate(tInst->templateName, typeArgs, constArgs);
                    if (instName.empty())
                        return BuildResult();
                    funcName = instName;
                }
                else if (auto indexExpr = dynamic_cast<Sad::AST::IndexExpr *>(call->callee.get()))
                {
                    // (AR) استدعاء غير مباشر: callee هو IndexExpr (مثل: عمليات[0](2) أو حاسبة["جمع"](3,5))
                    //      نبني IndexExpr أولاً → يُرجع closure pointer
                    //      ثم نبني الوسائط ونُصدر CLOSURE_CALL
                    // (EN) Indirect call: callee is IndexExpr (e.g. ops[0](2) or calc["add"](3,5))
                    //      Build IndexExpr first → returns closure pointer
                    //      Then build arguments and emit CLOSURE_CALL
                    BuildResult closureResult = b_.buildExprIndex(indexExpr);
                    // (AR) منطق بناء الوسائط وإصدار CLOSURE_CALL مشترك مع مسار LambdaExpr
                    // (EN) Argument-building + CLOSURE_CALL emission shared with LambdaExpr path
                    return emitClosureCallFromCallee(closureResult, call, "indexed closure call");
                }
                else if (auto lambdaExpr = dynamic_cast<Sad::AST::LambdaExpr *>(call->callee.get()))
                {
                    // (AR) [ISSUE-048] استدعاء لامدا فوريّ (IIFE): «(لامدا(س) => …)(5)».
                    //      نبني اللامدا أوّلاً (CLOSURE_CREATE يُرجِع مؤشّر إغلاق) ثم نُصدر
                    //      CLOSURE_CALL — نظير مسار IndexExpr أعلاه. كان callee غير المباشر
                    //      يسقط في «else» فيُرفَض ⇒ خرج فارغ في المترجم.
                    // (EN) [ISSUE-048] Immediately-invoked lambda (IIFE): build the lambda
                    //      (CLOSURE_CREATE → closure pointer) then emit CLOSURE_CALL, mirroring
                    //      the IndexExpr path. Previously a lambda callee fell into the
                    //      unsupported `else` branch → empty output in the compiler.
                    BuildResult closureResult = b_.buildExprLambda(lambdaExpr);
                    // (AR) نفس منطق إصدار CLOSURE_CALL في مسار IndexExpr — مُستخرَج للمُساعد
                    // (EN) Same CLOSURE_CALL emission as the IndexExpr path — factored to helper
                    return emitClosureCallFromCallee(closureResult, call, "IIFE lambda call");
                }
                else
                {
                    // (AR) لا ندعم استدعاءات غير مباشرة حالياً
                    // (EN) We don't support indirect calls currently
                    b_.errors_.push_back("Error: Only direct function calls are supported");
                    return BuildResult();
                }

                // ========================================================================
                // (AR) مرحلة 1: توسيع الماكرو (إذا كان isMacroCall = true)
                //      مستخرجة إلى buildMacroCallExpansion() في sir_builder_calls_macro.cpp
                // (EN) Phase 1: Macro expansion (if isMacroCall = true)
                //      Extracted to buildMacroCallExpansion() in sir_builder_calls_macro.cpp
                // ========================================================================
                {
                    auto macroResult = buildMacroCallExpansion(call, funcName);
                    if (macroResult.has_value())
                        return *macroResult;
                }

                // ========================================================================
                // (AR) مرحلة 2: استدعاء عامل () على كائن (callable object invoke)
                //      مستخرجة إلى buildCallableObjectInvoke() في sir_builder_calls_special.cpp
                // (EN) Phase 2: Callable object invoke
                //      Extracted to buildCallableObjectInvoke() in sir_builder_calls_special.cpp
                // ========================================================================
                {
                    auto callableResult = buildCallableObjectInvoke(call, funcName);
                    if (callableResult.has_value())
                        return *callableResult;
                }

                // ========================================================================
                // (AR) مرحلة 3: استدعاء باني الأب (super constructor)
                //      مستخرجة إلى buildSuperConstructorCall() في sir_builder_calls_special.cpp
                // (EN) Phase 3: Super constructor call
                //      Extracted to buildSuperConstructorCall() in sir_builder_calls_special.cpp
                // ========================================================================
                {
                    auto superResult = buildSuperConstructorCall(call, funcName);
                    if (superResult.has_value())
                        return *superResult;
                }

                // ========================================================================
                // (AR) مرحلة 3.5: فحص إنشاء كائن بدون 'جديد' (class-as-function)
                //      القاعدة: اسم_صنف(معاملات) بدون 'جديد' يُنشئ كائناً
                //      يُفحص قبل بناء الوسائط لأننا نفوّض لـ b_.buildNewObject
                //      الذي يبني الوسائط ويستنتج أنواع الحقول بنفسه
                //      هذا يتوافق مع المفسر الذي يدعم نفس القاعدة
                // (EN) Phase 3.5: Check for class-as-function (create object without 'new')
                //      Rule: ClassName(args) without 'new' creates an object
                //      Checked BEFORE building arguments because we delegate to b_.buildNewObject
                //      which builds arguments and infers field types itself
                // ========================================================================
                {
                    auto sirClass = b_.module_->getClass(funcName);
                    if (sirClass)
                    {
                        // (AR) الاسم صنف → إنشاء NewExpr مؤقت وتفويض لـ b_.buildNewObject
                        //      نستخدم الوسائط الأصلية من AST (لم تُبنَ بعد)
                        // (EN) Name is a class → create temp NewExpr and delegate to b_.buildNewObject
                        //      Using original AST arguments (not yet built)
                        Sad::AST::NewExpr tempNewExpr(funcName);
                        for (auto &arg : call->arguments)
                        {
                            tempNewExpr.arguments.push_back(std::move(arg));
                        }
                        auto result = b_.buildNewObject(&tempNewExpr);
                        // (AR) إعادة الوسائط إلى call node الأصلي (تجنب تدمير AST)
                        // (EN) Restore arguments to original call node (avoid AST corruption)
                        for (auto &arg : tempNewExpr.arguments)
                        {
                            call->arguments.push_back(std::move(arg));
                        }
                        return result;
                    }
                }

                // ========================================================================
                // (AR) مرحلة 4: بناء قائمة الوسائط + ملء القيم الافتراضية
                //      مستخرجة إلى buildCallArgumentsList() و fillDefaultCallArguments()
                //      في sir_builder_calls_args.cpp
                // (EN) Phase 4: Build argument list + fill defaults
                //      Extracted to buildCallArgumentsList() and fillDefaultCallArguments()
                //      in sir_builder_calls_args.cpp
                // ========================================================================
                std::vector<SIROperand> argOperands;
                std::vector<BuildResult> argResults;
                if (!buildCallArgumentsList(call, argOperands, argResults))
                    return BuildResult();
                fillDefaultCallArguments(call, funcName, argOperands, argResults);

                // (AR) الخطوة 2.5: التحقق من الدوال المضمنة
                // (EN) Step 2.5: Check for builtin functions
                // ========================================================================

                // ========================================================================
                // (AR) إصلاح: تحقق أولاً إذا كانت الدالة معرّفة من المستخدم
                // (EN) Fix: First check if function is user-defined before checking builtins
                // هذا يمنع الدوال المضمنة من التداخل مع دوال المستخدم التي تحمل نفس الاسم
                // This prevents builtins from shadowing user-defined functions with same name
                // (AR) نفحص أيضاً الأسماء المستعارة للامدا — لأن لامدا مُسندة لمتغير (مثل: جمع = لامدا...)
                //       يجب أن تأخذ الأولوية على الدوال المضمنة التي تحمل نفس الاسم
                // (EN) Also check lambda aliases — a lambda assigned to a variable (e.g. جمع = lambda...)
                //       must take priority over builtins with the same name
                // (AR) [إصلاح] فحص المتغيرات أيضاً — لامدا مُسندة لمتغير (مثل: تحقق = لامدا...)
                //       لا تُسجّل في b_.lambdaAliases_ في نظام الإغلاقات الجديد (CLOSURE_CALL)
                //       لكن المتغير يحمل closure، فيجب أن يأخذ الأولوية على الدوال المضمنة
                //       بدون هذا: "تحقق" كـ builtin verify يطغى على لامدا المستخدم بنفس الاسم
                //       نفحص فقط المتغيرات من نوع Function لتجنب طغيان متغير عادي (رقم/نص)
                //       على دالة مضمنة بنفس الاسم (مثلاً: متغير طول = 5 لا يطغى على طول())
                // (EN) [Fix] Also check variables — lambda assigned to variable (e.g. check = lambda...)
                //       is NOT registered in b_.lambdaAliases_ in new closure system (CLOSURE_CALL)
                //       but the variable holds a closure, so it must take priority over builtins
                //       Without this: builtin "تحقق" (verify) shadows user's lambda with same name
                //       Only check Function-typed variables to avoid shadowing builtins with int/string vars
                // ========================================================================
                VariableInfo *closureVarCheck = b_.lookupVariable(funcName);
                bool isClosureVariable = (closureVarCheck != nullptr && closureVarCheck->type == SadTypeKind::Function);
                bool isUserDefinedFunction = (b_.functionTable_.find(funcName) != b_.functionTable_.end()) ||
                                             (b_.lambdaAliases_.find(funcName) != b_.lambdaAliases_.end()) ||
                                             isClosureVariable;

                // (AR) دالة طول() - STRING_LEN للنصوص، ARRAY_LEN للمصفوفات
                // (EN) length() function - STRING_LEN for strings, ARRAY_LEN for arrays

                // ========================================================================
                // (AR) التحقق من الدوال المدمجة (مقسمة إلى ملفين)
                // (EN) Check builtin functions (split into two files)
                // ========================================================================
                // (AR) أولاً: SIMD متجهات — يجب الفحص قبل غيرها لأن "متجه" قد يتعارض مع
                //      أسماء صنوف محتملة. نعتمد على isUserDefinedFunction لتجاوزنا.
                // (EN) First: SIMD vectors — must be checked before others because "متجه"
                //      may conflict with potential class names. We rely on isUserDefinedFunction.
                auto simdResult = b_.buildBuiltinCallSimd(funcName, isUserDefinedFunction, argResults, argOperands);
                if (simdResult.has_value())
                {
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG] buildFunctionCall: handled by BuiltinSimd! type="
                              << static_cast<int>(simdResult.value().type) << std::endl;
#endif
                    return simdResult.value();
                }
                auto builtinResult = b_.buildBuiltinCallCore(funcName, isUserDefinedFunction, argResults, argOperands);
                if (builtinResult.has_value())
                {
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG] buildFunctionCall: handled by BuiltinCore! type="
                              << static_cast<int>(builtinResult.value().type) << std::endl;
#endif
                    return builtinResult.value();
                }
                builtinResult = b_.buildBuiltinCallSystem(funcName, isUserDefinedFunction, argResults, argOperands);
                if (builtinResult.has_value())
                {
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG] buildFunctionCall: handled by BuiltinSystem! type="
                              << static_cast<int>(builtinResult.value().type) << std::endl;
#endif
                    return builtinResult.value();
                }

                // ========================================================================
                // (AR) الخطوة 2.6: التحقق من الدوال المضمنة للشبكة
                //      مقابس TCP/UDP، عميل HTTP، خادم HTTP، أدوات الشبكة
                // (EN) Step 2.6: Check network builtin functions
                //      TCP/UDP sockets, HTTP client, HTTP server, network utilities
                // ========================================================================
                builtinResult = b_.buildBuiltinCallNetwork(funcName, isUserDefinedFunction, argResults, argOperands);
                if (builtinResult.has_value())
                {
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG] buildFunctionCall: handled by BuiltinNetwork! type="
                              << static_cast<int>(builtinResult.value().type) << std::endl;
#endif
                    return builtinResult.value();
                }

                // ========================================================================
                // (AR) الخطوة 2.7: حل الأسماء المستعارة للامدا
                // (EN) Step 2.7: Resolve lambda aliases
                // مثال: ف(5) حيث ف = لامدا(س): س + 1 → funcName = "__lambda_0"
                // ========================================================================
                {
                    auto aliasIt = b_.lambdaAliases_.find(funcName);
                    if (aliasIt != b_.lambdaAliases_.end())
                    {
                        std::string realLambdaName = aliasIt->second;
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[LAMBDA] Resolved alias '" << funcName
                                  << "' -> '" << realLambdaName << "'" << std::endl;
#endif
                        funcName = realLambdaName;

                        // (AR) إضافة المتغيرات المُلتقطة كمعاملات إضافية مخفية
                        // (EN) Append captured variables as extra hidden arguments
                        auto capIt = b_.closureCaptures_.find(realLambdaName);
                        if (capIt != b_.closureCaptures_.end())
                        {
                            for (const auto &cap : capIt->second)
                            {
                                VariableInfo *capVar = b_.lookupVariable(cap.varName);
                                if (capVar)
                                {
                                    // (AR) تحميل القيمة الحالية للمتغير المُلتقط
                                    // (EN) Load current value of captured variable
                                    std::string loadReg = b_.newTempRegister();
                                    SIRInstruction loadInst;
                                    loadInst.opcode = SIROpcode::LOAD;
                                    loadInst.result = SIROperand::Register(loadReg, capVar->type);
                                    loadInst.operands.push_back(SIROperand::Register(capVar->registerName, capVar->type));
                                    if (b_.currentBlock_)
                                        b_.currentBlock_->addInstruction(loadInst);

                                    argOperands.push_back(SIROperand::Register(loadReg, capVar->type));
                                }
                                else
                                {
                                    // (AR) الرجوع إلى معلومات الالتقاط المخزنة
                                    // (EN) Fallback to stored capture info
                                    argOperands.push_back(SIROperand::Register(cap.registerName, cap.type));
                                }
                            }
#ifdef SIR_BUILDER_DEBUG
                            std::cerr << "[LAMBDA] Injected " << capIt->second.size()
                                      << " captured args for '" << realLambdaName << "'" << std::endl;
#endif
                        }

                        // ================================================================
                        // (AR) إضافة __env = 0 كمعامل أخير (دائماً)
                        //      جميع اللامدات تُعرّف بمعامل __env كمعامل أخير
                        //      حتى بدون التقاطات، يجب تمرير 0 كقيمة __env
                        //      بدون هذا: الدالة تُستدعى بعدد معاملات أقل مما تتوقعه
                        //      مما يؤدي إلى قراءة قيم trash من المكدس
                        // (EN) Append __env = 0 as last argument (always)
                        //      All lambdas are defined with __env as last parameter
                        //      Even without captures, must pass 0 as __env value
                        //      Without this: function called with fewer args than expected
                        //      leading to reading garbage values from stack
                        // ================================================================
                        argOperands.push_back(SIROperand::ConstantI64(0));
                    }
                }

                // ========================================================================
                // (AR) الخطوة 3: البحث عن الدالة والحصول على نوع الإرجاع
                // (EN) Step 3: Look up function and get return type
                // المصدر: sir_builder.h:719 - b_.functionTable_
                // المصدر: sir_builder.h:162-175 - FunctionInfo struct
                // ========================================================================
                SadTypeKind returnType = SadTypeKind::Void; // (AR) افتراضياً void

                auto it = b_.functionTable_.find(funcName);
                if (it != b_.functionTable_.end())
                {
                    // (AR) الدالة موجودة - استخدم نوع الإرجاع (sir_builder.h:165)
                    // (EN) Function found - use return type
                    returnType = it->second.returnType;
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG] buildFunctionCall: found '" << funcName
                              << "' retType=" << static_cast<int>(returnType) << std::endl;
#endif

                    // ================================================================
                    // (AR) استنتاج أنواع المعاملات من موقع الاستدعاء
                    //      إذا كان المعامل I64 (افتراضي/غير محدد) والوسيط STRING/F64/BOOL
                    //      نحدّث نوع المعامل في b_.functionTable_ فقط (ليس في SIRFunction المبنية)
                    //      لأن SIRFunction قد تكون بُنيت بالفعل بأنواع مستنتجة صحيحة
                    // (EN) Infer parameter types from call-site arguments
                    //      If param is I64 (default/unknown) and arg is STRING/F64/BOOL,
                    //      update param type in b_.functionTable_ only (not in built SIRFunction)
                    //      because SIRFunction may already have correct inferred types
                    // ================================================================
                    auto &funcInfo = it->second;
                    for (size_t i = 0; i < argResults.size() && i < funcInfo.parameters.size(); i++)
                    {
                        SadTypeKind argType = argResults[i].type;
                        SadTypeKind &paramType = funcInfo.parameters[i].type;

                        if (paramType == SadTypeKind::Integer && argType == SadTypeKind::String)
                        {
                            // (AR) تحديث إلى STRING — المعامل يستقبل نصوصاً
                            // (EN) Update to STRING — parameter receives strings
                            paramType = SadTypeKind::String;
                        }
                    }

                    // ================================================================
                    // (AR) إصلاح جذري: تحويل الوسائط تلقائياً عند عدم تطابق النوع
                    //      لغة ص ديناميكية النوع — نفس الدالة قد تُستدعى بأنواع مختلفة
                    //      مثال: تحقق("نص", "نص") ثم تحقق(صحيح, صحيح)
                    //      المفسر يتعامل مع هذا تلقائياً عبر Value variant
                    //      المترجم يحتاج تحويلاً صريحاً: BOOL→STRING, I64→STRING
                    // (EN) Radical fix: auto-convert args when type doesn't match param
                    //      Sad language is dynamically typed — same function may be called
                    //      with different types. Interpreter handles this via Value variant.
                    //      Compiler needs explicit conversion: BOOL→STRING, I64→STRING.
                    // ================================================================
                    for (size_t i = 0; i < argOperands.size() && i < funcInfo.parameters.size(); i++)
                    {
                        SadTypeKind paramType = funcInfo.parameters[i].type;
                        SadTypeKind argType = argOperands[i].dataType;

                        if (paramType == SadTypeKind::String && argType != SadTypeKind::String)
                        {
                            // (AR) التحويل يعتمد على ما إذا كانت القيمة ثابتة أم سجل
                            // (EN) Conversion depends on whether the value is constant or register
                            if (argOperands[i].type == SIROperandType::CONSTANT)
                            {
                                // (AR) ثابت: نحوّل مباشرة إلى ConstantString
                                // (EN) Constant: convert directly to ConstantString
                                if (argType == SadTypeKind::Boolean)
                                {
                                    std::string strVal = argOperands[i].boolValue ? "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad" : // "صحيح"
                                                             "\xd8\xae\xd8\xb7\xd8\xa3";                                 // "خطأ"
                                    argOperands[i] = SIROperand::ConstantString(strVal);
                                }
                                else if (argType == SadTypeKind::Integer)
                                {
                                    argOperands[i] = SIROperand::ConstantString(
                                        std::to_string(argOperands[i].intValue));
                                }
                                else if (argType == SadTypeKind::Float)
                                {
                                    // (AR) إصلاح: استخدام %.6f + حذف أصفار زائدة لمطابقة دقة المفسر
                                    //      المفسر يستخدم std::fixed << setprecision(6) + strip trailing zeros
                                    //      %g يعطي 6 أرقام معنوية (3.14159) بينما المفسر يعطي 6 خانات عشرية (3.141593)
                                    // (EN) Fix: use %.6f + strip trailing zeros to match interpreter precision
                                    //      interpreter uses std::fixed << setprecision(6) + strip trailing zeros
                                    char buf[64];
                                    std::snprintf(buf, sizeof(buf), "%.6f", argOperands[i].floatValue);
                                    // (AR) حذف الأصفار الزائدة بعد النقطة العشرية
                                    // (EN) Strip trailing zeros after decimal point
                                    char *dot = strchr(buf, '.');
                                    if (dot)
                                    {
                                        char *end = buf + strlen(buf) - 1;
                                        while (end > dot && *end == '0')
                                            end--;
                                        if (*end == '.')
                                            end--;
                                        *(end + 1) = '\0';
                                    }
                                    argOperands[i] = SIROperand::ConstantString(buf);
                                }
                            }
                            else
                            {
                                // (AR) سجل: نستخدم تعليمة تحويل SIR
                                // (EN) Register: use SIR conversion instruction
                                SIROpcode convOp;
                                if (argType == SadTypeKind::Boolean)
                                {
                                    convOp = SIROpcode::BOOL_TO_STRING;
                                }
                                else if (argType == SadTypeKind::Integer)
                                {
                                    convOp = SIROpcode::I64_TO_STRING;
                                }
                                else if (argType == SadTypeKind::Float)
                                {
                                    convOp = SIROpcode::F64_TO_STRING;
                                }
                                else
                                {
                                    continue; // (AR) لا يمكن تحويل هذا النوع
                                }
                                std::string strReg = b_.newTempRegister();
                                SIRInstruction convInst;
                                convInst.opcode = convOp;
                                convInst.result = SIROperand::Register(strReg, SadTypeKind::String);
                                convInst.operands.push_back(argOperands[i]);
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(convInst);
                                argOperands[i] = SIROperand::Register(strReg, SadTypeKind::String);
                            }
                        }
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: function found, returnType="
                              << static_cast<int>(returnType) << std::endl;
#endif
                }
                else
                {
                    // (AR) الدالة غير موجودة - تحقق إذا كانت قالباً
                    // (EN) Function not found - check if it's a template
                    if (b_.templateFunctions_.find(funcName) != b_.templateFunctions_.end())
                    {
                        std::cout << "[Template] Found template function: " << funcName << std::endl;

                        // (AR) استنتاج الأنواع من المعاملات
                        // (EN) Infer types from arguments
                        std::vector<SadTypeKind> inferredTypes;
                        for (const auto &argResult : argResults)
                        {
                            inferredTypes.push_back(argResult.type);
                        }

                        // (AR) إنشاء نسخة من القالب مع الأنواع المستنتجة
                        // (EN) Instantiate template with inferred types
                        std::string instanceName = b_.instantiateTemplate(funcName, inferredTypes);

                        if (instanceName.empty())
                        {
                            std::cerr << "[Template Error] Failed to instantiate template: " << funcName << std::endl;
                            b_.errors_.push_back("Error: Failed to instantiate template '" + funcName + "'");
                            return BuildResult();
                        }

                        // (AR) استخدم اسم النسخة المُنشأة
                        // (EN) Use the instantiated function name
                        funcName = instanceName;

                        // (AR) ابحث عن نوع الإرجاع للنسخة المُنشأة
                        // (EN) Look up return type for instantiated function
                        auto instIt = b_.functionTable_.find(funcName);
                        if (instIt != b_.functionTable_.end())
                        {
                            returnType = instIt->second.returnType;
                        }
                        else
                        {
                            std::cerr << "[Template Error] Instantiated function not in function table: " << funcName << std::endl;
                        }

                        std::cout << "[Template] Using instantiated function: " << funcName << std::endl;
                    }
                    else
                    {
// (AR) قد تكون دالة مدمجة غير مسجلة بعد
// (EN) May be a builtin function not registered yet
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildFunctionCall: function '" << funcName
                                  << "' not found (no template), assuming VOID return" << std::endl;
#endif
                    }
                }

                // ========================================================================
                // (AR) الخطوة 3.5: فحص الاستدعاء عبر بنية إغلاق (Closure)
                //      إذا كان الاسم ليس في b_.functionTable_ ولا b_.lambdaAliases_ ولا b_.templateFunctions_
                //      لكنه متغير معروف → يحمل مؤشر بنية إغلاق (closure struct)
                //      نُصدر CLOSURE_CALL الذي يستخرج fn_ptr و env_ptr من البنية
                //      ويستدعي fn_ptr(args..., env_ptr) — يعمل مع/بدون التقاطات
                // (EN) Step 3.5: Check for closure call via closure struct
                //      If name is not in b_.functionTable_, b_.lambdaAliases_, or b_.templateFunctions_
                //      but IS a known variable → it holds a closure struct pointer
                //      Emit CLOSURE_CALL which extracts fn_ptr + env_ptr and calls fn(args, env)
                // ========================================================================
                if (b_.functionTable_.find(funcName) == b_.functionTable_.end() &&
                    b_.lambdaAliases_.find(funcName) == b_.lambdaAliases_.end() &&
                    b_.templateFunctions_.find(funcName) == b_.templateFunctions_.end())
                {
                    VariableInfo *varInfo = b_.lookupVariable(funcName);

                    // ================================================================
                    // (AR) سدّ السقوط الصامت: اسم ليس دالّة معرّفة ولا لامدا ولا قالبًا
                    //      ولا متغيّر إغلاق ⇒ خطأ قاطع «دالة غير معرّفة». السلوك القديم
                    //      كان يُصدر نداءً خارجيًّا void ويستبدل قيمته بـ i64 0 صامتًا —
                    //      شيفرة فاسدة تمرّ بنجاح كاذب (اكتُشفت في نواة نظام النحلة).
                    //      يُستثنى وضع الوحدة (--module): الربط المتأخّر عبر الوحدات مشروع.
                    // (EN) Close the silent fallthrough: unknown name (not a function,
                    //      lambda, template, or closure variable) is a hard error.
                    //      Previously emitted a void extern call and substituted i64 0.
                    //      Module mode (--module) keeps late cross-module binding.
                    // ================================================================
                    if (!varInfo && !b_.isModuleMode())
                    {
                        // (AR) العلاجات الثلاثة: عرّف الدالّة، أو صرّح «خارجي(...)»
                        //      عند الاستدعاء مع «صدّر» عند التعريف في ملفّ آخر، أو
                        //      استخدم --module للربط المتأخّر عبر الوحدات.
                        // (EN) Three remedies: define it; or «خارجي» at the call with
                        //      «صدّر» at the definition in another file; or --module
                        //      for late cross-module linking.
                        std::string undefMsg =
                            "خطأ: استدعاء دالة غير معرّفة '" + funcName +
                            "' — عرّفها، أو أعلنها «خارجي(...)» هنا مع «صدّر» عند تعريفها، "
                            "أو استخدم --module للربط عبر الوحدات "
                            "(undefined function call)";
                        std::cerr << undefMsg << std::endl;
                        b_.errors_.push_back(undefMsg);
                        return BuildResult();
                    }

                    if (varInfo)
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildFunctionCall: '" << funcName
                                  << "' is a variable (closure call)" << std::endl;
#endif
                        // (AR) تحميل مؤشر بنية الإغلاق من المتغير
                        // (EN) Load the closure struct pointer from the variable
                        std::string loadReg = b_.newTempRegister();
                        SIRInstruction loadInst;
                        loadInst.opcode = SIROpcode::LOAD;
                        loadInst.result = SIROperand::Register(loadReg, SadTypeKind::Function);
                        loadInst.operands.push_back(SIROperand::Register(varInfo->registerName, SadTypeKind::Function));
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(loadInst);

                        // (AR) تحديد نوع إرجاع الإغلاق:
                        //      الإغلاقات ليست في b_.functionTable_ لذا returnType قد يكون Void خطأً.
                        //      [إصلاح] نبحث أولاً عن اسم دالة اللامدا المرتبطة بالمتغير
                        //      ونأخذ نوع الإرجاع من b_.functionTable_ — هذا يحافظ على Boolean
                        //      بدلاً من التراجع للافتراضي Integer.
                        //      مثال: تحقق = لامدا(س) => س > 10 → closureLambdaName = "__lambda_0"
                        //             → b_.functionTable_["__lambda_0"].returnType = Boolean
                        //             → CLOSURE_CALL result.dataType = Boolean → يطبع صحيح/خطأ
                        // (EN) Determine closure return type:
                        //      [Fix] First look up the associated lambda function name from VariableInfo
                        //      and get return type from b_.functionTable_ — preserves Boolean type
                        SadTypeKind closureRetType = returnType;
                        if (!varInfo->closureLambdaName.empty())
                        {
                            auto lambdaIt = b_.functionTable_.find(varInfo->closureLambdaName);
                            if (lambdaIt != b_.functionTable_.end())
                            {
                                closureRetType = lambdaIt->second.returnType;
                            }
                        }
                        if (closureRetType == SadTypeKind::Void)
                        {
                            closureRetType = SadTypeKind::Integer;
                        }

                        // (AR) إنشاء تعليمة CLOSURE_CALL
                        //      المعامل الأول = مؤشر بنية الإغلاق
                        //      الباقي = وسائط الاستدعاء الصريحة
                        //      CLOSURE_CALL يستخرج fn_ptr و env_ptr ويستدعي fn(args, env)
                        // (EN) Create CLOSURE_CALL instruction
                        //      First operand = closure struct pointer
                        //      Rest = explicit call arguments
                        //      CLOSURE_CALL extracts fn_ptr + env_ptr and calls fn(args, env)
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction closureCallInst;
                        closureCallInst.opcode = SIROpcode::CLOSURE_CALL;
                        closureCallInst.result = SIROperand::Register(resultReg, closureRetType);
                        closureCallInst.operands.push_back(SIROperand::Register(loadReg, SadTypeKind::Function));
                        for (const auto &argOp : argOperands)
                        {
                            closureCallInst.operands.push_back(argOp);
                        }
                        // (AR) تمرير اسم دالة اللامدا في comment — يستخدمه emitClosureCall
                        //      للبحث عن التوقيع الحقيقي وتحويل أنواع الوسائط
                        // (EN) Pass lambda function name in comment — used by emitClosureCall
                        //      to look up real signature and convert argument types
                        if (!varInfo->closureLambdaName.empty())
                        {
                            closureCallInst.comment = "lambda:" + varInfo->closureLambdaName;
                        }
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(closureCallInst);

                        // (AR) استدعاء غير مباشر لمرجع دالّة مولّدة (`متغير د = عد` ثمّ `د()`):
                        //      CLOSURE_CALL أعاد مقبض المولّد الخام؛ نستهلكه في مصفوفة عبر
                        //      GENERATOR_CONSUME تمامًا كالاستدعاء المباشر، وإلّا يُعامَل المقبض
                        //      كمصفوفة زيفًا فينهار التكرار. النوع Array ليطابق `نوع()` المفسّرَ.
                        // (EN) Indirect call of a generator func-ref (`var d = count` then `d()`):
                        //      CLOSURE_CALL returned the raw generator handle; consume it into an
                        //      array via GENERATOR_CONSUME just like a direct call, else the handle
                        //      is falsely treated as an array and iteration crashes. Type Array to
                        //      match the interpreter's `نوع()`.
                        if (varInfo->isGeneratorFuncRef)
                        {
                            std::string consumeReg = b_.newTempRegister();
                            SIRInstruction consumeInst(SIROpcode::GENERATOR_CONSUME);
                            consumeInst.result = SIROperand::Register(consumeReg, SadTypeKind::Array);
                            consumeInst.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::Pointer));
                            consumeInst.comment = "consume generator yields (indirect)";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(consumeInst);
                            BuildResult genResult(consumeReg, SadTypeKind::Array);
                            genResult.isDirectValue = true;
                            return genResult;
                        }

                        BuildResult closureResult(resultReg, closureRetType);
                        closureResult.isDirectValue = true;
                        return closureResult;
                    }
                }

                // ================================================================
                // (AR) [Fix #049-Part2] استنتاج نوع الإرجاع لدوال الرتبة العليا (pass-through)
                //      إذا كان نوع الإرجاع Integer (الافتراضي الناتج عن استدعاء غير مباشر)
                //      وأحد الوسائط إغلاق بنوع إرجاع معروف، نفحص SIR الدالة:
                //      - هل يوجد CLOSURE_CALL/CALL_INDIRECT يُعيد نتيجته مباشرة عبر RET؟
                //      - إذا نعم: الدالة "pass-through" تُعيد نتيجة اللامدا
                //      - نستخدم نوع إرجاع اللامدا المُمررة بدلاً من الافتراضي Integer
                //      مثال: دالة طبّق(دالة_, قيمة) → ارجع دالة_(قيمة)
                //             طبّق(ربط, "عالم") حيث ربط لامدا تُرجع نصاً → returnType = String
                // (EN) [Fix #049-Part2] Return type inference for higher-order pass-through functions
                //      If return type is Integer (default from indirect call inference)
                //      and an argument is a closure with known return type, check SIR:
                //      - Does a CLOSURE_CALL/CALL_INDIRECT result flow directly to RET?
                //      - If yes: function is "pass-through", returns the lambda's result
                //      - Use the passed lambda's return type instead of default Integer
                // ================================================================
                if (returnType == SadTypeKind::Integer && it != b_.functionTable_.end() &&
                    it->second.sirFunction)
                {
                    auto &funcInfo = it->second;
                    // ================================================================
                    // (AR) الخطوة أ: فحص نمط pass-through في SIR
                    //      [Fix #049-v2] المشكلة القديمة: CLOSURE_CALL → STORE → LOAD → RET
                    //      كان الكود يبحث عن RET.operand مباشرةً في indirectCallResults،
                    //      لكن النمط الشائع هو: %r = CLOSURE_CALL → STORE %r, %var → LOAD %tmp, %var → RET %tmp
                    //      الحل: بناء جداول بحث LOAD و STORE وتتبع السلسلة
                    //
                    // (EN) Step A: Detect pass-through pattern in SIR
                    //      [Fix #049-v2] Old problem: CLOSURE_CALL → STORE → LOAD → RET
                    //      Code was checking RET.operand directly in indirectCallResults,
                    //      but common pattern is: %r = CLOSURE_CALL → STORE %r, %var → LOAD %tmp, %var → RET %tmp
                    //      Fix: build LOAD/STORE lookup tables and trace the chain
                    // ================================================================
                    std::set<std::string> indirectCallResults;
                    bool hasPassThrough = false;

                    // (AR) بناء جدولي LOAD و STORE للتتبع عبر السلسلة
                    // (EN) Build LOAD/STORE lookup tables for chain tracing
                    std::unordered_map<std::string, std::string> passLoadSrc;  // load_result → source_var
                    std::unordered_map<std::string, std::string> passStoreVal; // dest_var → stored_value

                    for (const auto &block : funcInfo.sirFunction->basicBlocks)
                    {
                        for (const auto &inst : block->instructions)
                        {
                            if ((inst.opcode == SIROpcode::CLOSURE_CALL ||
                                 inst.opcode == SIROpcode::CALL_INDIRECT) &&
                                inst.result.has_value())
                            {
                                indirectCallResults.insert(inst.result->name);
                            }
                            if (inst.opcode == SIROpcode::LOAD &&
                                inst.result.has_value() && !inst.operands.empty())
                            {
                                passLoadSrc[inst.result->name] = inst.operands[0].name;
                            }
                            if (inst.opcode == SIROpcode::STORE && inst.operands.size() >= 2)
                            {
                                // (AR) operands[1] = الوجهة، operands[0] = المصدر
                                passStoreVal[inst.operands[1].name] = inst.operands[0].name;
                            }
                        }
                    }

                    // (AR) فحص RET مع تتبع سلسلة LOAD→STORE
                    // (EN) Check RET with LOAD→STORE chain tracing
                    for (const auto &block : funcInfo.sirFunction->basicBlocks)
                    {
                        for (const auto &inst : block->instructions)
                        {
                            if (inst.opcode == SIROpcode::RET && !inst.operands.empty())
                            {
                                const std::string &retReg = inst.operands[0].name;

                                // (AR) الحالة أ: RET يُعيد مباشرةً نتيجة CLOSURE_CALL
                                // (EN) Case A: RET directly returns CLOSURE_CALL result
                                if (indirectCallResults.count(retReg))
                                {
                                    hasPassThrough = true;
                                    break;
                                }

                                // (AR) الحالة ب: RET %tmp ← LOAD %tmp, %var ← STORE %closureCallResult, %var
                                // (EN) Case B: RET %tmp ← LOAD %tmp, %var ← STORE %closureCallResult, %var
                                auto it_l = passLoadSrc.find(retReg);
                                if (it_l != passLoadSrc.end())
                                {
                                    const std::string &loadedVar = it_l->second;
                                    auto it_s = passStoreVal.find(loadedVar);
                                    if (it_s != passStoreVal.end() &&
                                        indirectCallResults.count(it_s->second))
                                    {
                                        hasPassThrough = true;
                                        break;
                                    }
                                }
                            }
                        }
                        if (hasPassThrough)
                            break;
                    }

                    // (AR) الخطوة ب: إذا كانت pass-through، ابحث عن نوع إرجاع اللامدا المُمررة
                    // (EN) Step B: If pass-through, find the passed lambda's return type
                    if (hasPassThrough)
                    {
                        for (size_t i = 0; i < argResults.size() && i < call->arguments.size(); i++)
                        {
                            if (argResults[i].type == SadTypeKind::Function)
                            {
                                // (AR) فحص: هل الوسيط متغير يحمل إغلاقاً بنوع إرجاع معروف؟
                                // (EN) Check: is the argument a variable holding a closure with known return type?
                                if (auto *argVarExpr = dynamic_cast<const Sad::AST::VariableExpr *>(
                                        call->arguments[i].get()))
                                {
                                    VariableInfo *argVarInfo = b_.lookupVariable(argVarExpr->name);
                                    if (argVarInfo && !argVarInfo->closureLambdaName.empty())
                                    {
                                        auto lambdaIt = b_.functionTable_.find(argVarInfo->closureLambdaName);
                                        if (lambdaIt != b_.functionTable_.end() &&
                                            lambdaIt->second.returnType != SadTypeKind::Integer &&
                                            lambdaIt->second.returnType != SadTypeKind::Void)
                                        {
                                            returnType = lambdaIt->second.returnType;
#ifndef NDEBUG
                                            std::cout << "[DEBUG] buildFunctionCall: pass-through detected for '"
                                                      << funcName << "', specialized returnType to "
                                                      << static_cast<int>(returnType) << " from lambda '"
                                                      << argVarInfo->closureLambdaName << "'" << std::endl;
#endif
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // ========================================================================
                // (AR) الخطوة 4: إنشاء سجل النتيجة وتعليمة الاستدعاء
                // (EN) Step 4: Create result register and call instruction
                // المصدر: sir_instruction.h:231-238 - SIRInstruction::Call()
                // المصدر: sir_types.h:390-395 - SIROperand::Function()
                // ========================================================================
                std::string resultReg = b_.newTempRegister();

                // (AR) إنشاء معامل الدالة (sir_types.h:390)
                // (EN) Create function operand
                SIROperand funcOp = SIROperand::Function(funcName);

                // (AR) إنشاء معامل النتيجة (sir_types.h:355)
                // (EN) Create result operand
                SIROperand resultOp = SIROperand::Register(resultReg, returnType);

                // (AR) إنشاء تعليمة الاستدعاء (sir_instruction.h:231)
                // (EN) Create call instruction
                // SIRInstruction::Call(result, function, args)
                SIRInstruction callInst = SIRInstruction::Call(resultOp, funcOp, argOperands);

                // ================================================================
                // (AR) تمرير اسم الصنف المُرجع إلى الباكند عبر حقل comment:
                //      إذا كانت الدالة تُرجع كائناً (لديها returnClassName في b_.functionTable_)،
                //      نُخزّن اسم الصنف في comment بتنسيق "__return_class__:ClassName"
                //      حتى يتمكن emitCall في الباكند من تحديث objectClassMap.
                //      هذا ضروري لأن SIRInstruction لا يحمل metadata للأنواع المركبة.
                // (EN) Pass return class name to backend via comment field:
                //      If function returns an object (has returnClassName in b_.functionTable_),
                //      store class name in comment as "__return_class__:ClassName"
                //      so emitCall in backend can update objectClassMap.
                // ================================================================
                if (it != b_.functionTable_.end() && !it->second.returnClassName.empty())
                {
                    callInst.comment = "__return_class__:" + it->second.returnClassName;
                }

                // ========================================================================
                // (AR) الخطوة 5: إضافة التعليمة إلى الكتلة الحالية
                // (EN) Step 5: Add instruction to current block
                // المصدر: sir_builder.h:712 - b_.currentBlock_
                // ========================================================================
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(callInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: added CALL instruction to block" << std::endl;
#endif
                }
                else
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: WARNING - no current block!" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) الخطوة 6: إذا كانت الدالة مولّد، أضف GENERATOR_CONSUME لجمع القيم
                // (EN) Step 6: If callee is a generator, add GENERATOR_CONSUME to collect values
                // ========================================================================
                if (it != b_.functionTable_.end() && it->second.isGenerator)
                {
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[GEN] Emitting GENERATOR_CONSUME for generator '" << funcName << "'" << std::endl;
#endif

                    // (AR) CALL أعاد المقبض (PTR) — الآن نستهلكه في مصفوفة. نوع الناتج
                    //      Array لا Integer: المستهلك يجمع القيم في مصفوفة (تنفيذ فوريّ)،
                    //      فيطابق `نوع()` المفسّرَ الذي يُرجع «مصفوفة» لمقبض المولّد — كان
                    //      Integer يُظهره «رقم» فيكسر التكافؤ المزدوج على نوع(). القيمة
                    //      (بِتّات المصفوفة كـi64) لا تتغيّر؛ فقط الوسم النوعيّ.
                    // (EN) CALL returned the handle (PTR); now consume it into an array. The
                    //      result type is Array not Integer: the consumer collects yields into
                    //      an array (eager), so `نوع()` matches the interpreter's «مصفوفة» for
                    //      a generator handle — Integer reported «رقم», breaking dual
                    //      equivalence on typeof. The value (array bits as i64) is unchanged;
                    //      only the type tag.
                    std::string consumeReg = b_.newTempRegister();
                    SIRInstruction consumeInst;
                    consumeInst.opcode = SIROpcode::GENERATOR_CONSUME;
                    consumeInst.result = SIROperand::Register(consumeReg, SadTypeKind::Array);
                    consumeInst.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::Pointer));
                    consumeInst.comment = "consume generator yields";

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(consumeInst);
                    }

                    return BuildResult(consumeReg, SadTypeKind::Array);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildFunctionCall: returning result reg='" << resultReg
                          << "', type=" << static_cast<int>(returnType) << std::endl;
#endif

                // ================================================================
                // (AR) تتبع نوع الصنف المُرجع من الدالة:
                //      إذا كانت الدالة مُسجّلة في b_.functionTable_ وتحمل returnClassName،
                //      ننقل هذه المعلومة إلى BuildResult حتى يتمكن buildLocalVariable
                //      من تسجيل المتغير في b_.classInstanceTypes_.
                //      مثال: متغير ن = اصنع_نقطة() → result.className = "نقطة"
                //      هذا يحل مشكلة: الوصول لحقول كائن مُرجع من دالة كان يتعطل (segfault)
                //      لأن المترجم لم يكن يعرف أن المتغير يحمل كائناً
                // (EN) Track return class type from function:
                //      If function is in b_.functionTable_ and has returnClassName,
                //      transfer this info to BuildResult so buildLocalVariable can
                //      register the variable in b_.classInstanceTypes_.
                //      This fixes: accessing fields of object returned from function was crashing
                //      because compiler didn't know the variable holds an object
                // ================================================================
                BuildResult result(resultReg, returnType);
                if (it != b_.functionTable_.end() && !it->second.returnClassName.empty())
                {
                    result.className = it->second.returnClassName;
                    // (AR) سجّل أيضاً في b_.classInstanceTypes_ بالسجل المؤقت
                    //      حتى لو لم يُخزّن في متغير مباشرة
                    // (EN) Also register in b_.classInstanceTypes_ by temp register
                    //      even if not stored in variable directly
                    b_.classInstanceTypes_[resultReg] = it->second.returnClassName;
                }

                // ================================================================
                // (AR) [Fix #049-Part1] نشر اسم اللامدا المُرجعة إلى BuildResult
                //      إذا كانت الدالة تُرجع إغلاقاً (لديها returnLambdaName)،
                //      ننقل هذا الاسم إلى BuildResult.closureLambdaName
                //      حتى يتمكن buildLocalVariable من تسجيله في VariableInfo.
                //      هذا يحل مشكلة: متغير أ = مصنع("أ", 10) ثم أ() ← كان يُعيد i64 كرقم
                //      لأن closureLambdaName لم يكن يُنشر من FunctionInfo.returnLambdaName
                //      إلى BuildResult، فـ CLOSURE_CALL لم يعرف نوع إرجاع اللامدا.
                // (EN) [Fix #049-Part1] Propagate returned lambda name to BuildResult
                //      If function returns a closure (has returnLambdaName),
                //      transfer to BuildResult.closureLambdaName so buildLocalVariable
                //      can register it in VariableInfo. Fixes: var a = factory("a", 10)
                //      then a() was returning i64 as number because closureLambdaName
                //      was not propagated from FunctionInfo.returnLambdaName.
                // ================================================================
                if (it != b_.functionTable_.end() && !it->second.returnLambdaName.empty())
                {
                    result.closureLambdaName = it->second.returnLambdaName;
                }

                // ================================================================
                // (AR) [Fix #099] نشر أنواع القنوات من معاملات الدالة إلى وسائط الاستدعاء
                //      عندما تُمرر قناة كوسيط لدالة تُرسل إليها (أرسل)، يُسجّل
                //      نوع العنصر في b_.channelTypeMap_ باسم المعامل (داخل الدالة).
                //      لكن عند الاستقبال (استقبل) من نفس القناة في النطاق الخارجي،
                //      الاسم مختلف (ق_تحيات vs ق) فلا يُعثر على النوع.
                //      الحل: عند الاستدعاء، ننشر b_.channelTypeMap_ من أسماء المعاملات
                //      إلى أسماء الوسائط في موقع الاستدعاء.
                // (EN) [Fix #099] Propagate channel types from function params to call-site args
                //      When a channel is passed to a function that sends to it,
                //      the element type is registered under the parameter name.
                //      But when receiving from the same channel in the outer scope,
                //      the variable name differs, so type lookup fails.
                //      Fix: at call site, propagate b_.channelTypeMap_ entries from
                //      parameter names to argument variable names.
                // ================================================================
                if (it != b_.functionTable_.end())
                {
                    auto &funcInfo = it->second;
                    for (size_t i = 0; i < funcInfo.parameters.size() && i < call->arguments.size(); i++)
                    {
                        auto ctIt = b_.channelTypeMap_.find(funcInfo.parameters[i].name);
                        if (ctIt != b_.channelTypeMap_.end())
                        {
                            // (AR) وُجد نوع قناة مُسجّل باسم المعامل — ننشره لاسم الوسيط
                            // (EN) Found channel type registered under param name — propagate to arg name
                            if (auto *argVar = dynamic_cast<const AST::VariableExpr *>(
                                    call->arguments[i].get()))
                            {
                                b_.channelTypeMap_[argVar->name] = ctIt->second;
                            }
                            if (Sad::Security::BoundsChecker::checkArrayIndex(i, argOperands.size()))
                            {
                                b_.channelTypeMap_[argOperands[i].name] = ctIt->second;
                            }
                        }
                    }
                }

                return result;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
