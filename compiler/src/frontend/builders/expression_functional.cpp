// ============================================================================
// sir_builder_expr_functional.cpp
// ============================================================================
// Functional expression builders (lambda, range, list/dict/set comprehension, generators)
// ============================================================================
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include <set>
#include <functional>
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildExprLambda
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprLambda(AST::LambdaExpr *lambdaExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found LambdaExpr with "
                          << lambdaExpr->parameters.size() << " params" << std::endl;
#endif

                // (AR) إنشاء اسم فريد للدالة المجهولة
                // (EN) Create unique name for anonymous function
                std::string lambdaName = "__lambda_" + std::to_string(b_.nextTempRegister_++);

                // (AR) جمع أسماء المعاملات
                // (EN) Collect parameter names
                std::set<std::string> paramNames;
                for (const auto &param : lambdaExpr->parameters)
                {
                    paramNames.insert(param.name);
                }

                // (AR) تحليل المتغيرات الحرة (الملتقطة من النطاق الخارجي)
                // (EN) Analyze free variables (captured from outer scope)
                // (AR) [ISSUE-053] نرفع الراية كي يُلتقَط هدف الإسناد (متغيّر مُعدَّل) كمتغيّر حرّ
                //      في مسار اللامدا فقط؛ نحفظ القيمة السابقة ونستعيدها لأمان التداخل.
                // (EN) [ISSUE-053] Raise the flag so an assignment target (mutated var) is captured
                //      as free on the lambda path only; save/restore prior value for nesting safety.
                bool prevCaptureAssign = b_.captureAssignTargetsInClosure_;
                b_.captureAssignTargetsInClosure_ = true;
                std::set<std::string> freeVars;
                if (lambdaExpr->body)
                {
                    b_.collectFreeVarsExpr(lambdaExpr->body.get(), paramNames, freeVars);
                }
                if (lambdaExpr->blockBody)
                {
                    std::set<std::string> boundCopy = paramNames;
                    b_.collectFreeVarsStmt(lambdaExpr->blockBody.get(), boundCopy, freeVars);
                }
                b_.captureAssignTargetsInClosure_ = prevCaptureAssign;

                // (AR) البحث عن المتغيرات الملتقطة في النطاق الحالي
                // (EN) Look up captured variables in current scope
                std::vector<SIRBuilderContext::CaptureInfo> captures;
                for (const auto &fv : freeVars)
                {
                    auto *varPtr = b_.lookupVariable(fv);
                    if (varPtr)
                    {
                        SIRBuilderContext::CaptureInfo ci;
                        ci.varName = fv;
                        ci.registerName = varPtr->registerName;
                        ci.type = varPtr->type;
                        ci.declaredSurfaceType = varPtr->declaredSurfaceType; // (AR) [خ٧] نقل النوع السطحيّ عبر الالتقاط
                        ci.elementType = varPtr->elementType;                 // (AR) [GAP 1] نقل نوع عنصر المصفوفة المُلتقَطة (Any للمختلطة)
                        captures.push_back(ci);
#ifndef NDEBUG
                        std::cout << "[DEBUG] Lambda capture: " << fv
                                  << " (reg=" << ci.registerName << ", type="
                                  << static_cast<int>(ci.type) << ")" << std::endl;
#endif
                    }
                }

                // (AR) حفظ التقاطات الإغلاق
                // (EN) Store closure captures
                if (!captures.empty())
                {
                    b_.closureCaptures_[lambdaName] = captures;
                }

                // ================================================================
                // (AR) بناء معاملات الدالة:
                //      المعاملات الصريحة + معامل __env واحد مخفي (دائماً)
                //      __env يحمل مؤشراً لمصفوفة المتغيرات الملتقطة على الكومة
                //      إذا لم تكن هناك التقاطات: __env = 0 ولا يُستخدم
                //      هذا يضمن توقيعاً موحداً لجميع اللامدات
                // (EN) Build function parameters:
                //      Explicit params + one hidden __env param (always)
                //      __env holds a pointer to heap-allocated captures array
                //      If no captures: __env = 0 and unused
                //      This ensures a uniform signature for all lambdas
                // ================================================================

                // ────────────────────────────────────────────────────────
                // (AR) استنتاج أنواع المعاملات من تحليل جسم اللامدا
                //      بدلاً من تعيين Integer ثابت لجميع المعاملات (الخطأ القديم)
                //      نحلل كيفية استخدام كل معامل في الجسم لاستنتاج نوعه الصحيح
                //      هذا يضمن أن LLVM ينشئ التوقيع الصحيح (ptr للنصوص، i64 للأرقام)
                // (EN) Infer parameter types from lambda body analysis
                //      Instead of hardcoding Integer for all params (old bug)
                //      we analyze how each param is used in the body to infer its correct type
                //      This ensures LLVM generates correct signature (ptr for strings, i64 for ints)
                // ────────────────────────────────────────────────────────
                auto inferredParamTypes = b_.inferLambdaParamTypes(lambdaExpr, paramNames);

                std::vector<SIRParameter> sirParams;
                for (const auto &param : lambdaExpr->parameters)
                {
                    SadTypeKind paramType = SadTypeKind::Integer; // (AR) افتراضي / (EN) Default
                    // (AR) الأولوية 1: نوع AST صريح (المبرمج حدد النوع)
                    // (EN) Priority 1: explicit AST type (programmer specified)
                    if (param.type != Types::SadTypeKind::Unknown)
                    {
                        paramType = b_.astTypeToSIRType(param.type);
                    }
                    // (AR) الأولوية 2: نوع مُستنتج من تحليل الجسم
                    // (EN) Priority 2: type inferred from body analysis
                    else
                    {
                        auto it = inferredParamTypes.find(param.name);
                        if (it != inferredParamTypes.end())
                        {
                            paramType = it->second;
                        }
                    }
                    sirParams.push_back(SIRParameter(param.name, paramType));
                }
                // (AR) [GAP 4] توسيعُ معاملات اللامدا التي رُصِد في المسح المُسبَق أنّها
                //      تستقبل مصفوفةً مختلطةً/متجانسةً غيرَ صحيحةٍ في موقع نداء ⇒ نجعلها
                //      مصفوفةً بنوع عنصرٍ Any، فتُقرأ فهرستُها داخل الجسم موسومةً زمنَ
                //      التشغيل لا عدديًّا. نُطبّقه على المعاملات غيرِ المُصرَّحة نوعًا فقط
                //      (paramType الحاليّ Integer الافتراضيّ)، ولا نُسجّل إلّا Any (آمنٌ
                //      عبر homogKind ولو شارك المعاملَ مواقعُ مختلفةٌ — كـGAP 3b).
                // (EN) [GAP 4] Widen lambda params that the pre-pass flagged as receiving a
                //      mixed / homogeneous-non-int array at a call site ⇒ make them an Array
                //      with element type Any, so their indexing inside the body reads runtime-
                //      tagged, not as int. Applied only to params left at the default Integer
                //      (untyped); records only Any (safe via homogKind across call sites, as GAP 3b).
                {
                    auto anyIt = b_.scanLambdaParamAny_.find(lambdaExpr);
                    if (anyIt != b_.scanLambdaParamAny_.end())
                    {
                        for (size_t i = 0; i < lambdaExpr->parameters.size() && i < sirParams.size(); ++i)
                        {
                            if (anyIt->second.count(i) > 0 &&
                                lambdaExpr->parameters[i].type == Types::SadTypeKind::Unknown &&
                                sirParams[i].type == SadTypeKind::Integer)
                            {
                                sirParams[i].type = SadTypeKind::Array;
                            }
                        }
                    }
                    // (AR) [موجة الجسر الموسوم — t05] النظيرُ العدديُّ: معاملٌ بلغَه
                    //      وسيطٌ عشريٌّ في موقعِ نداءٍ يُوسَّعُ **قيمةً** Any (%SadDyn)
                    //      فيُغلَّفُ الوسيطُ بوسمِه ويحسبُ الجسمُ ديناميًّا — لا مصفوفةً
                    //      (خريطةُ GAP 4 أعلاه). بوّابةُ Integer تُبقي أولويّةَ المصفوفةِ
                    //      إن وُسِمَ الفهرسُ في الخريطتَين معًا.
                    // (EN) [Tagged-bridge wave — t05] The scalar counterpart: a param
                    //      that received a float argument at a call site widens to a
                    //      tagged VALUE Any (%SadDyn) — the argument is boxed with its
                    //      tag and the body computes dynamically — not to an Array
                    //      (GAP 4's map above). The Integer gate keeps the Array
                    //      priority if an index is flagged in both maps.
                    auto dynIt = b_.scanLambdaParamDynAny_.find(lambdaExpr);
                    if (dynIt != b_.scanLambdaParamDynAny_.end())
                    {
                        for (size_t i = 0; i < lambdaExpr->parameters.size() && i < sirParams.size(); ++i)
                        {
                            if (dynIt->second.count(i) > 0 &&
                                lambdaExpr->parameters[i].type == Types::SadTypeKind::Unknown &&
                                sirParams[i].type == SadTypeKind::Integer)
                            {
                                sirParams[i].type = SadTypeKind::Any;
                            }
                        }
                    }
                }
                // (AR) إضافة معامل __env كمعامل أخير (دائماً — حتى بدون التقاطات)
                // (EN) Add __env as last parameter (always — even without captures)
                sirParams.push_back(SIRParameter(environmentParameterName(), SadTypeKind::Integer));

                // (AR) استنتاج نوع الإرجاع
                // (EN) Infer return type
                SadTypeKind retType = SadTypeKind::Integer;
                if (lambdaExpr->blockBody)
                {
                    retType = b_.inferReturnTypeFromBody(lambdaExpr->blockBody.get());
                }

                // (AR) إنشاء دالة SIR للـ lambda
                // (EN) Create SIR function for lambda
                auto lambdaFunc = std::make_shared<SIRFunction>(lambdaName, retType);
                for (const auto &lp : sirParams)
                    lambdaFunc->addParameter(lp);

                // (AR) حفظ السياق الحالي
                // (EN) Save current context
                auto savedCtx = b_.saveContext();

                // (AR) تعيين سياق الدالة الجديدة
                // (EN) Set new function context
                b_.currentFunction_ = lambdaFunc;
                auto entryBlock = b_.createBasicBlock("lambda_entry");
                lambdaFunc->addBasicBlock(entryBlock);
                b_.currentBlock_ = entryBlock;

                b_.enterScope();

                // (AR) تسجيل المعاملات الصريحة كمتغيرات محلية
                //      نستخدم الأنواع المُستنتجة من sirParams (وليس Integer ثابت)
                //      هذا يضمن أن المتغيرات داخل جسم اللامدا تحمل النوع الصحيح
                // (EN) Register explicit parameters as local variables
                //      Use inferred types from sirParams (not hardcoded Integer)
                //      This ensures variables inside lambda body have the correct type
                for (size_t i = 0; i < lambdaExpr->parameters.size(); ++i)
                {
                    std::string paramReg = "%" + lambdaExpr->parameters[i].name;
                    VariableInfo paramVar;
                    paramVar.name = lambdaExpr->parameters[i].name;
                    paramVar.type = sirParams[i].type; // (AR) النوع المُستنتج / (EN) Inferred type
                    // (AR) [GAP 4] المعاملُ المُوسَّعُ إلى مصفوفةٍ في المسح المُسبَق يحمل
                    //      نوعَ عنصرٍ Any، كي تختار فهرستُه داخل الجسم مسارَ القراءة الموسوم.
                    // (EN) [GAP 4] A param widened to Array by the pre-pass carries element
                    //      type Any, so its indexing inside the body selects the tagged read path.
                    {
                        auto anyIt2 = b_.scanLambdaParamAny_.find(lambdaExpr);
                        if (anyIt2 != b_.scanLambdaParamAny_.end() && anyIt2->second.count(i) > 0 &&
                            paramVar.type == SadTypeKind::Array)
                            paramVar.elementType = SadTypeKind::Any;
                    }
                    paramVar.registerName = paramReg;
                    paramVar.isMutable = false;
                    paramVar.scopeLevel = b_.currentScopeLevel_;
                    // (② rfcs#46) معامل لامدا مُنمَّط بصنفٍ: انقل اسم الصنف كي يُحلّ الوصول
                    //   العضويّ داخل الجسم (ح.دلتا_س) ويُتحقَّق نوعيًّا. يُسجَّل بالاسم و«%الاسم»
                    //   (نظير معاملات class_operator.cpp) لأنّ باني VariableExpr يبحث بكليهما.
                    // ⚠ ملاحظة (ع-1): مُحلّل اللامدا الحاليّ (parseLambda) لا يقرأ معاملات
                    //   مُنمَّطة (النوع قبل الاسم)، فـtypeName يبقى فارغًا للامدا وهذا الفرع لا
                    //   يُفعَّل عمليًّا اليوم — إبقاؤه استباقٌ لدعمٍ لاحق. المسارُ المدعوم الآن
                    //   لمعالِج حدثٍ يقرأ «حدث» هو **دالّة مسمّاة** `دالة معالِج(حدث ح)` (تنميط
                    //   معاملها يجري في مسار تعريف الدوالّ لا هنا)، تُمرَّر كمرجع للمعدّل.
                    const std::string &paramTypeName = lambdaExpr->parameters[i].typeName;
                    if (!paramTypeName.empty() && b_.module_ &&
                        b_.module_->getClass(paramTypeName))
                    {
                        paramVar.className = paramTypeName;
                        b_.classInstanceTypes_[paramVar.name] = paramTypeName;
                        b_.classInstanceTypes_[paramReg] = paramTypeName;
                    }
                    b_.addVariable(paramVar);
                }

                // ================================================================
                // (AR) تحميل المتغيرات الملتقطة من بيئة الإغلاق __env
                //      كل متغير ملتقط يُقرأ من env[i] عبر تعليمة ENV_LOAD
                //      ثم يُسجّل كمتغير محلي باسمه الأصلي داخل اللامدا
                // (EN) Load captured variables from closure environment __env
                //      Each capture is read from env[i] via ENV_LOAD instruction
                //      Then registered as a local variable with its original name
                // ================================================================
                for (size_t i = 0; i < captures.size(); i++)
                {
                    // (AR) [Fix #51] تحميل القيمة من env[i] عبر ENV_LOAD
                    // (EN) [Fix #51] Load value from env[i] via ENV_LOAD
                    std::string loadReg = b_.newTempRegister();
                    SIRInstruction envLoadInst;
                    envLoadInst.opcode = SIROpcode::ENV_LOAD;
                    envLoadInst.result = SIROperand::Register(loadReg, captures[i].type);
                    envLoadInst.operands.push_back(SIROperand::Register(environmentSlotName(), SadTypeKind::Integer));
                    envLoadInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(i)));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(envLoadInst);

                    // ================================================================
                    // (AR) [Fix #51] إنشاء متغير محلي عبر STORE لتخزين القيمة من env
                    //      في LLVM codegen: STORE يُنشئ alloca تلقائياً عند عدم وجود ptr
                    //      نستخدم اسم سجل في فضاء الأسماء الداخلي (makeCaptureSlotName) ليكون alloca-compatible
                    //      هذا يُحوّل المتغير الملتقط من "قيمة مؤقتة" إلى "مكان قابل للتعديل"
                    // (EN) [Fix #51] Create local variable via STORE for env value
                    //      In LLVM codegen: STORE auto-creates alloca when ptr not found
                    //      Use an internal-namespace register name (makeCaptureSlotName), alloca-compatible
                    // ================================================================
                    std::string allocaName = makeCaptureSlotName(captures[i].varName, i);
                    SIRInstruction storeInit;
                    storeInit.opcode = SIROpcode::STORE;
                    storeInit.operands.push_back(SIROperand::Register(loadReg, captures[i].type));
                    storeInit.operands.push_back(SIROperand::Register(allocaName, captures[i].type));
                    storeInit.comment = "init captured var from env[" + std::to_string(i) + "]";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(storeInit);

                    // (AR) [Fix #51] تسجيل المتغير مع alloca كـ registerName
                    // (EN) [Fix #51] Register captured variable with alloca as registerName
                    VariableInfo capVar;
                    capVar.name = captures[i].varName;
                    capVar.type = captures[i].type;
                    capVar.declaredSurfaceType = captures[i].declaredSurfaceType; // (AR) [خ٧] نقل النوع السطحيّ للمتغيّر المُلتقَط
                    capVar.elementType = captures[i].elementType;                 // (AR) [GAP 1] نقل نوع عنصر المصفوفة المُلتقَطة كي تختار الفهرسة مسارَ القراءة الموسوم
                    capVar.registerName = allocaName;
                    capVar.isMutable = true;
                    capVar.scopeLevel = b_.currentScopeLevel_;
                    capVar.isCaptured = true;
                    capVar.captureIndex = static_cast<int>(i);
                    capVar.envRegister = environmentSlotName();
                    b_.addVariable(capVar);
                }

                // (AR) بناء جسم الـ lambda
                // (EN) Build lambda body
                if (lambdaExpr->body)
                {
                    // (AR) تعبير واحد - إرجاع تلقائي
                    // (EN) Single expression - auto-return
                    auto bodyResult = buildExpression(lambdaExpr->body.get());

                    // ================================================================
                    // (AR) تحديث نوع الإرجاع من نوع التعبير الفعلي
                    //      مثال: لامدا (س) => س > 10 → bodyResult.type = Boolean
                    //      بدون هذا: retType يبقى Integer (الافتراضي) وتضيع معلومة Boolean
                    //      مما يجعل builtin.print يطبع 0/1 بدلاً من خطأ/صحيح
                    // (EN) Update return type from actual expression type
                    //      e.g.: lambda (x) => x > 10 → bodyResult.type = Boolean
                    //      Without this: retType stays Integer (default) and Boolean info is lost
                    // ================================================================
                    if (bodyResult.type != SadTypeKind::Void && bodyResult.type != SadTypeKind::Unknown)
                    {
                        retType = bodyResult.type;
                        lambdaFunc->returnType = retType;
                        // (AR) تحديث FunctionInfo سيتم لاحقاً عند التسجيل في b_.functionTable_
                    }

                    SIRInstruction retInst;
                    retInst.opcode = SIROpcode::RET;
                    retInst.operands.push_back(SIROperand::Register(bodyResult.registerName, bodyResult.type));
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(retInst);
                    }
                }
                else if (lambdaExpr->blockBody)
                {
                    // (AR) جسم كتلي - بناء الجمل
                    // (EN) Block body - build statements
                    b_.buildStatement(lambdaExpr->blockBody.get());

                    // ================================================================
                    // (AR) [Fix #52] تحديث نوع الإرجاع من تعليمات RET الفعلية
                    //      b_.inferReturnTypeFromBody يعمل على AST قبل بناء المتغيرات الملتقطة
                    //      لذا لا يعرف أنواعها. بعد بناء الجسم، المتغيرات مسجلة بأنواعها
                    //      الصحيحة، فنفحص تعليمات RET الفعلية ونأخذ النوع منها.
                    //      هذا يصلح مشكلة إرجاع مؤشر بدل نص من الإغلاقات.
                    // (EN) [Fix #52] Update return type from actual RET instructions
                    //      b_.inferReturnTypeFromBody works on AST before captured vars are built,
                    //      so it doesn't know their types. After building body, vars are registered
                    //      with correct types, so we scan actual RET instructions for the real type.
                    //      This fixes closures returning pointer instead of string.
                    // ================================================================
                    if (lambdaFunc)
                    {
                        for (const auto &block : lambdaFunc->basicBlocks)
                        {
                            for (const auto &inst : block->instructions)
                            {
                                if (inst.opcode == SIROpcode::RET && !inst.operands.empty())
                                {
                                    SadTypeKind actualRetType = inst.operands[0].dataType;
                                    if (actualRetType != SadTypeKind::Void &&
                                        actualRetType != SadTypeKind::Unknown &&
                                        actualRetType != SadTypeKind::Integer &&
                                        retType == SadTypeKind::Integer)
                                    {
                                        // (AR) النوع الفعلي أدق من Integer الافتراضي
                                        // (EN) Actual type is more specific than default Integer
                                        retType = actualRetType;
                                        lambdaFunc->returnType = retType;
                                    }
                                    else if (retType == SadTypeKind::Integer &&
                                             actualRetType == SadTypeKind::Integer)
                                    {
                                        // (AR) قد يكون Integer فعلياً — نفحص السجل المصدري
                                        //      إذا كان من متغير ملتقط بنوع مختلف، نعتمد نوع الملتقط
                                        // (EN) Might truly be Integer — check source register
                                        //      If from a captured var with different type, use capture type
                                        const std::string &srcReg = inst.operands[0].name;
                                        for (const auto &cap : captures)
                                        {
                                            std::string capAllocaName = makeCaptureSlotPrefix(cap.varName);
                                            if (srcReg.find(capAllocaName) != std::string::npos ||
                                                srcReg == "%" + cap.varName)
                                            {
                                                if (cap.type != SadTypeKind::Integer)
                                                {
                                                    retType = cap.type;
                                                    lambdaFunc->returnType = retType;
                                                }
                                                break;
                                            }
                                        }
                                    }
                                    break; // (AR) أول RET يكفي / (EN) First RET is enough
                                }
                            }
                            if (retType != SadTypeKind::Integer)
                                break;
                        }
                    }

                    // (AR) إضافة RET_VOID في نهاية الكتلة إن لم يكن هناك return
                    // (EN) Add RET_VOID at end if no return
                    if (b_.currentBlock_ && !b_.currentBlock_->instructions.empty())
                    {
                        auto &lastInst = b_.currentBlock_->instructions.back();
                        if (lastInst.opcode != SIROpcode::RET && lastInst.opcode != SIROpcode::RET_VOID)
                        {
                            SIRInstruction retVoid;
                            retVoid.opcode = SIROpcode::RET_VOID;
                            b_.currentBlock_->addInstruction(retVoid);
                        }
                    }
                    else if (b_.currentBlock_)
                    {
                        SIRInstruction retVoid;
                        retVoid.opcode = SIROpcode::RET_VOID;
                        b_.currentBlock_->addInstruction(retVoid);
                    }
                }

                b_.exitScope();

                // (AR) إضافة الدالة للوحدة
                // (EN) Add function to module
                if (b_.module_)
                {
                    b_.module_->addFunction(lambdaFunc);
                }

                // (AR) تسجيل في جدول الدوال
                // (EN) Register in function table
                FunctionInfo lambdaInfo;
                lambdaInfo.name = lambdaName;
                lambdaInfo.returnType = retType;
                for (const auto &sp : sirParams)
                {
                    lambdaInfo.parameters.push_back(sp);
                }
                b_.functionTable_[lambdaName] = lambdaInfo;

                // (AR) استعادة السياق السابق
                // (EN) Restore previous context
                b_.restoreContext(std::move(savedCtx));

                // ================================================================
                // (AR) إنشاء بنية الإغلاق (Closure) على الكومة
                //      CLOSURE_CREATE ينشئ بنية {fn_ptr, env_ptr} على الكومة
                //      - fn_ptr: مؤشر لدالة اللامدا
                //      - env_ptr: مؤشر لمصفوفة المتغيرات الملتقطة (0 إن لم تكن)
                //      هذا يُرجع سجلاً حقيقياً (ليس ثابتاً) يحمل مؤشر بنية الإغلاق
                //      بعكس النهج القديم الذي كان يُرجع اسم الدالة كثابت.
                // (EN) Create Closure struct on heap
                //      CLOSURE_CREATE creates {fn_ptr, env_ptr} on heap
                //      Returns a real register (not constant) holding closure pointer
                // ================================================================
                std::string closureReg = b_.newTempRegister();
                SIRInstruction closureInst;
                closureInst.opcode = SIROpcode::CLOSURE_CREATE;
                closureInst.result = SIROperand::Register(closureReg, SadTypeKind::Function);
                // (AR) لاحقةُ «;ret:» تحملُ نوعَ عائدِ اللامدا إلى جسرِ البروتوكولِ
                //      الموسومِ في الخلفيّةِ (يَسِمُ بها العائدَ المُغلَّف).
                // (EN) The «;ret:» suffix carries the lambda's return kind to the
                //      backend's tagged-protocol bridge (it tags the boxed return).
                closureInst.comment =
                    std::string(Sad::Compiler::kClosureRetKindMarker) +
                    std::to_string(static_cast<int>(lambdaInfo.returnType));
                // (AR) المعامل الأول: مؤشر الدالة
                // (EN) First operand: function pointer
                closureInst.operands.push_back(SIROperand::Function(lambdaName));
                // (AR) المعاملات التالية: قيم المتغيرات الملتقطة (لإنشاء مصفوفة env)
                // (EN) Remaining operands: captured variable values (to create env array)
                for (const auto &cap : captures)
                {
                    // (AR) تحميل القيمة الحالية للمتغير الملتقط من النطاق الخارجي
                    // (EN) Load current value of captured variable from outer scope
                    VariableInfo *capVar = b_.lookupVariable(cap.varName);
                    if (capVar)
                    {
                        std::string capLoadReg = b_.newTempRegister();
                        SIRInstruction capLoadInst;
                        capLoadInst.opcode = SIROpcode::LOAD;
                        capLoadInst.result = SIROperand::Register(capLoadReg, capVar->type);
                        capLoadInst.operands.push_back(SIROperand::Register(capVar->registerName, capVar->type));
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(capLoadInst);
                        closureInst.operands.push_back(SIROperand::Register(capLoadReg, capVar->type));
                    }
                    else
                    {
                        closureInst.operands.push_back(SIROperand::Register(cap.registerName, cap.type));
                    }
                }
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(closureInst);

                // (AR) إرجاع سجل الإغلاق — سجل حقيقي مُعرَّف بواسطة CLOSURE_CREATE
                //      constantValue يحمل اسم اللامدا للاستخدام في التتبع فقط
                //      isConstant = false لأن القيمة في سجل حقيقي (ليس ثابتاً)
                // (EN) Return closure register — real register defined by CLOSURE_CREATE
                //      constantValue holds lambda name for tracking only
                //      isConstant = false because value is in a real register
                BuildResult result(closureReg, SadTypeKind::Function);
                result.constantValue = lambdaName;
                result.closureLambdaName = lambdaName;
                result.isConstant = false;
                return result;
            }

            // ============================================================================
            // buildExprRange
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprRange(AST::RangeExpr *rangeExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found RangeExpr" << std::endl;
#endif

                // (AR) بناء بداية ونهاية المدى
                // (EN) Build range start and end
                auto startResult = buildExpression(rangeExpr->start.get());
                auto endResult = buildExpression(rangeExpr->end.get());

                // (AR) تجسيد الثابت في سجلّ قبل التخزين (start/end قد يكونان ثابتين
                //      كـ 1..10). يطابق عرف materializeResult في باني الصفوف/الخرائط.
                // (EN) Materialize a constant into a register before storing (start/end
                //      may be constants as in 1..10). Mirrors the tuple/map builders'
                //      materializeResult convention.
                auto materialize = [&](BuildResult &res) -> SIROperand
                {
                    if (res.isConstant)
                    {
                        std::string reg = b_.newTempRegister();
                        res.registerName = reg;
                        SIRInstruction moveInst(SIROpcode::MOVE);
                        moveInst.result = SIROperand::Register(reg, res.type);
                        if (res.type == SadTypeKind::String)
                            moveInst.operands.push_back(SIROperand::ConstantString(res.constantValue));
                        else if (res.type == SadTypeKind::Float)
                            moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(res.constantValue)));
                        else if (res.type == SadTypeKind::Boolean)
                            moveInst.operands.push_back(SIROperand::ConstantBool(res.constantValue == "true" || res.constantValue == "1"));
                        else
                        {
                            try
                            {
                                moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(res.constantValue)));
                            }
                            catch (const std::exception &)
                            {
                                moveInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                        }
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(moveInst);
                        res.isConstant = false;
                    }
                    return SIROperand::Register(res.registerName, res.type);
                };

                SIROperand startOp = materialize(startResult);
                SIROperand endOp = materialize(endResult);

                // (AR) تمثيل المدى القيميّ المستقلّ كصفٍّ من عنصرين [بداية، نهاية].
                //      الصفوف مدعومة كاملًا حرًّا (لا تخطيط أصناف)، بخلاف التمثيل
                //      السابق (ALLOC + STORE بأسماء حقول) الذي كان يفشل حرًّا بـ
                //      «No class mapping». حلقات «لكل في مدى» لا تمرّ هنا (تعالج
                //      RangeExpr مباشرةً)، وما من مستهلك يقرأ start/end بالاسم.
                // (EN) Represent a standalone range value as a 2-element tuple
                //      [start, end]. Tuples are fully supported freestanding (no class
                //      mapping), unlike the prior ALLOC + named-field STORE form that
                //      failed freestanding with "No class mapping". For-range loops
                //      never reach here (they lower RangeExpr directly), and no
                //      consumer reads start/end by name.
                std::string rangeReg = b_.newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::TUPLE_NEW;
                allocInst.result = SIROperand::Register(rangeReg, SadTypeKind::Tuple);
                allocInst.operands.push_back(SIROperand::ConstantI64(2));
                allocInst.operands.push_back(SIROperand::ConstantI64(2)); // (AR) طول ابتدائي=2 لتجاوز فحص الحدود
                allocInst.comment = "range tuple [start, end]";

                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(allocInst);

                // (AR) تخزين البداية في الخانة 0 والنهاية في الخانة 1
                // (EN) Store start at slot 0, end at slot 1
                SIRInstruction setStartInst(SIROpcode::ARRAY_SET);
                setStartInst.operands.push_back(SIROperand::Register(rangeReg, SadTypeKind::Tuple));
                setStartInst.operands.push_back(SIROperand::ConstantI64(0));
                setStartInst.operands.push_back(startOp);
                setStartInst.comment = "range[0] = start";
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(setStartInst);

                SIRInstruction setEndInst(SIROpcode::ARRAY_SET);
                setEndInst.operands.push_back(SIROperand::Register(rangeReg, SadTypeKind::Tuple));
                setEndInst.operands.push_back(SIROperand::ConstantI64(1));
                setEndInst.operands.push_back(endOp);
                setEndInst.comment = "range[1] = end";
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(setEndInst);

                return BuildResult(rangeReg, SadTypeKind::Tuple);
            }

            // ============================================================================
            // buildExprListComp
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
