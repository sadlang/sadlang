/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/memory/closure_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include "sad_dyn_repr.h"
#include "sir_constants.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {

        // ============================================================================
        // (AR) عمليات الإغلاق والذاكرة - bitwise, heap, closures, environment
        // (EN) Closure and memory operations - bitwise, heap, closures, environment
        // (AR) تم فصل هذا الملف عن llvm_codegen_concurrency.cpp وفق قاعدة CW-05
        // ============================================================================
        llvm::Value *ClosureCodeGen::emitSar(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "SAR"}});
                return nullptr;
            }
            llvm::Value *lhs = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *rhs = cg_.resolveOperand(inst->operands[1]);
            if (!lhs || !rhs)
                return nullptr;
            llvm::Value *result = cg_.builder_->CreateAShr(lhs, rhs, "sar");
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ClosureCodeGen::emitRol(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ROL"}});
                return nullptr;
            }
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *amount = cg_.resolveOperand(inst->operands[1]);
            if (!val || !amount)
                return nullptr;
            // ROL(x, n) = (x << n) | (x >> (64 - n))
            auto *bits = llvm::ConstantInt::get(cg_.getInt64Type(), 64);
            llvm::Value *shl = cg_.builder_->CreateShl(val, amount, "rol.shl");
            llvm::Value *sub = cg_.builder_->CreateSub(bits, amount, "rol.sub");
            llvm::Value *shr = cg_.builder_->CreateLShr(val, sub, "rol.shr");
            llvm::Value *result = cg_.builder_->CreateOr(shl, shr, "rol");
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // Phase N: Missing Control Flow / تدفق تحكم ناقص
        // ============================================================================

        llvm::Value *ClosureCodeGen::emitCallIndirect(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "CALL_INDIRECT"}});
                return nullptr;
            }
            // operand[0] = function pointer, rest = args
            llvm::Value *fnPtr = cg_.resolveOperand(inst->operands[0]);
            if (!fnPtr)
                return nullptr;

            // If fnPtr is an integer, convert to pointer
            if (fnPtr->getType()->isIntegerTy())
            {
                fnPtr = cg_.builder_->CreateIntToPtr(fnPtr, llvm::PointerType::getUnqual(*cg_.context_), "fnptr");
            }

            // Collect arguments
            std::vector<llvm::Value *> args;
            std::vector<llvm::Type *> argTypes;
            for (size_t i = 1; i < inst->operands.size(); i++)
            {
                llvm::Value *arg = cg_.resolveOperand(inst->operands[i]);
                if (arg)
                {
                    args.push_back(arg);
                    argTypes.push_back(arg->getType());
                }
            }

            // Determine return type
            llvm::Type *retType = cg_.getInt64Type();
            if (inst->result.has_value())
            {
                if (inst->result->dataType == SadTypeKind::Void)
                    retType = llvm::Type::getVoidTy(*cg_.context_);
                else if (inst->result->dataType == SadTypeKind::Float)
                    retType = cg_.getDoubleType();
                else if (inst->result->dataType == SadTypeKind::Boolean)
                    retType = llvm::Type::getInt1Ty(*cg_.context_);
            }

            auto *funcType = llvm::FunctionType::get(retType, argTypes, false);
            llvm::Value *result = cg_.builder_->CreateCall(funcType, fnPtr, args,
                                                       retType->isVoidTy() ? "" : "call_indirect");

            if (inst->result.has_value() && !retType->isVoidTy())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // Phase N: Missing Memory Operations / عمليات ذاكرة ناقصة
        // ============================================================================

        llvm::Value *ClosureCodeGen::emitAllocHeap(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ALLOC_HEAP"}});
                return nullptr;
            }
            llvm::Value *size = cg_.resolveOperand(inst->operands[0]);
            if (!size)
                return nullptr;


            llvm::Value *result = cg_.emitMalloc(size, "heap_alloc");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ClosureCodeGen::emitFreeMem(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "FREE"}});
                return nullptr;
            }
            llvm::Value *ptr = cg_.resolveOperand(inst->operands[0]);
            if (!ptr)
                return nullptr;

            if (ptr->getType()->isIntegerTy())
            {
                ptr = cg_.builder_->CreateIntToPtr(ptr, llvm::PointerType::getUnqual(*cg_.context_), "free.ptr");
            }

            cg_.emitFreeCall(ptr);

            return llvm::ConstantInt::get(cg_.getInt64Type(), 0);
        }

        // ============================================================================
        // (AR) emitClosureCreate — إنشاء بنية إغلاق على الكومة
        //      البنية: { fn_ptr: i64, env_ptr: i64, dynbr_ptr: i64 } (sir_constants.h)
        //      - fn_ptr: مؤشر لدالة اللامدا (ptrtoint) — عقد i64 التاريخي بحرفه
        //      - env_ptr: مؤشر لمصفوفة المتغيرات الملتقطة (0 إن لم تكن)
        //      - dynbr_ptr: جسر البروتوكول الموسوم (%SadDyn) — 0 في الوضع الحر
        //
        //      المعاملات:
        //      - operands[0]: Function (@fn_name) — اسم اللامدا
        //      - operands[1..N]: قيم المتغيرات الملتقطة
        //
        //      الخطوات:
        //      1. إذا كانت هناك التقاطات: تخصيص مصفوفة env على الكومة
        //         وتخزين قيم الالتقاطات فيها
        //      2. توليد جسر البروتوكول الموسوم (خارج الوضع الحر)
        //      3. تخصيص بنية الإغلاق (kClosureStructBytes) على الكومة
        //      4. تخزين fn_ptr@0 وenv_ptr@1 وdynbr_ptr@2 وإرجاع المؤشر كـ i64
        //
        // (EN) emitClosureCreate — Create closure struct on heap
        //      Struct layout: { fn_ptr: i64, env_ptr: i64, dynbr_ptr: i64 }
        //      (see sir_constants.h; slots 0/1 are the historical i64 contract)
        //      Steps:
        //      1. If captures: allocate env array, store capture values
        //      2. Synthesize the tagged-protocol bridge (skipped when freestanding)
        //      3. Allocate closure struct (kClosureStructBytes)
        //      4. Store fn_ptr@0, env_ptr@1, dynbr_ptr@2; return pointer as i64
        // ============================================================================
        llvm::Value *ClosureCodeGen::emitClosureCreate(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "CLOSURE_CREATE"}});
                return nullptr;
            }

            // (AR) الحصول على مؤشر دالة اللامدا
            // (EN) Get lambda function pointer
            llvm::Function *lambdaFn = nullptr;
            std::string fnName = inst->operands[0].name;
            lambdaFn = cg_.module_->getFunction(fnName);
            if (!lambdaFn)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", std::string("CLOSURE_CREATE: function '") + fnName + "' not found in module"}});
                return nullptr;
            }

            // ================================================================
            // (AR) فحص: هل الدالة تأخذ __env كمعامل أخير؟
            //      إذا لم تأخذ __env (دالة عادية تُستخدم كمرجع)
            //      نُنشئ wrapper function تلقائياً يأخذ نفس المعاملات + __env
            //      ويستدعي الدالة الأصلية بدون __env
            //      هذا ضروري لأن CLOSURE_CALL يُضيف env_ptr دائماً
            // (EN) Check: does the function take __env as last parameter?
            //      If not (regular function used as reference), create a wrapper
            //      that takes same params + __env and calls original without __env
            //      Required because CLOSURE_CALL always appends env_ptr
            // ================================================================
            bool needsWrapper = false;
            if (inst->comment.find("func-ref:") == 0)
            {
                // (AR) دالة عادية مُستخدمة كمرجع — تحتاج wrapper
                needsWrapper = true;
            }

            // ================================================================
            // (AR) [إصلاح UB في Release — شامل] — BF-04/CW-13
            //      بروتوكول CLOSURE_CALL يفترض دائماً أن اللامدا ترجع i64.
            //      أي نوع إرجاع آخر يُسبب تعارض نوع → سلوك غير محدد في Release.
            //      الحالات المُصلَحة:
            //        • i1  (bool)   → i64: ZExt (توسيع صفري)
            //        • i32 (int32)  → i64: SExt (توسيع بالإشارة)
            //        • double/float → i64: BitCast (إعادة تفسير البتات)
            //        • ptr          → i64: PtrToInt
            //
            // (AR) تمييز مسارين:
            //      1. isFuncRefWrapper: دالة عادية تُضاف لها __env
            //      2. isRetConvWrapper: لامدا تحمل __env أصلاً، تحتاج تحويل نوع إرجاع فقط
            //
            // (EN) [Comprehensive UB fix in Release] — BF-04/CW-13
            //      Two distinct wrapper paths:
            //      1. isFuncRefWrapper: regular func, __env added as extra param
            //      2. isRetConvWrapper: lambda already has __env, only return type conversion needed
            // ================================================================
            bool isFuncRefWrapper = needsWrapper; // set by func-ref check above
            bool isRetConvWrapper = false;
            {
                llvm::Type *origRet = lambdaFn->getReturnType();
                isRetConvWrapper = !isFuncRefWrapper &&
                                   !origRet->isVoidTy() &&
                                   !origRet->isIntegerTy(64) &&
                                   (origRet->isIntegerTy(1) ||  // bool
                                    origRet->isIntegerTy(32) || // int32
                                    origRet->isDoubleTy() ||    // double
                                    origRet->isFloatTy() ||     // float
                                    origRet->isPointerTy());    // ptr
                if (isRetConvWrapper)
                    needsWrapper = true;
            }

            llvm::Function *targetFn = lambdaFn;
            if (needsWrapper)
            {
                // (AR) إنشاء wrapper: نفس المعاملات + i64 __env → يستدعي الأصلية بدون __env
                std::string wrapperName = "__wrap_" + fnName;
                llvm::Function *existingWrapper = cg_.module_->getFunction(wrapperName);
                if (existingWrapper)
                {
                    targetFn = existingWrapper;
                }
                else
                {
                    llvm::Type *origRetType = lambdaFn->getReturnType();

                    // ============================================================
                    // (AR) تحديد نوع إرجاع الـ wrapper:
                    //      - func-ref بدون تحويل نوع: إرجاع i64 (البروتوكول)
                    //      - lambda مع تحويل نوع إرجاع: i64 دائماً
                    //      - void: يبقى void
                    // (EN) Determine wrapper return type:
                    //      - func-ref without return conv: i64 (protocol)
                    //      - lambda with return type conv: always i64
                    //      - void: stays void
                    // ============================================================
                    bool needsRetConv = isRetConvWrapper &&
                                        !origRetType->isVoidTy() &&
                                        !origRetType->isIntegerTy(64);
                    llvm::Type *wrapperRetType = origRetType->isVoidTy()
                                                     ? origRetType
                                                     : (isFuncRefWrapper ? origRetType
                                                                         : cg_.getInt64Type());

                    // ============================================================
                    // (AR) مسار 1: func-ref — أضف __env كمعامل أخير (تُتجاهل)
                    //      لأن الدالة الأصلية لا تعرف __env.
                    // (AR) مسار 2: lambda conv — نفس المعاملات بالضبط (تحمل __env أصلاً).
                    //
                    // (EN) Path 1: func-ref — add __env as extra param (ignored)
                    //      because original function doesn't know about __env.
                    // (EN) Path 2: lambda conv — exact same params (already has __env).
                    // ============================================================
                    std::vector<llvm::Type *> wrapperParamTypes;
                    for (auto &arg : lambdaFn->args())
                    {
                        wrapperParamTypes.push_back(arg.getType());
                    }
                    if (isFuncRefWrapper)
                    {
                        // (AR) func-ref: أضف __env (سيُتجاهل في الاستدعاء الداخلي)
                        // (EN) func-ref: add __env (ignored in inner call)
                        wrapperParamTypes.push_back(cg_.getInt64Type());
                    }
                    // (AR) lambda conv: المعاملات كافية، لا إضافة

                    auto *wrapperFnType = llvm::FunctionType::get(
                        wrapperRetType, wrapperParamTypes, false);
                    auto *wrapperFn = llvm::Function::Create(
                        wrapperFnType, llvm::Function::InternalLinkage, wrapperName, cg_.module_.get());

                    // (AR) بناء جسم الـ wrapper
                    auto *entryBB = llvm::BasicBlock::Create(*cg_.context_, "entry", wrapperFn);
                    llvm::IRBuilder<> wrapperBuilder(entryBB);

                    // ============================================================
                    // (AR) بناء قائمة الوسائط للاستدعاء الداخلي:
                    //      - func-ref: كل معاملات الدالة الأصلية (بدون __env الجديدة)
                    //      - lambda conv: كل معاملات الـ wrapper (مطابقة للامدا)
                    // (EN) Build call args:
                    //      - func-ref: all original fn params (without new __env)
                    //      - lambda conv: all wrapper params (same as lambda)
                    // ============================================================
                    std::vector<llvm::Value *> callArgs;
                    size_t callArgCount = isFuncRefWrapper
                                              ? lambdaFn->arg_size()  // استبعاد __env المُضافة
                                              : lambdaFn->arg_size(); // lambda: كل المعاملات
                    for (size_t i = 0; i < callArgCount; i++)
                    {
                        callArgs.push_back(wrapperFn->getArg(static_cast<unsigned>(i)));
                    }

                    llvm::Value *retVal = wrapperBuilder.CreateCall(
                        lambdaFn, callArgs,
                        origRetType->isVoidTy() ? "" : "wrap.call");

                    // ============================================================
                    // (AR) تحويل قيمة الإرجاع حسب النوع
                    // (EN) Convert return value based on type
                    // ============================================================
                    if (origRetType->isVoidTy())
                    {
                        wrapperBuilder.CreateRetVoid();
                    }
                    else if (!needsRetConv)
                    {
                        // (AR) func-ref أو i64 — لا تحويل
                        // (EN) func-ref or i64 — no conversion
                        wrapperBuilder.CreateRet(retVal);
                    }
                    else if (origRetType->isIntegerTy(1))
                    {
                        // (AR) i1 → i64: توسيع صفري
                        // (EN) i1 → i64: zero-extend
                        wrapperBuilder.CreateRet(
                            wrapperBuilder.CreateZExt(retVal, cg_.getInt64Type(), "bool.to.i64"));
                    }
                    else if (origRetType->isIntegerTy(32))
                    {
                        // (AR) i32 → i64: توسيع بالإشارة
                        // (EN) i32 → i64: sign-extend
                        wrapperBuilder.CreateRet(
                            wrapperBuilder.CreateSExt(retVal, cg_.getInt64Type(), "i32.to.i64"));
                    }
                    else if (origRetType->isDoubleTy())
                    {
                        // (AR) double → i64: إعادة تفسير البتات (IEEE 754)
                        // (EN) double → i64: bit-reinterpret (IEEE 754)
                        wrapperBuilder.CreateRet(
                            wrapperBuilder.CreateBitCast(retVal, cg_.getInt64Type(), "dbl.to.i64"));
                    }
                    else if (origRetType->isFloatTy())
                    {
                        // (AR) float → double → i64: توسيع ثم إعادة تفسير
                        // (EN) float → double → i64: extend then bit-reinterpret
                        llvm::Value *asDouble = wrapperBuilder.CreateFPExt(
                            retVal, cg_.getDoubleType(), "flt.to.dbl");
                        wrapperBuilder.CreateRet(
                            wrapperBuilder.CreateBitCast(asDouble, cg_.getInt64Type(), "dbl.to.i64"));
                    }
                    else if (origRetType->isPointerTy())
                    {
                        // (AR) ptr → i64: PtrToInt
                        // (EN) ptr → i64: PtrToInt
                        wrapperBuilder.CreateRet(
                            wrapperBuilder.CreatePtrToInt(retVal, cg_.getInt64Type(), "ptr.to.i64"));
                    }
                    else
                    {
                        // (AR) نوع غير متوقع — إرجاع كما هو (احتياطي)
                        // (EN) Unexpected type — return as-is (fallback)
                        wrapperBuilder.CreateRet(retVal);
                    }

                    targetFn = wrapperFn;
                }
            }

            llvm::Value *fnPtrI64 = cg_.builder_->CreatePtrToInt(targetFn, cg_.getInt64Type(), "fn.ptr.i64");

            // (AR) عدد المتغيرات الملتقطة
            // (EN) Number of captured variables
            size_t numCaptures = inst->operands.size() - 1;

            // (AR) إنشاء مصفوفة البيئة (env) إذا كانت هناك التقاطات
            // (EN) Create environment array if there are captures
            llvm::Value *envI64 = llvm::ConstantInt::get(cg_.getInt64Type(), 0); // default: no env
            if (numCaptures > 0)
            {
                // (AR) تخصيص مصفوفة env على الكومة: numCaptures * 8 بايت
                // (EN) Allocate env array on heap: numCaptures * 8 bytes
                llvm::Value *envSize = llvm::ConstantInt::get(cg_.getInt64Type(), numCaptures * 8);

                llvm::Value *envPtr = cg_.emitMalloc(envSize, "env.alloc");

                // (AR) تخزين كل قيمة ملتقطة في env[i]
                // (EN) Store each captured value at env[i]
                for (size_t i = 0; i < numCaptures; i++)
                {
                    llvm::Value *capVal = cg_.resolveOperand(inst->operands[1 + i]);
                    if (!capVal)
                        continue;

                    // (AR) التأكد من أن القيمة i64
                    // (EN) Ensure value is i64
                    if (!capVal->getType()->isIntegerTy(64))
                    {
                        if (capVal->getType()->isIntegerTy())
                            capVal = cg_.builder_->CreateZExt(capVal, cg_.getInt64Type(), "cap.ext");
                        else if (capVal->getType()->isDoubleTy())
                            capVal = cg_.builder_->CreateBitCast(capVal, cg_.getInt64Type(), "cap.dbl2i64");
                        else if (capVal->getType()->isPointerTy())
                            capVal = cg_.builder_->CreatePtrToInt(capVal, cg_.getInt64Type(), "cap.ptr2i64");
                    }

                    llvm::Value *idx = llvm::ConstantInt::get(cg_.getInt64Type(), i);
                    llvm::Value *gep = cg_.builder_->CreateGEP(cg_.getInt64Type(), envPtr, idx, "env.slot");
                    cg_.builder_->CreateStore(capVal, gep);
                }

                // (AR) تحويل مؤشر البيئة إلى i64
                // (EN) Convert env pointer to i64
                envI64 = cg_.builder_->CreatePtrToInt(envPtr, cg_.getInt64Type(), "env.i64");
            }

            // ════════════════════════════════════════════════════════════════
            // (AR) [موجة الجسر الموسوم] توليدُ جسرِ البروتوكولِ الموسوم للهدف:
            //      غلافٌ يقبل N×%SadDyn + i64 __env ويُعيد %SadDyn — يفكُّ كلَّ
            //      وسيطٍ إلى نوعِ معاملِ الهدفِ الساكنِ (جدولُ التحويلِ الموحَّد
            //      coerceToParamType يحترمُ الوسمَ: عشريٌّ⇒fptosi/بتّات، غيابٌ⇒بذرة
            //      العدم)، ينادي الهدفَ الخام، ثم يُغلِّفُ العائدَ بوسمِه الحقيقيِّ
            //      (نوعُ العائدِ يُمرَّرُ من الواجهةِ في لاحقةِ التعليقِ «;ret:» —
            //      فمؤشّرُ ptr وحدَه لا يفرّقُ نصًّا عن مصفوفة). مواقعُ النداءِ
            //      مجهولةُ الهدفِ (مرجعُ دالّةٍ معاملًا — وسمُ dynproto) وحدَها
            //      تسلكُه؛ ومسارُ i64 الخامُّ (خانتا 0/1) لا يُمَسُّ بحرفِه.
            // (EN) [Tagged-bridge wave] Synthesize the target's tagged-protocol
            //      bridge: a wrapper taking N×%SadDyn + i64 __env and returning
            //      %SadDyn — unboxes each argument to the target's static param
            //      type via the single conversion table (tag-respecting), calls
            //      the raw target, and boxes the return with its true kind
            //      (carried from the frontend in the «;ret:» comment suffix,
            //      since a bare ptr cannot tell a string from an array). Only
            //      unknown-target call sites (func-ref as a parameter — the
            //      dynproto marker) take it; the raw i64 path (slots 0/1) is
            //      byte-for-byte untouched.
            // ════════════════════════════════════════════════════════════════
            llvm::Value *dynBridgeI64 = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
            // (AR) الوضعُ الحرُّ (نواةُ النحلة/UEFI): لا جسرَ يُولَّد — درسُ «ميزانيّةِ
            //      الحافةِ»: نموُّ كودٍ صامتٌ لكلِّ مغلاقٍ يجرُّ آلاتِ SadDyn إلى ring-0،
            //      والواجهةُ لا تَسِمُ dynproto في هذا الوضعِ أصلًا فالخانةُ لا تُقرأ.
            // (EN) Freestanding (nahla/UEFI kernels): no bridge is synthesized — the
            //      edge-budget lesson: silent per-closure code growth would drag the
            //      SadDyn machinery into ring-0, and the frontend never marks dynproto
            //      in this mode, so the slot is never read.
            if (!cg_.freestanding_)
            {
                // (AR) نوعُ عائدِ الهدفِ من لاحقةِ التعليق (إن وُجدت).
                // (EN) Target return kind from the comment suffix (if present).
                SadTypeKind bridgeRetKind = SadTypeKind::Unknown;
                const size_t retMarkPos =
                    inst->comment.find(Sad::Compiler::kClosureRetKindMarker);
                if (retMarkPos != std::string::npos)
                {
                    const std::string retNum = inst->comment.substr(
                        retMarkPos + std::strlen(Sad::Compiler::kClosureRetKindMarker));
                    if (!retNum.empty())
                        bridgeRetKind = static_cast<SadTypeKind>(std::atoi(retNum.c_str()));
                }

                // (AR) العدّةُ المرئيّةُ للمستخدم: لامدا تحملُ __env أخيرًا فتُستثنى؛
                //      ومرجعُ الدالّةِ العاديُّ لا يعرفُ __env أصلًا.
                // (EN) User-visible arity: a lambda carries __env last (excluded);
                //      a plain func-ref never takes __env.
                const bool bridgeTargetTakesEnv = !isFuncRefWrapper;
                const size_t bridgeUserArity =
                    (bridgeTargetTakesEnv && lambdaFn->arg_size() > 0)
                        ? lambdaFn->arg_size() - 1
                        : lambdaFn->arg_size();

                const std::string bridgeName =
                    std::string(Sad::Compiler::kClosureDynBridgePrefix) + fnName;
                llvm::Function *bridgeFn = cg_.module_->getFunction(bridgeName);
                if (!bridgeFn)
                {
                    llvm::StructType *dynTy = getSadDynType(*cg_.context_);
                    std::vector<llvm::Type *> bridgeParams(bridgeUserArity,
                                                           static_cast<llvm::Type *>(dynTy));
                    bridgeParams.push_back(cg_.getInt64Type()); // __env
                    auto *bridgeFnType = llvm::FunctionType::get(dynTy, bridgeParams, false);
                    bridgeFn = llvm::Function::Create(bridgeFnType,
                                                      llvm::Function::InternalLinkage,
                                                      bridgeName, cg_.module_.get());
                    auto *bridgeEntry = llvm::BasicBlock::Create(
                        *cg_.context_, Sad::Compiler::kEntryBlockName, bridgeFn);
                    // (AR) جدولُ التحويلِ الموحَّدُ يبثُّ عبرَ باني cg_ — نُحوِّلُ نقطةَ
                    //      الإدراجِ مؤقّتًا إلى جسمِ الجسرِ ثم نُعيدُها.
                    // (EN) The unified conversion table emits through cg_'s builder —
                    //      temporarily retarget the insert point into the bridge body.
                    auto savedIP = cg_.builder_->saveIP();
                    cg_.builder_->SetInsertPoint(bridgeEntry);

                    // (AR) [حارسُ المولِّد] هدفٌ مولِّدٌ لا يُنادى عبرَ الجسرِ: مقبضُه
                    //      الخامُّ ليس قيمةً تُغلَّفُ، وتغليفُه كان يطبعُ قمامةً بايتيّةً
                    //      **بنجاحٍ ظاهريٍّ** حيث كان الأساسُ ينهار AV — انحدارُ قابليّةِ
                    //      اكتشافٍ مقيس. نرفعُ RUN033 صريحًا (المفسّرُ يدعمُ الحالةَ —
                    //      تباعُدٌ معلَنٌ صاخبٌ لا كذبةٌ صامتة).
                    // (EN) [Generator guard] A generator target must not be called
                    //      through the bridge: its raw handle is not a boxable value,
                    //      and boxing it printed byte garbage with a green exit code
                    //      where the baseline crashed with an AV — a measured
                    //      detectability regression. Raise RUN033 explicitly (the
                    //      interpreter supports the case — a declared LOUD divergence,
                    //      not a silent lie).
                    if (inst->comment.find(Sad::Compiler::kClosureGenMarker) !=
                        std::string::npos)
                    {
                        cg_.emitNullRaiseBody(
                            ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                            {{"type", Sad::Compiler::kGeneratorDynCallTypeText},
                             {"operation", Sad::Compiler::kGeneratorDynCallOperationText}},
                            bridgeName);
                        cg_.builder_->CreateUnreachable();
                    }
                    else
                    {
                        std::vector<llvm::Value *> bridgeCallArgs;
                        for (size_t i = 0; i < bridgeUserArity; ++i)
                        {
                            llvm::Type *want =
                                lambdaFn->getFunctionType()->getParamType(static_cast<unsigned>(i));
                            bridgeCallArgs.push_back(coerceToParamType(
                                cg_, bridgeFn->getArg(static_cast<unsigned>(i)), want,
                                SadTypeKind::Any));
                        }
                        if (bridgeTargetTakesEnv && lambdaFn->arg_size() > 0)
                            bridgeCallArgs.push_back(
                                bridgeFn->getArg(static_cast<unsigned>(bridgeUserArity)));

                        llvm::Type *bridgeTargetRet = lambdaFn->getReturnType();
                        llvm::Value *rawRet = cg_.builder_->CreateCall(
                            lambdaFn, bridgeCallArgs,
                            bridgeTargetRet->isVoidTy() ? "" : "dynbr.call");

                        llvm::Value *boxedRet = nullptr;
                        if (bridgeTargetRet->isVoidTy())
                        {
                            boxedRet = makeDyn(
                                cg_,
                                llvm::ConstantInt::get(llvm::Type::getInt8Ty(*cg_.context_),
                                                       DynKind::Void),
                                llvm::ConstantInt::get(cg_.getInt64Type(), 0));
                        }
                        else
                        {
                            SadTypeKind boxKind = bridgeRetKind;
                            if (boxKind == SadTypeKind::Unknown || boxKind == SadTypeKind::Void)
                            {
                                // (AR) احتياطٌ بلا لاحقة: الاستدلالُ من نوع LLVM وحدَه.
                                // (EN) Fallback without a suffix: infer from the LLVM type alone.
                                if (bridgeTargetRet->isDoubleTy())
                                    boxKind = SadTypeKind::Float;
                                else if (bridgeTargetRet->isIntegerTy(1))
                                    boxKind = SadTypeKind::Boolean;
                                else if (bridgeTargetRet->isPointerTy())
                                    boxKind = SadTypeKind::String;
                                else
                                    boxKind = SadTypeKind::Integer;
                            }
                            boxedRet = toDyn(cg_, rawRet, boxKind);
                        }
                        cg_.builder_->CreateRet(boxedRet);
                    }
                    cg_.builder_->restoreIP(savedIP);
                }
                dynBridgeI64 = cg_.builder_->CreatePtrToInt(bridgeFn, cg_.getInt64Type(),
                                                            "dynbr.ptr.i64");
            }

            // (AR) تخصيص بنية الإغلاق: ثلاث خانات i64 (انظر sir_constants.h —
            //      الخانتان 0/1 عقدُ i64 التاريخيُّ بحرفِه، و[2] جسرُ البروتوكولِ الموسوم).
            // (EN) Allocate closure struct: three i64 slots (see sir_constants.h —
            //      slots 0/1 are the historical i64 contract byte-for-byte; [2] is
            //      the tagged-protocol bridge).
            llvm::Value *closureSize = llvm::ConstantInt::get(
                cg_.getInt64Type(), Sad::Compiler::kClosureStructBytes);

            llvm::Value *closurePtr = cg_.emitMalloc(closureSize, "closure.alloc");

            // (AR) تخزين fn_ptr في offset 0
            // (EN) Store fn_ptr at offset 0
            llvm::Value *slot0 = cg_.builder_->CreateGEP(cg_.getInt64Type(), closurePtr,
                                                     llvm::ConstantInt::get(cg_.getInt64Type(), Sad::Compiler::kClosureFnSlotIndex), "closure.fn.slot");
            cg_.builder_->CreateStore(fnPtrI64, slot0);

            // (AR) تخزين env_ptr في offset 1
            // (EN) Store env_ptr at offset 1
            llvm::Value *slot1 = cg_.builder_->CreateGEP(cg_.getInt64Type(), closurePtr,
                                                     llvm::ConstantInt::get(cg_.getInt64Type(), Sad::Compiler::kClosureEnvSlotIndex), "closure.env.slot");
            cg_.builder_->CreateStore(envI64, slot1);

            // (AR) تخزين جسر البروتوكول الموسوم في offset 2
            // (EN) Store the tagged-protocol bridge at offset 2
            llvm::Value *slot2 = cg_.builder_->CreateGEP(cg_.getInt64Type(), closurePtr,
                                                     llvm::ConstantInt::get(cg_.getInt64Type(), Sad::Compiler::kClosureDynBridgeSlotIndex), "closure.dynbr.slot");
            cg_.builder_->CreateStore(dynBridgeI64, slot2);

            // (AR) إرجاع مؤشر بنية الإغلاق كـ i64
            // (EN) Return closure struct pointer as i64
            llvm::Value *result = cg_.builder_->CreatePtrToInt(closurePtr, cg_.getInt64Type(), "closure.i64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // (AR) emitClosureCall — استدعاء إغلاق عبر بنية الإغلاق
        //      1. استخراج fn_ptr من closure[0]
        //      2. استخراج env_ptr من closure[1]
        //      3. استدعاء fn_ptr(explicit_args..., env_ptr)
        //
        //      المعاملات:
        //      - operands[0]: مؤشر بنية الإغلاق (i64)
        //      - operands[1..N]: الوسائط الصريحة
        //
        //      اللامدا دائماً تأخذ __env كمعامل أخير:
        //      - بدون التقاطات: env_ptr = 0 ولا يُستخدم
        //      - مع التقاطات: env_ptr = مؤشر لمصفوفة القيم الملتقطة
        //
        // (EN) emitClosureCall — Call a closure via closure struct
        //      Lambda always takes __env as last param
        //      CLOSURE_CALL extracts fn_ptr + env_ptr, calls fn(args, env)
        // ============================================================================
        llvm::Value *ClosureCodeGen::emitClosureCall(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "CLOSURE_CALL"}});
                return nullptr;
            }

            // (AR) تحميل مؤشر بنية الإغلاق
            // (EN) Load closure struct pointer
            llvm::Value *closureI64 = cg_.resolveOperand(inst->operands[0]);
            if (!closureI64)
                return nullptr;

            // (AR) تحويل i64 إلى مؤشر i64*
            // (EN) Convert i64 to i64* pointer
            llvm::Value *closurePtr = cg_.builder_->CreateIntToPtr(
                closureI64, llvm::PointerType::getUnqual(cg_.getInt64Type()), "closure.ptr");

            // (AR) استخراج fn_ptr من closure[0]
            // (EN) Extract fn_ptr from closure[0]
            llvm::Value *fnSlot = cg_.builder_->CreateGEP(cg_.getInt64Type(), closurePtr,
                                                      llvm::ConstantInt::get(cg_.getInt64Type(), 0), "fn.slot");
            llvm::Value *fnPtrI64 = cg_.builder_->CreateLoad(cg_.getInt64Type(), fnSlot, "fn.ptr.i64");
            llvm::Value *fnPtr = cg_.builder_->CreateIntToPtr(
                fnPtrI64, llvm::PointerType::getUnqual(*cg_.context_), "fn.ptr");

            // (AR) استخراج env_ptr من closure[1]
            // (EN) Extract env_ptr from closure[1]
            llvm::Value *envSlot = cg_.builder_->CreateGEP(cg_.getInt64Type(), closurePtr,
                                                       llvm::ConstantInt::get(cg_.getInt64Type(), 1), "env.slot");
            llvm::Value *envI64 = cg_.builder_->CreateLoad(cg_.getInt64Type(), envSlot, "env.ptr.i64");

            // ════════════════════════════════════════════════════════════════
            // (AR) [موجة الجسر الموسوم] موقعُ نداءٍ مجهولُ الهدفِ (وسمُ dynproto —
            //      مرجعُ دالّةٍ وصلَ معاملًا): البروتوكولُ الخامُّ كان يبني التوقيعَ
            //      من أنواعِ الوسائطِ الساكنةِ بينما الهدفُ الحقيقيُّ بأنواعٍ أخرى —
            //      double عائدًا يُقرأ من RAX قمامةً (t01/t03 مقيس). نسلكُ جسرَ
            //      البروتوكولِ الموسومِ من الخانةِ [2]: كلُّ وسيطٍ يُغلَّفُ %SadDyn
            //      بوسمِ نوعِه الساكنِ، والعائدُ %SadDyn موسومٌ يستهلكه المصبُّ
            //      (نتيجةُ SIR منمَّطةٌ Any في الواجهة).
            // (EN) [Tagged-bridge wave] Unknown-target call site (the dynproto
            //      marker — a func-ref that arrived as a parameter): the raw
            //      protocol built the signature from the site's static arg types
            //      while the real target differs — a double return read from RAX
            //      as garbage (measured, t01/t03). Take the tagged bridge from
            //      slot [2]: each argument is boxed %SadDyn with its static kind,
            //      and the %SadDyn return carries its tag downstream (the SIR
            //      result is typed Any in the frontend).
            // ════════════════════════════════════════════════════════════════
            if (inst->comment.find(Sad::Compiler::kClosureDynProtoMarker) != std::string::npos)
            {
                llvm::StructType *dynTy = getSadDynType(*cg_.context_);
                llvm::Value *dynSlot = cg_.builder_->CreateGEP(
                    cg_.getInt64Type(), closurePtr,
                    llvm::ConstantInt::get(cg_.getInt64Type(),
                                           Sad::Compiler::kClosureDynBridgeSlotIndex),
                    "dynbr.slot");
                llvm::Value *dynPtrI64 =
                    cg_.builder_->CreateLoad(cg_.getInt64Type(), dynSlot, "dynbr.ptr.i64");
                llvm::Value *dynFnPtr = cg_.builder_->CreateIntToPtr(
                    dynPtrI64, llvm::PointerType::getUnqual(*cg_.context_), "dynbr.ptr");

                std::vector<llvm::Value *> dynArgs;
                std::vector<llvm::Type *> dynArgTypes;
                for (size_t i = 1; i < inst->operands.size(); i++)
                {
                    llvm::Value *rawArg = cg_.resolveOperand(inst->operands[i]);
                    if (!rawArg)
                    {
                        cg_.reportError(
                            ::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE,
                            {{"detail",
                              std::string("CLOSURE_CALL(dyn): failed to resolve argument:") +
                                  inst->operands[i].name}});
                        return nullptr;
                    }
                    dynArgs.push_back(toDyn(cg_, rawArg, inst->operands[i].dataType));
                    dynArgTypes.push_back(dynTy);
                }
                dynArgs.push_back(envI64);
                dynArgTypes.push_back(cg_.getInt64Type());

                auto *dynFnType = llvm::FunctionType::get(dynTy, dynArgTypes, false);
                llvm::Value *dynResult = cg_.builder_->CreateCall(
                    dynFnType, dynFnPtr, dynArgs, "closure.dyn.call");
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = dynResult;
                }
                return dynResult;
            }

            // (AR) بناء قائمة الوسائط: الوسائط الصريحة + env_ptr
            //      [إصلاح] نبحث عن الدالة الهدف من comment لمعرفة أنواع المعاملات المتوقعة
            //      ونحوّل كل وسيط ليطابق النوع المتوقع.
            //      مثال: لامدا تتوقع ptr (String) لكن المستدعي يمرر i64 (رقم)
            //             → نحوّل الرقم إلى نص عبر sprintf ثم نمرر ptr
            //      مثال: لامدا تتوقع ptr لكن المستدعي يمرر ptr → يبقى كما هو
            // (EN) Build argument list: explicit args + env_ptr
            //      [Fix] Look up target function from comment to know expected param types
            //      and convert each arg to match the expected type.
            std::vector<llvm::Value *> args;
            std::vector<llvm::Type *> argTypes;

            // (AR) محاولة العثور على الدالة الهدف لمعرفة أنواع المعاملات
            // (EN) Try to find target function to know parameter types
            llvm::Function *targetLambdaFn = nullptr;
            if (inst->comment.rfind(Sad::Compiler::kClosureLambdaCommentPrefix, 0) == 0)
            {
                const std::string lambdaName = inst->comment.substr(
                    std::strlen(Sad::Compiler::kClosureLambdaCommentPrefix));
                targetLambdaFn = cg_.module_->getFunction(lambdaName);
            }

            for (size_t i = 1; i < inst->operands.size(); i++)
            {
                llvm::Value *arg = cg_.resolveOperand(inst->operands[i]);
                if (arg)
                {
                    // (AR) تحويل الوسيط ليطابق نوع المعامل المتوقع في اللامدا
                    // (EN) Convert arg to match expected parameter type in lambda
                    if (targetLambdaFn)
                    {
                        // (AR) فهرس المعاملات: i-1 لأن operands[0] هو مؤشر الإغلاق
                        // (EN) Param index: i-1 because operands[0] is closure pointer
                        unsigned paramIdx = static_cast<unsigned>(i - 1);
                        if (paramIdx < targetLambdaFn->getFunctionType()->getNumParams())
                        {
                            llvm::Type *expectedType = targetLambdaFn->getFunctionType()->getParamType(paramIdx);
                            if (arg->getType() != expectedType)
                            {
                                if (expectedType->isPointerTy() && arg->getType()->isIntegerTy(64))
                                {
                                    // (AR) i64 → ptr: الوسيط رقم لكن اللامدا تتوقع نص
                                    //      نحوّل الرقم إلى نص عبر sprintf
                                    // (EN) i64 → ptr: arg is integer but lambda expects string
                                    //      Convert number to string via sprintf
                                    auto sprintfType = llvm::FunctionType::get(
                                        llvm::Type::getInt32Ty(*cg_.context_),
                                        {llvm::PointerType::getUnqual(*cg_.context_),
                                         llvm::PointerType::getUnqual(*cg_.context_)},
                                        true);
                                    auto sprintfFn = cg_.module_->getOrInsertFunction("sprintf", sprintfType);

                                    // (AR) تخصيص 32 بايت للنص
                                    llvm::Value *buf = cg_.emitMalloc(llvm::ConstantInt::get(cg_.getInt64Type(), 32), "num2str.buf");
                                    llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%lld", "num2str.fmt");
                                    cg_.builder_->CreateCall(sprintfFn, {buf, fmt, arg});
                                    arg = buf;
                                }
                                else if (expectedType->isIntegerTy(64) && arg->getType()->isPointerTy())
                                {
                                    // (AR) ptr → i64: تحويل مؤشر لعدد صحيح
                                    // (EN) ptr → i64: convert pointer to integer
                                    arg = cg_.builder_->CreatePtrToInt(arg, cg_.getInt64Type(), "arg.p2i");
                                }
                                else if (expectedType->isPointerTy() && arg->getType()->isPointerTy())
                                {
                                    // (AR) ptr → ptr: لا حاجة للتحويل (opaque pointers)
                                    // (EN) ptr → ptr: no conversion needed (opaque pointers)
                                }
                                else
                                {
                                    // (AR) [موجة الجسر الموسوم] بقيّةُ الأزواجِ إلى جدولِ
                                    //      التحويلِ الموحَّد — وأهمُّها: معاملُ لامدا وُسِّعَ
                                    //      إلى %SadDyn (مواقعُ نداءٍ عدديّةٌ مختلطة) فيُغلَّفُ
                                    //      الوسيطُ بوسمِه، وعشريٌّ↔صحيحٌ يُحوَّلُ قيمةً لا
                                    //      بتّاتٍ. السلسلةُ القديمةُ كانت تُسقِطُ الزوجَ غيرَ
                                    //      المغطّى فيمرُّ double إلى معاملِ i64 خامًّا —
                                    //      قمامةٌ صامتةٌ (t05 مقيس).
                                    // (EN) [Tagged-bridge wave] Every remaining pair goes to
                                    //      the unified conversion table — foremost: a lambda
                                    //      param widened to %SadDyn (mixed numeric call
                                    //      sites) gets the argument boxed with its tag, and
                                    //      float↔int converts by value, not bits. The old
                                    //      chain dropped uncovered pairs, letting a double
                                    //      reach an i64 param raw — silent garbage
                                    //      (measured, t05).
                                    arg = coerceToParamType(cg_, arg, expectedType,
                                                            inst->operands[i].dataType);
                                }
                            }
                        }
                    }
                    args.push_back(arg);
                    argTypes.push_back(arg->getType());
                }
                else
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", std::string("CLOSURE_CALL: failed to resolve argument:") + inst->operands[i].name}});
                    return nullptr;
                }
            } // (AR) إضافة env_ptr كمعامل أخير (دائماً)
            // (EN) Append env_ptr as last argument (always)
            args.push_back(envI64);
            argTypes.push_back(cg_.getInt64Type());

            // (AR) تحديد نوع الإرجاع
            //      [إصلاح] إذا وجدنا الدالة الهدف، نأخذ نوع الإرجاع من توقيعها الحقيقي
            //      هذا أدق من الاعتماد على SIR dataType الذي قد يكون Integer افتراضياً
            // (EN) Determine return type
            //      [Fix] If we found the target function, use its actual return type
            //      This is more accurate than relying on SIR dataType which may default to Integer
            llvm::Type *retType = cg_.getInt64Type();
            if (targetLambdaFn)
            {
                retType = targetLambdaFn->getReturnType();
            }
            else if (inst->result.has_value())
            {
                if (inst->result->dataType == SadTypeKind::Void)
                    retType = llvm::Type::getVoidTy(*cg_.context_);
                else if (inst->result->dataType == SadTypeKind::Float)
                    retType = cg_.getDoubleType();
                else if (inst->result->dataType == SadTypeKind::Boolean)
                    retType = llvm::Type::getInt1Ty(*cg_.context_);
                else if (inst->result->dataType == SadTypeKind::String ||
                         inst->result->dataType == SadTypeKind::Pointer)
                    retType = llvm::PointerType::getUnqual(*cg_.context_);
            }

            // ================================================================
            // (AR) [شبكة أمان] إذا كان نوع الإرجاع void لكن SIR يتوقع نتيجة،
            //      نستخدم i64 كنوع إرجاع افتراضي. هذا يحل حالة الدوال التي
            //      تُرجع إغلاقات ولكن أُعلنت بنوع void خطأً.
            // (EN) [Safety net] If return type is void but SIR expects a result,
            //      use i64 as default return type. This handles functions that
            //      return closures but were incorrectly declared as void.
            // ================================================================
            if (retType->isVoidTy() && inst->result.has_value() &&
                inst->result->dataType != SadTypeKind::Void)
            {
                retType = cg_.getInt64Type();
            }

            // (AR) إنشاء نوع الدالة واستدعاءها
            // (EN) Create function type and call
            auto *funcType = llvm::FunctionType::get(retType, argTypes, false);
            llvm::Value *result = cg_.builder_->CreateCall(funcType, fnPtr, args,
                                                       retType->isVoidTy() ? "" : "closure.call");

            if (inst->result.has_value() && !retType->isVoidTy())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // (AR) emitEnvLoad — تحميل متغير ملتقط من مصفوفة بيئة الإغلاق
        //      %val = env_load %env_ptr, INDEX
        //      يحوّل env_ptr (i64) إلى مؤشر، ثم يقرأ env[INDEX]
        //
        //      المعاملات:
        //      - operands[0]: مؤشر البيئة (i64, من المعامل __env)
        //      - operands[1]: الفهرس (ثابت i64)
        //
        // (EN) emitEnvLoad — Load captured variable from closure env array
        //      Converts env_ptr (i64) to pointer, reads env[INDEX]
        // ============================================================================
        llvm::Value *ClosureCodeGen::emitEnvLoad(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ENV_LOAD"}});
                return nullptr;
            }

            // (AR) تحميل مؤشر البيئة
            // (EN) Load env pointer
            llvm::Value *envI64 = cg_.resolveOperand(inst->operands[0]);
            if (!envI64)
                return nullptr;

            // (AR) تحويل i64 إلى i64*
            // (EN) Convert i64 to i64*
            llvm::Value *envPtr = cg_.builder_->CreateIntToPtr(
                envI64, llvm::PointerType::getUnqual(cg_.getInt64Type()), "env.ptr");

            // (AR) الحصول على الفهرس
            // (EN) Get index
            llvm::Value *idx = cg_.resolveOperand(inst->operands[1]);
            if (!idx)
                return nullptr;

            // (AR) حساب عنوان env[index] وتحميل القيمة كـ i64 دائماً
            //      (البيئة تُخزّن كل القيم كـ i64 — النصوص كـ PtrToInt)
            // (EN) Compute env[index] address and load value always as i64
            //      (env stores all values as i64 — strings stored as PtrToInt)
            llvm::Value *gep = cg_.builder_->CreateGEP(cg_.getInt64Type(), envPtr, idx, "env.gep");
            llvm::Value *val = cg_.builder_->CreateLoad(cg_.getInt64Type(), gep, "env.val");

            // ============================================================
            // (AR) [Fix #53] تحويل i64 → ptr للأنواع المؤشرية (نص/بنية/إغلاق)
            //      البيئة تُخزّن النصوص والمؤشرات كـ PtrToInt عند CLOSURE_CREATE
            //      لذا عند التحميل يجب عكس العملية: IntToPtr
            //      نوع النتيجة من SIR يُحدد ما إذا كان التحويل ضرورياً
            // (EN) [Fix #53] Convert i64 → ptr for pointer types (String/Struct/Function)
            //      env stores strings/pointers as PtrToInt at CLOSURE_CREATE time
            //      so on load we must reverse: IntToPtr
            //      result type from SIR determines if conversion is needed
            // ============================================================
            if (inst->result.has_value())
            {
                SadTypeKind resultKind = inst->result->dataType;
                bool isPointerKind = (resultKind == SadTypeKind::String ||
                                      resultKind == SadTypeKind::Struct ||
                                      resultKind == SadTypeKind::Function ||
                                      resultKind == SadTypeKind::Array ||
                                      resultKind == SadTypeKind::Map);
                if (isPointerKind)
                {
                    // (AR) عكس PtrToInt: تحويل i64 → ptr لاستخدامه كنص/بنية
                    // (EN) Reverse PtrToInt: convert i64 → ptr for use as string/struct
                    val = cg_.builder_->CreateIntToPtr(val,
                                                   llvm::PointerType::getUnqual(*cg_.context_), "env.val.ptr");
                }
                cg_.context_info_.namedValues[inst->result->name] = val;
            }
            return val;
        }

        // ============================================================================
        // (AR) [Fix #51] emitEnvStore — تخزين قيمة في مصفوفة بيئة الإغلاق
        //      %env_store %value, %env_ptr, INDEX
        //      يحوّل env_ptr (i64) إلى مؤشر، ثم يكتب القيمة في env[INDEX]
        //      هذا يضمن أن تعديلات المتغيرات الملتقطة تنعكس عبر استدعاءات الإغلاق
        //
        //      المعاملات:
        //      - operands[0]: القيمة المراد تخزينها (i64)
        //      - operands[1]: مؤشر البيئة (i64, من المعامل __env)
        //      - operands[2]: الفهرس (ثابت i64)
        //
        // (EN) [Fix #51] emitEnvStore — Store value into closure env array
        //      Converts env_ptr (i64) to pointer, writes value at env[INDEX]
        //      Ensures captured variable mutations persist across closure calls
        // ============================================================================
        llvm::Value *ClosureCodeGen::emitEnvStore(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ENV_STORE"}});
                return nullptr;
            }

            // (AR) تحميل القيمة المراد تخزينها
            // (EN) Load value to store
            llvm::Value *value = cg_.resolveOperand(inst->operands[0]);
            if (!value)
                return nullptr;

            // (AR) تحميل مؤشر البيئة
            // (EN) Load env pointer
            llvm::Value *envI64 = cg_.resolveOperand(inst->operands[1]);
            if (!envI64)
                return nullptr;

            // (AR) تحويل i64 إلى i64*
            // (EN) Convert i64 to i64*
            llvm::Value *envPtr = cg_.builder_->CreateIntToPtr(
                envI64, llvm::PointerType::getUnqual(cg_.getInt64Type()), "env.store.ptr");

            // (AR) الحصول على الفهرس
            // (EN) Get index
            llvm::Value *idx = cg_.resolveOperand(inst->operands[2]);
            if (!idx)
                return nullptr;

            // (AR) التأكد من أن القيمة i64
            // (EN) Ensure value is i64
            if (!value->getType()->isIntegerTy(64))
            {
                if (value->getType()->isIntegerTy())
                    value = cg_.builder_->CreateZExt(value, cg_.getInt64Type(), "env.store.ext");
                else if (value->getType()->isDoubleTy())
                    value = cg_.builder_->CreateBitCast(value, cg_.getInt64Type(), "env.store.dbl2i64");
                else if (value->getType()->isPointerTy())
                    value = cg_.builder_->CreatePtrToInt(value, cg_.getInt64Type(), "env.store.ptr2i64");
            }

            // (AR) حساب عنوان env[index] وتخزين القيمة
            // (EN) Compute env[index] address and store value
            llvm::Value *gep = cg_.builder_->CreateGEP(cg_.getInt64Type(), envPtr, idx, "env.store.gep");
            cg_.builder_->CreateStore(value, gep);

            return value;
        }

    } // namespace LLVM
} // namespace Sad