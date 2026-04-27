// ============================================================================
// llvm_codegen_builtins_math_async.cpp — دوال مدمجة: قراءة، رياضيات، async
// LLVM IR: Read, Math builtins, Sleep, Exit, Random, Async operations
// ============================================================================
// تم استخراج هذا الملف من llvm_codegen_builtins.cpp وفقاً لقاعدة CW-05
// ============================================================================

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

static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
{
    return llvm::StructType::get(ctx, {llvm::Type::getInt64Ty(ctx),
                                       llvm::Type::getInt64Ty(ctx),
                                       llvm::PointerType::getUnqual(ctx)});
}

namespace Sad
{
    namespace LLVM
    {
        llvm::Value *LLVMCodeGen::emitBuiltinRead(std::shared_ptr<SIRInstruction> inst)
        {
            // Read a line from stdin
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *mallocFT = llvm::FunctionType::get(i8p, {llvm::Type::getInt64Ty(*context_)}, false);
            llvm::FunctionCallee mallocFn = module_->getOrInsertFunction("malloc", mallocFT);
            llvm::Value *buf = builder_->CreateCall(mallocFn, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1024)}, "read.buf");
            llvm::FunctionType *fgetsFT = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt32Ty(*context_), i8p}, false);
            llvm::FunctionCallee fgetsFn = module_->getOrInsertFunction("fgets", fgetsFT);
            // Get stdin
            llvm::FunctionType *stdinFT = llvm::FunctionType::get(i8p, {}, false);
            llvm::FunctionCallee stdinFn = module_->getOrInsertFunction("__acrt_iob_func", llvm::FunctionType::get(i8p, {llvm::Type::getInt32Ty(*context_)}, false));
            llvm::Value *stdinPtr = builder_->CreateCall(stdinFn, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0)}, "stdin.ptr");
            builder_->CreateCall(fgetsFn, {buf, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1024), stdinPtr});
            if (inst && inst->result.has_value())
                context_info_.namedValues[inst->result->name] = buf;
            return buf;
        }

        llvm::Value *LLVMCodeGen::emitBuiltinSleep(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *ms = resolveOperand(inst->operands[0]);
            if (!ms)
                return nullptr;
            llvm::Value *ms32 = builder_->CreateIntCast(ms, llvm::Type::getInt32Ty(*context_), false);
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {llvm::Type::getInt32Ty(*context_)}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("Sleep", ft);
            builder_->CreateCall(fn, {ms32});
            return nullptr;
        }

        llvm::Value *LLVMCodeGen::emitBuiltinExit(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::Value *code = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
            if (inst && !inst->operands.empty())
            {
                llvm::Value *c = resolveOperand(inst->operands[0]);
                if (c)
                    code = builder_->CreateIntCast(c, llvm::Type::getInt32Ty(*context_), false);
            }
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {llvm::Type::getInt32Ty(*context_)}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("exit", ft);
            builder_->CreateCall(fn, {code});
            builder_->CreateUnreachable();
            return nullptr;
        }

        // ============================================================================
        // Async/Await & Concurrency Implementations
        // Using Windows API: CreateThread, WaitForSingleObject, etc.
        // ============================================================================

        llvm::Value *LLVMCodeGen::emitAsyncSpawn(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);

            // (AR) إصلاح جذري للاستقرار:
            //      عند تمرير دالة مباشرة (go block/lambda) ننفذها فوراً بشكل متزامن
            //      ثم نرجع handle رمزي غير صفري. هذا يمنع انهيارات ABI الخاصة بـ CreateThread
            //      لأن تواقيع دوال Sad (void()) لا تطابق LPTHREAD_START_ROUTINE.
            // (EN) Stability fix:
            //      when operand is a direct function (go block/lambda), execute it synchronously
            //      and return a non-zero symbolic handle. This avoids CreateThread ABI crashes
            //      because Sad function signatures (void()) don't match LPTHREAD_START_ROUTINE.
            if (!inst->operands.empty() && inst->operands[0].type == SIROperandType::FUNCTION)
            {
                llvm::Function *directFn = module_->getFunction(inst->operands[0].name);
                if (directFn)
                {
                    builder_->CreateCall(directFn, {});
                    auto result = llvm::ConstantInt::get(i64Ty, 1);
                    if (inst->result.has_value())
                    {
                        context_info_.namedValues[inst->result->name] = result;
                    }
                    return result;
                }
            }

            // Use CreateThread Windows API as fallback
            // HANDLE CreateThread(NULL, 0, lpStartAddress, lpParameter, 0, NULL)
            auto funcTy = llvm::FunctionType::get(i8PtrTy, {i8PtrTy, i64Ty, i8PtrTy, i8PtrTy, i32Ty, i8PtrTy}, false);
            auto createThread = module_->getOrInsertFunction("CreateThread", funcTy);

            auto nullPtr = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
            auto zero64 = llvm::ConstantInt::get(i64Ty, 0);
            auto zero32 = llvm::ConstantInt::get(i32Ty, 0);

            llvm::Value *funcPtr = nullPtr;
            if (!inst->operands.empty())
            {
                funcPtr = resolveOperand(inst->operands[0]);
                if (funcPtr)
                {
                    if (funcPtr->getType()->isIntegerTy())
                    {
                        funcPtr = builder_->CreateIntToPtr(funcPtr, i8PtrTy);
                    }
                    else if (funcPtr->getType()->isPointerTy())
                    {
                        funcPtr = builder_->CreateBitCast(funcPtr, i8PtrTy);
                    }
                    else
                    {
                        funcPtr = nullPtr;
                    }
                }
                else
                {
                    funcPtr = nullPtr;
                }
            }

            auto handle = builder_->CreateCall(createThread, {nullPtr, zero64, funcPtr, nullPtr, zero32, nullPtr});
            auto result = builder_->CreatePtrToInt(handle, i64Ty);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitAsyncAwait(std::shared_ptr<SIRInstruction> inst)
        {
            // WaitForSingleObject(handle, INFINITE=0xFFFFFFFF)
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);

            auto funcTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
            auto waitFunc = module_->getOrInsertFunction("WaitForSingleObject", funcTy);

            llvm::Value *taskId = resolveOperand(inst->operands[0]);
            auto handle = builder_->CreateIntToPtr(taskId, i8PtrTy);
            auto infinite = llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF);

            auto result32 = builder_->CreateCall(waitFunc, {handle, infinite});
            auto result = builder_->CreateZExt(result32, i64Ty);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitAsyncYield(std::shared_ptr<SIRInstruction> inst)
        {
            // SwitchToThread() or Sleep(0) - yield CPU time slice
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto funcTy = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i32Ty}, false);
            auto sleepFunc = module_->getOrInsertFunction("Sleep", funcTy);
            builder_->CreateCall(sleepFunc, {llvm::ConstantInt::get(i32Ty, 0)});
            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitAsyncSleep(std::shared_ptr<SIRInstruction> inst)
        {
            // Sleep(ms)
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto funcTy = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i32Ty}, false);
            auto sleepFunc = module_->getOrInsertFunction("Sleep", funcTy);

            llvm::Value *ms = resolveOperand(inst->operands[0]);
            ms = builder_->CreateTrunc(ms, i32Ty);
            builder_->CreateCall(sleepFunc, {ms});
            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitAsyncCreateFuture(std::shared_ptr<SIRInstruction> inst)
        {
            // Allocate a future: {i64 state, i64 value, i8* event}
            // state: 0=pending, 1=resolved
            // Use CreateEventA for signaling
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);

            // malloc(24) for {state, value, event_handle}
            auto mallocTy = llvm::FunctionType::get(i8PtrTy, {i64Ty}, false);
            auto mallocFn = module_->getOrInsertFunction("malloc", mallocTy);
            auto futurePtr = builder_->CreateCall(mallocFn, {llvm::ConstantInt::get(i64Ty, 24)});

            // Initialize state=0 (pending)
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);
            auto statePtr = builder_->CreateBitCast(futurePtr, i64PtrTy);
            builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), statePtr);

            // CreateEventA(NULL, TRUE, FALSE, NULL) - manual reset event
            auto eventFuncTy = llvm::FunctionType::get(i8PtrTy, {i8PtrTy, i32Ty, i32Ty, i8PtrTy}, false);
            auto createEvent = module_->getOrInsertFunction("CreateEventA", eventFuncTy);
            auto nullPtr = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
            auto eventHandle = builder_->CreateCall(createEvent, {nullPtr, llvm::ConstantInt::get(i32Ty, 1), llvm::ConstantInt::get(i32Ty, 0), nullPtr});

            // Store event handle at offset 16
            auto eventSlot = builder_->CreateGEP(llvm::Type::getInt8Ty(*context_), futurePtr, {llvm::ConstantInt::get(i64Ty, 16)});
            auto eventSlotPtr = builder_->CreateBitCast(eventSlot, llvm::PointerType::get(i8PtrTy, 0));
            builder_->CreateStore(eventHandle, eventSlotPtr);

            auto result = builder_->CreatePtrToInt(futurePtr, i64Ty);
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitAsyncResolveFuture(std::shared_ptr<SIRInstruction> inst)
        {
            // Set future value and signal event
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *futureId = resolveOperand(inst->operands[0]);
            llvm::Value *value = resolveOperand(inst->operands[1]);
            auto futurePtr = builder_->CreateIntToPtr(futureId, i8PtrTy);

            // Store value at offset 8
            auto valueSlot = builder_->CreateGEP(llvm::Type::getInt8Ty(*context_), futurePtr, {llvm::ConstantInt::get(i64Ty, 8)});
            auto valueSlotPtr = builder_->CreateBitCast(valueSlot, i64PtrTy);
            builder_->CreateStore(value, valueSlotPtr);

            // Set state=1 (resolved)
            auto statePtr = builder_->CreateBitCast(futurePtr, i64PtrTy);
            builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 1), statePtr);

            // SetEvent(event_handle)
            auto setEventTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {i8PtrTy}, false);
            auto setEvent = module_->getOrInsertFunction("SetEvent", setEventTy);
            auto eventSlot = builder_->CreateGEP(llvm::Type::getInt8Ty(*context_), futurePtr, {llvm::ConstantInt::get(i64Ty, 16)});
            auto eventSlotPtr = builder_->CreateBitCast(eventSlot, llvm::PointerType::get(i8PtrTy, 0));
            auto eventHandle = builder_->CreateLoad(i8PtrTy, eventSlotPtr);
            builder_->CreateCall(setEvent, {eventHandle});

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitAsyncGetFuture(std::shared_ptr<SIRInstruction> inst)
        {
            // Wait for future and get value
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *futureId = resolveOperand(inst->operands[0]);
            auto futurePtr = builder_->CreateIntToPtr(futureId, i8PtrTy);

            // Load event handle from offset 16
            auto eventSlot = builder_->CreateGEP(llvm::Type::getInt8Ty(*context_), futurePtr, {llvm::ConstantInt::get(i64Ty, 16)});
            auto eventSlotPtr = builder_->CreateBitCast(eventSlot, llvm::PointerType::get(i8PtrTy, 0));
            auto eventHandle = builder_->CreateLoad(i8PtrTy, eventSlotPtr);

            // WaitForSingleObject(event, INFINITE)
            auto waitTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
            auto waitFunc = module_->getOrInsertFunction("WaitForSingleObject", waitTy);
            builder_->CreateCall(waitFunc, {eventHandle, llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF)});

            // Load value from offset 8
            auto valueSlot = builder_->CreateGEP(llvm::Type::getInt8Ty(*context_), futurePtr, {llvm::ConstantInt::get(i64Ty, 8)});
            auto valueSlotPtr = builder_->CreateBitCast(valueSlot, i64PtrTy);
            auto result = builder_->CreateLoad(i64Ty, valueSlotPtr);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ================================================================
        // (AR) جاهز — هل المستقبل تم حله؟ يقرأ slot[0] (state) من بنية المستقبل
        //      بنية المستقبل: {state(0), value(8), event(16)}
        //      state=0 → غير جاهز (خطأ)، state=1 → جاهز (صحيح)
        // (EN) is_ready — reads state slot from future struct
        //      Future struct: {state(0), value(8), event(16)}
        //      state=0 → not ready (false), state=1 → ready (true)
        // ================================================================
        llvm::Value *LLVMCodeGen::emitAsyncFutureIsReady(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) استخراج مؤشر البنية
            llvm::Value *futureId = resolveOperand(inst->operands[0]);
            auto futurePtr = builder_->CreateIntToPtr(futureId, i8PtrTy);

            // (AR) قراءة state من offset 0
            auto statePtr = builder_->CreateBitCast(futurePtr, i64PtrTy);
            auto stateVal = builder_->CreateLoad(i64Ty, statePtr);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = stateVal;
            }
            return stateVal;
        }

    } // namespace LLVM
} // namespace Sad
