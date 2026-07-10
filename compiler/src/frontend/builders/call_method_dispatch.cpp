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
#include "sad_ui/ui_modifiers.h" // (AR) أسماء معدّلات SadUI (مُولَّد من language-truth) — لا literals
#include "error_manager.h" // (AR) buildBilingualMessage من كتالوج الأخطاء (RUN_METHOD_NOT_FOUND)
#include "error_catalog.h" // (AR) RenderContext (حاملُ placeholders)
#include "error_codes.h"   // (AR) ErrorCode::RUN_METHOD_NOT_FOUND
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
            // buildMethodCall - بناء استدعاء طريقة على كائن
            // ============================================================================
            // مصدر التعريف / Source: class_nodes.h:245
            // التوقيع / Signature: BuildResult buildMethodCall(AST::MethodCallExpr* methodCallExpr);
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
                // (AR) اعتراض مبكر: استدعاء طريقة ساكنة عبر اسم الصنف
                //      مثال: عداد.زد() — "عداد" اسم صنف وليس متغيراً
                //      نكشفها عبر b_.staticMethods_ ونستدعي بدون self
                // (EN) Early intercept: static method call via class name
                //      Example: Counter.increment() — "Counter" is class, not variable
                //      Detect via b_.staticMethods_ and call without self
                // ================================================================
                if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(methodCallExpr->object.get()))
                {
                    std::string staticMethodKey = varExpr->name + "." + methodCallExpr->methodName;
                    if (b_.staticMethods_.count(staticMethodKey) > 0)
                    {
                        // (AR) طريقة ساكنة — استدعاء بدون self
                        // (EN) Static method — call without self
                        std::string fullName = varExpr->name + "." + methodCallExpr->methodName;

                        // (AR) بناء الوسائط (بدون self)
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

                        // (AR) تحديد نوع الإرجاع
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

                // ================================================================
                // (AR) اعتراض مبكر: بانِي متغيّر تعداد جبريّ ببيانات عبر اسم التعداد
                //      مثال: شكل.دائرة(5) — "شكل" اسم تعداد لا متغيّر؛ يُبنى عبر الدالة
                //      المُولَّدة __adt_ctor_شكل_دائرة (بلا self). نعترضه هنا قبل
                //      buildExpression على الكائن الذي يفشل بـ«Undefined variable شكل»
                //      (المسار العادي يعالج بانِي ADT عند الخطوة 4، لكن بعد فوات بناء
                //      الكائن الذي يسجّل خطأً زائفًا يُفشِل الترجمة).
                // (EN) Early intercept: ADT data-variant constructor via enum name.
                //      Example: شكل.دائرة(5) — "شكل" is an enum name, not a variable; it
                //      is built via the generated __adt_ctor_... function (no self). We
                //      intercept before buildExpression on the object, which would fail
                //      with "Undefined variable" and abort the build even though the
                //      later ADT-ctor path (Step 4) would emit the correct call.
                // ================================================================
                if (auto *enumVarExpr = dynamic_cast<Sad::AST::VariableExpr *>(methodCallExpr->object.get()))
                {
                    if (b_.adtEnumTable_.find(enumVarExpr->name) != b_.adtEnumTable_.end())
                    {
                        std::string ctorKey = enumVarExpr->name + "." + methodCallExpr->methodName;
                        auto ctorIt = b_.functionTable_.find(ctorKey);
                        if (ctorIt != b_.functionTable_.end() &&
                            ctorIt->second.name.find(Sad::Compiler::kAdtCtorPrefix) == 0)
                        {
                            // (AR) بناء الوسائط (بلا self) — نظير اعتراض الطريقة الساكنة
                            // (EN) Build arguments (no self) — mirrors the static-method intercept
                            std::vector<SIROperand> ctorArgs;
                            for (const auto &arg : methodCallExpr->arguments)
                            {
                                auto argResult = b_.buildExpression(arg.get());
                                if (argResult.isConstant && !argResult.constantValue.empty())
                                {
                                    if (argResult.type == SadTypeKind::String)
                                        ctorArgs.push_back(SIROperand::ConstantString(argResult.constantValue));
                                    else if (argResult.type == SadTypeKind::Float)
                                        ctorArgs.push_back(SIROperand::ConstantF64(std::stod(argResult.constantValue)));
                                    else
                                    {
                                        try
                                        {
                                            ctorArgs.push_back(SIROperand::ConstantI64(std::stoll(argResult.constantValue)));
                                        }
                                        catch (...)
                                        {
                                            ctorArgs.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                                        }
                                    }
                                }
                                else
                                {
                                    ctorArgs.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                                }
                            }

                            // (AR) ISSUE-076 (المسار أ′): سجّل أنواع حقول الحمولة من أنواع
                            //      وسائط الباني في adtEnumTable_ (الجدول العالميّ فيعبر حدود
                            //      الدوال). يُمكّن استخراج الحمولة بنوعها الحقيقيّ (عشريّ/نصّ/
                            //      منطقيّ) بدل تصليب Integer المطموس ⇒ يطابق النموذج الديناميّ
                            //      للمفسّر على كلّ المستهلكين (حساب/مقارنة/طباعة). **تعارض:** لو
                            //      سُجِّل نوعٌ مختلفٌ سابقًا لنفس الحقل (نفس الحالة أُنشئت بنوعين)
                            //      ⇒ تراجُع آمن إلى Unknown (يسلك مسار Integer المطموس القديم بلا
                            //      انحدار) بدل التزام نوعٍ قد يكذب في موقعٍ آخر.
                            // (EN) ISSUE-076 (path A′): register payload field types from the
                            //      constructor argument types into adtEnumTable_ (global table ⇒
                            //      crosses function boundaries). Enables extracting the payload
                            //      with its real type (float/string/bool) instead of the erased
                            //      hardcoded Integer ⇒ matches the interpreter's dynamic model on
                            //      every consumer (arith/compare/print). **Conflict:** if a
                            //      different type was registered before for the same field (same
                            //      variant built with two types) ⇒ safe fallback to Unknown (uses
                            //      the old erased-Integer path, no regression) rather than commit
                            //      to a type that may lie at another site.
                            {
                                auto &adtInfo = b_.adtEnumTable_[enumVarExpr->name];
                                for (auto &variant : adtInfo.variants)
                                {
                                    if (variant.name != methodCallExpr->methodName)
                                        continue;
                                    if (variant.fieldTypes.size() < ctorArgs.size())
                                        variant.fieldTypes.resize(ctorArgs.size(), SadTypeKind::Unknown);
                                    for (size_t fi = 0; fi < ctorArgs.size(); ++fi)
                                    {
                                        SadTypeKind argTy = ctorArgs[fi].dataType;
                                        SadTypeKind &slot = variant.fieldTypes[fi];
                                        if (slot == SadTypeKind::Unknown)
                                            slot = argTy;
                                        else if (slot != argTy)
                                            slot = SadTypeKind::Unknown; // (AR) تعارض ⇒ تراجُع آمن
                                    }
                                    break;
                                }
                            }

                            std::string ctorResultReg = b_.newTempRegister();
                            SIRInstruction ctorCall(SIROpcode::CALL);
                            ctorCall.result = SIROperand::Register(ctorResultReg, SadTypeKind::Struct);
                            ctorCall.operands.push_back(SIROperand::Function(ctorIt->second.name));
                            for (const auto &a : ctorArgs)
                                ctorCall.operands.push_back(a);
                            ctorCall.comment = "ADT constructor: " + ctorKey;
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(ctorCall);

                            // (AR) نضع اسم التعداد صنفًا للنتيجة كي تعمل **المطابقة**
                            //      (`طابق(ش)`) الّتي تفهرس على اسم التعداد في adtEnumTable_.
                            //      ملاحظة: هذا لازمٌ غير كافٍ للوصول المباشر للحقل
                            //      (`ش.نصف_القطر`) — className لا يُنشَر إلى classInstanceTypes_
                            //      فيبقى الوصول المباشر فجوةً موثّقة (ISSUE-077)؛ المطابقة تعمل.
                            // (EN) Tag the result's className with the enum name so **match**
                            //      (`match(s)`) works — it keys on the enum name in adtEnumTable_.
                            //      Note: necessary but NOT sufficient for direct field access
                            //      (`s.radius`) — className isn't propagated to classInstanceTypes_,
                            //      so direct access stays a documented gap (ISSUE-077); match works.
                            BuildResult ctorRes(ctorResultReg, SadTypeKind::Struct);
                            ctorRes.className = enumVarExpr->name;
                            return ctorRes;
                        }
                    }
                }

                // (AR) الخطوة 1: بناء تعبير الكائن
                // (EN) Step 1: Build object expression
                auto objResult = b_.buildExpression(methodCallExpr->object.get());

                // (AR) الخطوة 2: تحديد اسم الصنف من المتغير
                // (EN) Step 2: Determine class name from variable
                std::string className;

                // (AR) محاولة إيجاد اسم الصنف من VariableExpr
                // (EN) Try to find class name from VariableExpr
                if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr *>(methodCallExpr->object.get()))
                {
                    // (AR) البحث عن معلومات المتغير في b_.classInstanceTypes_
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

                // (AR) إذا كان 'هذا' (this)، نستخدم b_.currentClassName_
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
                // (AR) اعتراض مبكر: طرق القنوات بصيغة النقطة
                //      إذا كان الكائن قناة (__channel__)، نحوّل الاستدعاء مباشرة
                //      إلى تعليمات SIR دون المرور بمسار استدعاء الطرق العادي
                //      يجب أن يسبق كتلة البحث الذكي التي قد تمسح className
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
                            // (AR) الصنف الحالي لا يملك هذه الطريقة — نبحث في سلسلة الوراثة
                            //      مثال: كلب لا يملك تنفس() ← نبحث في حيوان (الأب) ← حيوان.تنفس()
                            //      مهم: لا نغيّر b_.classInstanceTypes_ — الكائن ما زال من نوع الابن
                            // (EN) Current class doesn't have this method — search inheritance chain
                            //      e.g.: كلب doesn't have تنفس() → search حيوان (parent) → حيوان.تنفس()
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
                                    // (AR) وجدنا الطريقة في الأب — نستخدمها دون تغيير نوع الكائن
                                    // (EN) Found method in parent — use it without changing object type
                                    className = classInfo->parentClass;
                                    foundInParent = true;
                                    break;
                                }
                                searchClass = classInfo->parentClass;
                            }
                            if (!foundInParent)
                            {
                                // (AR) لم توجد في سلسلة الوراثة — نبحث في أصناف أخرى (السلوك القديم)
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

                // (AR) الوسيط الأول: self (مؤشر الكائن)
                // (EN) First argument: self (object pointer)
                args.push_back(SIROperand::Register(objResult.registerName, objResult.type));

                // (AR) بقية الوسائط
                // (EN) Rest of arguments
                // (AR) تتبّع اسم دالة الإغلاق (اللامدا) ونوع إرجاعها
                //      لازمة لطرق المصفوفات عالية الرتبة (خريطة/رشح/اختزل/لكل)
                // (EN) Track closure lambda name and return type
                //      Needed for higher-order array methods (map/filter/reduce/forEach)
                std::string firstClosureLambdaName;
                SadTypeKind firstClosureRetType = SadTypeKind::Void;
                for (const auto &arg : methodCallExpr->arguments)
                {
                    auto argResult = b_.buildExpression(arg.get());
                    // (AR) تتبّع أول إغلاق (لامدا) مبني
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
                // (AR) الخطوة 3.5: فحص طرق المصفوفات المدمجة
                //      ⚠ نتخطى فحوص المدمجات إذا كان الكائن نسخة صنف يملك طريقة مطابقة
                //      مثال: صنف مكدس يملك طريقة اضف() — يجب استدعاء طريقة الصنف لا ARRAY_APPEND
                // (EN) Step 3.5: Check for builtin array methods
                //      ⚠ Skip builtin checks if the object is a class instance with a matching method
                //      e.g.: class مكدس has اضف() method — should CALL class method, not ARRAY_APPEND
                // ========================================================================
                std::string methodName = methodCallExpr->methodName;

                // (AR) فحص: هل هذه طريقة صنف مسجلة؟ إن كانت كذلك نتخطى فحوص المدمجات
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
                    // (AR) طريقة غير معروفة على نوعٍ مدمجٍ قيميّ (نص/مصفوفة/خريطة): كلّ
                    //      طرقها المضمَّنة فُحِصت أعلاه، وليست مقبض واجهة (Pointer، يُعالَج
                    //      أدناه) ولا مثيل صنف مستخدم (className فارغ). كان المسار يسقط
                    //      لبناء نداء طريقة صنف بنوع إرجاع Unknown ⇒ تُنشئ الخلفية
                    //      getNullValue على النوع Unknown (Constants.cpp:382) ⇒ انهيار
                    //      LLVM. نُصدر بدلًا منه تشخيص RUN_METHOD_NOT_FOUND (نفس رسالة
                    //      المفسّر: «الطريقة X غير موجودة في الصنف <نوع>») ونُسجّله في
                    //      errors_ فيُفشِل السائق البناءَ نظيفًا قبل التوليد
                    //      (compiler_driver_analysis: hasErrors()). حصريًّا للأنواع
                    //      القيميّة المدمجة كي لا يمسّ مقابض الواجهة أو أصناف المستخدم.
                    // (EN) Unknown method on a concrete builtin value type (string/
                    //      array/map): all its builtin methods were tried above; it is
                    //      not a widget handle (Pointer, handled below) nor a user-class
                    //      instance (empty className). The path used to fall through to a
                    //      class-method call with Unknown return type ⇒ the backend built
                    //      getNullValue on Unknown ⇒ LLVM crash. Emit RUN_METHOD_NOT_FOUND
                    //      instead (interpreter parity) and record it in errors_ so the
                    //      driver fails the build cleanly before codegen.
                    if (className.empty() &&
                        (objResult.type == SadTypeKind::String ||
                         objResult.type == SadTypeKind::Array ||
                         objResult.type == SadTypeKind::Map))
                    {
                        Sad::Errors::RenderContext ctx;
                        ctx.placeholders = {
                            {"method", methodName},
                            {"class", std::string(sirTypeToString(objResult.type))},
                            {"suggestion_clause", ""},
                            {"suggestion_clause_en", ""}};
                        b_.errors_.push_back(
                            Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND, ctx));
                        // (AR) نتيجة آمنة بنوعٍ محدَّد (لا Unknown) — البناء يُلغى عبر
                        //      hasErrors() فلا تُستهلك، لكنّها تمنع أيّ انهيار لاحق.
                        // (EN) Safe concrete-typed result (never Unknown); build aborts
                        //      via hasErrors() so it's unused, but prevents any later crash.
                        return BuildResult(b_.newTempRegister(), objResult.type);
                    }

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

                        // (AR) أسماء المعدّلات من مصدر الحقيقة (sad_ui/ui_modifiers.h المولَّد
                        //      من language-truth/ui_modifiers.yaml) — لا literals حرفيّة هنا،
                        //      ويستهلك المحرّكان الهيدر نفسه فيمتنع الانحراف بايتيًّا بالبناء.
                        //      سياسة: الأسماء القانونيّة فقط (لا بدائل).
                        // (EN) Modifier names from the SoT header (generated from
                        //      language-truth/ui_modifiers.yaml). Both engines consume the same
                        //      predicates, so byte-divergence is impossible by construction.
                        namespace mods = sad::ui::mods;
                        const bool isEvent = mods::isEvent(m);            // عند_* (حدث، L2)
                        const bool isAnimBegin = mods::isAnimate(m);      // حرّك (بدء سلسلة، L3)
                        const bool isAnimDuration = mods::isDuration(m);  // مدة
                        const bool isAnimEasing = mods::isEasing(m);      // منحنى
                        const bool isAnimDelay = mods::isDelay(m);        // تأخير
                        const bool isAnimRepeat = mods::isRepeat(m);      // تكرار
                        const bool isAnimAutoReverse = mods::isAutoReverse(m); // عكس_تلقائي
                        const bool isAnim = isAnimBegin || isAnimDuration || isAnimEasing ||
                                            isAnimDelay || isAnimRepeat || isAnimAutoReverse;
                        const bool isChild = mods::isChild(m);            // ابن (إضافة أطفال)

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
                            //      (نظير addChildBuilder في المفسّر). كلتاهما تضيفان **كلّ**
                            //      الوسائط — مطابقةً لـ addChildIfWidget في
                            //      ui_widget_method_call.cpp (وُحِّدت ابن/أبناء هناك).
                            //      args[0]=الأب، args[1..]=الأبناء. يعيد المقبض نفسه (أدناه)
                            //      فتتسلسل السلسلة كالمفسّر.
                            // (EN) Fluent ابن/أبناء → ADD_CHILD on the shared node. Both add
                            //      ALL args — matching the unified interpreter path.
                            // ════════════════════════════════════════════════════════
                            // (AR) ملاحظة تكافؤ: المفسّر يحرس نوع كلّ طفل
                            //      (isWidgetBuilder) ويتخطّى غير العناصر صمتًا؛ هنا نعتمد
                            //      على أنّ نظام الأنواع لا يُنتج مقبض Pointer لغير عنصرٍ في
                            //      هذا المسار (الوسائط مقابضُ عناصر)، وsad_add_child يحرس
                            //      nullptr. تباعدٌ نظريّ حدّيّ فقط لو مُرِّر مقبضٌ غير عنصر.
                            for (size_t i = 1; i < args.size(); ++i)
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

                // (AR) الخطوة 4: فحص إن كانت الدالة باني تعداد ADT
                //      إذا سُجلت باسم شكل.دائرة بينما اسمها الفعلي __adt_ctor_شكل_دائرة
                //      نستخدم الاسم الحقيقي ونحذف self من الوسائط
                // (EN) Step 4: Check if function is an ADT constructor
                //      If registered as شكل.دائرة but actual name is __adt_ctor_شكل_دائرة
                //      use the real name and remove self from arguments
                std::string callTargetName = fullMethodName;
                bool isADTCtor = false;
                auto ftIt = b_.functionTable_.find(fullMethodName);
                if (ftIt != b_.functionTable_.end())
                {
                    const auto &fInfo = ftIt->second;
                    // (AR) إذا كان الاسم الحقيقي يبدأ بـ __adt_ctor_ فهو باني ADT
                    // (EN) If real name starts with __adt_ctor_ it's an ADT constructor
                    if (fInfo.name.find(Sad::Compiler::kAdtCtorPrefix) == 0)
                    {
                        callTargetName = fInfo.name;
                        isADTCtor = true;
                    }
                }

                // (AR) الخطوة 5: إنشاء تعليمة CALL
                // (EN) Step 5: Create CALL instruction
                std::string resultReg = b_.newTempRegister();

                // (AR) فحص: هل الاستدعاء على «هذا»؟ ← نستخدم OBJECT_CALL للتوزيع الافتراضي
                //      الأصناف الابنة قد تعيد تعريف هذه الطريقة (تعدد الأشكال)
                //      مثال: داخل شكل.ارسم() ← هذا.ارسم() يجب أن يمر عبر vtable
                //             فإذا كان الكائن دائرة2 ← يُستدعى دائرة2.ارسم() لا شكل.ارسم()
                // (EN) Check: is call on `this`? → use OBJECT_CALL for virtual dispatch
                //      Subclasses may override this method (polymorphism)
                //      e.g.: inside شكل.ارسم() → هذا.ارسم() should go through vtable
                //             if object is دائرة2 → calls دائرة2.ارسم() not شكل.ارسم()
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
                        // (AR) التوزيع الافتراضي عبر vtable — OBJECT_CALL
                        // (EN) Virtual dispatch through vtable — OBJECT_CALL
                        SIRInstruction callInst;
                        callInst.opcode = SIROpcode::OBJECT_CALL;
                        callInst.result = SIROperand::Register(resultReg, returnType);
                        // operand[0]: self (مؤشر الكائن)
                        callInst.operands.push_back(args[0]);
                        // operand[1]: اسم الطريقة (كنص يُحلّ وقت التشغيل)
                        callInst.operands.push_back(SIROperand::ConstantString(methodCallExpr->methodName));
                        // operand[2+]: المعاملات الإضافية
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

                        // (AR) بواني ADT لا تأخذ self كوسيط أول — نتخطى args[0] (وهو self)
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
                // (AR) نشر اسم صنف القيمة المعادة (returnClassName) إلى BuildResult
                //      ضروري ليعرف المترجم نوع صنف الكائن المعاد من الاستدعاء.
                //      بدون هذا: يفشل الوصول لحقول كائنات معادة من method call
                //      مثال: متغير س = شكل.دائرة(5) ← س.نصف_القطر يحتاج معرفة أن س من النوع "شكل"
                //      يعمل مع بواني ADT وطرق الأصناف العادية
                // (EN) Propagate returnClassName to BuildResult
                //      Necessary so compiler knows the returned object's class type.
                //      Without this: field access on objects from method calls fails
                //      Example: var s = Shape.Circle(5) → s.radius needs to know s is type "Shape"
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
