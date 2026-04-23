/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
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
#include <limits>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        // ============================================================================
        // (AR) عمليات الكوروتينات والمولدات
        // (EN) Coroutine and generator operations
        // (AR) تم فصله من llvm_codegen_array_file_coro.cpp وفق CW-05
        // ============================================================================
        void LLVMCodeGen::emitCoroutinePreamble(std::shared_ptr<SIRFunction> sirFunc, llvm::Function *llvmFunc)
        {
            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i8Ty = llvm::Type::getInt8Ty(*context_);
            auto i1Ty = llvm::Type::getInt1Ty(*context_);
            auto tokenTy = llvm::Type::getTokenTy(*context_);
            auto voidTy = llvm::Type::getVoidTy(*context_);

            // (AR) التأكد من أننا في كتلة entry
            // (EN) Make sure we're in the entry block
            auto *entryBB = &llvmFunc->getEntryBlock();
            builder_->SetInsertPoint(entryBB);

            // (AR) إنشاء الكتل الخاصة بالكوروتين
            // (EN) Create coroutine-specific blocks
            context_info_.coroFinalBB = llvm::BasicBlock::Create(*context_, "coro.final", llvmFunc);
            context_info_.coroCleanupBB = llvm::BasicBlock::Create(*context_, "coro.cleanup", llvmFunc);
            context_info_.coroSuspendBB = llvm::BasicBlock::Create(*context_, "coro.suspend", llvmFunc);

            // (AR) كتلة لاستمرار التنفيذ بعد التعليق الأولي
            // (EN) Block for continuing execution after initial suspend
            auto *initResumeBB = llvm::BasicBlock::Create(*context_, "coro.init.resume", llvmFunc);

            context_info_.coroSuspendCount = 0;

            // === Step 1: Allocate promise (for return value) ===
            llvm::Value *promise = builder_->CreateAlloca(i64Ty, nullptr, "coro.promise");
            builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), promise);
            context_info_.coroPromise = promise;

            // === Step 2: coro.id ===
            auto coroIdFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_id);
            llvm::Value *coroId = builder_->CreateCall(coroIdFn, {llvm::ConstantInt::get(i32Ty, 0), promise, llvm::ConstantPointerNull::get(ptrTy), llvm::ConstantPointerNull::get(ptrTy)}, "coro.id");
            context_info_.coroId = coroId;

            // === Step 3: coro.size ===
            auto coroSizeFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_size, {i64Ty});
            llvm::Value *coroSize = builder_->CreateCall(coroSizeFn, {}, "coro.size");

            // === Step 4: malloc ===
            auto mallocTy = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFn = module_->getOrInsertFunction("malloc", mallocTy);
            llvm::Value *coroAlloc = builder_->CreateCall(mallocFn, {coroSize}, "coro.alloc");

            // === Step 5: coro.begin ===
            auto coroBeginFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_begin);
            llvm::Value *coroHdl = builder_->CreateCall(coroBeginFn, {coroId, coroAlloc}, "coro.hdl");
            context_info_.coroHandle = coroHdl;

            // === Step 6: Initial suspend (required for CoroSplit) ===
            // (AR) التعليق الأولي: يُعيد التحكم للمستدعي بعد إنشاء الإطار
            // (EN) Initial suspend: returns control to caller after frame creation
            // After CoroSplit, the ramp function returns here, and the body
            // runs when coro.resume is called by the caller.
            auto coroSaveFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_save);
            llvm::Value *initSave = builder_->CreateCall(coroSaveFn, {coroHdl}, "coro.init.save");

            auto coroSuspFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_suspend);
            llvm::Value *initSusp = builder_->CreateCall(coroSuspFn, {
                                                                         initSave, llvm::ConstantInt::get(i1Ty, 0) // isFinal=false (initial suspend)
                                                                     },
                                                         "coro.init.susp");

            auto *initSwitch = builder_->CreateSwitch(initSusp, context_info_.coroSuspendBB, 2);
            initSwitch->addCase(llvm::ConstantInt::get(i8Ty, 0), initResumeBB);                // resume → continue body
            initSwitch->addCase(llvm::ConstantInt::get(i8Ty, 1), context_info_.coroCleanupBB); // destroy → cleanup

            // (AR) الآن نستمر في كتلة الاستئناف الأولي — هنا سيتم إدراج جسم الدالة
            // (EN) Now continue in init resume block — function body will be inserted here
            builder_->SetInsertPoint(initResumeBB);
            context_info_.currentBlock = initResumeBB;

            // (AR) حفظ المقبض في السياق حتى يمكن استخدامه في await و return
            // (EN) Save handle in context for use in await and return
            context_info_.namedValues["__coro_hdl"] = coroHdl;
            context_info_.namedValues["__coro_promise"] = promise;

            std::cerr << "[CORO] Preamble emitted for '" << sirFunc->getName() << "'" << std::endl;
        }

        void LLVMCodeGen::emitCoroutineEpilogue()
        {
            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8Ty = llvm::Type::getInt8Ty(*context_);
            auto i1Ty = llvm::Type::getInt1Ty(*context_);
            auto tokenTy = llvm::Type::getTokenTy(*context_);
            auto voidTy = llvm::Type::getVoidTy(*context_);

            llvm::Value *coroHdl = context_info_.coroHandle;
            llvm::Value *coroId = context_info_.coroId;

            if (!coroHdl || !coroId)
            {
                reportError("Coroutine epilogue: missing handle or id");
                return;
            }

            // === coro.final block ===
            // (AR) تعليق نهائي: يُخبر LLVM أن الكوروتين انتهى
            // (EN) Final suspend: tells LLVM the coroutine is done
            builder_->SetInsertPoint(context_info_.coroFinalBB);

            auto coroSaveFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_save);
            llvm::Value *finalSave = builder_->CreateCall(coroSaveFn, {coroHdl}, "coro.final.save");

            auto coroSuspFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_suspend);
            llvm::Value *finalSusp = builder_->CreateCall(coroSuspFn, {
                                                                          finalSave, llvm::ConstantInt::get(i1Ty, 1) // isFinal=true
                                                                      },
                                                          "coro.final.susp");

            auto *finalSwitch = builder_->CreateSwitch(finalSusp, context_info_.coroSuspendBB, 2);
            finalSwitch->addCase(llvm::ConstantInt::get(i8Ty, 0), context_info_.coroSuspendBB);
            finalSwitch->addCase(llvm::ConstantInt::get(i8Ty, 1), context_info_.coroCleanupBB);

            // === coro.cleanup block ===
            // (AR) تنظيف: تحرير ذاكرة الإطار
            // (EN) Cleanup: free frame memory
            builder_->SetInsertPoint(context_info_.coroCleanupBB);

            auto coroFreeFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_free);
            llvm::Value *mem = builder_->CreateCall(coroFreeFn, {coroId, coroHdl}, "coro.mem");

            auto freeTy = llvm::FunctionType::get(voidTy, {ptrTy}, false);
            auto freeFn = module_->getOrInsertFunction("free", freeTy);
            builder_->CreateCall(freeFn, {mem});

            builder_->CreateBr(context_info_.coroSuspendBB);

            // === coro.suspend block ===
            // (AR) تعليق: إنهاء الكوروتين وإرجاع المقبض
            // (EN) Suspend: end coroutine and return handle
            builder_->SetInsertPoint(context_info_.coroSuspendBB);

            auto coroEndFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_end);
            builder_->CreateCall(coroEndFn, {coroHdl,
                                             llvm::ConstantInt::get(i1Ty, 0),
                                             llvm::ConstantTokenNone::get(*context_)});

            builder_->CreateRet(coroHdl);

            // (AR) تنظيف سياق الكوروتين
            // (EN) Clean up coroutine context
            context_info_.coroHandle = nullptr;
            context_info_.coroPromise = nullptr;
            context_info_.coroId = nullptr;
            context_info_.coroCleanupBB = nullptr;
            context_info_.coroSuspendBB = nullptr;
            context_info_.coroFinalBB = nullptr;

            std::cerr << "[CORO] Epilogue emitted" << std::endl;
        }

        llvm::Value *LLVMCodeGen::emitCoroSuspend(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) تعبير انتظر: استدعاء الكوروتين الداخلي → تكرار الاستئناف → الحصول على النتيجة
            // (EN) Await expression: call inner coroutine → resume loop → get result

            if (!inst || inst->operands.empty())
            {
                reportError("CORO_SUSPEND requires at least 1 operand (coroutine handle)");
                return nullptr;
            }

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i1Ty = llvm::Type::getInt1Ty(*context_);

            // (AR) الحصول على مقبض الكوروتين الداخلي
            // (EN) Get inner coroutine handle
            llvm::Value *innerHdl = resolveOperand(inst->operands[0]);
            if (!innerHdl)
            {
                reportError("CORO_SUSPEND: failed to resolve inner coroutine handle");
                return nullptr;
            }

            // (AR) تحويل إلى مؤشر إذا كان i64
            // (EN) Convert to pointer if i64
            if (innerHdl->getType()->isIntegerTy())
            {
                innerHdl = builder_->CreateIntToPtr(innerHdl, ptrTy, "await.hdl.ptr");
            }

            int suspId = context_info_.coroSuspendCount++;
            std::string prefix = "await." + std::to_string(suspId);

            auto *curFunc = context_info_.currentFunction;

            // (AR) إنشاء كتل حلقة الاستئناف
            // (EN) Create resume loop blocks
            auto *loopBB = llvm::BasicBlock::Create(*context_, prefix + ".loop", curFunc);
            auto *resumeBB = llvm::BasicBlock::Create(*context_, prefix + ".resume", curFunc);
            auto *doneBB = llvm::BasicBlock::Create(*context_, prefix + ".done", curFunc);
            auto *contBB = llvm::BasicBlock::Create(*context_, prefix + ".cont", curFunc);

            builder_->CreateBr(loopBB);

            // === Loop: check if inner coroutine is done ===
            builder_->SetInsertPoint(loopBB);
            auto coroDoneFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_done);
            llvm::Value *done = builder_->CreateCall(coroDoneFn, {innerHdl}, prefix + ".isdone");
            builder_->CreateCondBr(done, doneBB, resumeBB);

            // === Resume: resume inner coroutine ===
            builder_->SetInsertPoint(resumeBB);
            auto coroResumeFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_resume);
            builder_->CreateCall(coroResumeFn, {innerHdl});
            builder_->CreateBr(loopBB);

            // === Done: get result from inner coroutine's promise ===
            builder_->SetInsertPoint(doneBB);
            auto coroPromiseFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_promise, {});
            llvm::Value *innerPromise = builder_->CreateCall(coroPromiseFn, {
                                                                                innerHdl, llvm::ConstantInt::get(i32Ty, 8), // alignment
                                                                                llvm::ConstantInt::get(i1Ty, 0)             // from = false
                                                                            },
                                                             prefix + ".promise");

            llvm::Value *result = builder_->CreateLoad(i64Ty, innerPromise, prefix + ".result");

            // (AR) تدمير الكوروتين الداخلي
            // (EN) Destroy inner coroutine
            auto coroDestroyFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_destroy);
            builder_->CreateCall(coroDestroyFn, {innerHdl});

            builder_->CreateBr(contBB);

            // === Continue ===
            builder_->SetInsertPoint(contBB);
            context_info_.currentBlock = contBB;

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        llvm::Value *LLVMCodeGen::emitCoroReturn(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) إرجاع قيمة من كوروتين: تخزين في الوعد + القفز للإنهاء النهائي
            // (EN) Return value from coroutine: store in promise + jump to final suspend

            if (!inst || inst->operands.empty())
            {
                // (AR) لا قيمة — فقط اقفز للإنهاء
                // (EN) No value — just jump to final
                if (context_info_.coroFinalBB)
                {
                    builder_->CreateBr(context_info_.coroFinalBB);
                }
                return nullptr;
            }

            llvm::Value *retVal = resolveOperand(inst->operands[0]);
            if (!retVal)
            {
                if (context_info_.coroFinalBB)
                {
                    builder_->CreateBr(context_info_.coroFinalBB);
                }
                return nullptr;
            }

            // (AR) تخزين القيمة في الوعد
            // (EN) Store value in promise
            if (context_info_.coroPromise)
            {
                auto i64Ty = llvm::Type::getInt64Ty(*context_);
                llvm::Value *storeVal = retVal;

                // (AR) تحويل النوع إذا لزم الأمر
                // (EN) Cast type if needed
                if (retVal->getType()->isDoubleTy())
                {
                    storeVal = builder_->CreateFPToSI(retVal, i64Ty, "coro.ret.toi64");
                }
                else if (retVal->getType()->isPointerTy())
                {
                    storeVal = builder_->CreatePtrToInt(retVal, i64Ty, "coro.ret.ptrtoi64");
                }
                else if (retVal->getType() != i64Ty)
                {
                    storeVal = builder_->CreateSExtOrTrunc(retVal, i64Ty, "coro.ret.cast");
                }

                builder_->CreateStore(storeVal, context_info_.coroPromise);
            }

            // (AR) القفز إلى كتلة الإنهاء النهائي
            // (EN) Branch to final suspend block
            if (context_info_.coroFinalBB)
            {
                builder_->CreateBr(context_info_.coroFinalBB);
            }

            return retVal;
        }

        // ============================================================================
        // emitGeneratorYield - أنتج قيمة من مولّد
        // Yield a value from a generator (store in promise + non-final coro.suspend)
        // ============================================================================
        llvm::Value *LLVMCodeGen::emitGeneratorYield(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("GENERATOR_YIELD requires at least 1 operand (value to yield)");
                return nullptr;
            }

            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8Ty = llvm::Type::getInt8Ty(*context_);
            auto i1Ty = llvm::Type::getInt1Ty(*context_);

            // (AR) الحصول على القيمة المُنتجة
            // (EN) Get the yielded value
            llvm::Value *yieldVal = resolveOperand(inst->operands[0]);
            if (!yieldVal)
            {
                reportError("GENERATOR_YIELD: failed to resolve yield value");
                return nullptr;
            }

            // (AR) تخزين القيمة في الوعد (promise)
            // (EN) Store value in promise
            if (context_info_.coroPromise)
            {
                llvm::Value *storeVal = yieldVal;

                // (AR) تحويل النوع إذا لزم
                // (EN) Cast type if needed
                if (yieldVal->getType()->isDoubleTy())
                {
                    storeVal = builder_->CreateFPToSI(yieldVal, i64Ty, "gen.yield.toi64");
                }
                else if (yieldVal->getType()->isPointerTy())
                {
                    storeVal = builder_->CreatePtrToInt(yieldVal, i64Ty, "gen.yield.ptrtoi64");
                }
                else if (yieldVal->getType() != i64Ty)
                {
                    storeVal = builder_->CreateSExtOrTrunc(yieldVal, i64Ty, "gen.yield.cast");
                }

                builder_->CreateStore(storeVal, context_info_.coroPromise);
            }

            // (AR) حفظ + تعليق غير نهائي (non-final suspend)
            // (EN) Save + non-final suspend
            llvm::Value *coroHdl = context_info_.coroHandle;
            if (!coroHdl)
            {
                reportError("GENERATOR_YIELD: no coroutine handle");
                return nullptr;
            }

            int suspId = context_info_.coroSuspendCount++;
            std::string prefix = "gen.yield." + std::to_string(suspId);

            auto *curFunc = context_info_.currentFunction;

            auto coroSaveFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_save);
            llvm::Value *save = builder_->CreateCall(coroSaveFn, {coroHdl}, prefix + ".save");

            auto coroSuspFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_suspend);
            llvm::Value *susp = builder_->CreateCall(coroSuspFn, {
                                                                     save, llvm::ConstantInt::get(i1Ty, 0) // isFinal=false
                                                                 },
                                                     prefix + ".susp");

            // (AR) switch: 0 → استئناف (متابعة الجسم), 1 → تنظيف (تدمير)
            // (EN) switch: 0 → resume (continue body), 1 → cleanup (destroy)
            auto *resumeBB = llvm::BasicBlock::Create(*context_, prefix + ".resume", curFunc);

            auto *sw = builder_->CreateSwitch(susp, context_info_.coroSuspendBB, 2);
            sw->addCase(llvm::ConstantInt::get(i8Ty, 0), resumeBB);                    // resume → continue
            sw->addCase(llvm::ConstantInt::get(i8Ty, 1), context_info_.coroCleanupBB); // destroy → cleanup

            // (AR) متابعة التنفيذ في كتلة الاستئناف
            // (EN) Continue execution in resume block
            builder_->SetInsertPoint(resumeBB);
            context_info_.currentBlock = resumeBB;

            std::cerr << "[GEN] Emitted GENERATOR_YIELD #" << suspId << std::endl;
            return nullptr;
        }

        // ============================================================================
        // ============================================================================
        // emitGeneratorConsume - استهلاك مولّد وتجميع القيم المُنتَجة في مصفوفة SadArray
        // Consume a generator and collect all yielded values into a SadArray
        // Returns a pointer to a SadArray { i64 length, i64 capacity, ptr data }
        // ============================================================================
        llvm::Value *LLVMCodeGen::emitGeneratorConsume(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("GENERATOR_CONSUME requires at least 1 operand (generator handle)");
                return nullptr;
            }

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i1Ty = llvm::Type::getInt1Ty(*context_);

            // (AR) نوع بنية SadArray: {i64 length, i64 capacity, ptr data}
            // (EN) SadArray struct type: {i64 length, i64 capacity, ptr data}
            llvm::StructType *arrTy = llvm::StructType::getTypeByName(*context_, "SadArray");
            if (!arrTy)
            {
                arrTy = llvm::StructType::create(*context_, {i64Ty, i64Ty, ptrTy}, "SadArray");
            }

            // (AR) الحصول على مقبض المولّد
            // (EN) Get generator handle
            llvm::Value *genHdl = resolveOperand(inst->operands[0]);
            if (!genHdl)
            {
                reportError("GENERATOR_CONSUME: failed to resolve generator handle");
                return nullptr;
            }

            // (AR) تحويل إلى مؤشر إذا كان i64
            // (EN) Convert to pointer if i64
            if (genHdl->getType()->isIntegerTy())
            {
                genHdl = builder_->CreateIntToPtr(genHdl, ptrTy, "gen.hdl.ptr");
            }

            auto *curFunc = context_info_.currentFunction;

            // ================================================================
            // (AR) الخطوة 1: تخصيص SadArray أولية بسعة 8
            // (EN) Step 1: Allocate initial SadArray with capacity 8
            // ================================================================
            auto mallocTy = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFn = module_->getOrInsertFunction("malloc", mallocTy);

            // (AR) حساب حجم بنية SadArray
            // (EN) Calculate SadArray struct size
            const llvm::DataLayout &DL = module_->getDataLayout();
            uint64_t arrStructSize = DL.getTypeAllocSize(arrTy);

            llvm::Value *arrPtr = builder_->CreateCall(
                mallocFn, {llvm::ConstantInt::get(i64Ty, arrStructSize)}, "gen.arr");

            // length = 0
            llvm::Value *lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "gen.arr.len");
            builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), lenGep);

            // capacity = 8
            llvm::Value *capGep = builder_->CreateStructGEP(arrTy, arrPtr, 1, "gen.arr.cap");
            llvm::Value *initCap = llvm::ConstantInt::get(i64Ty, 8);
            builder_->CreateStore(initCap, capGep);

            // data = malloc(8 * 8) — 8 عناصر × 8 بايت لكل i64
            // data = malloc(8 * 8) — 8 elements × 8 bytes per i64
            llvm::Value *dataPtr = builder_->CreateCall(
                mallocFn, {llvm::ConstantInt::get(i64Ty, 64)}, "gen.arr.data");
            llvm::Value *dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "gen.arr.datagep");
            builder_->CreateStore(dataPtr, dataGep);

            // ================================================================
            // (AR) الخطوة 2: حلقة استئناف المولّد وجمع القيم
            // (EN) Step 2: Resume loop collecting yielded values
            // ================================================================
            auto *loopBB = llvm::BasicBlock::Create(*context_, "gen.loop", curFunc);
            auto *yieldBB = llvm::BasicBlock::Create(*context_, "gen.yield", curFunc);
            auto *growBB = llvm::BasicBlock::Create(*context_, "gen.grow", curFunc);
            auto *storeBB = llvm::BasicBlock::Create(*context_, "gen.store", curFunc);
            auto *endBB = llvm::BasicBlock::Create(*context_, "gen.end", curFunc);

            builder_->CreateBr(loopBB);

            // === Loop: resume generator and check if done ===
            builder_->SetInsertPoint(loopBB);

            auto coroResumeFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_resume);
            builder_->CreateCall(coroResumeFn, {genHdl});

            auto coroDoneFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_done);
            llvm::Value *done = builder_->CreateCall(coroDoneFn, {genHdl}, "gen.isdone");
            builder_->CreateCondBr(done, endBB, yieldBB);

            // === Yield: read value from promise ===
            builder_->SetInsertPoint(yieldBB);

            auto coroPromiseFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_promise, {});
            llvm::Value *promisePtr = builder_->CreateCall(coroPromiseFn, {
                                                                              genHdl, llvm::ConstantInt::get(i32Ty, 8), // alignment
                                                                              llvm::ConstantInt::get(i1Ty, 0)           // from = false
                                                                          },
                                                           "gen.promise");

            llvm::Value *yieldedVal = builder_->CreateLoad(i64Ty, promisePtr, "gen.val");

            // (AR) فحص: هل الطول == السعة؟ إذا نعم → توسيع المصفوفة
            // (EN) Check: is length == capacity? If yes → grow array
            llvm::Value *curLen = builder_->CreateLoad(i64Ty,
                                                       builder_->CreateStructGEP(arrTy, arrPtr, 0, "gen.len.gep1"), "gen.curlen");
            llvm::Value *curCap = builder_->CreateLoad(i64Ty,
                                                       builder_->CreateStructGEP(arrTy, arrPtr, 1, "gen.cap.gep1"), "gen.curcap");
            llvm::Value *needGrow = builder_->CreateICmpEQ(curLen, curCap, "gen.needgrow");
            builder_->CreateCondBr(needGrow, growBB, storeBB);

            // === Grow: double capacity and realloc data ===
            builder_->SetInsertPoint(growBB);

            llvm::Value *newCap = builder_->CreateMul(curCap, llvm::ConstantInt::get(i64Ty, 2), "gen.newcap");
            builder_->CreateStore(newCap,
                                  builder_->CreateStructGEP(arrTy, arrPtr, 1, "gen.cap.gep2"));

            // (AR) realloc(data, newCap * 8)
            // (EN) realloc(data, newCap * 8)
            auto reallocTy = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty}, false);
            auto reallocFn = module_->getOrInsertFunction("realloc", reallocTy);
            llvm::Value *oldData = builder_->CreateLoad(ptrTy,
                                                        builder_->CreateStructGEP(arrTy, arrPtr, 2, "gen.data.gep.grow"), "gen.olddata");
            llvm::Value *newSize = builder_->CreateMul(newCap, llvm::ConstantInt::get(i64Ty, 8), "gen.newsz");
            llvm::Value *newData = builder_->CreateCall(reallocFn, {oldData, newSize}, "gen.newdata");
            builder_->CreateStore(newData,
                                  builder_->CreateStructGEP(arrTy, arrPtr, 2, "gen.data.gep.grow2"));

            builder_->CreateBr(storeBB);

            // === Store: write yielded value into array[length], increment length ===
            builder_->SetInsertPoint(storeBB);

            // (AR) PHI لمؤشر البيانات (من yieldBB أو growBB)
            // (EN) PHI for data pointer (from yieldBB or growBB)
            // (AR) نعيد قراءة الطول والبيانات من البنية لتجنب PHI المعقد
            // (EN) Re-read length and data from struct to avoid complex PHI
            llvm::Value *storeLen = builder_->CreateLoad(i64Ty,
                                                         builder_->CreateStructGEP(arrTy, arrPtr, 0, "gen.len.gep.store"), "gen.storelen");
            llvm::Value *storeData = builder_->CreateLoad(ptrTy,
                                                          builder_->CreateStructGEP(arrTy, arrPtr, 2, "gen.data.gep.store"), "gen.storedata");

            // data[length] = yieldedVal
            llvm::Value *elemPtr = builder_->CreateGEP(i64Ty, storeData, {storeLen}, "gen.elemptr");
            builder_->CreateStore(yieldedVal, elemPtr);

            // length++
            llvm::Value *newLen = builder_->CreateAdd(storeLen, llvm::ConstantInt::get(i64Ty, 1), "gen.newlen");
            builder_->CreateStore(newLen,
                                  builder_->CreateStructGEP(arrTy, arrPtr, 0, "gen.len.gep.inc"));

            builder_->CreateBr(loopBB);

            // === End: destroy generator and return array pointer as i64 ===
            builder_->SetInsertPoint(endBB);

            auto coroDestroyFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_destroy);
            builder_->CreateCall(coroDestroyFn, {genHdl});

            // (AR) تحويل مؤشر المصفوفة إلى i64 (MSB tagging: bit63=0 → ptr)
            // (EN) Convert array pointer to i64 (MSB tagging: bit63=0 → ptr)
            llvm::Value *arrAsI64 = builder_->CreatePtrToInt(arrPtr, i64Ty, "gen.arr.i64");

            context_info_.currentBlock = endBB;

            // (AR) تخزين النتيجة في السجل
            // (EN) Store result in register
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = arrAsI64;
            }

            std::cerr << "[GEN] Emitted GENERATOR_CONSUME" << std::endl;
            return arrAsI64;
        }
    } // namespace LLVM
} // namespace Sad