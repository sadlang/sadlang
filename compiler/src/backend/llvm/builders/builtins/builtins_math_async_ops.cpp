// ============================================================================
// llvm_codegen_builtins_math_async.cpp — دوال مدمجة: قراءة، رياضيات، async
// LLVM IR: Read, Math builtins, Sleep, Exit, Random, Async operations
// ============================================================================
// تم استخراج هذا الملف من llvm_codegen_builtins.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "llvm_codegen.h"
#include "builders/builtins/builtin_funcs_codegen.h"
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
        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinRead(std::shared_ptr<SIRInstruction> inst)
        {
            // Read a line from stdin
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *mallocFT = llvm::FunctionType::get(i8p, {llvm::Type::getInt64Ty(*cg_.context_)}, false);
            llvm::FunctionCallee mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocFT);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFn, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1024)}, "read.buf");
            llvm::FunctionType *fgetsFT = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt32Ty(*cg_.context_), i8p}, false);
            llvm::FunctionCallee fgetsFn = cg_.module_->getOrInsertFunction("fgets", fgetsFT);
            // Get stdin
            llvm::FunctionType *stdinFT = llvm::FunctionType::get(i8p, {}, false);
            llvm::FunctionCallee stdinFn = cg_.module_->getOrInsertFunction("__acrt_iob_func", llvm::FunctionType::get(i8p, {llvm::Type::getInt32Ty(*cg_.context_)}, false));
            llvm::Value *stdinPtr = cg_.builder_->CreateCall(stdinFn, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_), 0)}, "stdin.ptr");
            cg_.builder_->CreateCall(fgetsFn, {buf, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_), 1024), stdinPtr});
            if (inst && inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = buf;
            return buf;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinSleep(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *ms = cg_.resolveOperand(inst->operands[0]);
            if (!ms)
                return nullptr;
            llvm::Value *ms32 = cg_.builder_->CreateIntCast(ms, llvm::Type::getInt32Ty(*cg_.context_), false);
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {llvm::Type::getInt32Ty(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("Sleep", ft);
            cg_.builder_->CreateCall(fn, {ms32});
            return nullptr;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinExit(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::Value *code = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_), 0);
            if (inst && !inst->operands.empty())
            {
                llvm::Value *c = cg_.resolveOperand(inst->operands[0]);
                if (c)
                    code = cg_.builder_->CreateIntCast(c, llvm::Type::getInt32Ty(*cg_.context_), false);
            }
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {llvm::Type::getInt32Ty(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("exit", ft);
            cg_.builder_->CreateCall(fn, {code});
            cg_.builder_->CreateUnreachable();
            return nullptr;
        }

        // ============================================================================
        // Async/Await & Concurrency Implementations
        // Using Windows API: CreateThread, WaitForSingleObject, etc.
        // ============================================================================

        llvm::Value *BuiltinFuncsCodeGen::emitAsyncSpawn(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);

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
                llvm::Function *directFn = cg_.module_->getFunction(inst->operands[0].name);
                if (directFn)
                {
                    cg_.builder_->CreateCall(directFn, {});
                    auto result = llvm::ConstantInt::get(i64Ty, 1);
                    if (inst->result.has_value())
                    {
                        cg_.context_info_.namedValues[inst->result->name] = result;
                    }
                    return result;
                }
            }

            // Use CreateThread Windows API as fallback
            // HANDLE CreateThread(NULL, 0, lpStartAddress, lpParameter, 0, NULL)
            auto funcTy = llvm::FunctionType::get(i8PtrTy, {i8PtrTy, i64Ty, i8PtrTy, i8PtrTy, i32Ty, i8PtrTy}, false);
            auto createThread = cg_.module_->getOrInsertFunction("CreateThread", funcTy);

            auto nullPtr = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0));
            auto zero64 = llvm::ConstantInt::get(i64Ty, 0);
            auto zero32 = llvm::ConstantInt::get(i32Ty, 0);

            llvm::Value *funcPtr = nullPtr;
            if (!inst->operands.empty())
            {
                funcPtr = cg_.resolveOperand(inst->operands[0]);
                if (funcPtr)
                {
                    if (funcPtr->getType()->isIntegerTy())
                    {
                        funcPtr = cg_.builder_->CreateIntToPtr(funcPtr, i8PtrTy);
                    }
                    else if (funcPtr->getType()->isPointerTy())
                    {
                        funcPtr = cg_.builder_->CreateBitCast(funcPtr, i8PtrTy);
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

            auto handle = cg_.builder_->CreateCall(createThread, {nullPtr, zero64, funcPtr, nullPtr, zero32, nullPtr});
            auto result = cg_.builder_->CreatePtrToInt(handle, i64Ty);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitAsyncAwait(std::shared_ptr<SIRInstruction> inst)
        {
            // WaitForSingleObject(handle, INFINITE=0xFFFFFFFF)
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);

            auto funcTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
            auto waitFunc = cg_.module_->getOrInsertFunction("WaitForSingleObject", funcTy);

            llvm::Value *taskId = cg_.resolveOperand(inst->operands[0]);
            auto handle = cg_.builder_->CreateIntToPtr(taskId, i8PtrTy);
            auto infinite = llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF);

            auto result32 = cg_.builder_->CreateCall(waitFunc, {handle, infinite});
            auto result = cg_.builder_->CreateZExt(result32, i64Ty);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitAsyncYield(std::shared_ptr<SIRInstruction> inst)
        {
            // SwitchToThread() or Sleep(0) - yield CPU time slice
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto funcTy = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i32Ty}, false);
            auto sleepFunc = cg_.module_->getOrInsertFunction("Sleep", funcTy);
            cg_.builder_->CreateCall(sleepFunc, {llvm::ConstantInt::get(i32Ty, 0)});
            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitAsyncSleep(std::shared_ptr<SIRInstruction> inst)
        {
            // Sleep(ms)
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto funcTy = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i32Ty}, false);
            auto sleepFunc = cg_.module_->getOrInsertFunction("Sleep", funcTy);

            llvm::Value *ms = cg_.resolveOperand(inst->operands[0]);
            ms = cg_.builder_->CreateTrunc(ms, i32Ty);
            cg_.builder_->CreateCall(sleepFunc, {ms});
            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitAsyncCreateFuture(std::shared_ptr<SIRInstruction> inst)
        {
            // Allocate a future: {i64 state, i64 value, i8* event}
            // state: 0=pending, 1=resolved
            // Use CreateEventA for signaling
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // malloc(24) for {state, value, event_handle}
            auto mallocTy = llvm::FunctionType::get(i8PtrTy, {i64Ty}, false);
            auto mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocTy);
            auto futurePtr = cg_.builder_->CreateCall(mallocFn, {llvm::ConstantInt::get(i64Ty, 24)});

            // Initialize state=0 (pending)
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);
            auto statePtr = cg_.builder_->CreateBitCast(futurePtr, i64PtrTy);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), statePtr);

            // CreateEventA(NULL, TRUE, FALSE, NULL) - manual reset event
            auto eventFuncTy = llvm::FunctionType::get(i8PtrTy, {i8PtrTy, i32Ty, i32Ty, i8PtrTy}, false);
            auto createEvent = cg_.module_->getOrInsertFunction("CreateEventA", eventFuncTy);
            auto nullPtr = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0));
            auto eventHandle = cg_.builder_->CreateCall(createEvent, {nullPtr, llvm::ConstantInt::get(i32Ty, 1), llvm::ConstantInt::get(i32Ty, 0), nullPtr});

            // Store event handle at offset 16
            auto eventSlot = cg_.builder_->CreateGEP(llvm::Type::getInt8Ty(*cg_.context_), futurePtr, {llvm::ConstantInt::get(i64Ty, 16)});
            auto eventSlotPtr = cg_.builder_->CreateBitCast(eventSlot, llvm::PointerType::get(i8PtrTy, 0));
            cg_.builder_->CreateStore(eventHandle, eventSlotPtr);

            auto result = cg_.builder_->CreatePtrToInt(futurePtr, i64Ty);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitAsyncResolveFuture(std::shared_ptr<SIRInstruction> inst)
        {
            // Set future value and signal event
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *futureId = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *value = cg_.resolveOperand(inst->operands[1]);
            auto futurePtr = cg_.builder_->CreateIntToPtr(futureId, i8PtrTy);

            // Store value at offset 8
            auto valueSlot = cg_.builder_->CreateGEP(llvm::Type::getInt8Ty(*cg_.context_), futurePtr, {llvm::ConstantInt::get(i64Ty, 8)});
            auto valueSlotPtr = cg_.builder_->CreateBitCast(valueSlot, i64PtrTy);
            cg_.builder_->CreateStore(value, valueSlotPtr);

            // Set state=1 (resolved)
            auto statePtr = cg_.builder_->CreateBitCast(futurePtr, i64PtrTy);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 1), statePtr);

            // SetEvent(event_handle)
            auto setEventTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(*cg_.context_), {i8PtrTy}, false);
            auto setEvent = cg_.module_->getOrInsertFunction("SetEvent", setEventTy);
            auto eventSlot = cg_.builder_->CreateGEP(llvm::Type::getInt8Ty(*cg_.context_), futurePtr, {llvm::ConstantInt::get(i64Ty, 16)});
            auto eventSlotPtr = cg_.builder_->CreateBitCast(eventSlot, llvm::PointerType::get(i8PtrTy, 0));
            auto eventHandle = cg_.builder_->CreateLoad(i8PtrTy, eventSlotPtr);
            cg_.builder_->CreateCall(setEvent, {eventHandle});

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitAsyncGetFuture(std::shared_ptr<SIRInstruction> inst)
        {
            // Wait for future and get value
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *futureId = cg_.resolveOperand(inst->operands[0]);
            auto futurePtr = cg_.builder_->CreateIntToPtr(futureId, i8PtrTy);

            // Load event handle from offset 16
            auto eventSlot = cg_.builder_->CreateGEP(llvm::Type::getInt8Ty(*cg_.context_), futurePtr, {llvm::ConstantInt::get(i64Ty, 16)});
            auto eventSlotPtr = cg_.builder_->CreateBitCast(eventSlot, llvm::PointerType::get(i8PtrTy, 0));
            auto eventHandle = cg_.builder_->CreateLoad(i8PtrTy, eventSlotPtr);

            // WaitForSingleObject(event, INFINITE)
            auto waitTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
            auto waitFunc = cg_.module_->getOrInsertFunction("WaitForSingleObject", waitTy);
            cg_.builder_->CreateCall(waitFunc, {eventHandle, llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF)});

            // Load value from offset 8
            auto valueSlot = cg_.builder_->CreateGEP(llvm::Type::getInt8Ty(*cg_.context_), futurePtr, {llvm::ConstantInt::get(i64Ty, 8)});
            auto valueSlotPtr = cg_.builder_->CreateBitCast(valueSlot, i64PtrTy);
            auto result = cg_.builder_->CreateLoad(i64Ty, valueSlotPtr);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
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
        llvm::Value *BuiltinFuncsCodeGen::emitAsyncFutureIsReady(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) استخراج مؤشر البنية
            llvm::Value *futureId = cg_.resolveOperand(inst->operands[0]);
            auto futurePtr = cg_.builder_->CreateIntToPtr(futureId, i8PtrTy);

            // (AR) قراءة state من offset 0
            auto statePtr = cg_.builder_->CreateBitCast(futurePtr, i64PtrTy);
            auto stateVal = cg_.builder_->CreateLoad(i64Ty, statePtr);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = stateVal;
            }
            return stateVal;
        }

    } // namespace LLVM
} // namespace Sad
