// ============================================================================
// sir_builder_control_branch.cpp
// (AR) Branches: return, break, continue, assignment, localvar, if
// (EN) Branch control flow: return, break, continue, assignment, local variables, if/else
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
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

            // ============================================================================
            // buildReturnStatement - بناء جملة return
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:409
            // التوقيع / Signature: void buildReturnStatement(AST::ReturnStmt* retStmt);
            //
            // المعاملات / Parameters:
            // - retStmt: AST::ReturnStmt* = Sad::AST::ReturnStmt* (sir_builder.h:409)
            //
            // ReturnStmt Members (statements.h:266):
            // - value: ExprPtr (line 268) - optional, can be nullptr
            //
            // الدوال المستدعاة / Called functions:
            // - b_.buildExpression: sir_builder.h:432
            // ============================================================================
            void StatementBuilder::buildReturnStatement(AST::ReturnStmt *retStmt)
            {
                if (!retStmt)
                {
                    return;
                }

                // (AR) الحاجز ٧: العودة تخرج من كلّ «حاول» مُحيطة في الدالّة ⇒ خفّض
                //      __sad_try_active بعددها، وإلّا تسرّب العدّادُ بعد موت الإطار فيقفز
                //      أوّلُ حارس قسمة/صفر لاحق لـjmpbuf ميّت (0xC0000005). يُبعَث قبل RET.
                // (EN) Barrier 7: a return leaves every enclosing «try» in the function ⇒
                //      decrement __sad_try_active by that count, else the counter leaks after
                //      the frame dies and the next division-by-zero guard longjmps into a dead
                //      jmpbuf (0xC0000005). Emitted before the RET.
                for (int i = 0; i < b_.currentTryDepth_ && b_.currentBlock_; ++i)
                {
                    SIRInstruction tryExit;
                    tryExit.opcode = SIROpcode::CALL;
                    tryExit.operands.push_back(SIROperand::Function("__sad_try_exit"));
                    tryExit.comment = "barrier7: exit active try (return)";
                    b_.currentBlock_->addInstruction(tryExit);
                }

                // ================================================================
                // (AR) FIX X06: بناء تعبير الإرجاع قبل defer
                //      المشكلة: emitRunDeferredClosures() يغير b_.currentBlock_ الى defer_skip،
                //      وعند بناء التعبير بعده، namedValues تشير الى alloca من نطاق مختلف.
                //      مثال: في match، متغير "ضريبة" لكل case له alloca منفصل. عند بناء
                //      ارجع "..." + ضريبة في defer_skip، يجد المترجم آخر alloca لـ ضريبة
                //      (من آخر case) بدلاً من alloca الـ case الحالي، فتكون القيمة 0.
                //      الحل: بناء التعبير في body block + تخزينه في alloca فريد لضمان SSA.
                // (EN) FIX X06: Build return expression BEFORE defer
                //      Problem: emitRunDeferredClosures() changes b_.currentBlock_ to defer_skip,
                //      and building the expression after it uses wrong alloca from another scope.
                //      In match: each case has its own "var" alloca. Building "..." + var
                //      in defer_skip finds the LAST case's alloca (value=0) not current case's.
                //      Solution: Build in current body block + store in unique temp alloca.
                // ================================================================
                std::string prebuiltRetReg;
                SadTypeKind prebuiltRetType = SadTypeKind::Void;
                bool hasPrebuiltRet = false;
                // (AR) ISSUE-056: علامة أنّ تعبير الإرجاع نداءٌ يُرجع فراغاً وبُني مسبقاً
                //      لأثره الجانبيّ فقط — يمنع المسار العاديّ من إعادة بنائه (تباعد مزدوج).
                // (EN) ISSUE-056: flag that the return expr is a void-returning call already
                //      built for its side effect only — prevents the normal path rebuilding it.
                bool voidSideEffectPrebuilt = false;

                if (retStmt->value && !b_.currentDeferStackReg_.empty() && b_.finallyStack_.empty() &&
                    (!b_.currentFunction_ || !b_.currentFunction_->isCoroutine))
                {
                    BuildResult preResult = b_.buildExpression(retStmt->value.get());
                    if (!preResult.registerName.empty() && !preResult.isConstant)
                    {
                        // (AR) تخزين القيمة في alloca مؤقت قبل defer لضمان SSA correctness
                        // (EN) Store value in temp alloca before defer for SSA correctness
                        std::string tempAllocaReg = b_.newTempRegister();
                        SadTypeKind storeType = preResult.type;

                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::ALLOC;
                        allocInst.result = SIROperand::Register(tempAllocaReg, storeType);
                        allocInst.comment = "temp alloca for return value (FIX X06: before defer)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(allocInst);

                        SIRInstruction storeInst;
                        storeInst.opcode = SIROpcode::STORE;
                        storeInst.operands.push_back(SIROperand::Register(preResult.registerName, storeType));
                        storeInst.operands.push_back(SIROperand::Register(tempAllocaReg, storeType));
                        storeInst.comment = "store return value before defer cleanup (FIX X06)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(storeInst);

                        prebuiltRetReg = tempAllocaReg;
                        prebuiltRetType = storeType;
                        hasPrebuiltRet = true;
                    }
                    else if (preResult.registerName.empty() && !preResult.isConstant)
                    {
                        // (AR) ISSUE-056: التعبير نداءٌ يُرجع فراغاً — نُفِّذ أثره الجانبيّ (طباعة)
                        //      مرّة واحدة هنا، ولا قيمة تُرجَع. نعلّمه لئلا يُعاد بناؤه في المسار
                        //      العاديّ (كان المفسّر يطبع مرّة والمترجم مرّتين).
                        // (EN) ISSUE-056: void-returning call — its side effect (print) already
                        //      ran once here and there is no value to return. Flag it so the
                        //      normal path does NOT rebuild it (interp printed once, compiler twice).
                        voidSideEffectPrebuilt = true;
                    }
                }

                // ================================================================
                // (AR) تنفيذ الجمل المؤجلة (أجّل/defer) قبل RET بترتيب LIFO
                // (EN) Execute deferred statements (defer) before RET in LIFO order
                // ================================================================
                if (!b_.currentDeferStackReg_.empty())
                {
                    emitRunDeferredClosures();
                }
                else if (!b_.deferredStatements_.empty())
                {
                    for (auto it = b_.deferredStatements_.rbegin(); it != b_.deferredStatements_.rend(); ++it)
                    {
                        if (*it)
                            buildStatement(*it);
                    }
                }

                // ================================================================
                // (AR) فحص سياق finally: إذا كنا داخل try/catch مع أخيراً
                //      لا نُصدر RET مباشرة — نحفظ القيمة ونقفز لـ finally لضمان تنفيذه
                //      الأنواع المدعومة: 0=فراغ، 1=رقم/منطقي، 2=نص/مؤشر، 3=عشري
                //      هذا الفحص يجب أن يكون بعد defer وقبل كل شيء آخر
                // (EN) Check finally context: if inside try/catch with finally block
                //      Don't emit direct RET — save value and branch to finally to guarantee execution
                //      Supported types: 0=void, 1=integer/bool, 2=string/ptr, 3=float
                //      This check must come after defer processing and before everything else
                // ================================================================
                if (!b_.finallyStack_.empty())
                {
                    auto &ctx = b_.finallyStack_.back();

                    if (retStmt->value)
                    {
                        // (AR) بناء قيمة الإرجاع
                        // (EN) Build return value expression
                        BuildResult valResult = b_.buildExpression(retStmt->value.get());

                        // (AR) إذا كانت القيمة في alloca (متغير محلي) → نحتاج LOAD أولاً
                        // (EN) If value is in alloca (local variable) → need LOAD first
                        std::string actualReg = valResult.registerName;
                        SadTypeKind actualType = valResult.type;

                        if (!valResult.isConstant && !actualReg.empty() &&
                            !valResult.isFieldAccess && !valResult.isDirectValue &&
                            actualType != SadTypeKind::String && actualType != SadTypeKind::Function &&
                            valResult.className.empty())
                        {
                            std::string vn = actualReg;
                            if (!vn.empty() && vn[0] == '%')
                                vn = vn.substr(1);
                            if (b_.lookupVariable(vn))
                            {
                                std::string loadedReg = b_.newTempRegister();
                                SIRInstruction loadInst;
                                loadInst.opcode = SIROpcode::LOAD;
                                loadInst.result = SIROperand::Register(loadedReg, actualType);
                                loadInst.operands.push_back(SIROperand::Register(actualReg, actualType));
                                loadInst.comment = "load variable for finally return";
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(loadInst);
                                actualReg = loadedReg;
                            }
                        }

                        // (AR) تحديد typeCode وتخزين القيمة في السجل المناسب
                        // (EN) Determine typeCode and store value in appropriate register
                        int typeCode = 0;
                        if (actualType == SadTypeKind::Integer || actualType == SadTypeKind::Boolean)
                        {
                            typeCode = 1;
                            SIRInstruction stI;
                            stI.opcode = SIROpcode::STORE;
                            if (valResult.isConstant && !valResult.constantValue.empty())
                            {
                                if (actualType == SadTypeKind::Integer)
                                    stI.operands.push_back(SIROperand::ConstantI64(std::stoll(valResult.constantValue)));
                                else
                                    stI.operands.push_back(SIROperand::ConstantBool(valResult.constantValue == "true"));
                            }
                            else
                                stI.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stI.operands.push_back(SIROperand::Register(ctx.retValI64Reg, SadTypeKind::Integer));
                            stI.comment = "store integer finally return value";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stI);
                        }
                        else if (actualType == SadTypeKind::String || actualType == SadTypeKind::Pointer)
                        {
                            typeCode = 2;
                            SIRInstruction stP;
                            stP.opcode = SIROpcode::STORE;
                            if (valResult.isConstant && actualType == SadTypeKind::String)
                                stP.operands.push_back(SIROperand::ConstantString(valResult.constantValue));
                            else
                                stP.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stP.operands.push_back(SIROperand::Register(ctx.retValPtrReg, SadTypeKind::String));
                            stP.comment = "store string/ptr finally return value";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stP);
                        }
                        else if (actualType == SadTypeKind::Float)
                        {
                            typeCode = 3;
                            SIRInstruction stF;
                            stF.opcode = SIROpcode::STORE;
                            if (valResult.isConstant)
                                stF.operands.push_back(SIROperand::ConstantF64(std::stod(valResult.constantValue)));
                            else
                                stF.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stF.operands.push_back(SIROperand::Register(ctx.retValI64Reg, SadTypeKind::Float));
                            stF.comment = "store float finally return value";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stF);
                        }
                        else if (!valResult.registerName.empty())
                        {
                            // (AR) نوع غير معروف — نعاملهم كرقم
                            // (EN) Unknown type — treat as integer
                            typeCode = 1;
                            SIRInstruction stU;
                            stU.opcode = SIROpcode::STORE;
                            stU.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stU.operands.push_back(SIROperand::Register(ctx.retValI64Reg, SadTypeKind::Integer));
                            stU.comment = "store unknown-type finally return value as i64";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stU);
                        }

                        // (AR) تخزين typeCode في سجل النوع
                        // (EN) Store typeCode in type register
                        {
                            SIRInstruction stTC;
                            stTC.opcode = SIROpcode::STORE;
                            stTC.operands.push_back(SIROperand::ConstantI64(typeCode));
                            stTC.operands.push_back(SIROperand::Register(ctx.retTypeReg, SadTypeKind::Integer));
                            stTC.comment = "store finally return type code = " + std::to_string(typeCode);
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stTC);
                        }
                    }
                    // (AR) ارجع بدون قيمة → type = 0 (void) — مبدئياً 0 من التهيئة
                    // (EN) Return without value → type = 0 (void) — already 0 from initialization

                    // (AR) تعيين علامة has_return = 1 لإعلام finally بوجود ارجع منتظر
                    // (EN) Set has_return = 1 to notify finally that a return is pending
                    {
                        SIRInstruction stHR;
                        stHR.opcode = SIROpcode::STORE;
                        stHR.operands.push_back(SIROperand::ConstantI64(1));
                        stHR.operands.push_back(SIROperand::Register(ctx.hasReturnReg, SadTypeKind::Integer));
                        stHR.comment = "set finally has_return = 1 (ارجع intercepted by finally)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(stHR);
                    }

                    // (AR) القفز إلى كتلة finally بدلاً من إصدار RET مباشر
                    // (EN) Branch to finally block instead of emitting direct RET
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(ctx.finallyLabel)));

                    return; // (AR) لا نُصدر RET — finally سيُصدره بعد تنفيذه
                }

                // (AR) إذا كنا داخل كوروتين، نستخدم CORO_RETURN بدلاً من RET
                // (EN) Inside a coroutine, use CORO_RETURN instead of RET
                if (b_.currentFunction_ && b_.currentFunction_->isCoroutine && retStmt->value)
                {
                    emitPopFunctionCleanupHandler();
                    BuildResult valueResult = b_.buildExpression(retStmt->value.get());
                    SIRInstruction coroRet;
                    coroRet.opcode = SIROpcode::CORO_RETURN;
                    if (valueResult.isConstant && !valueResult.constantValue.empty())
                    {
                        switch (valueResult.type)
                        {
                        case SadTypeKind::Integer:
                            coroRet.operands.push_back(SIROperand::ConstantI64(std::stoll(valueResult.constantValue)));
                            break;
                        case SadTypeKind::Float:
                            coroRet.operands.push_back(SIROperand::ConstantF64(std::stod(valueResult.constantValue)));
                            break;
                        case SadTypeKind::String:
                            coroRet.operands.push_back(SIROperand::ConstantString(valueResult.constantValue));
                            break;
                        default:
                            coroRet.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
                        }
                    }
                    else
                    {
                        coroRet.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(coroRet);
                    return;
                }

                // (AR) ReturnStmt::value: ExprPtr (statements.h:268)
                // (EN) Build return instruction
                // (AR) ISSUE-056: تخطَّ المسار العاديّ إن كان التعبير فراغاً بُني مسبقاً
                //      (وإلّا أُعيد بناؤه فتَكرّر أثره الجانبيّ) — يُعامَل كإرجاع فارغ.
                // (EN) ISSUE-056: skip the normal path when a void expr was already prebuilt
                //      (else it re-runs its side effect) — treat as a bare void return.
                if (retStmt->value && !voidSideEffectPrebuilt)
                {
                    // (AR) بناء تعبير القيمة المُرجعة
                    // (EN) Build return value expression
                    // (AR) FIX X06: استخدام القيمة المُحسوبة مسبقاً إذا توفرت
                    // (EN) FIX X06: use prebuilt return value if available
                    BuildResult valueResult;
                    if (hasPrebuiltRet)
                    {
                        // (AR) تحميل القيمة من الـ alloca المؤقت (محسوب في body block قبل defer)
                        // (EN) Load value from temp alloca (computed in body block before defer)
                        std::string loadedRetReg = b_.newTempRegister();
                        SIRInstruction loadRetInst;
                        loadRetInst.opcode = SIROpcode::LOAD;
                        loadRetInst.result = SIROperand::Register(loadedRetReg, prebuiltRetType);
                        loadRetInst.operands.push_back(SIROperand::Register(prebuiltRetReg, prebuiltRetType));
                        loadRetInst.comment = "load prebuilt return value (FIX X06)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(loadRetInst);
                        valueResult.registerName = loadedRetReg;
                        valueResult.type = prebuiltRetType;
                        valueResult.isDirectValue = true;
                    }
                    else
                    {
                        valueResult = b_.buildExpression(retStmt->value.get());
                    }

                    // ================================================================
                    // (AR) تتبع نوع الصنف المُرجع:
                    //      إذا كانت القيمة المُرجعة كائن (لديها className)، نُسجّل ذلك في
                    //      b_.functionTable_ حتى يتمكن b_.buildFunctionCall لاحقاً من معرفة أن
                    //      هذه الدالة تُرجع كائناً من صنف معيّن.
                    //      هذا ضروري لتتبع نوع الكائن عبر استدعاءات الدوال.
                    //      مثال: دالة اصنع_نقطة() → ارجع جديد نقطة(1,2) → returnClassName = "نقطة"
                    //      بدون هذا: متغير ن = اصنع_نقطة() → ن.س يتعطل لأن المترجم لا يعرف أن ن كائن
                    // (EN) Track return class type:
                    //      If the returned value is an object (has className), record it in
                    //      b_.functionTable_ so b_.buildFunctionCall can later know this function
                    //      returns an object of a specific class.
                    //      Without this: var p = makePoint() → p.x crashes because compiler
                    //      doesn't know p is an object.
                    // ================================================================
                    if (!valueResult.className.empty() && b_.currentFunction_)
                    {
                        auto ftIt = b_.functionTable_.find(b_.currentFunction_->name);
                        if (ftIt != b_.functionTable_.end())
                        {
                            ftIt->second.returnClassName = valueResult.className;
                        }
                    }
                    // (AR) أيضاً: إذا كان التعبير المُرجع هو جديد ClassName() مباشرة
                    // (EN) Also: if the return expression is directly new ClassName()
                    if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(retStmt->value.get()))
                    {
                        if (b_.currentFunction_)
                        {
                            auto ftIt = b_.functionTable_.find(b_.currentFunction_->name);
                            if (ftIt != b_.functionTable_.end() && ftIt->second.returnClassName.empty())
                            {
                                ftIt->second.returnClassName = newExpr->className;
                            }
                        }
                    }
                    // (AR) وأيضاً: إذا كان المتغير المُرجع مُسجّل في b_.classInstanceTypes_
                    // (EN) Also: if returned variable is tracked in b_.classInstanceTypes_
                    if (valueResult.className.empty() && b_.currentFunction_)
                    {
                        // (AR) التحقق من b_.classInstanceTypes_ بالاسم بدون %
                        // (EN) Check b_.classInstanceTypes_ by name without %
                        std::string varName = valueResult.registerName;
                        if (!varName.empty() && varName[0] == '%')
                        {
                            varName = varName.substr(1);
                        }
                        auto ciIt = b_.classInstanceTypes_.find(varName);
                        if (ciIt != b_.classInstanceTypes_.end())
                        {
                            auto ftIt = b_.functionTable_.find(b_.currentFunction_->name);
                            if (ftIt != b_.functionTable_.end() && ftIt->second.returnClassName.empty())
                            {
                                ftIt->second.returnClassName = ciIt->second;
                            }
                        }
                    }

                    // (AR) توليد تعليمة RET مع القيمة
                    // (EN) Generate RET instruction with value
                    SIRInstruction retInst;
                    retInst.opcode = SIROpcode::RET;

                    // (AR) إذا كانت القيمة ثابتة، أرجعها مباشرة (يجب أن يكون هذا قبل فحص %)
                    // (EN) If value is constant, return it directly (must check before % check)
                    if (valueResult.isConstant && !valueResult.constantValue.empty())
                    {
                        // (AR) القيمة ثابتة
                        // (EN) Value is constant
                        switch (valueResult.type)
                        {
                        case SadTypeKind::Integer:
                            retInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valueResult.constantValue)));
                            break;
                        case SadTypeKind::Float:
                            retInst.operands.push_back(SIROperand::ConstantF64(std::stod(valueResult.constantValue)));
                            break;
                        case SadTypeKind::Boolean:
                            retInst.operands.push_back(SIROperand::ConstantBool(valueResult.constantValue == "true"));
                            break;
                        case SadTypeKind::String:
                            retInst.operands.push_back(SIROperand::ConstantString(valueResult.constantValue));
                            break;
                        // ================================================================
                        // (AR) إرجاع بنية إغلاق (Closure)
                        //      نظام الإغلاقات الجديد: buildLambdaExpr يُنشئ CLOSURE_CREATE
                        //      ويُرجع سجلاً حقيقياً (isConstant=false)
                        //      لن نصل هنا (حالة Function مع isConstant=true) إلا إذا
                        //      كانت دالة عادية مُمررة كقيمة — نُرجعها كـ Function operand
                        // (EN) Return closure struct
                        //      New closure system: buildLambdaExpr creates CLOSURE_CREATE
                        //      and returns real register (isConstant=false)
                        //      We only reach here (Function with isConstant=true) for
                        //      regular functions passed as values — return as Function operand
                        // ================================================================
                        case SadTypeKind::Function:
                        {
                            retInst.operands.push_back(SIROperand::Function(valueResult.constantValue));
                            break;
                        }
                        default:
                            retInst.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
                        }
                    }
                    // ================================================================
                    // (AR) فحص: هل القيمة في alloca فعلي (متغير محلي أو معامل دالة)؟
                    //      إذا كان اسم السجل يطابق متغيراً معروفاً → LOAD مطلوب
                    //      وإلا → السجل قيمة مباشرة (من عملية حسابية، استدعاء، إلخ)
                    // (EN) Check: is the value in an actual alloca (local var or param)?
                    //      If register name matches a known variable → LOAD needed
                    //      Otherwise → register is a direct value (from binary op, call, etc)
                    // ================================================================
                    else if (!valueResult.registerName.empty() && valueResult.registerName[0] == '%' && !valueResult.isFieldAccess && !valueResult.isDirectValue && valueResult.type != SadTypeKind::String && valueResult.type != SadTypeKind::Function && valueResult.className.empty())
                    {
                        // (AR) تحقق: هل السجل يشير إلى متغير محلي (alloca)؟
                        //      المتغيرات المحلية: %variableName (غير رقمية)
                        //      السجلات المؤقتة: %N (رقمية) — لا تحتاج LOAD
                        // (EN) Check: does register refer to a local variable (alloca)?
                        //      Local variables: %variableName (non-numeric)
                        //      Temp registers: %N (numeric) — no LOAD needed
                        std::string varName = valueResult.registerName.substr(1); // إزالة %
                        VariableInfo *maybeVar = b_.lookupVariable(varName);
                        bool isAllocaVar = maybeVar != nullptr;

                        // (AR) تحقق إضافي: إذا لم يكن متغيراً لكنه بادئة inlining
                        //      مثل %_inl0_variable → نحاول البحث بالاسم بعد البادئة
                        // (EN) Extra check: if not a variable but has inlining prefix
                        //      like %_inl0_variable → try looking up after prefix
                        if (!isAllocaVar && varName.size() > 4 && varName.substr(0, 4) == "_inl")
                        {
                            size_t underscorePos = varName.find('_', 4);
                            if (underscorePos != std::string::npos && underscorePos + 1 < varName.size())
                            {
                                std::string originalName = varName.substr(underscorePos + 1);
                                VariableInfo *inlinedVar = b_.lookupVariable(originalName);
                                isAllocaVar = inlinedVar != nullptr;
                            }
                        }

                        if (isAllocaVar)
                        {
                            // (AR) القيمة في عنوان alloca محلي — نحتاج لتحميلها
                            // (EN) Value is in local alloca address — need to load it
                            std::string loadedReg = b_.newTempRegister();

                            SIRInstruction loadInst;
                            loadInst.opcode = SIROpcode::LOAD;
                            loadInst.result = SIROperand::Register(loadedReg, valueResult.type);
                            loadInst.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));

                            if (b_.currentBlock_)
                            {
                                b_.currentBlock_->addInstruction(loadInst);
                            }

                            SIROperand retOperand = SIROperand::Register(loadedReg, valueResult.type);
                            retInst.operands.push_back(retOperand);
                        }
                        else
                        {
                            // (AR) سجل مؤقت (قيمة مباشرة) — لا نحتاج LOAD
                            // (EN) Temp register (direct value) — no LOAD needed
                            SIROperand retOperand = SIROperand::Register(valueResult.registerName, valueResult.type);
                            retInst.operands.push_back(retOperand);
                        }
                    }
                    else
                    {
                        // (AR) قيمة في سجل مؤقت
                        // (EN) Value in temporary register
                        SIROperand retOperand = SIROperand::Register(valueResult.registerName, valueResult.type);
                        retInst.operands.push_back(retOperand);
                    }

                    if (b_.currentBlock_)
                    {
                        if (b_.finallyStack_.empty())
                        {
                            emitPopFunctionCleanupHandler();
                        }
                        b_.currentBlock_->addInstruction(retInst);
                    }
                }
                else
                {
                    // (AR) توليد تعليمة RET_VOID
                    // (EN) Generate RET_VOID instruction
                    SIRInstruction retInst;
                    retInst.opcode = SIROpcode::RET_VOID;
                    if (b_.currentBlock_)
                    {
                        if (b_.finallyStack_.empty())
                        {
                            emitPopFunctionCleanupHandler();
                        }
                        b_.currentBlock_->addInstruction(retInst);
                    }
                }
            }

            // ============================================================================
            // buildBreakStatement - بناء جملة break
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:417
            // التوقيع / Signature: void buildBreakStatement(AST::BreakStmt* breakStmt);
            //
            // BreakStmt (statements.h:360):
            // - لا توجد أعضاء إضافية
            // - No additional members
            // ============================================================================
            void StatementBuilder::buildBreakStatement(AST::BreakStmt *breakStmt)
            {
                if (!breakStmt)
                {
                    return;
                }

                // ========================================================================
                // (AR) جملة break: القفز إلى نهاية الحلقة الحالية
                //      نستخدم مكدس الحلقات (b_.loopStack_) للحصول على تسمية كتلة الخروج
                //      ثم نولّد تعليمة قفز غير شرطي (BR) إلى تلك الكتلة
                //
                // (EN) break statement: Jump to the end of current loop
                //      We use the loop stack (b_.loopStack_) to get the exit block label
                //      Then generate an unconditional branch (BR) to that block
                // ========================================================================
                LoopContext *loop = b_.getCurrentLoop();
                if (!loop)
                {
                    b_.errors_.push_back("(AR) خطأ: جملة 'قف' خارج حلقة. (EN) Error: 'break' outside of loop.");
                    return;
                }

                // (AR) الحاجز ٧: «قف» يخرج من «حاول» الواقعة بين الحلقة ونقطة الكسر ⇒
                //      خفّض __sad_try_active بعددها. / (EN) Barrier 7: break leaves the «try»s
                //      between the loop and the break point ⇒ decrement __sad_try_active.
                for (int i = loop->tryDepthAtEntry; i < b_.currentTryDepth_ && b_.currentBlock_; ++i)
                {
                    SIRInstruction tryExit;
                    tryExit.opcode = SIROpcode::CALL;
                    tryExit.operands.push_back(SIROperand::Function("__sad_try_exit"));
                    tryExit.comment = "barrier7: exit active try (break)";
                    b_.currentBlock_->addInstruction(tryExit);
                }

                // (AR) توليد قفز غير شرطي إلى كتلة خروج الحلقة
                // (EN) Generate unconditional branch to loop exit block
                SIROperand exitLabel = SIROperand::Label(loop->breakLabel);
                SIRInstruction brInst = SIRInstruction::Branch(exitLabel);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brInst);
                }

                // (AR) إنشاء كتلة جديدة للكود بعد break (كود ميت)
                //      هذا ضروري لأن LLVM يتطلب أن كل كتلة تنتهي بمُنهِي واحد فقط
                // (EN) Create new block for code after break (dead code)
                //      Required because LLVM needs each block to end with exactly one terminator
                std::string afterBreakLabel = b_.newLabel("after_break");
                auto afterBreakBlock = b_.createBasicBlock(afterBreakLabel);
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(afterBreakBlock);
                }
                b_.currentBlock_ = afterBreakBlock;
            }

            // ============================================================================
            // buildContinueStatement - بناء جملة continue
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:425
            // التوقيع / Signature: void buildContinueStatement(AST::ContinueStmt* continueStmt);
            //
            // ContinueStmt (statements.h:392):
            // - لا توجد أعضاء إضافية
            // - No additional members
            // ============================================================================
            void StatementBuilder::buildContinueStatement(AST::ContinueStmt *continueStmt)
            {
                if (!continueStmt)
                {
                    return;
                }

                // ========================================================================
                // (AR) جملة continue: القفز إلى بداية تكرار الحلقة التالي
                //      - في حلقة while: نقفز إلى كتلة الشرط (while_cond)
                //      - في حلقة for: نقفز إلى كتلة الزيادة (for_inc)
                //        ثم الزيادة ستقفز بدورها للشرط
                //      نستخدم continueLabel من مكدس الحلقات
                //
                // (EN) continue statement: Jump to next loop iteration
                //      - In while loop: jump to condition block (while_cond)
                //      - In for loop: jump to increment block (for_inc)
                //        then increment jumps to condition
                //      We use continueLabel from the loop stack
                // ========================================================================
                LoopContext *loop = b_.getCurrentLoop();
                if (!loop)
                {
                    b_.errors_.push_back("(AR) خطأ: جملة 'أكمل' خارج حلقة. (EN) Error: 'continue' outside of loop.");
                    return;
                }

                // (AR) الحاجز ٧: «أكمل» يخرج من «حاول» الواقعة بين الحلقة ونقطة الاستمرار ⇒
                //      خفّض __sad_try_active بعددها. / (EN) Barrier 7: continue leaves the «try»s
                //      between the loop and the continue point ⇒ decrement __sad_try_active.
                for (int i = loop->tryDepthAtEntry; i < b_.currentTryDepth_ && b_.currentBlock_; ++i)
                {
                    SIRInstruction tryExit;
                    tryExit.opcode = SIROpcode::CALL;
                    tryExit.operands.push_back(SIROperand::Function("__sad_try_exit"));
                    tryExit.comment = "barrier7: exit active try (continue)";
                    b_.currentBlock_->addInstruction(tryExit);
                }

                // (AR) توليد قفز غير شرطي إلى كتلة استمرار الحلقة
                // (EN) Generate unconditional branch to loop continue block
                SIROperand continueLabel = SIROperand::Label(loop->continueLabel);
                SIRInstruction brInst = SIRInstruction::Branch(continueLabel);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brInst);
                }

                // (AR) إنشاء كتلة جديدة للكود بعد continue (كود ميت)
                //      هذا ضروري لأن LLVM يتطلب أن كل كتلة تنتهي بمُنهِي واحد فقط
                // (EN) Create new block for code after continue (dead code)
                //      Required because LLVM needs each block to end with exactly one terminator
                std::string afterContinueLabel = b_.newLabel("after_continue");
                auto afterContinueBlock = b_.createBasicBlock(afterContinueLabel);
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(afterContinueBlock);
                }
                b_.currentBlock_ = afterContinueBlock;
            }

            // ============================================================================
            // buildAssignment - بناء إسناد متغير
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:429
            // التوقيع / Signature: void buildAssignment(AST::AssignExpr* assignment);
            //
            // المعاملات / Parameters:
            // - assignment: AST::AssignExpr* = Sad::AST::AssignExpr* (sir_builder.h:429)
            //
            // AssignExpr Members (expressions.h:247):
            // - name: std::string (line 249)
            // - value: ExprPtr (line 250)
            //
            // الدوال المستدعاة / Called functions:
            // - b_.buildExpression: sir_builder.h:432
            // - b_.lookupVariable: sir_builder.h:597
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad

