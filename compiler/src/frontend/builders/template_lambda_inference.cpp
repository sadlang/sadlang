// ============================================================================
// sir_builder_lambda_inference.cpp — استدلال أنواع معاملات لامبدا
// (AR) مسح مواقع الاستدعاء واستنتاج أنواع معاملات اللامبدا تلقائياً
// (EN) Scan call sites and infer lambda parameter types automatically
// تم استخراج هذا الملف من sir_builder_type_inference.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "builders/template_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // (AR) جداول أسماء الطرق — لاستدلال أنواع معاملات اللامبدا
            // (EN) Method name tables — for lambda param type inference
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

            void TemplateBuilder::scanCallSitesInExpr(const Sad::AST::Expression *expr)
            {
                if (!expr)
                    return;

                // (AR) ??????? ???? � ?????? ????? ??????? ?????? b_.functionTable_
                // (EN) Function call � infer arg types and update b_.functionTable_
                if (auto *call = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                {
                    // (AR) ??????? ??? ??????
                    // (EN) Extract function name
                    std::string funcName;
                    if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(call->callee.get()))
                    {
                        funcName = varExpr->name;
                    }

                    if (!funcName.empty())
                    {
                        auto it = b_.functionTable_.find(funcName);

                        // ═══════════════════════════════════════════════════════════════
                        // (AR) إصلاح: إذا لم نجد الدالة، نتحقق إذا كان استدعاء باني صنف
                        //      بدون كلمة "جديد". في لغة ص، كائن_حي("حي") يُحلَّل كـ CallExpr
                        //      لكن الباني مسجّل كـ "كائن_حي.باني" في b_.functionTable_
                        //      بدون هذا: أنواع معاملات الباني لا تُحدَّث من call-site
                        //      مما يؤدي لبقاء المعاملات كـ Integer بدلاً من String
                        // (EN) Fix: If function not found, check if it's a class constructor
                        //      call without "new" keyword. In Sad, ClassName("arg") is parsed
                        //      as CallExpr but constructor is registered as "ClassName.باني"
                        //      Without this: constructor param types don't get updated from call-site
                        //      causing params to remain Integer instead of String
                        // ═══════════════════════════════════════════════════════════════
                        bool isImplicitCtorCall = false;
                        if (it == b_.functionTable_.end())
                        {
                            // (AR) إصلاح: بدلاً من b_.module_->getClass() (غير متاح في Phase 1.7)
                            //      نبحث مباشرة عن "اسم.باني" في b_.functionTable_
                            //      مسجّل في Phase 1.35 قبل Phase 1.7
                            // (EN) Fix: Instead of b_.module_->getClass() (unavailable in Phase 1.7)
                            //      look directly for "name.باني" in b_.functionTable_
                            //      registered in Phase 1.35 before Phase 1.7
                            std::string ctorName = funcName + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني
                            it = b_.functionTable_.find(ctorName);
                            isImplicitCtorCall = (it != b_.functionTable_.end());
                        }

                        if (it != b_.functionTable_.end())
                        {
                            auto &funcInfo = it->second;
                            // (AR) عند استدعاء الباني ضمنياً، المعامل الأول هو self → نزيح بـ 1
                            // (EN) For implicit constructor calls, first param is self → offset by 1
                            size_t paramOffset = isImplicitCtorCall ? 1 : 0;
                            for (size_t i = 0; i < call->arguments.size() && (i + paramOffset) < funcInfo.parameters.size(); i++)
                            {
                                SadTypeKind argType = inferExprType(call->arguments[i].get());
                                SadTypeKind &paramType = funcInfo.parameters[i + paramOffset].type;

                                // (AR) ??? ??? ??????? I64 (??????? ?? UNKNOWN) ??????? ??? ???? ???????
                                // (EN) If param is I64 (default from UNKNOWN) and arg is a more specific type
                                if (paramType == SadTypeKind::Integer && argType == SadTypeKind::String)
                                {
                                    paramType = SadTypeKind::String;
#ifdef SIR_BUILDER_DEBUG
                                    std::cerr << "[TYPE-INFER] " << funcName << " param[" << i
                                              << "] '" << funcInfo.parameters[i + paramOffset].name
                                              << "': I64 -> STRING" << std::endl;
#endif
                                }
                                else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Float)
                                {
                                    paramType = SadTypeKind::Float;
                                }
                                else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Boolean)
                                {
                                    paramType = SadTypeKind::Boolean;
                                }
                                // ═══════════════════════════════════════════════════════════
                                // (AR) إصلاح: استنتاج نوع المصفوفة + نوع عناصرها
                                //      عند تمرير مصفوفة كوسيط، نحدّث نوع المعامل إلى Array
                                //      ونفحص عناصر المصفوفة (إن كانت ArrayExpr) لتحديد elementType
                                //      بدون هذا: forEach على مصفوفة نصوص يعامل العناصر كأرقام
                                // (EN) Fix: Infer array type + element type
                                //      When array literal is passed, update param to Array
                                //      and inspect elements to determine elementType
                                //      Without this: forEach over string array treats elements as integers
                                // ═══════════════════════════════════════════════════════════
                                else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Array)
                                {
                                    paramType = SadTypeKind::Array;
                                    // (AR) فحص عناصر المصفوفة لتحديد نوع العنصر
                                    // (EN) Inspect array elements to determine element type
                                    if (auto *arrExpr = dynamic_cast<const Sad::AST::ArrayExpr *>(call->arguments[i].get()))
                                    {
                                        if (!arrExpr->elements.empty())
                                        {
                                            SadTypeKind firstElemType = inferExprType(arrExpr->elements[0].get());
                                            funcInfo.parameters[i + paramOffset].elementType = firstElemType;
                                        }
                                    }
                                }
                                // (AR) إصلاح جذري: عند تضارب الأنواع (مثلاً STRING ثم BOOL)
                                //      نبقي STRING كنوع عام — التحويل يحدث عند الاستدعاء
                                //      في sir_builder_calls.cpp عبر BOOL_TO_STRING/I64_TO_STRING
                                // (EN) Radical fix: if types conflict (e.g. STRING then BOOL)
                                //      keep STRING as common type — conversion happens at call site
                                //      in sir_builder_calls.cpp via BOOL_TO_STRING/I64_TO_STRING
                                else if (paramType == SadTypeKind::String && argType != SadTypeKind::String)
                                {
                                    // (AR) STRING ???? � ??????? ??? ??? ?????????
                                    // (EN) STRING stays � conversion happens at call site
#ifdef SIR_BUILDER_DEBUG
                                    std::cerr << "[TYPE-INFER] " << funcName << " param[" << i
                                              << "] '" << funcInfo.parameters[i + paramOffset].name
                                              << "': keeping STRING (call-site will convert "
                                              << static_cast<int>(argType) << ")" << std::endl;
#endif
                                }
                                else if (paramType != SadTypeKind::Integer && argType == SadTypeKind::String)
                                {
                                    // (AR) ????? ??? STRING � ???? ?? ????? ?????? ????????
                                    // (EN) Promote to STRING � string is the widest comparable type
                                    paramType = SadTypeKind::String;
#ifdef SIR_BUILDER_DEBUG
                                    std::cerr << "[TYPE-INFER] " << funcName << " param[" << i
                                              << "] '" << funcInfo.parameters[i + paramOffset].name
                                              << "': " << static_cast<int>(paramType)
                                              << " -> STRING (widened)" << std::endl;
#endif
                                }
                            }

                            // ================================================================
                            // (AR) ??????? 1.75: ??????? ????? ??????? ?????????
                            //      ????? ????? ???? (???? ?? b_.classInstanceTypes_) ?????
                            //      ?????? ??? ????? ?? b_.paramClassTypes_ ????????? ??????
                            //      ?? b_.buildFunction ??? ???? ??? ?????? + inferReturnTypeFromBody
                            // (EN) Phase 1.75: Infer class names for function parameters
                            //      When an object (tracked in b_.classInstanceTypes_) is passed as arg
                            //      register class name in b_.paramClassTypes_ for later use
                            //      in b_.buildFunction when building body + inferReturnTypeFromBody
                            // ================================================================
                            for (size_t i = 0; i < call->arguments.size() && (i + paramOffset) < funcInfo.parameters.size(); i++)
                            {
                                const auto &arg = call->arguments[i];
                                if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(arg.get()))
                                {
                                    // (AR) تحقق: هل هذا المتغير كائن مسجل في b_.classInstanceTypes_?
                                    // (EN) Check: is this variable an object tracked in b_.classInstanceTypes_?
                                    auto ciIt = b_.classInstanceTypes_.find(varExpr->name);
                                    if (ciIt != b_.classInstanceTypes_.end())
                                    {
                                        b_.paramClassTypes_[funcName][funcInfo.parameters[i + paramOffset].name] = ciIt->second;
                                    }
                                }
                            }
                        }
                    }

                    // (AR) ??? ??????? ????? (?? ????? ??? ????????? ???????)
                    // (EN) Scan arguments too (may contain nested calls)
                    for (const auto &arg : call->arguments)
                    {
                        scanCallSitesInExpr(arg.get());
                    }
                    return;
                }

                // ================================================================
                // (AR) ????? ???? (NewExpr) � ??????? ????? ????? ??????
                //      ??? CallExpr ??? ????? ?? "???.????" ???????? ????? ?? self
                // (EN) New expression (NewExpr) � infer constructor arg types
                //      Like CallExpr but name is "class.????" and first param is self
                // ================================================================
                if (auto *newExpr = dynamic_cast<const Sad::AST::NewExpr *>(expr))
                {
                    std::string ctorName = newExpr->className + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .????
                    auto it = b_.functionTable_.find(ctorName);
                    if (it != b_.functionTable_.end())
                    {
                        auto &funcInfo = it->second;
                        // params[0] = self, params[1..N] = user params
                        for (size_t i = 0; i < newExpr->arguments.size(); i++)
                        {
                            size_t paramIdx = i + 1; // +1 ????? self
                            if (paramIdx >= funcInfo.parameters.size())
                                break;

                            SadTypeKind argType = inferExprType(newExpr->arguments[i].get());
                            SadTypeKind &paramType = funcInfo.parameters[paramIdx].type;

                            if (paramType == SadTypeKind::Integer && argType == SadTypeKind::String)
                                paramType = SadTypeKind::String;
                            else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Float)
                                paramType = SadTypeKind::Float;
                            else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Boolean)
                                paramType = SadTypeKind::Boolean;
                            else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Array)
                                paramType = SadTypeKind::Array;
                        }
                    }

                    // (AR) ??? ????? NewExpr ?????
                    for (const auto &arg : newExpr->arguments)
                        scanCallSitesInExpr(arg.get());
                    return;
                }

                // ================================================================
                // (AR) ??????? ????? � MethodCallExpr
                //      ???????? ????? ??????? ?????? ??????? ???: ???.????("???")
                //      ????? ?? b_.functionTable_ ???? "???.?????"
                // (EN) Method call � MethodCallExpr
                //      Infer param types for static methods like: Class.method("arg")
                //      Name in b_.functionTable_ is "Class.method"
                // ================================================================
                if (auto *methodCall = dynamic_cast<const Sad::AST::MethodCallExpr *>(expr))
                {
                    std::string funcName;
                    if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(methodCall->object.get()))
                    {
                        funcName = varExpr->name + "." + methodCall->methodName;
                    }

                    if (!funcName.empty())
                    {
                        auto it = b_.functionTable_.find(funcName);
                        if (it != b_.functionTable_.end())
                        {
                            auto &funcInfo = it->second;

                            for (size_t i = 0; i < methodCall->arguments.size() && i < funcInfo.parameters.size(); i++)
                            {
                                SadTypeKind argType = inferExprType(methodCall->arguments[i].get());
                                SadTypeKind &paramType = funcInfo.parameters[i].type;

                                if (paramType == SadTypeKind::Integer && argType == SadTypeKind::String)
                                {
                                    paramType = SadTypeKind::String;
#ifdef SIR_BUILDER_DEBUG
                                    std::cerr << "[TYPE-INFER] " << funcName << " param[" << i
                                              << "] '" << funcInfo.parameters[i].name
                                              << "': I64 -> STRING (MethodCall)" << std::endl;
#endif
                                }
                                else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Float)
                                    paramType = SadTypeKind::Float;
                                else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Boolean)
                                    paramType = SadTypeKind::Boolean;
                                else if (paramType == SadTypeKind::String && argType != SadTypeKind::String)
                                {
                                    // (AR) STRING ???? � ??????? ??? ??? ?????????
                                }
                                else if (paramType != SadTypeKind::Integer && argType == SadTypeKind::String)
                                {
                                    paramType = SadTypeKind::String;
                                }
                            }
                        }
                    }

                    // (AR) ??? ??????? ??????? (?? ????? ??? ????????? ???????)
                    for (const auto &arg : methodCall->arguments)
                        scanCallSitesInExpr(arg.get());
                    scanCallSitesInExpr(methodCall->object.get());
                    return;
                }

                // (AR) ????? ???? ??? � ???? ?????? ????????
                // (EN) Member access � scan object recursively
                if (auto *memberAccess = dynamic_cast<const Sad::AST::MemberAccessExpr *>(expr))
                {
                    scanCallSitesInExpr(memberAccess->object.get());
                    return;
                }

                // (AR) ????? ?????
                // (EN) Binary expression
                if (auto *bin = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
                {
                    scanCallSitesInExpr(bin->left.get());
                    scanCallSitesInExpr(bin->right.get());
                    return;
                }

                // (AR) ????? ?????
                // (EN) Unary expression
                if (auto *unary = dynamic_cast<const Sad::AST::UnaryExpr *>(expr))
                {
                    scanCallSitesInExpr(unary->operand.get());
                    return;
                }
            }

            // ============================================================================
            // scanCallSitesInStmt - ??? ?????? ????? ????? ?? ????????? ??????
            // ============================================================================
            void TemplateBuilder::scanCallSitesInStmt(const Sad::AST::Statement *stmt)
            {
                if (!stmt)
                    return;

                // (AR) ???? ???
                // (EN) Block statement
                if (auto *block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                {
                    for (const auto &s : block->statements)
                    {
                        scanCallSitesInStmt(s.get());
                    }
                    return;
                }

                // (AR) ???? ????? (???? ????????? ?????? ????????)
                // (EN) Expression statement (includes standalone function calls)
                if (auto *exprStmt = dynamic_cast<const Sad::AST::ExprStmt *>(stmt))
                {
                    scanCallSitesInExpr(exprStmt->expression.get());
                    return;
                }

                // (AR) ????? ????? ?? ???????
                // (EN) Variable declaration with initializer
                if (auto *varDecl = dynamic_cast<const Sad::AST::VarDeclStmt *>(stmt))
                {
                    if (varDecl->initializer)
                    {
                        scanCallSitesInExpr(varDecl->initializer.get());
                    }
                    return;
                }

                // (AR) ???? ?????
                // (EN) If statement
                if (auto *ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                {
                    scanCallSitesInExpr(ifStmt->condition.get());
                    scanCallSitesInStmt(ifStmt->thenBranch.get());
                    if (ifStmt->elseBranch)
                        scanCallSitesInStmt(ifStmt->elseBranch.get());
                    return;
                }

                // (AR) ???? while
                // (EN) While loop
                if (auto *whileStmt = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                {
                    scanCallSitesInExpr(whileStmt->condition.get());
                    scanCallSitesInStmt(whileStmt->body.get());
                    return;
                }

                // (AR) ???? for-range
                // (EN) For-range loop
                if (auto *forStmt = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                {
                    scanCallSitesInExpr(forStmt->iterable.get());

                    // ================================================================
                    // (AR) إصلاح X08: استنتاج نوع متغير الحلقة من نوع عنصر المصفوفة
                    //      بدون هذا: لكل أ في ["نص1"، "نص2"] { دالة(أ) }
                    //                  inferExprType(أ) = Integer لأن أ غير مسجّل
                    //      مما يُبقي نوع معامل الدالة Integer بدلاً من String
                    // (EN) FIX X08: Infer loop variable type from iterable element type
                    //      Without this: for x in ["str1", "str2"] { func(x) }
                    //                    inferExprType(x) = Integer since x is not registered
                    //      This prevents param type from being updated to String
                    // ================================================================
                    SadTypeKind iterVarType = SadTypeKind::Integer; // افتراضي

                    // (AR) استنتاج نوع العنصر من ArrayExpr مباشرة
                    // (EN) Infer element type from direct ArrayExpr
                    if (auto *arrExpr = dynamic_cast<const Sad::AST::ArrayExpr *>(forStmt->iterable.get()))
                    {
                        if (!arrExpr->elements.empty())
                        {
                            SadTypeKind elemType = inferExprType(arrExpr->elements[0].get());
                            if (elemType != SadTypeKind::Integer) // Integer = افتراضي، يعني لم يُستنتج
                                iterVarType = elemType;
                        }
                    }
                    // (AR) أو من متغير عالمي مسجّل بـ elementType
                    // (EN) Or from a global variable registered with elementType
                    else if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(forStmt->iterable.get()))
                    {
                        for (auto scopeIt = b_.scopeStack_.rbegin(); scopeIt != b_.scopeStack_.rend(); ++scopeIt)
                        {
                            auto it = scopeIt->find(varExpr->name);
                            if (it != scopeIt->end())
                            {
                                if (it->second.elementType != SadTypeKind::Integer &&
                                    it->second.elementType != SadTypeKind::Void)
                                {
                                    iterVarType = it->second.elementType;
                                }
                                break;
                            }
                        }
                    }

                    // (AR) تسجيل متغير الحلقة في نطاق مؤقت أثناء المسح
                    // (EN) Register loop variable in a temporary scope during scanning
                    if (iterVarType != SadTypeKind::Integer) // فقط إذا استنتجنا نوعاً محدداً
                    {
                        b_.enterScope();
                        VariableInfo iterVarInfo;
                        iterVarInfo.name = forStmt->variable;
                        iterVarInfo.type = iterVarType;
                        iterVarInfo.registerName = "%" + forStmt->variable;
                        b_.addVariable(iterVarInfo);
                        scanCallSitesInStmt(forStmt->body.get());
                        b_.exitScope();
                    }
                    else
                    {
                        scanCallSitesInStmt(forStmt->body.get());
                    }
                    return;
                }

                // (AR) جملة return
                // (EN) Return statement
                if (auto *retStmt = dynamic_cast<const Sad::AST::ReturnStmt *>(stmt))
                {
                    if (retStmt->value)
                        scanCallSitesInExpr(retStmt->value.get());
                    return;
                }

                // (AR) جملة حاول/امسك/أخيراً — يجب مسح كتل الثلاثة
                //      إصلاح X08: بدون هذا كانت الاستدعاءات داخل try لا تُفحص
                // (EN) TryCatch statement — must scan all three blocks
                //      FIX X08: Without this, calls inside try were not scanned
                if (auto *tryStmt = dynamic_cast<const Sad::AST::TryStmt *>(stmt))
                {
                    if (tryStmt->tryBlock)
                        scanCallSitesInStmt(tryStmt->tryBlock.get());
                    for (const auto &clause : tryStmt->catchClauses)
                    {
                        if (clause.body)
                            scanCallSitesInStmt(clause.body.get());
                    }
                    if (tryStmt->finallyBlock)
                        scanCallSitesInStmt(tryStmt->finallyBlock.get());
                    return;
                }

                // (AR) جملة أطلق (goroutine) — يجب فحص تعبير الاستدعاء وكتلة الجسم
                //      بدون هذا: أطلق دالة("نص") لا يُستنتج نوع المعامل كـ String
                //      مما يؤدي لمعاملة المعامل كـ Integer داخل الدالة
                // (EN) GoStmt (goroutine) — must scan call expression and block body
                //      Without this: go func("text") doesn't infer param as String
                //      causing the parameter to be treated as Integer inside the function
                if (auto *goStmt = dynamic_cast<const Sad::AST::GoStmt *>(stmt))
                {
                    if (goStmt->expression)
                        scanCallSitesInExpr(goStmt->expression.get());
                    if (goStmt->blockBody)
                        scanCallSitesInStmt(goStmt->blockBody.get());
                    return;
                }
            }

            // ============================================================================

            // ============================================================================
            // inferLambdaParamFromExpr � ????? ????? ???????? ????? ??????? ???????
            // ============================================================================
            void TemplateBuilder::inferLambdaParamFromExpr(
                const ::Sad::AST::Expression *expr,
                const std::set<std::string> &paramNames,
                std::unordered_map<std::string, SadTypeKind> &result)
            {
                if (!expr)
                    return;

                // ????????????????????????????????????????????????????
                // (AR) ????? ????? � ?????? ?????? ??????
                //      ????: "????? " + ??? ? ??? ?? String
                //      ????: ? + 3.14 ? ? ?? Float
                // (EN) Binary expression � most common case
                // ????????????????????????????????????????????????????
                if (auto *bin = dynamic_cast<const ::Sad::AST::BinaryExpr *>(expr))
                {
                    inferLambdaParamFromExpr(bin->left.get(), paramNames, result);
                    inferLambdaParamFromExpr(bin->right.get(), paramNames, result);

                    // (AR) فحص الطرفين: إذا كان أحد الطرفين معامل لامدا والآخر له نوع معروف
                    //      نستنتج نوع المعامل من السياق
                    //      مثال: "مرحباً " + اسم → اسم يصبح String
                    //      مثال: س + 3.14 → س يصبح Float
                    // (EN) Check both sides: if one side is a lambda param and other has known type
                    //      infer param type from context
                    auto checkSide = [&](const ::Sad::AST::Expression *paramSide, const ::Sad::AST::Expression *otherSide)
                    {
                        auto *varA = dynamic_cast<const ::Sad::AST::VariableExpr *>(paramSide);
                        if (!varA || paramNames.find(varA->name) == paramNames.end())
                            return;
                        if (result.count(varA->name) > 0)
                            return;
                        SadTypeKind otherType = inferExprType(otherSide);
                        if (otherType == SadTypeKind::String)
                            result[varA->name] = SadTypeKind::String;
                        else if (otherType == SadTypeKind::Float)
                            result[varA->name] = SadTypeKind::Float;
                    };
                    checkSide(bin->left.get(), bin->right.get());
                    checkSide(bin->right.get(), bin->left.get());
                    // (AR) أولوية الاستنتاج: عمليات المقارنة العددية (>، <، >=، <=) تُلغي String
                    //      مثال: إذا (عمر >= 30) → عمر = Integer حتى لو كان مستنتجاً كـ String
                    //      من جملة concat سابقة مثل ("نص" + عمر)
                    //      الحالة المطلوبة: lambda(اسم، عمر) { ارجع "نص"+عمر; إذا(عمر>=30)... }
                    //      اسم = String (من concat)، عمر = Integer (من المقارنة — أولوية أعلى)
                    // (EN) Inference priority: numeric comparison ops override String inference
                    //      e.g.: if (age >= 30) → age = Integer even if previously String
                    //      from a concat like ("text" + age)
                    {
                        using TT = Sad::Lexer::TokenType;
                        if (bin->op == TT::OP_GREATER || bin->op == TT::OP_LESS ||
                            bin->op == TT::OP_GREATER_EQUAL || bin->op == TT::OP_LESS_EQUAL)
                        {
                            auto numericOverride = [&](const ::Sad::AST::Expression *ps, const ::Sad::AST::Expression *os)
                            {
                                auto *v = dynamic_cast<const ::Sad::AST::VariableExpr *>(ps);
                                if (!v || paramNames.find(v->name) == paramNames.end())
                                    return;
                                SadTypeKind ot = inferExprType(os);
                                if (ot == SadTypeKind::Integer || ot == SadTypeKind::Float)
                                    result[v->name] = ot; // (AR) يُلغي String لصالح النوع العددي
                            };
                            numericOverride(bin->left.get(), bin->right.get());
                            numericOverride(bin->right.get(), bin->left.get());
                        }
                    }
                    return;
                }

                // (AR) ??????? ????
                if (auto *call = dynamic_cast<const ::Sad::AST::CallExpr *>(expr))
                {
                    for (const auto &arg : call->arguments)
                        inferLambdaParamFromExpr(arg.get(), paramNames, result);
                    inferLambdaParamFromExpr(call->callee.get(), paramNames, result);

                    std::string funcName;
                    if (auto *varExpr = dynamic_cast<const ::Sad::AST::VariableExpr *>(call->callee.get()))
                        funcName = varExpr->name;

                    if (!funcName.empty())
                    {
                        auto funcIt = b_.functionTable_.find(funcName);
                        if (funcIt != b_.functionTable_.end())
                        {
                            const auto &funcInfo = funcIt->second;
                            for (size_t i = 0; i < call->arguments.size() && i < funcInfo.parameters.size(); i++)
                            {
                                auto *argVar = dynamic_cast<const ::Sad::AST::VariableExpr *>(call->arguments[i].get());
                                if (!argVar || paramNames.find(argVar->name) == paramNames.end())
                                    continue;
                                if (result.count(argVar->name) > 0)
                                    continue;

                                SadTypeKind targetType = funcInfo.parameters[i].type;
                                if (targetType != SadTypeKind::Integer)
                                    result[argVar->name] = targetType;
                            }
                        }
                    }
                    return;
                }

                // (AR) ??????? ????? � ???? ??? ??????
                if (auto *methodCall = dynamic_cast<const ::Sad::AST::MethodCallExpr *>(expr))
                {
                    inferLambdaParamFromExpr(methodCall->object.get(), paramNames, result);
                    for (const auto &arg : methodCall->arguments)
                        inferLambdaParamFromExpr(arg.get(), paramNames, result);

                    auto *objVar = dynamic_cast<const ::Sad::AST::VariableExpr *>(methodCall->object.get());
                    if (objVar && paramNames.count(objVar->name) > 0 && result.count(objVar->name) == 0)
                    {
                        if (kStringMethodNames.count(methodCall->methodName) > 0)
                            result[objVar->name] = SadTypeKind::String;
                        else if (kArrayMethodNames.count(methodCall->methodName) > 0)
                            result[objVar->name] = SadTypeKind::Array;
                    }
                    return;
                }

                // (AR) ????? ?????
                if (auto *unary = dynamic_cast<const ::Sad::AST::UnaryExpr *>(expr))
                {
                    inferLambdaParamFromExpr(unary->operand.get(), paramNames, result);
                    return;
                }
                // (AR) ????? ???? ?????
                if (auto *tern = dynamic_cast<const ::Sad::AST::TernaryExpr *>(expr))
                {
                    inferLambdaParamFromExpr(tern->condition.get(), paramNames, result);
                    inferLambdaParamFromExpr(tern->trueExpr.get(), paramNames, result);
                    inferLambdaParamFromExpr(tern->falseExpr.get(), paramNames, result);
                    return;
                }
                // (AR) ???? ????
                if (auto *idx = dynamic_cast<const ::Sad::AST::IndexExpr *>(expr))
                {
                    inferLambdaParamFromExpr(idx->object.get(), paramNames, result);
                    inferLambdaParamFromExpr(idx->index.get(), paramNames, result);
                    return;
                }
                // (AR) ???? ???
                if (auto *mem = dynamic_cast<const ::Sad::AST::MemberAccessExpr *>(expr))
                {
                    inferLambdaParamFromExpr(mem->object.get(), paramNames, result);
                    return;
                }
                // (AR) ?????? ?????
                if (auto *arrLit = dynamic_cast<const ::Sad::AST::ArrayExpr *>(expr))
                {
                    for (const auto &el : arrLit->elements)
                        inferLambdaParamFromExpr(el.get(), paramNames, result);
                    return;
                }
                // (AR) ????? ??????? � ??? ?????? ?????? ?? ????
            }

            // ============================================================================
            // inferLambdaParamFromStmt � ????? ???? ???????? ????? ??????? ???????
            // ============================================================================
            void TemplateBuilder::inferLambdaParamFromStmt(
                const ::Sad::AST::Statement *stmt,
                const std::set<std::string> &paramNames,
                std::unordered_map<std::string, SadTypeKind> &result)
            {
                if (!stmt)
                    return;

                if (auto *block = dynamic_cast<const ::Sad::AST::BlockStmt *>(stmt))
                {
                    for (const auto &s : block->statements)
                        inferLambdaParamFromStmt(s.get(), paramNames, result);
                    return;
                }
                if (auto *exprStmt = dynamic_cast<const ::Sad::AST::ExprStmt *>(stmt))
                {
                    if (exprStmt->expression)
                        inferLambdaParamFromExpr(exprStmt->expression.get(), paramNames, result);
                    return;
                }
                if (auto *retStmt = dynamic_cast<const ::Sad::AST::ReturnStmt *>(stmt))
                {
                    if (retStmt->value)
                        inferLambdaParamFromExpr(retStmt->value.get(), paramNames, result);
                    return;
                }
                if (auto *varDecl = dynamic_cast<const ::Sad::AST::VarDeclStmt *>(stmt))
                {
                    if (varDecl->initializer)
                        inferLambdaParamFromExpr(varDecl->initializer.get(), paramNames, result);
                    return;
                }
                if (auto *ifStmt = dynamic_cast<const ::Sad::AST::IfStmt *>(stmt))
                {
                    inferLambdaParamFromExpr(ifStmt->condition.get(), paramNames, result);
                    inferLambdaParamFromStmt(ifStmt->thenBranch.get(), paramNames, result);
                    if (ifStmt->elseBranch)
                        inferLambdaParamFromStmt(ifStmt->elseBranch.get(), paramNames, result);
                    return;
                }
                if (auto *whileStmt = dynamic_cast<const ::Sad::AST::WhileStmt *>(stmt))
                {
                    inferLambdaParamFromExpr(whileStmt->condition.get(), paramNames, result);
                    inferLambdaParamFromStmt(whileStmt->body.get(), paramNames, result);
                    return;
                }
                if (auto *forStmt = dynamic_cast<const ::Sad::AST::ForRangeStmt *>(stmt))
                {
                    inferLambdaParamFromExpr(forStmt->iterable.get(), paramNames, result);
                    inferLambdaParamFromStmt(forStmt->body.get(), paramNames, result);
                    return;
                }
            }

            // ============================================================================
            // b_.inferLambdaParamTypes � ??????? ????? ??????? ??????? ?? ?????
            // ============================================================================
            std::unordered_map<std::string, SadTypeKind> SIRBuilder::inferLambdaParamTypes(
                ::Sad::AST::LambdaExpr *lambdaExpr,
                const std::set<std::string> &paramNames)
            {
                std::unordered_map<std::string, SadTypeKind> result;

                // (AR) ?????? 1: ??? ????? AST ???????
                for (const auto &param : lambdaExpr->parameters)
                {
                    if (param.type != Data::DataType::UNKNOWN)
                        result[param.name] = astTypeToSIRType(param.type);
                }
                if (result.size() == lambdaExpr->parameters.size())
                    return result;

                // (AR) ?????? 2: ????? ??? ???????
                if (lambdaExpr->body)
                    inferLambdaParamFromExpr(lambdaExpr->body.get(), paramNames, result);
                if (lambdaExpr->blockBody)
                    inferLambdaParamFromStmt(lambdaExpr->blockBody.get(), paramNames, result);

#ifndef NDEBUG
                for (const auto &kv : result)
                {
                    std::cout << "[DEBUG] Lambda param '" << kv.first << "' inferred type: "
                              << static_cast<int>(kv.second) << std::endl;
                }
#endif

                return result;
            }

            // ============================================================================
            // inferParamTypesFromCallSites - ??????? 1.7: ??????? ????? ?????????
            // ============================================================================
            // (AR) ???? ???????? ?????? ????? ?? ????? ????????? ??????
            //      ????? ????????? ?? b_.functionTable_ ????? ???? ????? I64
            //      (?? DataType::UNKNOWN) ??????? ?????? ???? ???????
            // (EN) Scans the entire program for call sites and updates parameter
            //      types in b_.functionTable_ when type is I64 (from DataType::UNKNOWN)
            //      and actual argument is more specific
            // ============================================================================
            void TemplateBuilder::inferParamTypesFromCallSites(AST::ProgramNode *program)
            {
                if (!program)
                    return;

                // (AR) ??? ?? ????? ?? ???????? � ??? ???? ???????? ???? ??????
                // (EN) Scan all statements � multiple passes for transitive inference
                for (int pass = 0; pass < 3; pass++)
                {
                    for (const auto &stmt : *program)
                    {
                        if (!stmt)
                            continue;

                        // (AR) ??? ????? ??????
                        // (EN) Scan function bodies
                        AST::FunctionDecl *funcDecl = nullptr;
                        if (auto fd = dynamic_cast<Sad::AST::FunctionDecl *>(stmt.get()))
                        {
                            funcDecl = fd;
                        }
                        else if (auto exportDecl = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                        {
                            if (exportDecl->declaration)
                            {
                                funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(exportDecl->declaration.get());
                            }
                        }
                        else if (auto exportStmt = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                        {
                            if (exportStmt->declaration)
                            {
                                funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(exportStmt->declaration.get());
                            }
                        }

                        if (funcDecl && funcDecl->body)
                        {
                            b_.currentScanFuncName_ = funcDecl->name;
                            scanCallSitesInStmt(funcDecl->body.get());
                            b_.currentScanFuncName_.clear();
                        }

                        // (AR) ??? ????? ????????? ?? ??????? ??????
                        // (EN) Scan top-level executable statements
                        scanCallSitesInStmt(stmt.get());
                    }
                }
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
