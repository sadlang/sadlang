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

#include "sir_builder.h"
#include "builders/call_builder.h"
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
            BuildResult CallBuilder::buildMethodCall(AST::MethodCallExpr *methodCallExpr)
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
                //      ????: ????.??() — "????" ??? ??? ???? ?????
                //      ????? ??? ??? b_.staticMethods_ ??????? ???? self
                // (EN) Early intercept: static method call via class name
                //      Example: Counter.increment() — "Counter" is class, not variable
                //      Detect via b_.staticMethods_ and call without self
                // ================================================================
                if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(methodCallExpr->object.get()))
                {
                    std::string staticMethodKey = varExpr->name + "." + methodCallExpr->methodName;
                    if (b_.staticMethods_.count(staticMethodKey) > 0)
                    {
                        // (AR) ???? ????? — ??????? ???? self
                        // (EN) Static method — call without self
                        std::string fullName = varExpr->name + "." + methodCallExpr->methodName;

                        // (AR) ???? ??????? (???? self)
                        // (EN) Build arguments (no self)
                        std::vector<SIROperand> args;
                        for (const auto &arg : methodCallExpr->arguments)
                        {
                            auto argResult = b_.buildExpression(arg.get());
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
                        auto ftIt = b_.functionTable_.find(staticMethodKey);
                        if (ftIt != b_.functionTable_.end())
                        {
                            returnType = ftIt->second.returnType;
                        }

                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction callInst(SIROpcode::CALL);
                        callInst.result = SIROperand::Register(resultReg, returnType);
                        callInst.operands.push_back(SIROperand::Function(fullName));
                        for (const auto &a : args)
                            callInst.operands.push_back(a);
                        callInst.comment = "Static method call: " + staticMethodKey;

                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(callInst);

                        return BuildResult(resultReg, returnType);
                    }
                }

                // (AR) ?????? 1: ???? ????? ??????
                // (EN) Step 1: Build object expression
                auto objResult = b_.buildExpression(methodCallExpr->object.get());

                // (AR) ?????? 2: ????? ??? ????? ?? ???????
                // (EN) Step 2: Determine class name from variable
                std::string className;

                // (AR) ????? ????? ??? ????? ?? VariableExpr
                // (EN) Try to find class name from VariableExpr
                if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr *>(methodCallExpr->object.get()))
                {
                    // (AR) ????? ?? ??????? ??????? ?? b_.classInstanceTypes_
                    // (EN) Look up variable info in b_.classInstanceTypes_
                    if (b_.classInstanceTypes_.find(varExpr->name) != b_.classInstanceTypes_.end())
                    {
                        className = b_.classInstanceTypes_[varExpr->name];
                    }
                    else
                    {
                        VariableInfo *objVar = b_.lookupVariable(varExpr->name);
                        if (objVar && !objVar->className.empty())
                        {
                            className = objVar->className;
                        }
                    }
                }

                // (AR) ??? '???' (this), ?????? b_.currentClassName_
                // (EN) If 'this', use b_.currentClassName_
                if (auto thisExpr = dynamic_cast<Sad::AST::ThisExpr *>(methodCallExpr->object.get()))
                {
                    className = b_.currentClassName_;
                }

                // (AR) «الأساس.طريقة()» — استدعاء طريقة الأب (ISSUE-019): الكائن هو self (buildExpression
                //      لـSuperExpr يُرجع self)، لكن نوجّه اسم الصنف إلى أب currentClassName_ فتُستدعى
                //      طريقة الأب لا المتجاوَزة. يطابق سلوك المفسّر بعد إصلاحه.
                // (EN) «الأساس.method()» — super method call (ISSUE-019): object is self (buildExpression
                //      of SuperExpr returns self), but redirect the class name to the parent of
                //      currentClassName_ so the parent's (non-overridden) method runs. Mirrors the
                //      fixed interpreter behavior.
                if (dynamic_cast<Sad::AST::SuperExpr *>(methodCallExpr->object.get()))
                {
                    auto curCls = b_.module_ ? b_.module_->getClass(b_.currentClassName_) : nullptr;
                    if (curCls && !curCls->parentClass.empty())
                    {
                        className = curCls->parentClass;
                    }
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
                //      مثال: دالة f(ق) ثم ق.أرسل(...) حيث لا يوجد b_.classInstanceTypes_ للمعامل.
                //      نحدد القناة من اسم الطريقة لتفعيل اعتراض ASYNC_CHANNEL_*.
                // (EN) Infer pseudo object class when className is missing (common for parameters).
                //      Example: f(ch) then ch.send(...), no b_.classInstanceTypes_ entry for parameter.
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

                // (AR) محاولة طرق القنوات المضمنة (أرسل/استقبل/أغلق/...)

                // (EN) Try channel method calls (send/recv/close/...)

                if (className == "__channel__")
                {

                    auto chanResult = b_.buildChannelMethodCall(methodCallExpr, objResult);

                    if (chanResult)
                        return *chanResult;
                }

                // (AR) محاولة طرق القفل (اقفل/افتح/...)

                // (EN) Try mutex method calls (lock/unlock/...)

                if (className == "__mutex__")
                {

                    auto mutResult = b_.buildMutexMethodCall(methodCallExpr, objResult);

                    if (mutResult)
                        return *mutResult;
                }

                // (AR) محاولة طرق المستقبل (عيّن/احصل/جاهز)

                // (EN) Try future method calls (set/get/is_ready)

                if (className == "__future__")
                {

                    auto futResult = b_.buildFutureMethodCall(methodCallExpr, objResult);

                    if (futResult)
                        return *futResult;
                }

                // (AR) محاولة طرق مجموعة الانتظار (أضف/أنهي/انتظر/العداد)

                // (EN) Try waitgroup method calls (add/done/wait/count)

                if (className == "__waitgroup__")
                {

                    auto wgResult = b_.buildWaitGroupMethodCall(methodCallExpr, objResult);

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
                //      أفضل من استخدام b_.currentClassName_ الذي قد يكون الصنف الخاطئ.
                // (EN) Smart class lookup: search all known classes for matching method.
                //      Better than b_.currentClassName_ fallback which may be the wrong class.
                {
                    std::string methodToFind = methodCallExpr->methodName;
                    // (AR) أولاً: تحقق أن الصنف الحالي/المعروف يملك الطريقة فعلاً
                    // (EN) First: check if currently found class actually has the method
                    if (!className.empty())
                    {
                        std::string candidate = className + "." + methodToFind;
                        if (b_.functionTable_.find(candidate) == b_.functionTable_.end())
                        {
                            // (AR) ????? ?????? ?? ????? ??? ??? ??????? — ???? ?? ????? ???????
                            //      ????: ???? ?? ???? ???() ? ???? ?? ??? (????) ? ???.???()
                            //      ???: ?? ?????? b_.classInstanceTypes_ ??? ?????? ?? ???? ?? ??? ?????
                            // (EN) Current class doesn't have this method — search inheritance chain
                            //      e.g.: ???? doesn't have ???() ? search ??? (parent) ? ???.???()
                            //      Important: don't change b_.classInstanceTypes_ — object is still child type
                            std::string searchClass = className;
                            bool foundInParent = false;
                            while (b_.module_)
                            {
                                auto classInfo = b_.module_->getClass(searchClass);
                                if (!classInfo || classInfo->parentClass.empty())
                                    break;
                                std::string parentCandidate = classInfo->parentClass + "." + methodToFind;
                                if (b_.functionTable_.find(parentCandidate) != b_.functionTable_.end())
                                {
                                    // (AR) ???? ??????? ?? ???? — ???????? ???? ????? ??? ??????
                                    // (EN) Found method in parent — use it without changing object type
                                    className = classInfo->parentClass;
                                    foundInParent = true;
                                    break;
                                }
                                searchClass = classInfo->parentClass;
                            }
                            if (!foundInParent)
                            {
                                // (AR) ?? ????? ?? ????? ??????? — ???? ?? ????? ???? (???? ????)
                                // (EN) Not found in inheritance chain — search other classes (legacy behavior)
                                className.clear();
                            }
                        }
                    }
                    if (className.empty())
                    {
                        // (AR) أولاً: عدّ الأصناف التي تملك هذه الطريقة
                        // (EN) First: count how many classes have this method
                        for (const auto &[fname, finfo] : b_.functionTable_)
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
                                b_.classInstanceTypes_[varExpr->name] = className;
                            }
                        }
                    }
                }

                // (AR) البحث عن الطريقة في جدول الدوال
                // (EN) Look up method in function table
                std::string fullMethodName = className + "." + methodCallExpr->methodName;

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMethodCall: full method name = '" << fullMethodName << "'" << std::endl;
#endif

                // (AR) تحديد نوع الإرجاع
                // (EN) Determine return type
                SadTypeKind returnType = SadTypeKind::Void;
                if (b_.functionTable_.find(fullMethodName) != b_.functionTable_.end())
                {
                    returnType = b_.functionTable_[fullMethodName].returnType;
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
                    auto argResult = b_.buildExpression(arg.get());
                    // (AR) ???? ??? ????? (?????) ?????
                    // (EN) Track first closure (lambda) built
                    if (!argResult.closureLambdaName.empty() && firstClosureLambdaName.empty())
                    {
                        firstClosureLambdaName = argResult.closureLambdaName;
                        auto lambdaIt = b_.functionTable_.find(argResult.closureLambdaName);
                        if (lambdaIt != b_.functionTable_.end())
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
                //      ????: ??? ????? ????? ???? ???() — ??? ??????? ????? ????? ?? ARRAY_APPEND
                // (EN) Step 3.5: Check for builtin array methods
                //      ? Skip builtin checks if the object is a class instance with a matching method
                //      e.g.: class ????? has ???() method — should CALL class method, not ARRAY_APPEND
                // ========================================================================
                std::string methodName = methodCallExpr->methodName;

                // (AR) ???: ?? ??? ????? ??? ??????? ??? ??? ????? ??? ????? ???????
                // (EN) Check: is this a registered class method? If so, skip builtin checks
                bool isRegisteredClassMethod = (!className.empty() &&
                                                b_.functionTable_.find(fullMethodName) != b_.functionTable_.end());

                if (!isRegisteredClassMethod)
                {
                    // (AR) محاولة طرق المصفوفات الأساسية (أضف/حجم/أزل/فارغة/يحتوي/رتب/...)
                    // (EN) Try basic array methods (push/size/remove/empty/contains/sort/...)
                    auto arrBasicResult = b_.buildArrayBasicMethodCall(objResult, methodName, args);
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
                                VariableInfo *arrVar = b_.lookupVariable(varExpr->name);
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
                    auto arrHOResult = b_.buildArrayHigherOrderMethodCall(
                        objResult, methodName, args, firstClosureLambdaName, firstClosureRetType);
                    if (arrHOResult)
                        return *arrHOResult;

                    // (AR) محاولة طرق النصوص المضمنة (قسم/استبدل/يبدأ_بـ/ينتهي_بـ/...)
                    // (EN) Try string builtin methods (split/replace/startsWith/endsWith/...)
                    auto strResult = b_.buildStringBuiltinMethodCall(objResult, methodName, args);
                    if (strResult)
                        return *strResult;

                    // (AR) محاولة طرق الخرائط المضمنة (مفاتيح/قيم/حجم/فارغة/يحتوي)
                    // (EN) Try map builtin methods (keys/values/size/empty/contains)
                    auto mapResult = b_.buildMapBuiltinMethodCall(objResult, methodName, args);
                    if (mapResult)
                        return *mapResult;

                    // ════════════════════════════════════════════════════════════════
                    // (AR) م.أ-3 (RFC sadlang-rfcs#10 / إغلاق P0-3): معدّل واجهة انسيابيّ.
                    //      نداء طريقة على مقبض عنصر واجهة (Pointer) تعذّر استنتاج صنفٍ له
                    //      وليس طريقة مصفوفة/نص/خريطة ⇒ يُعامَل بدلالة WidgetBuilder في
                    //      المفسّر (ui_widget_method_call.cpp): يُطبَّق المعدّل كأثر جانبيّ
                    //      ويُعاد العنصرُ نفسه ليتسلسل ويبقى «كائنًا».
                    //
                    //      حاسم: قبل هذا كان المسار يسقط إلى نداءٍ بإرجاع VOID لرمزٍ غير
                    //      موجود، فإذا أُسنِد ناتج السلسلة إلى متغيّر استدعت الخلفية
                    //      getNullValue(void) ⇒ انهيار LLVM (UNREACHABLE). إعادةُ مقبض
                    //      العنصر (Pointer) تُزيل الانهيار وتجعل نوع(السلسلة)=«كائن».
                    //
                    //      الحارس className.empty() يقصُر هذا على المقابض الواجهيّة (التي
                    //      لا صنفَ مستخدمٍ لها)؛ مثيلاتُ الأصناف يُستنتَج صنفُها فلا تمرّ هنا.
                    //
                    //      أثرُ المعدّل مُطبَّقٌ فعليًّا في المخرَج المرسوم عبر ABI زمن تشغيلٍ
                    //      عامّ (SET_PROP_*/ADD_EVENT/ANIM_*/PROP_JOIN_*/ADD_CHILD) يحاكي
                    //      setIRProperty/addIREvent/addChild في المفسّر — انظر الفروع أدناه.
                    // (EN) Phase A-3: fluent UI modifier. A method on a widget handle
                    //      (Pointer) with no resolvable user class, and not an
                    //      array/string/map method, follows interpreter WidgetBuilder
                    //      semantics: apply the modifier and RETURN THE WIDGET so it
                    //      chains and stays an object. Returning the handle removes the
                    //      getNullValue(void) crash that the prior VOID assumption caused
                    //      on assignment. The modifier effect IS emitted into the compiled
                    //      rendered path via a generic runtime ABI (SET_PROP_*/ADD_EVENT/
                    //      ANIM_*/PROP_JOIN_*/ADD_CHILD) mirroring the interpreter — see below.
                    // ════════════════════════════════════════════════════════════════
                    if (className.empty() && objResult.type == SadTypeKind::Pointer)
                    {
                        // ════════════════════════════════════════════════════════════
                        // (AR) م-أ3ر: أثر المعدّل الانسيابيّ في المخرَج المرسوم.
                        //      L1: خاصّيّة عامّة بالاسم (نظير setIRProperty في المفسّر):
                        //          اسم الطريقة = اسم الخاصيّة، والوسيط = القيمة.
                        //      L2: عند_*/on_* = حدث ⇒ تسجيلُ IREvent على العقدة المشتركة
                        //          (نظير addIREvent) وربطُ ردّ النداء (المُغلِّف) للإرسال.
                        //      L3: سلاسل التحريك (حرّك/مدة/…) عبر آلة حالة على IRNode.
                        //      الأبناء (ابن/أبناء) ⇒ ADD_CHILD على العقدة المشتركة؛ ومتعدّد
                        //      وسائط الخصائص (>1) ⇒ مُجمِّع وقت تشغيل (PROP_JOIN).
                        // (EN) Phase A-3: apply the fluent modifier in the rendered output.
                        //      L1: generic named property (mirrors setIRProperty).
                        //      L2: عند_*/on_* events register an IREvent on the shared node
                        //          (mirrors addIREvent) and bind the closure for dispatch.
                        //      L3: animation chains (حرّك/مدة/…) via a state machine on IRNode.
                        //      Children (ابن/أبناء) ⇒ ADD_CHILD on the shared node; >1-arg
                        //      properties ⇒ a runtime accumulator (PROP_JOIN).
                        // ════════════════════════════════════════════════════════════
                        const std::string &m = methodName;
                        SIROperand handleOp =
                            SIROperand::Register(objResult.registerName, SadTypeKind::Pointer);
                        const size_t numModifierArgs = args.empty() ? 0 : args.size() - 1; // args[0]=العنصر

                        // (AR) عند_*/on_* = حدث (L2): يُسجَّل على العقدة ويُربَط ردّ النداء.
                        const bool isEvent =
                            (m.rfind("\xd8\xb9\xd9\x86\xd8\xaf_", 0) == 0) || // عند_
                            (m.rfind("on_", 0) == 0);
                        // (AR) معدّلات التحريك (L3): سلسلة حالة فوق IRNode تحاكي
                        //      WidgetBuilder. كلٌّ يُصدِر رمزًا مخصّصًا (begin/مدة/منحنى/…).
                        const bool isAnimBegin =
                            m == "\xd8\xad\xd8\xb1\xd9\x91\xd9\x83" || m == "animate" ||
                            m == "\xd8\xaa\xd8\xad\xd8\xb1\xd9\x8a\xd9\x83" || m == "\xd8\xad\xd8\xb1\xd9\x83";
                        const bool isAnimDuration = m == "\xd9\x85\xd8\xaf\xd8\xa9" || m == "duration";
                        const bool isAnimEasing =
                            m == "\xd9\x85\xd9\x86\xd8\xad\xd9\x86\xd9\x89" || m == "easing" || m == "\xd9\x85\xd9\x86\xd8\xad\xd9\x86\xd8\xa7";
                        const bool isAnimDelay = m == "\xd8\xaa\xd8\xa3\xd8\xae\xd9\x8a\xd8\xb1" || m == "delay";
                        const bool isAnimRepeat = m == "\xd8\xaa\xd9\x83\xd8\xb1\xd8\xa7\xd8\xb1" || m == "repeat";
                        const bool isAnimAutoReverse =
                            m == "\xd8\xb9\xd9\x83\xd8\xb3_\xd8\xaa\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x8a" || m == "autoReverse" || m == "auto_reverse";
                        const bool isAnim = isAnimBegin || isAnimDuration || isAnimEasing ||
                                            isAnimDelay || isAnimRepeat || isAnimAutoReverse;
                        // (AR) ابن/أبناء ⇒ إضافةُ أطفالٍ للعقدة المشتركة (ADD_CHILD أدناه).
                        const bool isChild =
                            m == "\xd8\xa7\xd8\xa8\xd9\x86" || m == "child" ||
                            m == "\xd8\xa3\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1" || m == "children";

                        if (isEvent)
                        {
                            // (AR) L2: نُسجِّل الحدث على العقدة المشتركة (نظير addIREvent:
                            //      اسم الطريقة ⇒ IREventType عبر stringToIREventType في
                            //      القلب، فالتطابق دقيق) ونحفظ ردّ النداء (المُغلِّف) +
                            //      بياناته لإرسالٍ مستقبليّ (لا مُرسِل في وقت التشغيل بعد).
                            //      حاسم للتكافؤ: لا نُسجّل حدثًا بلا معالِج — نظير المفسّر
                            //      (ui_widget_method_call.cpp:88: if (!args.empty()))؛
                            //      وإلّا اختلف getEvents() بين المحرّكين. args[0]=العنصر،
                            //      args[1]=ردّ النداء، args[2]=بيانات اختياريّة.
                            // (EN) L2: register the event on the shared node (method name →
                            //      IREventType via core stringToIREventType ⇒ exact parity)
                            //      and store the closure + data for FUTURE dispatch (no
                            //      runtime dispatcher yet). Parity-critical: register NO
                            //      event when there is no handler — mirrors the interpreter
                            //      (line 88), else getEvents() diverges between engines.
                            if (args.size() > 1)
                            {
                                SIRInstruction inst(SIROpcode::BUILTIN_UI_ADD_EVENT);
                                inst.operands.push_back(handleOp);
                                inst.operands.push_back(SIROperand::ConstantString(m)); // اسم الحدث
                                inst.operands.push_back(args[1]);      // ردّ النداء (مُغلِّف)
                                if (args.size() > 2)
                                    inst.operands.push_back(args[2]);  // بيانات المستخدم (اختياريّة)
                                if (b_.currentBlock_)
                                    b_.currentBlock_->instructions.push_back(inst);
                            }
                        }
                        else if (isAnim)
                        {
                            // ════════════════════════════════════════════════════════
                            // (AR) L3: سلسلة تحريك انسيابيّة. نحاكي آلة حالة WidgetBuilder
                            //      في المفسّر عبر دوال وقت تشغيلٍ تعمل على نفس IRNode:
                            //        حرّك(أنواع) ⇒ sad_anim_begin (يبدأ مجموعة؛ يدعم
                            //          المركّب بالفاصلة كالمفسّر)؛
                            //        مدة/تأخير ⇒ عشريّ، تكرار ⇒ صحيح، منحنى ⇒ نصّ،
                            //        عكس_تلقائي ⇒ منطقيّ (بلا وسيط ⇒ true)،
                            //      وكلّها تُطبَّق على المجموعة النشطة (نظير
                            //      applyToActiveAnimations). الحالة محفوظة على العنصر بين
                            //      النداءات؛ ومعدّلٌ غير تحريكيّ يُنهي السلسلة (في setter).
                            // (EN) L3: fluent animation chain mirroring the interpreter's
                            //      WidgetBuilder state machine over the same IRNode.
                            // ════════════════════════════════════════════════════════
                            if (isAnimBegin)
                            {
                                // (AR) نجمع أسماء الأنواع من الوسائط النصّيّة الثابتة في سلسلة
                                //      مفصولة بفواصل (يفكّها وقت التشغيل ويبدأ مجموعة مركّبة
                                //      عند تعدّدها) — نظير جمع المفسّر للوسائط وتقسيمها.
                                // ⚠ قيدٌ موثَّق (تباعد عن المفسّر): نقتصر على الوسائط الثابتة
                                //   النصّيّة؛ وسيطُ نوعٍ متغيّر (سجلّ) يُسقَط هنا فيقع وقت
                                //   التشغيل على الاحتياطيّ "fadeIn"، بينما المفسّر يقيّمه بـ
                                //   toString. أسماء التحريك ثوابت حرفيّة في العمليّ؛ دعمُ
                                //   السجلّات يحتاج مُجمِّع وقت تشغيل (كمسار الخصائص متعدّدة
                                //   الوسائط) وهو شريحة لاحقة.
                                std::string typesCsv;
                                for (size_t i = 1; i < args.size(); ++i)
                                {
                                    if (args[i].type == SIROperandType::CONSTANT &&
                                        args[i].dataType == SadTypeKind::String)
                                    {
                                        if (!typesCsv.empty()) typesCsv += ",";
                                        typesCsv += args[i].name;
                                    }
                                }
                                SIRInstruction inst(SIROpcode::BUILTIN_UI_ANIM_BEGIN);
                                inst.operands.push_back(handleOp);
                                inst.operands.push_back(SIROperand::ConstantString(typesCsv));
                                if (b_.currentBlock_)
                                    b_.currentBlock_->instructions.push_back(inst);
                            }
                            else if (numModifierArgs >= 1 || isAnimAutoReverse)
                            {
                                // (AR) معدّل ضبطٍ على المجموعة النشطة. يحتاج وسيطًا، عدا
                                //      عكس_تلقائي الذي يفترض true بلا وسيط (نظير المفسّر).
                                //      مدة/منحنى/… بلا وسيط: لا نُصدِر شيئًا (المفسّر يتجاهلها).
                                SIROpcode op = isAnimDuration ? SIROpcode::BUILTIN_UI_ANIM_DURATION
                                             : isAnimEasing   ? SIROpcode::BUILTIN_UI_ANIM_EASING
                                             : isAnimDelay    ? SIROpcode::BUILTIN_UI_ANIM_DELAY
                                             : isAnimRepeat   ? SIROpcode::BUILTIN_UI_ANIM_REPEAT
                                                              : SIROpcode::BUILTIN_UI_ANIM_AUTO_REVERSE;
                                SIRInstruction inst(op);
                                inst.operands.push_back(handleOp);
                                if (numModifierArgs >= 1)
                                    inst.operands.push_back(args[1]);
                                else
                                    inst.operands.push_back(SIROperand::ConstantI64(1)); // عكس_تلقائي() ⇒ true
                                if (b_.currentBlock_)
                                    b_.currentBlock_->instructions.push_back(inst);
                            }
                        }
                        else if (isChild)
                        {
                            // ════════════════════════════════════════════════════════
                            // (AR) ابن/أبناء الانسيابيّة ⇒ ADD_CHILD على العقدة المشتركة
                            //      (نظير WidgetBuilder::addChild في المفسّر). «ابن» يضيف
                            //      الأوّل فقط؛ «أبناء» يضيف الكلّ — مطابقةً لـ
                            //      ui_widget_method_call.cpp:50-82. args[0]=الأب، args[1..]=الأبناء.
                            //      يعيد المقبض نفسه (أدناه) فتتسلسل السلسلة كالمفسّر.
                            // (EN) Fluent ابن/أبناء → ADD_CHILD on the shared node (mirrors
                            //      WidgetBuilder::addChild). «ابن» adds only the first;
                            //      «أبناء» adds all — matching the interpreter.
                            // ════════════════════════════════════════════════════════
                            // (AR) ملاحظة تكافؤ: المفسّر يحرس نوع كلّ طفل
                            //      (isWidgetBuilder) ويتخطّى غير العناصر صمتًا؛ هنا نعتمد
                            //      على أنّ نظام الأنواع لا يُنتج مقبض Pointer لغير عنصرٍ في
                            //      هذا المسار (الوسائط مقابضُ عناصر)، وsad_add_child يحرس
                            //      nullptr. تباعدٌ نظريّ حدّيّ فقط لو مُرِّر مقبضٌ غير عنصر.
                            const bool isSingleChild =
                                (m == "\xd8\xa7\xd8\xa8\xd9\x86" || m == "child");
                            const size_t lim =
                                isSingleChild ? std::min<size_t>(args.size(), 2) : args.size();
                            for (size_t i = 1; i < lim; ++i)
                            {
                                SIRInstruction inst(SIROpcode::BUILTIN_UI_ADD_CHILD);
                                inst.operands.push_back(handleOp);  // الأب
                                inst.operands.push_back(args[i]);   // الطفل
                                if (b_.currentBlock_)
                                    b_.currentBlock_->instructions.push_back(inst);
                            }
                        }
                        else if (numModifierArgs <= 1)
                        {
                            // (AR) .م() بلا وسيط ⇒ منطقيّ true. نستعمل ConstantI64(1)
                            //      (لا ConstantBool) لصراحة النوع: يُنتِج ثابتًا صحيحًا
                            //      يقبله مسار emit الدفاعيّ مباشرةً بلا التباس. (ConstantBool
                            //      سيعمل أيضًا لأنّ SIROperand اتّحادٌ يشارك boolValue تخزينَ
                            //      intValue، لكنّ I64 أوضح دلالةً وأضمن للنوع.)
                            SIROpcode op = SIROpcode::BUILTIN_UI_SET_PROP_BOOL;
                            SIROperand valueOp = SIROperand::ConstantI64(1);
                            if (numModifierArgs == 1)
                            {
                                // (AR) args[1] مبنيّ مسبقًا كثابت/سجلّ (الأسطر ~394–415)؛
                                //      نُمرّره كما هو ونختار الرمز بحسب نوعه الفعليّ (dataType).
                                //      نُفرِد الصحيح عن العشريّ ليُخزَّن int64_t كما في المفسّر
                                //      (widget_builder.cpp: toInt64 ⇒ متغاير int64_t لا double).
                                const SIROperand &val = args[1];
                                switch (val.dataType)
                                {
                                case SadTypeKind::String:
                                    op = SIROpcode::BUILTIN_UI_SET_PROP_STR;  break;
                                case SadTypeKind::Boolean:
                                    op = SIROpcode::BUILTIN_UI_SET_PROP_BOOL; break;
                                case SadTypeKind::Integer:
                                    op = SIROpcode::BUILTIN_UI_SET_PROP_INT;  break;
                                case SadTypeKind::Float:
                                    op = SIROpcode::BUILTIN_UI_SET_PROP_NUM;  break;
                                default:
                                    op = SIROpcode::BUILTIN_UI_SET_PROP_STR;  break;
                                }
                                valueOp = val;
                            }
                            SIRInstruction inst(op);
                            inst.operands.push_back(handleOp);
                            inst.operands.push_back(SIROperand::ConstantString(m)); // اسم الخاصيّة
                            inst.operands.push_back(valueOp);
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(inst);
                        }
                        else // numModifierArgs > 1 (isChild محسوم أعلاه)
                        {
                            // ════════════════════════════════════════════════════════
                            // (AR) خاصّيّة متعدّدة الوسائط ⇒ قيمٌ مفصولة بفواصل (نظير
                            //      المفسّر ui_widget_method_call.cpp:239-249: يجمع
                            //      args[i].toString() بفواصل ويضبطها خاصّيّةً نصّيّة).
                            //      نمرّ عبر مُجمِّعٍ في وقت التشغيل (مصدر تنسيقٍ واحد يطابق
                            //      Value::toString) فيصحّ حتى مع وسائط السجلّات لا الثوابت:
                            //        لكلّ وسيط: PROP_JOIN_ADD (يُنسَّق ويُضاف للمجمِّع)؛
                            //        ثمّ PROP_JOIN_COMMIT (يدمج بفواصل ⇒ setProperty نصّيّ).
                            // (EN) Multi-arg property → comma-joined string (mirrors the
                            //      interpreter). Routed through a runtime accumulator (one
                            //      formatting source matching Value::toString) so it is
                            //      faithful even for register args, not just constants.
                            // ════════════════════════════════════════════════════════
                            for (size_t i = 1; i < args.size(); ++i)
                            {
                                SIRInstruction add(SIROpcode::BUILTIN_UI_PROP_JOIN_ADD);
                                add.operands.push_back(handleOp);
                                add.operands.push_back(args[i]);
                                if (b_.currentBlock_)
                                    b_.currentBlock_->instructions.push_back(add);
                            }
                            SIRInstruction commit(SIROpcode::BUILTIN_UI_PROP_JOIN_COMMIT);
                            commit.operands.push_back(handleOp);
                            commit.operands.push_back(SIROperand::ConstantString(m)); // اسم الخاصيّة
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(commit);
                        }

                        // (AR) نُعيد المقبض نفسه (Pointer، بلا صنف) ليبقى «كائنًا» ويتسلسل.
                        // (EN) Return the same handle so the chain continues and stays an object.
                        return BuildResult(objResult.registerName, SadTypeKind::Pointer);
                    }
                } // (AR) نهاية if (!isRegisteredClassMethod)

                // (AR) ?????? 4: ??? ??? ???? ?????? ???? ADT
                //      ??? ???? ?????? ????? ?????? ???.????? ??? ????? ?????? __adt_ctor_???_?????
                //      ?????? ????? ??????? ????? self ?? ?????????
                // (EN) Step 4: Check if function is an ADT constructor
                //      If registered as ???.????? but actual name is __adt_ctor_???_?????
                //      use the real name and remove self from arguments
                std::string callTargetName = fullMethodName;
                bool isADTCtor = false;
                auto ftIt = b_.functionTable_.find(fullMethodName);
                if (ftIt != b_.functionTable_.end())
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
                std::string resultReg = b_.newTempRegister();

                // (AR) ???: ?? ????????? ??? «???» ? ??? ??????? OBJECT_CALL ??????? ?????????
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
                    for (const auto &[fname, _finfo] : b_.functionTable_)
                    {
                        auto dotPos = fname.find('.');
                        if (dotPos == std::string::npos)
                            continue;

                        std::string candClass = fname.substr(0, dotPos);
                        std::string candMethod = fname.substr(dotPos + 1);
                        if (candMethod != methodCallExpr->methodName || candClass == className)
                            continue;

                        std::string walkClass = candClass;
                        while (b_.module_)
                        {
                            auto ci = b_.module_->getClass(walkClass);
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
                    if (!hasOverridingSubclassMethod && b_.module_)
                    {
                        const std::string &methodNameToFind = methodCallExpr->methodName;

                        // (AR) ابحث عن الأب الأعلى الذي يملك الطريقة
                        // (EN) Find the highest ancestor with the method
                        std::string ancestor = className;
                        {
                            std::string walk = className;
                            while (true)
                            {
                                auto ci = b_.module_->getClass(walk);
                                if (!ci || ci->parentClass.empty())
                                    break;
                                std::string parentKey = ci->parentClass + "." + methodNameToFind;
                                if (b_.functionTable_.find(parentKey) != b_.functionTable_.end())
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
                            for (const auto &[fname, _finfo2] : b_.functionTable_)
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
                                while (b_.module_)
                                {
                                    auto ci = b_.module_->getClass(walk);
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

                // (AR) استدعاء الأساس «الأساس.طريقة()» غير افتراضيّ عمدًا (ISSUE-019): يجب نداءٌ مباشرٌ
                //      لتطبيق الأب، لا توزيع vtable الذي يعود لطريقة الابن المتجاوَزة (الكائن self من نوع
                //      الابن) ⇒ تكرار لانهائيّ وانهيار. نُجبر النداء المباشر لـ«اسم_الأب.الطريقة».
                // (EN) Super call «الأساس.method()» is intentionally non-virtual (ISSUE-019): it must be a
                //      direct call to the parent's implementation, not vtable dispatch which would resolve
                //      back to the child's override (self is the child type) ⇒ infinite recursion + crash.
                //      Force a direct CALL to «parentClass.method».
                if (dynamic_cast<Sad::AST::SuperExpr *>(methodCallExpr->object.get()))
                {
                    isObjectCall = false;
                }

                if (b_.currentBlock_)
                {
                    if ((isThisCall || isObjectCall) && !isADTCtor)
                    {
                        // (AR) ??????? ????????? ??? vtable — OBJECT_CALL
                        // (EN) Virtual dispatch through vtable — OBJECT_CALL
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
                        b_.currentBlock_->addInstruction(callInst);
                    }
                    else
                    {
                        // (AR) إصدار تعليمة CALL لباني الأب
                        // (EN) Emit CALL instruction for parent constructor
                        SIRInstruction callInst;
                        callInst.opcode = SIROpcode::CALL;
                        callInst.result = SIROperand::Register(resultReg, returnType);
                        callInst.operands.push_back(SIROperand::Function(callTargetName));

                        // (AR) ?????? ADT ?? ???? self ?????? ??? — ????? args[0] (??? self)
                        // (EN) ADT constructors don't take self as first arg — skip args[0] (which is self)
                        size_t startIdx = isADTCtor ? 1 : 0;
                        for (size_t i = startIdx; i < args.size(); ++i)
                        {
                            callInst.operands.push_back(args[i]);
                        }
                        b_.currentBlock_->addInstruction(callInst);
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
                if (ftIt != b_.functionTable_.end() && !ftIt->second.returnClassName.empty())
                {
                    methodResult.className = ftIt->second.returnClassName;
                    b_.classInstanceTypes_[resultReg] = ftIt->second.returnClassName;
                }
                return methodResult;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
