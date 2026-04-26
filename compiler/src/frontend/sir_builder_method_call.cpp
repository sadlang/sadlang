// ============================================================================
// sir_builder_method_call.cpp - بناء استدعاءات الطرق (methods) على الكائنات
// ============================================================================
// (AR) هذا الملف يحتوي على دالة buildMethodCall المسؤولة عن:
//   - اعتراض طرق التزامن (قنوات، أقفال، مستقبلات، مجموعات انتظار)
//   - اعتراض الطرق المضمنة (مصفوفات، نصوص، خرائط)
//   - الاستدعاء العام للطرق عبر SIR CALL
//
// (EN) This file contains buildMethodCall responsible for:
//   - Intercepting concurrency methods (channels, mutexes, futures, waitgroups)
//   - Intercepting builtin methods (arrays, strings, maps)
//   - General method call dispatch via SIR CALL
//
// (AR) تم فصل هذا الملف عن sir_builder_calls_objects.cpp وفق قاعدة CW-05
// (EN) Split from sir_builder_calls_objects.cpp per CW-05 rule
// ============================================================================

#include <string>
#include <cstdio>
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

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // ============================================================================
            // buildMethodCall - ???? ??????? ????? ??? ????
            // ============================================================================
            // ???? ??????? / Source: class_nodes.h:245
            // ??????? / Signature: BuildResult buildMethodCall(AST::MethodCallExpr* methodCallExpr);
            //
            // MethodCallExpr Members:
            // - object: std::unique_ptr<Expr> (line 247)
            // - methodName: std::string (line 248)
            // - arguments: std::vector<std::unique_ptr<Expr>> (line 249)
            // ============================================================================
            BuildResult SIRBuilder::buildMethodCall(AST::MethodCallExpr *methodCallExpr)
            {
                if (!methodCallExpr)
                {
                    return BuildResult();
                }
#ifndef NDEBUG
                std::cout << "[DEBUG] buildMethodCall: calling method '"
                          << methodCallExpr->methodName << "'" << std::endl;
#endif

                // ================================================================
                // (AR) ?????? ????: ??????? ???? ????? ??? ??? ?????
                //      ????: ????.??() � "????" ??? ??? ???? ?????
                //      ????? ??? ??? staticMethods_ ??????? ???? self
                // (EN) Early intercept: static method call via class name
                //      Example: Counter.increment() � "Counter" is class, not variable
                //      Detect via staticMethods_ and call without self
                // ================================================================
                if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(methodCallExpr->object.get()))
                {
                    std::string staticMethodKey = varExpr->name + "." + methodCallExpr->methodName;
                    if (staticMethods_.count(staticMethodKey) > 0)
                    {
                        // (AR) ???? ????? � ??????? ???? self
                        // (EN) Static method � call without self
                        std::string fullName = varExpr->name + "." + methodCallExpr->methodName;

                        // (AR) ???? ??????? (???? self)
                        // (EN) Build arguments (no self)
                        std::vector<SIROperand> args;
                        for (const auto &arg : methodCallExpr->arguments)
                        {
                            auto argResult = buildExpression(arg.get());
                            if (argResult.isConstant && !argResult.constantValue.empty())
                            {
                                if (argResult.type == SadTypeKind::String)
                                    args.push_back(SIROperand::ConstantString(argResult.constantValue));
                                else if (argResult.type == SadTypeKind::Float)
                                    args.push_back(SIROperand::ConstantF64(std::stod(argResult.constantValue)));
                                else
                                {
                                    try
                                    {
                                        args.push_back(SIROperand::ConstantI64(std::stoll(argResult.constantValue)));
                                    }
                                    catch (...)
                                    {
                                        args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                                    }
                                }
                            }
                            else
                            {
                                args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                            }
                        }

                        // (AR) ????? ??? ???????
                        // (EN) Determine return type
                        SadTypeKind returnType = SadTypeKind::Integer;
                        auto ftIt = functionTable_.find(staticMethodKey);
                        if (ftIt != functionTable_.end())
                        {
                            returnType = ftIt->second.returnType;
                        }

                        std::string resultReg = newTempRegister();
                        SIRInstruction callInst(SIROpcode::CALL);
                        callInst.result = SIROperand::Register(resultReg, returnType);
                        callInst.operands.push_back(SIROperand::Function(fullName));
                        for (const auto &a : args)
                            callInst.operands.push_back(a);
                        callInst.comment = "Static method call: " + staticMethodKey;

                        if (currentBlock_)
                            currentBlock_->addInstruction(callInst);

                        return BuildResult(resultReg, returnType);
                    }
                }

                // (AR) ?????? 1: ???? ????? ??????
                // (EN) Step 1: Build object expression
                auto objResult = buildExpression(methodCallExpr->object.get());

                // (AR) ?????? 2: ????? ??? ????? ?? ???????
                // (EN) Step 2: Determine class name from variable
                std::string className;

                // (AR) ????? ????? ??? ????? ?? VariableExpr
                // (EN) Try to find class name from VariableExpr
                if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr *>(methodCallExpr->object.get()))
                {
                    // (AR) ????? ?? ??????? ??????? ?? classInstanceTypes_
                    // (EN) Look up variable info in classInstanceTypes_
                    if (classInstanceTypes_.find(varExpr->name) != classInstanceTypes_.end())
                    {
                        className = classInstanceTypes_[varExpr->name];
                    }
                    else
                    {
                        VariableInfo *objVar = lookupVariable(varExpr->name);
                        if (objVar && !objVar->className.empty())
                        {
                            className = objVar->className;
                        }
                    }
                }

                // (AR) ??? '???' (this), ?????? currentClassName_
                // (EN) If 'this', use currentClassName_
                if (auto thisExpr = dynamic_cast<Sad::AST::ThisExpr *>(methodCallExpr->object.get()))
                {
                    className = currentClassName_;
                }

                // ================================================================
                // (AR) ?????? ????: ??? ??????? ????? ??????
                //      ??? ??? ?????? ???? (__channel__)? ????? ????????? ??????
                //      ??? ?????? SIR ???? ?????? ?????? ????? ?? ???? ??????
                //      ??? ?? ???? ??? ???? ????? ????? ????? ???? className
                // (EN) Early intercept: channel dot-syntax methods
                //      If object is a channel (__channel__), convert directly to SIR
                //      Must be before smart lookup block which would clear className
                // ================================================================
                // (AR) استدلال نوع الكائن عند غياب className (شائع مع معاملات الدوال)
                //      مثال: دالة f(ق) ثم ق.أرسل(...) حيث لا يوجد classInstanceTypes_ للمعامل.
                //      نحدد القناة من اسم الطريقة لتفعيل اعتراض ASYNC_CHANNEL_*.
                // (EN) Infer pseudo object class when className is missing (common for parameters).
                //      Example: f(ch) then ch.send(...), no classInstanceTypes_ entry for parameter.
                //      Classify as channel from method name to enable ASYNC_CHANNEL_* intercepts.
                if (className.empty())
                {
                    const std::string &methodName = methodCallExpr->methodName;
                    const bool isChannelMethod =
                        methodName == "\xD8\xA3\xD8\xB1\xD8\xB3\xD9\x84" || methodName == "\xD8\xA7\xD8\xB1\xD8\xB3\xD9\x84" || methodName == "send" ||
                        methodName == "\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84" || methodName == "receive" || methodName == "recv" ||
                        methodName == "\xD8\xA3\xD8\xBA\xD9\x84\xD9\x82" || methodName == "\xD8\xA7\xD8\xBA\xD9\x84\xD9\x82" || methodName == "close" ||
                        methodName == "\xD9\x85\xD8\xBA\xD9\x84\xD9\x82\xD8\xA9" || methodName == "isClosed" || methodName == "is_closed" ||
                        methodName == "\xD8\xA7\xD9\x84\xD8\xB3\xD8\xB9\xD8\xA9" || methodName == "capacity" ||
                        methodName == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD8\xA7\xD8\xB1\xD8\xB3\xD9\x84" || methodName == "try_send" ||
                        methodName == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84" || methodName == "try_recv" ||
                        methodName == "\xD8\xA3\xD8\xB1\xD8\xB3\xD9\x84_\xD8\xA8\xD9\x85\xD9\x87\xD9\x84\xD8\xA9" || methodName == "send_timeout" ||
                        methodName == "\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84_\xD8\xA8\xD9\x85\xD9\x87\xD9\x84\xD8\xA9" || methodName == "recv_timeout";

                    if (isChannelMethod)
                    {
                        className = "__channel__";
                    }
                }

                // (AR) …״­״§ˆ„״© ״·״±‚ ״§„‚†ˆ״§״× ״§„…״¶…†״© (״£״±״³„/״§״³״×‚״¨„/״£״÷„‚/...)

                // (EN) Try channel method calls (send/recv/close/...)

                if (className == "__channel__")
                {

                    auto chanResult = buildChannelMethodCall(methodCallExpr, objResult);

                    if (chanResult)
                        return *chanResult;
                }

                // (AR) …״­״§ˆ„״© ״·״±‚ ״§„‚„ (״§‚„/״§״×״­/...)

                // (EN) Try mutex method calls (lock/unlock/...)

                if (className == "__mutex__")
                {

                    auto mutResult = buildMutexMethodCall(methodCallExpr, objResult);

                    if (mutResult)
                        return *mutResult;
                }

                // (AR) …״­״§ˆ„״© ״·״±‚ ״§„…״³״×‚״¨„ (״¹‘†/״§״­״µ„/״¬״§‡״²)

                // (EN) Try future method calls (set/get/is_ready)

                if (className == "__future__")
                {

                    auto futResult = buildFutureMethodCall(methodCallExpr, objResult);

                    if (futResult)
                        return *futResult;
                }

                // (AR) …״­״§ˆ„״© ״·״±‚ …״¬…ˆ״¹״© ״§„״§†״×״¸״§״± (״£״¶/״£†‡/״§†״×״¸״±/״§„״¹״¯״§״¯)

                // (EN) Try waitgroup method calls (add/done/wait/count)

                if (className == "__waitgroup__")
                {

                    auto wgResult = buildWaitGroupMethodCall(methodCallExpr, objResult);

                    if (wgResult)
                        return *wgResult;
                }

                // (AR) [Fix #063-1] متغيرات لتتبع استنتاج className عبر smart lookup
                //      تُستخدم لاحقاً لتحديد الحاجة لـ OBJECT_CALL (توزيع vtable ديناميكي)
                // (EN) [Fix #063-1] Variables to track className inference via smart lookup
                //      Used later to determine if OBJECT_CALL (vtable dispatch) is needed
                bool classNameWasInferred = false;
                int methodClassCount = 0;

                // (AR) بحث ذكي عن الصنف: بحث في جميع الأصناف المعروفة عن الطريقة المطلوبة.
                //      أفضل من استخدام currentClassName_ الذي قد يكون الصنف الخاطئ.
                // (EN) Smart class lookup: search all known classes for matching method.
                //      Better than currentClassName_ fallback which may be the wrong class.
                {
                    std::string methodToFind = methodCallExpr->methodName;
                    // (AR) أولاً: تحقق أن الصنف الحالي/المعروف يملك الطريقة فعلاً
                    // (EN) First: check if currently found class actually has the method
                    if (!className.empty())
                    {
                        std::string candidate = className + "." + methodToFind;
                        if (functionTable_.find(candidate) == functionTable_.end())
                        {
                            // (AR) ????? ?????? ?? ????? ??? ??? ??????? � ???? ?? ????? ???????
                            //      ????: ???? ?? ???? ???() ? ???? ?? ??? (????) ? ???.???()
                            //      ???: ?? ?????? classInstanceTypes_ ??? ?????? ?? ???? ?? ??? ?????
                            // (EN) Current class doesn't have this method � search inheritance chain
                            //      e.g.: ???? doesn't have ???() ? search ??? (parent) ? ???.???()
                            //      Important: don't change classInstanceTypes_ � object is still child type
                            std::string searchClass = className;
                            bool foundInParent = false;
                            while (module_)
                            {
                                auto classInfo = module_->getClass(searchClass);
                                if (!classInfo || classInfo->parentClass.empty())
                                    break;
                                std::string parentCandidate = classInfo->parentClass + "." + methodToFind;
                                if (functionTable_.find(parentCandidate) != functionTable_.end())
                                {
                                    // (AR) ???? ??????? ?? ???? � ???????? ???? ????? ??? ??????
                                    // (EN) Found method in parent � use it without changing object type
                                    className = classInfo->parentClass;
                                    foundInParent = true;
                                    break;
                                }
                                searchClass = classInfo->parentClass;
                            }
                            if (!foundInParent)
                            {
                                // (AR) ?? ????? ?? ????? ??????? � ???? ?? ????? ???? (???? ????)
                                // (EN) Not found in inheritance chain � search other classes (legacy behavior)
                                className.clear();
                            }
                        }
                    }
                    if (className.empty())
                    {
                        // (AR) أولاً: عدّ الأصناف التي تملك هذه الطريقة
                        // (EN) First: count how many classes have this method
                        for (const auto &[fname, finfo] : functionTable_)
                        {
                            auto dotPos = fname.find('.');
                            if (dotPos != std::string::npos && fname.substr(dotPos + 1) == methodToFind)
                            {
                                methodClassCount++;
                                if (className.empty())
                                {
                                    className = fname.substr(0, dotPos);
                                }
                            }
                        }
                        if (!className.empty())
                        {
                            classNameWasInferred = true;
                            if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr *>(methodCallExpr->object.get()))
                            {
                                classInstanceTypes_[varExpr->name] = className;
                            }
                        }
                    }
                }

                // (AR) ????? ?? ??????? ?? ???? ??????
                // (EN) Look up method in function table
                std::string fullMethodName = className + "." + methodCallExpr->methodName;

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMethodCall: full method name = '" << fullMethodName << "'" << std::endl;
#endif

                // (AR) تحديد نوع الإرجاع
                // (EN) Determine return type
                SadTypeKind returnType = SadTypeKind::Void;
                if (functionTable_.find(fullMethodName) != functionTable_.end())
                {
                    returnType = functionTable_[fullMethodName].returnType;
                }

                // (AR) الخطوة 3: بناء المعاملات
                // (EN) Step 3: Build arguments
                std::vector<SIROperand> args;

                // (AR) ??????? ?????: self (???? ??????)
                // (EN) First argument: self (object pointer)
                args.push_back(SIROperand::Register(objResult.registerName, objResult.type));

                // (AR) ???? ?????????
                // (EN) Rest of arguments
                // (AR) ???? ??? ??????? ???????? ???????? ???? ???????
                //      ????? ???? ????????? ????? ??????? (?????/???/?????/???)
                // (EN) Track closure lambda name and return type
                //      Needed for higher-order array methods (map/filter/reduce/forEach)
                std::string firstClosureLambdaName;
                SadTypeKind firstClosureRetType = SadTypeKind::Void;
                for (const auto &arg : methodCallExpr->arguments)
                {
                    auto argResult = buildExpression(arg.get());
                    // (AR) ???? ??? ????? (?????) ?????
                    // (EN) Track first closure (lambda) built
                    if (!argResult.closureLambdaName.empty() && firstClosureLambdaName.empty())
                    {
                        firstClosureLambdaName = argResult.closureLambdaName;
                        auto lambdaIt = functionTable_.find(argResult.closureLambdaName);
                        if (lambdaIt != functionTable_.end())
                            firstClosureRetType = lambdaIt->second.returnType;
                    }
                    if (argResult.isConstant && !argResult.constantValue.empty())
                    {
                        switch (argResult.type)
                        {
                        case SadTypeKind::Integer:
                            args.push_back(SIROperand::ConstantI64(std::stoll(argResult.constantValue)));
                            break;
                        case SadTypeKind::Float:
                            args.push_back(SIROperand::ConstantF64(std::stod(argResult.constantValue)));
                            break;
                        case SadTypeKind::String:
                            args.push_back(SIROperand::ConstantString(argResult.constantValue));
                            break;
                        default:
                            args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                            break;
                        }
                    }
                    else
                    {
                        args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                    }
                }

                // ========================================================================
                // (AR) ?????? 3.5: ??? ????? ??????? ?????????
                //      ? ?? ???? ????? ??????? ??? ??? ?????? ????? ????? ????? ???? ?????
                //      ????: ??? ????? ????? ???? ???() � ??? ??????? ????? ????? ?? ARRAY_APPEND
                // (EN) Step 3.5: Check for builtin array methods
                //      ? Skip builtin checks if the object is a class instance with a matching method
                //      e.g.: class ????? has ???() method � should CALL class method, not ARRAY_APPEND
                // ========================================================================
                std::string methodName = methodCallExpr->methodName;

                // (AR) ???: ?? ??? ????? ??? ??????? ??? ??? ????? ??? ????? ???????
                // (EN) Check: is this a registered class method? If so, skip builtin checks
                bool isRegisteredClassMethod = (!className.empty() &&
                                                functionTable_.find(fullMethodName) != functionTable_.end());

                if (!isRegisteredClassMethod)
                {
                    // (AR) محاولة طرق المصفوفات الأساسية (أضف/حجم/أزل/فارغة/يحتوي/رتب/...)
                    // (EN) Try basic array methods (push/size/remove/empty/contains/sort/...)
                    auto arrBasicResult = buildArrayBasicMethodCall(objResult, methodName, args);
                    if (arrBasicResult)
                    {
                        // ================================================================
                        // (AR) نشر نوع عنصر المصفوفة عند استدعاء اضف()
                        //      عندما نضيف عنصراً لمصفوفة فارغة (elementType == Void)،
                        //      نُحدّث VariableInfo.elementType من نوع العنصر المُضاف.
                        //      هذا يسمح لحلقات foreach لاحقاً بمعرفة نوع العناصر.
                        //      بدون هذا: متغير نتائج = [] ثم نتائج.اضف(نص) →
                        //      foreach يفترض Integer → يطبع أرقام بدل نصوص
                        // (EN) Propagate array element type on append()
                        //      When appending to an empty-typed array (elementType == Void),
                        //      update VariableInfo.elementType from the appended element's type.
                        //      This lets subsequent foreach loops know the element type.
                        //      Without this: var arr = [] then arr.append(string) →
                        //      foreach assumes Integer → prints numbers instead of strings
                        // ================================================================
                        if (methodName == "\xD8\xA3\xD8\xB6\xD9\x81" || // أضف
                            methodName == "\xD8\xA7\xD8\xB6\xD9\x81" || // اضف
                            methodName == "push" || methodName == "append")
                        {
                            if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(
                                    methodCallExpr->object.get()))
                            {
                                VariableInfo *arrVar = lookupVariable(varExpr->name);
                                if (arrVar && arrVar->elementType == SadTypeKind::Void &&
                                    args.size() > 1 && args[1].dataType != SadTypeKind::Void)
                                {
                                    arrVar->elementType = args[1].dataType;
                                }
                            }
                        }
                        return *arrBasicResult;
                    }

                    // (AR) محاولة طرق المصفوفات العليا (خريطة/رشح/اختزل/لكل)
                    // (EN) Try higher-order array methods (map/filter/reduce/forEach)
                    auto arrHOResult = buildArrayHigherOrderMethodCall(
                        objResult, methodName, args, firstClosureLambdaName, firstClosureRetType);
                    if (arrHOResult)
                        return *arrHOResult;

                    // (AR) محاولة طرق النصوص المضمنة (قسم/استبدل/يبدأ_بـ/ينتهي_بـ/...)
                    // (EN) Try string builtin methods (split/replace/startsWith/endsWith/...)
                    auto strResult = buildStringBuiltinMethodCall(objResult, methodName, args);
                    if (strResult)
                        return *strResult;

                    // (AR) محاولة طرق الخرائط المضمنة (مفاتيح/قيم/حجم/فارغة/يحتوي)
                    // (EN) Try map builtin methods (keys/values/size/empty/contains)
                    auto mapResult = buildMapBuiltinMethodCall(objResult, methodName, args);
                    if (mapResult)
                        return *mapResult;
                } // (AR) نهاية if (!isRegisteredClassMethod)

                // (AR) ?????? 4: ??? ??? ???? ?????? ???? ADT
                //      ??? ???? ?????? ????? ?????? ???.????? ??? ????? ?????? __adt_ctor_???_?????
                //      ?????? ????? ??????? ????? self ?? ?????????
                // (EN) Step 4: Check if function is an ADT constructor
                //      If registered as ???.????? but actual name is __adt_ctor_???_?????
                //      use the real name and remove self from arguments
                std::string callTargetName = fullMethodName;
                bool isADTCtor = false;
                auto ftIt = functionTable_.find(fullMethodName);
                if (ftIt != functionTable_.end())
                {
                    const auto &fInfo = ftIt->second;
                    // (AR) ??? ????? ??????? ???? ?? __adt_ctor_ ??? ???? ADT
                    // (EN) If real name starts with __adt_ctor_ it's an ADT constructor
                    if (fInfo.name.find("__adt_ctor_") == 0)
                    {
                        callTargetName = fInfo.name;
                        isADTCtor = true;
                    }
                }

                // (AR) ?????? 5: ????? ?????? CALL
                // (EN) Step 5: Create CALL instruction
                std::string resultReg = newTempRegister();

                // (AR) ???: ?? ????????? ??? �???� ? ??? ??????? OBJECT_CALL ??????? ?????????
                //      ??? ??????? ??????? ?? ???? ????? ??? ??????? (???? ???????)
                //      ????: ???? ???.???() ? ???.???() ??? ?? ??? ??? vtable
                //             ???? ??? ?????? ?????2 ? ???????? ?????2.???() ?? ???.???()
                // (EN) Check: is call on `this`? ? use OBJECT_CALL for virtual dispatch
                //      Subclasses may override this method (polymorphism)
                //      e.g.: inside ???.???() ? ???.???() should go through vtable
                //             if object is ?????2 ? calls ?????2.???() not ???.???()
                bool isThisCall = dynamic_cast<Sad::AST::ThisExpr *>(methodCallExpr->object.get()) != nullptr;

                // (AR) نستخدم OBJECT_CALL عند الحاجة الفعلية لتعدد الأشكال:
                //      1) استدعاء this (قد يُعاد تعريفه في الأصناف الابنة)
                //      2) وجود صنف ابن يملك نفس اسم الدالة ويَرِث من className
                //      3) [Fix #063-1] وجود أصناف أخوات (siblings) تشترك في أب مشترك وتملك نفس الطريقة
                //         هذا ضروري عندما يكون className مستنتجاً من نوع عنصر المصفوفة
                //         مثال: [دائرة، مربع، مثلث] → ش.مساحة() حيث className="دائرة"
                //         مربع ومثلث لا يرثان من دائرة بل من شكل (أب مشترك)
                //         بدون هذا الإصلاح: جميع العناصر تستدعي دائرة.مساحة()
                //      غير ذلك نفضّل CALL المباشر للحفاظ على دقة نوع الإرجاع.
                // (EN) Use OBJECT_CALL when polymorphic dispatch is needed:
                //      1) this-call (can be overridden in subclasses)
                //      2) a subclass defines same method name and inherits from className
                //      3) [Fix #063-1] sibling classes sharing a common ancestor have the same method
                //         Needed when className is inferred from array element type
                //         e.g., [circle, square, triangle] → s.area() where className="circle"
                //         square/triangle don't inherit from circle but from shape (common parent)
                //         Without this: all elements call circle.area()
                //      Otherwise prefer direct CALL to preserve return-type fidelity.
                bool hasOverridingSubclassMethod = false;
                if (!className.empty())
                {
                    // (AR) الفحص الأول: أصناف ابنة ترث مباشرة من className
                    // (EN) Check 1: subclasses that inherit directly from className
                    for (const auto &[fname, _finfo] : functionTable_)
                    {
                        auto dotPos = fname.find('.');
                        if (dotPos == std::string::npos)
                            continue;

                        std::string candClass = fname.substr(0, dotPos);
                        std::string candMethod = fname.substr(dotPos + 1);
                        if (candMethod != methodCallExpr->methodName || candClass == className)
                            continue;

                        std::string walkClass = candClass;
                        while (module_)
                        {
                            auto ci = module_->getClass(walkClass);
                            if (!ci || ci->parentClass.empty())
                                break;
                            if (ci->parentClass == className)
                            {
                                hasOverridingSubclassMethod = true;
                                break;
                            }
                            walkClass = ci->parentClass;
                        }

                        if (hasOverridingSubclassMethod)
                            break;
                    }

                    // (AR) [Fix #063-1] الفحص الثاني: أصناف أخوات عبر سلسلة الوراثة
                    //      نبحث عن الأب الأعلى الذي يملك نفس الطريقة، ثم نتحقق
                    //      إن كان هناك أصناف أخرى ترث منه وتملك نفس الطريقة
                    // (EN) [Fix #063-1] Check 2: sibling classes via inheritance chain
                    //      Find the highest ancestor with the same method, then check
                    //      if other classes inherit from it and have the same method
                    if (!hasOverridingSubclassMethod && module_)
                    {
                        const std::string &methodNameToFind = methodCallExpr->methodName;

                        // (AR) ابحث عن الأب الأعلى الذي يملك الطريقة
                        // (EN) Find the highest ancestor with the method
                        std::string ancestor = className;
                        {
                            std::string walk = className;
                            while (true)
                            {
                                auto ci = module_->getClass(walk);
                                if (!ci || ci->parentClass.empty())
                                    break;
                                std::string parentKey = ci->parentClass + "." + methodNameToFind;
                                if (functionTable_.find(parentKey) != functionTable_.end())
                                {
                                    ancestor = ci->parentClass;
                                }
                                walk = ci->parentClass;
                            }
                        }

                        // (AR) إن وجدنا أباً مختلفاً عن className، نبحث عن أصناف أخوات
                        //      ترث من نفس الأب وتملك نفس الطريقة
                        // (EN) If we found an ancestor different from className, look for siblings
                        //      that inherit from the same ancestor and have the same method
                        if (ancestor != className)
                        {
                            for (const auto &[fname, _finfo2] : functionTable_)
                            {
                                auto dotPos = fname.find('.');
                                if (dotPos == std::string::npos)
                                    continue;
                                std::string candClass = fname.substr(0, dotPos);
                                std::string candMethod = fname.substr(dotPos + 1);
                                if (candMethod != methodNameToFind || candClass == className)
                                    continue;

                                // (AR) تحقق إن كان candClass يرث من ancestor
                                // (EN) Check if candClass inherits from ancestor
                                std::string walk = candClass;
                                while (module_)
                                {
                                    auto ci = module_->getClass(walk);
                                    if (!ci || ci->parentClass.empty())
                                        break;
                                    if (ci->parentClass == ancestor)
                                    {
                                        hasOverridingSubclassMethod = true;
                                        break;
                                    }
                                    walk = ci->parentClass;
                                }
                                if (hasOverridingSubclassMethod)
                                    break;
                            }
                        }
                    }
                }

                bool isObjectCall = isThisCall || hasOverridingSubclassMethod;

                // (AR) [Fix #063-1] إذا تم استنتاج className عبر smart lookup وعدة أصناف تملك نفس الطريقة،
                //      يجب استخدام OBJECT_CALL لتوزيع vtable الديناميكي وقت التشغيل.
                //      هذا ضروري عند تكرار حلقة forEach على مصفوفة كائنات من أصناف مختلفة:
                //      مثال: [دائرة، مربع، مثلث].مساحة() — كل كائن يحتاج استدعاء دالته الخاصة.
                //      بدون هذا الإصلاح: جميع الكائنات تستدعي أول صنف وُجد (مثلاً دائرة.مساحة).
                // (EN) [Fix #063-1] If className was inferred via smart lookup AND multiple classes
                //      have the same method, use OBJECT_CALL for runtime vtable dispatch.
                //      Essential for forEach over mixed-type object arrays:
                //      e.g., [circle, square, triangle].area() — each object needs its own method.
                //      Without this fix: all objects call the first class found (e.g., circle.area).
                if (classNameWasInferred && methodClassCount > 1)
                {
                    isObjectCall = true;
                }

                if (currentBlock_)
                {
                    if ((isThisCall || isObjectCall) && !isADTCtor)
                    {
                        // (AR) ??????? ????????? ??? vtable � OBJECT_CALL
                        // (EN) Virtual dispatch through vtable � OBJECT_CALL
                        SIRInstruction callInst;
                        callInst.opcode = SIROpcode::OBJECT_CALL;
                        callInst.result = SIROperand::Register(resultReg, returnType);
                        // operand[0]: self (???? ??????)
                        callInst.operands.push_back(args[0]);
                        // operand[1]: ??? ??????? (???? ???? ????? ?????)
                        callInst.operands.push_back(SIROperand::ConstantString(methodCallExpr->methodName));
                        // operand[2+]: ????????? ????????
                        for (size_t i = 1; i < args.size(); ++i)
                        {
                            callInst.operands.push_back(args[i]);
                        }
                        currentBlock_->addInstruction(callInst);
                    }
                    else
                    {
                        // (AR) ??????? ????? � CALL (?????? ??????)
                        // (EN) Direct call � CALL (original behavior)
                        SIRInstruction callInst;
                        callInst.opcode = SIROpcode::CALL;
                        callInst.result = SIROperand::Register(resultReg, returnType);
                        callInst.operands.push_back(SIROperand::Function(callTargetName));

                        // (AR) ?????? ADT ?? ???? self ?????? ??? � ????? args[0] (??? self)
                        // (EN) ADT constructors don't take self as first arg � skip args[0] (which is self)
                        size_t startIdx = isADTCtor ? 1 : 0;
                        for (size_t i = startIdx; i < args.size(); ++i)
                        {
                            callInst.operands.push_back(args[i]);
                        }
                        currentBlock_->addInstruction(callInst);
                    }
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMethodCall: result in register '" << resultReg << "'" << std::endl;
#endif

                // ================================================================
                // (AR) ??? ??? ????? ??????? (returnClassName) ??? BuildResult
                //      ??? ????? ??? ???? ??????? ??? ?????? ??????? ?? ????????? ???????.
                //      ???? ???: ?????? ????? ???? ????? ?? method call ????
                //      ????: ????? ? = ???.?????(5) ? ?.???_????? ????? ????? ?? ? ?? ??? "???"
                //      ???? ?? ADT constructors ?????? ??????? ???????
                // (EN) Propagate returnClassName to BuildResult
                //      Necessary so compiler knows the returned object's class type.
                //      Without this: field access on objects from method calls fails
                //      Example: var s = Shape.Circle(5) ? s.radius needs to know s is type "Shape"
                //      Works for ADT constructors and regular class methods
                // ================================================================
                BuildResult methodResult(resultReg, returnType);
                if (ftIt != functionTable_.end() && !ftIt->second.returnClassName.empty())
                {
                    methodResult.className = ftIt->second.returnClassName;
                    classInstanceTypes_[resultReg] = ftIt->second.returnClassName;
                }
                return methodResult;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
