/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
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
        llvm::Value *LLVMCodeGen::emitSar(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("SAR requires 2 operands");
                return nullptr;
            }
            llvm::Value *lhs = resolveOperand(inst->operands[0]);
            llvm::Value *rhs = resolveOperand(inst->operands[1]);
            if (!lhs || !rhs)
                return nullptr;
            llvm::Value *result = builder_->CreateAShr(lhs, rhs, "sar");
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitRol(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("ROL requires 2 operands");
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            llvm::Value *amount = resolveOperand(inst->operands[1]);
            if (!val || !amount)
                return nullptr;
            // ROL(x, n) = (x << n) | (x >> (64 - n))
            auto *bits = llvm::ConstantInt::get(getInt64Type(), 64);
            llvm::Value *shl = builder_->CreateShl(val, amount, "rol.shl");
            llvm::Value *sub = builder_->CreateSub(bits, amount, "rol.sub");
            llvm::Value *shr = builder_->CreateLShr(val, sub, "rol.shr");
            llvm::Value *result = builder_->CreateOr(shl, shr, "rol");
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // Phase N: Missing Control Flow / تدفق تحكم ناقص
        // ============================================================================

        llvm::Value *LLVMCodeGen::emitCallIndirect(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("CALL_INDIRECT requires at least 1 operand (function pointer)");
                return nullptr;
            }
            // operand[0] = function pointer, rest = args
            llvm::Value *fnPtr = resolveOperand(inst->operands[0]);
            if (!fnPtr)
                return nullptr;

            // If fnPtr is an integer, convert to pointer
            if (fnPtr->getType()->isIntegerTy())
            {
                fnPtr = builder_->CreateIntToPtr(fnPtr, llvm::PointerType::getUnqual(*context_), "fnptr");
            }

            // Collect arguments
            std::vector<llvm::Value *> args;
            std::vector<llvm::Type *> argTypes;
            for (size_t i = 1; i < inst->operands.size(); i++)
            {
                llvm::Value *arg = resolveOperand(inst->operands[i]);
                if (arg)
                {
                    args.push_back(arg);
                    argTypes.push_back(arg->getType());
                }
            }

            // Determine return type
            llvm::Type *retType = getInt64Type();
            if (inst->result.has_value())
            {
                if (inst->result->dataType == SadTypeKind::Void)
                    retType = llvm::Type::getVoidTy(*context_);
                else if (inst->result->dataType == SadTypeKind::Float)
                    retType = getDoubleType();
                else if (inst->result->dataType == SadTypeKind::Boolean)
                    retType = llvm::Type::getInt1Ty(*context_);
            }

            auto *funcType = llvm::FunctionType::get(retType, argTypes, false);
            llvm::Value *result = builder_->CreateCall(funcType, fnPtr, args,
                                                       retType->isVoidTy() ? "" : "call_indirect");

            if (inst->result.has_value() && !retType->isVoidTy())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // Phase N: Missing Memory Operations / عمليات ذاكرة ناقصة
        // ============================================================================

        llvm::Value *LLVMCodeGen::emitAllocHeap(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("ALLOC_HEAP requires 1 operand (size)");
                return nullptr;
            }
            llvm::Value *size = resolveOperand(inst->operands[0]);
            if (!size)
                return nullptr;

            auto *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
            auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *result = builder_->CreateCall(mallocFunc, {size}, "heap_alloc");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFreeMem(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("FREE requires 1 operand (pointer)");
                return nullptr;
            }
            llvm::Value *ptr = resolveOperand(inst->operands[0]);
            if (!ptr)
                return nullptr;

            if (ptr->getType()->isIntegerTy())
            {
                ptr = builder_->CreateIntToPtr(ptr, llvm::PointerType::getUnqual(*context_), "free.ptr");
            }

            auto *freeType = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*context_), {llvm::PointerType::getUnqual(*context_)}, false);
            auto freeFunc = module_->getOrInsertFunction("free", freeType);
            builder_->CreateCall(freeFunc, {ptr});

            return llvm::ConstantInt::get(getInt64Type(), 0);
        }

        // ============================================================================
        // (AR) emitClosureCreate — إنشاء بنية إغلاق على الكومة
        //      البنية: { fn_ptr: i64, env_ptr: i64 }
        //      - fn_ptr: مؤشر لدالة اللامدا (ptrtoint)
        //      - env_ptr: مؤشر لمصفوفة المتغيرات الملتقطة (0 إن لم تكن)
        //
        //      المعاملات:
        //      - operands[0]: Function (@fn_name) — اسم اللامدا
        //      - operands[1..N]: قيم المتغيرات الملتقطة
        //
        //      الخطوات:
        //      1. إذا كانت هناك التقاطات: تخصيص مصفوفة env على الكومة
        //         وتخزين قيم الالتقاطات فيها
        //      2. تخصيص بنية الإغلاق (16 بايت) على الكومة
        //      3. تخزين fn_ptr في offset 0 و env_ptr في offset 1
        //      4. إرجاع مؤشر البنية كـ i64
        //
        // (EN) emitClosureCreate — Create closure struct on heap
        //      Struct layout: { fn_ptr: i64, env_ptr: i64 }
        //      Steps:
        //      1. If captures: allocate env array, store capture values
        //      2. Allocate closure struct (16 bytes)
        //      3. Store fn_ptr at [0], env_ptr at [1]
        //      4. Return closure pointer as i64
        // ============================================================================
        llvm::Value *LLVMCodeGen::emitClosureCreate(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("CLOSURE_CREATE requires at least 1 operand (function)");
                return nullptr;
            }

            // (AR) الحصول على مؤشر دالة اللامدا
            // (EN) Get lambda function pointer
            llvm::Function *lambdaFn = nullptr;
            std::string fnName = inst->operands[0].name;
            lambdaFn = module_->getFunction(fnName);
            if (!lambdaFn)
            {
                reportError("CLOSURE_CREATE: function '" + fnName + "' not found in module");
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
                llvm::Function *existingWrapper = module_->getFunction(wrapperName);
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
                                                                         : getInt64Type());

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
                        wrapperParamTypes.push_back(getInt64Type());
                    }
                    // (AR) lambda conv: المعاملات كافية، لا إضافة

                    auto *wrapperFnType = llvm::FunctionType::get(
                        wrapperRetType, wrapperParamTypes, false);
                    auto *wrapperFn = llvm::Function::Create(
                        wrapperFnType, llvm::Function::InternalLinkage, wrapperName, module_.get());

                    // (AR) بناء جسم الـ wrapper
                    auto *entryBB = llvm::BasicBlock::Create(*context_, "entry", wrapperFn);
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
                            wrapperBuilder.CreateZExt(retVal, getInt64Type(), "bool.to.i64"));
                    }
                    else if (origRetType->isIntegerTy(32))
                    {
                        // (AR) i32 → i64: توسيع بالإشارة
                        // (EN) i32 → i64: sign-extend
                        wrapperBuilder.CreateRet(
                            wrapperBuilder.CreateSExt(retVal, getInt64Type(), "i32.to.i64"));
                    }
                    else if (origRetType->isDoubleTy())
                    {
                        // (AR) double → i64: إعادة تفسير البتات (IEEE 754)
                        // (EN) double → i64: bit-reinterpret (IEEE 754)
                        wrapperBuilder.CreateRet(
                            wrapperBuilder.CreateBitCast(retVal, getInt64Type(), "dbl.to.i64"));
                    }
                    else if (origRetType->isFloatTy())
                    {
                        // (AR) float → double → i64: توسيع ثم إعادة تفسير
                        // (EN) float → double → i64: extend then bit-reinterpret
                        llvm::Value *asDouble = wrapperBuilder.CreateFPExt(
                            retVal, getDoubleType(), "flt.to.dbl");
                        wrapperBuilder.CreateRet(
                            wrapperBuilder.CreateBitCast(asDouble, getInt64Type(), "dbl.to.i64"));
                    }
                    else if (origRetType->isPointerTy())
                    {
                        // (AR) ptr → i64: PtrToInt
                        // (EN) ptr → i64: PtrToInt
                        wrapperBuilder.CreateRet(
                            wrapperBuilder.CreatePtrToInt(retVal, getInt64Type(), "ptr.to.i64"));
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

            llvm::Value *fnPtrI64 = builder_->CreatePtrToInt(targetFn, getInt64Type(), "fn.ptr.i64");

            // (AR) عدد المتغيرات الملتقطة
            // (EN) Number of captured variables
            size_t numCaptures = inst->operands.size() - 1;

            // (AR) إنشاء مصفوفة البيئة (env) إذا كانت هناك التقاطات
            // (EN) Create environment array if there are captures
            llvm::Value *envI64 = llvm::ConstantInt::get(getInt64Type(), 0); // default: no env
            if (numCaptures > 0)
            {
                // (AR) تخصيص مصفوفة env على الكومة: numCaptures * 8 بايت
                // (EN) Allocate env array on heap: numCaptures * 8 bytes
                llvm::Value *envSize = llvm::ConstantInt::get(getInt64Type(), numCaptures * 8);
                auto *mallocType = llvm::FunctionType::get(
                    llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
                auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
                llvm::Value *envPtr = builder_->CreateCall(mallocFunc, {envSize}, "env.alloc");

                // (AR) تخزين كل قيمة ملتقطة في env[i]
                // (EN) Store each captured value at env[i]
                for (size_t i = 0; i < numCaptures; i++)
                {
                    llvm::Value *capVal = resolveOperand(inst->operands[1 + i]);
                    if (!capVal)
                        continue;

                    // (AR) التأكد من أن القيمة i64
                    // (EN) Ensure value is i64
                    if (!capVal->getType()->isIntegerTy(64))
                    {
                        if (capVal->getType()->isIntegerTy())
                            capVal = builder_->CreateZExt(capVal, getInt64Type(), "cap.ext");
                        else if (capVal->getType()->isDoubleTy())
                            capVal = builder_->CreateBitCast(capVal, getInt64Type(), "cap.dbl2i64");
                        else if (capVal->getType()->isPointerTy())
                            capVal = builder_->CreatePtrToInt(capVal, getInt64Type(), "cap.ptr2i64");
                    }

                    llvm::Value *idx = llvm::ConstantInt::get(getInt64Type(), i);
                    llvm::Value *gep = builder_->CreateGEP(getInt64Type(), envPtr, idx, "env.slot");
                    builder_->CreateStore(capVal, gep);
                }

                // (AR) تحويل مؤشر البيئة إلى i64
                // (EN) Convert env pointer to i64
                envI64 = builder_->CreatePtrToInt(envPtr, getInt64Type(), "env.i64");
            }

            // (AR) تخصيص بنية الإغلاق: 2 * i64 = 16 بايت
            // (EN) Allocate closure struct: 2 * i64 = 16 bytes
            llvm::Value *closureSize = llvm::ConstantInt::get(getInt64Type(), 16);
            auto *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
            auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *closurePtr = builder_->CreateCall(mallocFunc, {closureSize}, "closure.alloc");

            // (AR) تخزين fn_ptr في offset 0
            // (EN) Store fn_ptr at offset 0
            llvm::Value *slot0 = builder_->CreateGEP(getInt64Type(), closurePtr,
                                                     llvm::ConstantInt::get(getInt64Type(), 0), "closure.fn.slot");
            builder_->CreateStore(fnPtrI64, slot0);

            // (AR) تخزين env_ptr في offset 1
            // (EN) Store env_ptr at offset 1
            llvm::Value *slot1 = builder_->CreateGEP(getInt64Type(), closurePtr,
                                                     llvm::ConstantInt::get(getInt64Type(), 1), "closure.env.slot");
            builder_->CreateStore(envI64, slot1);

            // (AR) إرجاع مؤشر بنية الإغلاق كـ i64
            // (EN) Return closure struct pointer as i64
            llvm::Value *result = builder_->CreatePtrToInt(closurePtr, getInt64Type(), "closure.i64");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
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
        llvm::Value *LLVMCodeGen::emitClosureCall(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("CLOSURE_CALL requires at least 1 operand (closure pointer)");
                return nullptr;
            }

            // (AR) تحميل مؤشر بنية الإغلاق
            // (EN) Load closure struct pointer
            llvm::Value *closureI64 = resolveOperand(inst->operands[0]);
            if (!closureI64)
                return nullptr;

            // (AR) تحويل i64 إلى مؤشر i64*
            // (EN) Convert i64 to i64* pointer
            llvm::Value *closurePtr = builder_->CreateIntToPtr(
                closureI64, llvm::PointerType::getUnqual(getInt64Type()), "closure.ptr");

            // (AR) استخراج fn_ptr من closure[0]
            // (EN) Extract fn_ptr from closure[0]
            llvm::Value *fnSlot = builder_->CreateGEP(getInt64Type(), closurePtr,
                                                      llvm::ConstantInt::get(getInt64Type(), 0), "fn.slot");
            llvm::Value *fnPtrI64 = builder_->CreateLoad(getInt64Type(), fnSlot, "fn.ptr.i64");
            llvm::Value *fnPtr = builder_->CreateIntToPtr(
                fnPtrI64, llvm::PointerType::getUnqual(*context_), "fn.ptr");

            // (AR) استخراج env_ptr من closure[1]
            // (EN) Extract env_ptr from closure[1]
            llvm::Value *envSlot = builder_->CreateGEP(getInt64Type(), closurePtr,
                                                       llvm::ConstantInt::get(getInt64Type(), 1), "env.slot");
            llvm::Value *envI64 = builder_->CreateLoad(getInt64Type(), envSlot, "env.ptr.i64");

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
            if (!inst->comment.empty() && inst->comment.find("lambda:") == 0)
            {
                std::string lambdaName = inst->comment.substr(7);
                targetLambdaFn = module_->getFunction(lambdaName);
            }

            for (size_t i = 1; i < inst->operands.size(); i++)
            {
                llvm::Value *arg = resolveOperand(inst->operands[i]);
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
                                        llvm::Type::getInt32Ty(*context_),
                                        {llvm::PointerType::getUnqual(*context_),
                                         llvm::PointerType::getUnqual(*context_)},
                                        true);
                                    auto sprintfFn = module_->getOrInsertFunction("sprintf", sprintfType);
                                    auto mallocType = llvm::FunctionType::get(
                                        llvm::PointerType::getUnqual(*context_),
                                        {getInt64Type()}, false);
                                    auto mallocFn = module_->getOrInsertFunction("malloc", mallocType);
                                    // (AR) تخصيص 32 بايت للنص
                                    llvm::Value *buf = builder_->CreateCall(
                                        mallocFn, {llvm::ConstantInt::get(getInt64Type(), 32)}, "num2str.buf");
                                    llvm::Value *fmt = builder_->CreateGlobalStringPtr("%lld", "num2str.fmt");
                                    builder_->CreateCall(sprintfFn, {buf, fmt, arg});
                                    arg = buf;
                                }
                                else if (expectedType->isIntegerTy(64) && arg->getType()->isPointerTy())
                                {
                                    // (AR) ptr → i64: تحويل مؤشر لعدد صحيح
                                    // (EN) ptr → i64: convert pointer to integer
                                    arg = builder_->CreatePtrToInt(arg, getInt64Type(), "arg.p2i");
                                }
                                else if (expectedType->isPointerTy() && arg->getType()->isPointerTy())
                                {
                                    // (AR) ptr → ptr: لا حاجة للتحويل (opaque pointers)
                                    // (EN) ptr → ptr: no conversion needed (opaque pointers)
                                }
                            }
                        }
                    }
                    args.push_back(arg);
                    argTypes.push_back(arg->getType());
                }
                else
                {
                    reportError("CLOSURE_CALL: failed to resolve argument: " + inst->operands[i].name);
                    return nullptr;
                }
            } // (AR) إضافة env_ptr كمعامل أخير (دائماً)
            // (EN) Append env_ptr as last argument (always)
            args.push_back(envI64);
            argTypes.push_back(getInt64Type());

            // (AR) تحديد نوع الإرجاع
            //      [إصلاح] إذا وجدنا الدالة الهدف، نأخذ نوع الإرجاع من توقيعها الحقيقي
            //      هذا أدق من الاعتماد على SIR dataType الذي قد يكون Integer افتراضياً
            // (EN) Determine return type
            //      [Fix] If we found the target function, use its actual return type
            //      This is more accurate than relying on SIR dataType which may default to Integer
            llvm::Type *retType = getInt64Type();
            if (targetLambdaFn)
            {
                retType = targetLambdaFn->getReturnType();
            }
            else if (inst->result.has_value())
            {
                if (inst->result->dataType == SadTypeKind::Void)
                    retType = llvm::Type::getVoidTy(*context_);
                else if (inst->result->dataType == SadTypeKind::Float)
                    retType = getDoubleType();
                else if (inst->result->dataType == SadTypeKind::Boolean)
                    retType = llvm::Type::getInt1Ty(*context_);
                else if (inst->result->dataType == SadTypeKind::String ||
                         inst->result->dataType == SadTypeKind::Pointer)
                    retType = llvm::PointerType::getUnqual(*context_);
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
                retType = getInt64Type();
            }

            // (AR) إنشاء نوع الدالة واستدعاءها
            // (EN) Create function type and call
            auto *funcType = llvm::FunctionType::get(retType, argTypes, false);
            llvm::Value *result = builder_->CreateCall(funcType, fnPtr, args,
                                                       retType->isVoidTy() ? "" : "closure.call");

            if (inst->result.has_value() && !retType->isVoidTy())
            {
                context_info_.namedValues[inst->result->name] = result;
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
        llvm::Value *LLVMCodeGen::emitEnvLoad(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("ENV_LOAD requires 2 operands (env pointer, index)");
                return nullptr;
            }

            // (AR) تحميل مؤشر البيئة
            // (EN) Load env pointer
            llvm::Value *envI64 = resolveOperand(inst->operands[0]);
            if (!envI64)
                return nullptr;

            // (AR) تحويل i64 إلى i64*
            // (EN) Convert i64 to i64*
            llvm::Value *envPtr = builder_->CreateIntToPtr(
                envI64, llvm::PointerType::getUnqual(getInt64Type()), "env.ptr");

            // (AR) الحصول على الفهرس
            // (EN) Get index
            llvm::Value *idx = resolveOperand(inst->operands[1]);
            if (!idx)
                return nullptr;

            // (AR) حساب عنوان env[index] وتحميل القيمة
            // (EN) Compute env[index] address and load value
            llvm::Value *gep = builder_->CreateGEP(getInt64Type(), envPtr, idx, "env.gep");
            llvm::Value *val = builder_->CreateLoad(getInt64Type(), gep, "env.val");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = val;
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
        llvm::Value *LLVMCodeGen::emitEnvStore(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                reportError("ENV_STORE requires 3 operands (value, env pointer, index)");
                return nullptr;
            }

            // (AR) تحميل القيمة المراد تخزينها
            // (EN) Load value to store
            llvm::Value *value = resolveOperand(inst->operands[0]);
            if (!value)
                return nullptr;

            // (AR) تحميل مؤشر البيئة
            // (EN) Load env pointer
            llvm::Value *envI64 = resolveOperand(inst->operands[1]);
            if (!envI64)
                return nullptr;

            // (AR) تحويل i64 إلى i64*
            // (EN) Convert i64 to i64*
            llvm::Value *envPtr = builder_->CreateIntToPtr(
                envI64, llvm::PointerType::getUnqual(getInt64Type()), "env.store.ptr");

            // (AR) الحصول على الفهرس
            // (EN) Get index
            llvm::Value *idx = resolveOperand(inst->operands[2]);
            if (!idx)
                return nullptr;

            // (AR) التأكد من أن القيمة i64
            // (EN) Ensure value is i64
            if (!value->getType()->isIntegerTy(64))
            {
                if (value->getType()->isIntegerTy())
                    value = builder_->CreateZExt(value, getInt64Type(), "env.store.ext");
                else if (value->getType()->isDoubleTy())
                    value = builder_->CreateBitCast(value, getInt64Type(), "env.store.dbl2i64");
                else if (value->getType()->isPointerTy())
                    value = builder_->CreatePtrToInt(value, getInt64Type(), "env.store.ptr2i64");
            }

            // (AR) حساب عنوان env[index] وتخزين القيمة
            // (EN) Compute env[index] address and store value
            llvm::Value *gep = builder_->CreateGEP(getInt64Type(), envPtr, idx, "env.store.gep");
            builder_->CreateStore(value, gep);

            return value;
        }

    } // namespace LLVM
} // namespace Sad