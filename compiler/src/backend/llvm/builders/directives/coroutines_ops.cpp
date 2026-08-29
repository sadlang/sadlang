/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/directives/coroutines_codegen.h"
#include "sad_dyn_repr.h" // (AR) DynKind لتهيئة الحقل ٤ homogKind / (EN) DynKind for field 4 homogKind init
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
        void CoroutinesCodeGen::emitCoroutinePreamble(std::shared_ptr<SIRFunction> sirFunc, llvm::Function *llvmFunc)
        {
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto i1Ty = llvm::Type::getInt1Ty(*cg_.context_);
            auto tokenTy = llvm::Type::getTokenTy(*cg_.context_);
            auto voidTy = llvm::Type::getVoidTy(*cg_.context_);

            // (AR) التأكد من أننا في كتلة entry
            // (EN) Make sure we're in the entry block
            auto *entryBB = &llvmFunc->getEntryBlock();
            cg_.builder_->SetInsertPoint(entryBB);

            // (AR) إنشاء الكتل الخاصة بالكوروتين
            // (EN) Create coroutine-specific blocks
            cg_.context_info_.coroFinalBB = llvm::BasicBlock::Create(*cg_.context_, "coro.final", llvmFunc);
            cg_.context_info_.coroCleanupBB = llvm::BasicBlock::Create(*cg_.context_, "coro.cleanup", llvmFunc);
            cg_.context_info_.coroSuspendBB = llvm::BasicBlock::Create(*cg_.context_, "coro.suspend", llvmFunc);

            // (AR) كتلة لاستمرار التنفيذ بعد التعليق الأولي
            // (EN) Block for continuing execution after initial suspend
            auto *initResumeBB = llvm::BasicBlock::Create(*cg_.context_, "coro.init.resume", llvmFunc);

            cg_.context_info_.coroSuspendCount = 0;

            // === Step 1: Allocate promise (for return value) ===
            llvm::Value *promise = cg_.builder_->CreateAlloca(i64Ty, nullptr, "coro.promise");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), promise);
            cg_.context_info_.coroPromise = promise;

            // === Step 2: coro.id ===
            auto coroIdFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_id);
            llvm::Value *coroId = cg_.builder_->CreateCall(coroIdFn, {llvm::ConstantInt::get(i32Ty, 0), promise, llvm::ConstantPointerNull::get(ptrTy), llvm::ConstantPointerNull::get(ptrTy)}, "coro.id");
            cg_.context_info_.coroId = coroId;

            // === Step 3: coro.size ===
            auto coroSizeFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_size, {i64Ty});
            llvm::Value *coroSize = cg_.builder_->CreateCall(coroSizeFn, {}, "coro.size");

            // === Step 4: malloc ===
            llvm::Value *coroAlloc = cg_.emitMalloc(coroSize, "coro.alloc");

            // === Step 5: coro.begin ===
            auto coroBeginFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_begin);
            llvm::Value *coroHdl = cg_.builder_->CreateCall(coroBeginFn, {coroId, coroAlloc}, "coro.hdl");
            cg_.context_info_.coroHandle = coroHdl;

            // === Step 6: Initial suspend (required for CoroSplit) ===
            // (AR) التعليق الأولي: يُعيد التحكم للمستدعي بعد إنشاء الإطار
            // (EN) Initial suspend: returns control to caller after frame creation
            // After CoroSplit, the ramp function returns here, and the body
            // runs when coro.resume is called by the caller.
            auto coroSaveFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_save);
            llvm::Value *initSave = cg_.builder_->CreateCall(coroSaveFn, {coroHdl}, "coro.init.save");

            auto coroSuspFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_suspend);
            llvm::Value *initSusp = cg_.builder_->CreateCall(coroSuspFn, {
                                                                         initSave, llvm::ConstantInt::get(i1Ty, 0) // isFinal=false (initial suspend)
                                                                     },
                                                         "coro.init.susp");

            auto *initSwitch = cg_.builder_->CreateSwitch(initSusp, cg_.context_info_.coroSuspendBB, 2);
            initSwitch->addCase(llvm::ConstantInt::get(i8Ty, 0), initResumeBB);                // resume → continue body
            initSwitch->addCase(llvm::ConstantInt::get(i8Ty, 1), cg_.context_info_.coroCleanupBB); // destroy → cleanup

            // (AR) الآن نستمر في كتلة الاستئناف الأولي — هنا سيتم إدراج جسم الدالة
            // (EN) Now continue in init resume block — function body will be inserted here
            cg_.builder_->SetInsertPoint(initResumeBB);
            cg_.context_info_.currentBlock = initResumeBB;

            // (AR) حفظ المقبض في السياق حتى يمكن استخدامه في await و return
            // (EN) Save handle in context for use in await and return
            cg_.context_info_.namedValues["__coro_hdl"] = coroHdl;
            cg_.context_info_.namedValues["__coro_promise"] = promise;
        }

        void CoroutinesCodeGen::emitCoroutineEpilogue()
        {
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto i1Ty = llvm::Type::getInt1Ty(*cg_.context_);
            auto tokenTy = llvm::Type::getTokenTy(*cg_.context_);
            auto voidTy = llvm::Type::getVoidTy(*cg_.context_);

            llvm::Value *coroHdl = cg_.context_info_.coroHandle;
            llvm::Value *coroId = cg_.context_info_.coroId;

            if (!coroHdl || !coroId)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Coroutine"}});
                return;
            }

            // === coro.final block ===
            // (AR) تعليق نهائي: يُخبر LLVM أن الكوروتين انتهى
            // (EN) Final suspend: tells LLVM the coroutine is done
            cg_.builder_->SetInsertPoint(cg_.context_info_.coroFinalBB);

            auto coroSaveFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_save);
            llvm::Value *finalSave = cg_.builder_->CreateCall(coroSaveFn, {coroHdl}, "coro.final.save");

            auto coroSuspFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_suspend);
            llvm::Value *finalSusp = cg_.builder_->CreateCall(coroSuspFn, {
                                                                          finalSave, llvm::ConstantInt::get(i1Ty, 1) // isFinal=true
                                                                      },
                                                          "coro.final.susp");

            auto *finalSwitch = cg_.builder_->CreateSwitch(finalSusp, cg_.context_info_.coroSuspendBB, 2);
            finalSwitch->addCase(llvm::ConstantInt::get(i8Ty, 0), cg_.context_info_.coroSuspendBB);
            finalSwitch->addCase(llvm::ConstantInt::get(i8Ty, 1), cg_.context_info_.coroCleanupBB);

            // === coro.cleanup block ===
            // (AR) تنظيف: تحرير ذاكرة الإطار
            // (EN) Cleanup: free frame memory
            cg_.builder_->SetInsertPoint(cg_.context_info_.coroCleanupBB);

            auto coroFreeFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_free);
            llvm::Value *mem = cg_.builder_->CreateCall(coroFreeFn, {coroId, coroHdl}, "coro.mem");

            cg_.emitFreeCall(mem);

            cg_.builder_->CreateBr(cg_.context_info_.coroSuspendBB);

            // === coro.suspend block ===
            // (AR) تعليق: إنهاء الكوروتين وإرجاع المقبض
            // (EN) Suspend: end coroutine and return handle
            cg_.builder_->SetInsertPoint(cg_.context_info_.coroSuspendBB);

            auto coroEndFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_end);
            cg_.builder_->CreateCall(coroEndFn, {coroHdl,
                                             llvm::ConstantInt::get(i1Ty, 0),
                                             llvm::ConstantTokenNone::get(*cg_.context_)});

            cg_.builder_->CreateRet(coroHdl);

            // (AR) تنظيف سياق الكوروتين
            // (EN) Clean up coroutine context
            cg_.context_info_.coroHandle = nullptr;
            cg_.context_info_.coroPromise = nullptr;
            cg_.context_info_.coroId = nullptr;
            cg_.context_info_.coroCleanupBB = nullptr;
            cg_.context_info_.coroSuspendBB = nullptr;
            cg_.context_info_.coroFinalBB = nullptr;
        }

        llvm::Value *CoroutinesCodeGen::emitCoroSuspend(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) تعبير انتظر: استدعاء الكوروتين الداخلي → تكرار الاستئناف → الحصول على النتيجة
            // (EN) Await expression: call inner coroutine → resume loop → get result

            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "CORO_SUSPEND"}});
                return nullptr;
            }

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i1Ty = llvm::Type::getInt1Ty(*cg_.context_);

            // (AR) الحصول على مقبض الكوروتين الداخلي
            // (EN) Get inner coroutine handle
            llvm::Value *innerHdl = cg_.resolveOperand(inst->operands[0]);
            if (!innerHdl)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "CORO_SUSPEND"}});
                return nullptr;
            }

            // (AR) تحويل إلى مؤشر إذا كان i64
            // (EN) Convert to pointer if i64
            if (innerHdl->getType()->isIntegerTy())
            {
                innerHdl = cg_.builder_->CreateIntToPtr(innerHdl, ptrTy, "await.hdl.ptr");
            }

            int suspId = cg_.context_info_.coroSuspendCount++;
            std::string prefix = "await." + std::to_string(suspId);

            auto *curFunc = cg_.context_info_.currentFunction;

            // (AR) إنشاء كتل حلقة الاستئناف
            // (EN) Create resume loop blocks
            auto *loopBB = llvm::BasicBlock::Create(*cg_.context_, prefix + ".loop", curFunc);
            auto *resumeBB = llvm::BasicBlock::Create(*cg_.context_, prefix + ".resume", curFunc);
            auto *doneBB = llvm::BasicBlock::Create(*cg_.context_, prefix + ".done", curFunc);
            auto *contBB = llvm::BasicBlock::Create(*cg_.context_, prefix + ".cont", curFunc);

            cg_.builder_->CreateBr(loopBB);

            // === Loop: check if inner coroutine is done ===
            cg_.builder_->SetInsertPoint(loopBB);
            auto coroDoneFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_done);
            llvm::Value *done = cg_.builder_->CreateCall(coroDoneFn, {innerHdl}, prefix + ".isdone");
            cg_.builder_->CreateCondBr(done, doneBB, resumeBB);

            // === Resume: resume inner coroutine ===
            cg_.builder_->SetInsertPoint(resumeBB);
            auto coroResumeFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_resume);
            cg_.builder_->CreateCall(coroResumeFn, {innerHdl});
            cg_.builder_->CreateBr(loopBB);

            // === Done: get result from inner coroutine's promise ===
            cg_.builder_->SetInsertPoint(doneBB);
            auto coroPromiseFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_promise, {});
            llvm::Value *innerPromise = cg_.builder_->CreateCall(coroPromiseFn, {
                                                                                innerHdl, llvm::ConstantInt::get(i32Ty, 8), // alignment
                                                                                llvm::ConstantInt::get(i1Ty, 0)             // from = false
                                                                            },
                                                             prefix + ".promise");

            llvm::Value *result = cg_.builder_->CreateLoad(i64Ty, innerPromise, prefix + ".result");

            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 الوعاءُ عرضٌ لا نوع. فتُعادُ القيمةُ إلى نوعِها المُصرَّحِ في
            //      نتيجةِ التعليمة، وإلّا قُرِئت صحيحًا مهما كانت (ISSUE-177).
            //      ⚠️ و`bitcast` لا `sitofp` للعشريّ: الحمولةُ **نمطُ بتّاتِ** الـ
            //      double لا قيمتُه الصحيحة — ونظيرُها في `emitCoroReturn` يخزّن
            //      بالبتّات. والاثنان عقدٌ واحدٌ لو افترقا لَعاد الجوابُ خاطئًا صامتًا.
            // (EN) The container is a width, not a type: restore the value to the type
            //      declared on the instruction's result, or it reads as an integer
            //      whatever it holds. bitcast — not sitofp — for a float: the payload is
            //      the double's BIT PATTERN, matching the store side in emitCoroReturn.
            //      The two are one contract; if they drift the answer is silently wrong.
            // ════════════════════════════════════════════════════════════════
            if (inst->result.has_value())
            {
                switch (inst->result->dataType)
                {
                case Sad::Compiler::SIR::SadTypeKind::Float:
                    result = cg_.builder_->CreateBitCast(result, cg_.getDoubleType(),
                                                         prefix + ".result.f64");
                    break;
                case Sad::Compiler::SIR::SadTypeKind::String:
                case Sad::Compiler::SIR::SadTypeKind::Array:
                case Sad::Compiler::SIR::SadTypeKind::Map:
                case Sad::Compiler::SIR::SadTypeKind::Class:
                case Sad::Compiler::SIR::SadTypeKind::Struct:
                case Sad::Compiler::SIR::SadTypeKind::Pointer:
                    result = cg_.builder_->CreateIntToPtr(result, ptrTy, prefix + ".result.ptr");
                    break;
                default:
                    break;
                }
            }

            // (AR) تدمير الكوروتين الداخلي
            // (EN) Destroy inner coroutine
            auto coroDestroyFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_destroy);
            cg_.builder_->CreateCall(coroDestroyFn, {innerHdl});

            cg_.builder_->CreateBr(contBB);

            // === Continue ===
            cg_.builder_->SetInsertPoint(contBB);
            cg_.context_info_.currentBlock = contBB;

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        llvm::Value *CoroutinesCodeGen::emitCoroReturn(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) إرجاع قيمة من كوروتين: تخزين في الوعد + القفز للإنهاء النهائي
            // (EN) Return value from coroutine: store in promise + jump to final suspend

            if (!inst || inst->operands.empty())
            {
                // (AR) لا قيمة — فقط اقفز للإنهاء
                // (EN) No value — just jump to final
                if (cg_.context_info_.coroFinalBB)
                {
                    cg_.builder_->CreateBr(cg_.context_info_.coroFinalBB);
                }
                return nullptr;
            }

            llvm::Value *retVal = cg_.resolveOperand(inst->operands[0]);
            if (!retVal)
            {
                if (cg_.context_info_.coroFinalBB)
                {
                    cg_.builder_->CreateBr(cg_.context_info_.coroFinalBB);
                }
                return nullptr;
            }

            // (AR) تخزين القيمة في الوعد
            // (EN) Store value in promise
            if (cg_.context_info_.coroPromise)
            {
                auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
                llvm::Value *storeVal = retVal;

                // (AR) تحويل النوع إذا لزم الأمر
                // (EN) Cast type if needed
                // (AR) 🔴 `bitcast` لا `fptosi`: القطعُ يُتلِفُ القيمةَ **عند التخزين**
                //      فلا يُصلِحُها قارئٌ مهما عَلِمَ نوعَها — قِيس أنّ `ارجع 2.5`
                //      كانت تُعطي `2` (ISSUE-177). والنظيرُ في `emitCoroSuspend`
                //      يقرأ بالبتّات، والاثنان عقدٌ واحد.
                // (EN) bitcast, not fptosi: truncating destroys the value AT THE STORE, so
                //      no reader can recover it however well it knows the type — measured:
                //      `ارجع 2.5` yielded 2. The reader in emitCoroSuspend bitcasts back;
                //      the two are one contract.
                if (retVal->getType()->isDoubleTy())
                {
                    storeVal = cg_.builder_->CreateBitCast(retVal, i64Ty, "coro.ret.f64bits");
                }
                else if (retVal->getType()->isPointerTy())
                {
                    storeVal = cg_.builder_->CreatePtrToInt(retVal, i64Ty, "coro.ret.ptrtoi64");
                }
                else if (retVal->getType() != i64Ty)
                {
                    storeVal = cg_.builder_->CreateSExtOrTrunc(retVal, i64Ty, "coro.ret.cast");
                }

                cg_.builder_->CreateStore(storeVal, cg_.context_info_.coroPromise);
            }

            // (AR) القفز إلى كتلة الإنهاء النهائي
            // (EN) Branch to final suspend block
            if (cg_.context_info_.coroFinalBB)
            {
                cg_.builder_->CreateBr(cg_.context_info_.coroFinalBB);
            }

            return retVal;
        }

        // ============================================================================
        // emitGeneratorYield - أنتج قيمة من مولّد
        // Yield a value from a generator (store in promise + non-final coro.suspend)
        // ============================================================================
        llvm::Value *CoroutinesCodeGen::emitGeneratorYield(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "GENERATOR_YIELD"}});
                return nullptr;
            }

            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto i1Ty = llvm::Type::getInt1Ty(*cg_.context_);

            // (AR) الحصول على القيمة المُنتجة
            // (EN) Get the yielded value
            llvm::Value *yieldVal = cg_.resolveOperand(inst->operands[0]);
            if (!yieldVal)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "GENERATOR_YIELD"}});
                return nullptr;
            }

            // (AR) تخزين القيمة في الوعد (promise)
            // (EN) Store value in promise
            if (cg_.context_info_.coroPromise)
            {
                llvm::Value *storeVal = yieldVal;

                // (AR) تحويل النوع إذا لزم
                // (EN) Cast type if needed
                if (yieldVal->getType()->isDoubleTy())
                {
                    storeVal = cg_.builder_->CreateFPToSI(yieldVal, i64Ty, "gen.yield.toi64");
                }
                else if (yieldVal->getType()->isPointerTy())
                {
                    storeVal = cg_.builder_->CreatePtrToInt(yieldVal, i64Ty, "gen.yield.ptrtoi64");
                }
                else if (yieldVal->getType() != i64Ty)
                {
                    storeVal = cg_.builder_->CreateSExtOrTrunc(yieldVal, i64Ty, "gen.yield.cast");
                }

                cg_.builder_->CreateStore(storeVal, cg_.context_info_.coroPromise);
            }

            // (AR) حفظ + تعليق غير نهائي (non-final suspend)
            // (EN) Save + non-final suspend
            llvm::Value *coroHdl = cg_.context_info_.coroHandle;
            if (!coroHdl)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "GENERATOR_YIELD"}});
                return nullptr;
            }

            int suspId = cg_.context_info_.coroSuspendCount++;
            std::string prefix = "gen.yield." + std::to_string(suspId);

            auto *curFunc = cg_.context_info_.currentFunction;

            auto coroSaveFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_save);
            llvm::Value *save = cg_.builder_->CreateCall(coroSaveFn, {coroHdl}, prefix + ".save");

            auto coroSuspFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_suspend);
            llvm::Value *susp = cg_.builder_->CreateCall(coroSuspFn, {
                                                                     save, llvm::ConstantInt::get(i1Ty, 0) // isFinal=false
                                                                 },
                                                     prefix + ".susp");

            // (AR) switch: 0 → استئناف (متابعة الجسم), 1 → تنظيف (تدمير)
            // (EN) switch: 0 → resume (continue body), 1 → cleanup (destroy)
            auto *resumeBB = llvm::BasicBlock::Create(*cg_.context_, prefix + ".resume", curFunc);

            auto *sw = cg_.builder_->CreateSwitch(susp, cg_.context_info_.coroSuspendBB, 2);
            sw->addCase(llvm::ConstantInt::get(i8Ty, 0), resumeBB);                    // resume → continue
            sw->addCase(llvm::ConstantInt::get(i8Ty, 1), cg_.context_info_.coroCleanupBB); // destroy → cleanup

            // (AR) متابعة التنفيذ في كتلة الاستئناف
            // (EN) Continue execution in resume block
            cg_.builder_->SetInsertPoint(resumeBB);
            cg_.context_info_.currentBlock = resumeBB;

            // (AR) أنتج تعليمة بلا نتيجة (statement). لكن الموزّع المتدرّج يعتبر
            //      nullptr إشارةَ «غير معالَج ⇒ جرّب الطبقة التالية»، فكان يسقط عبر
            //      كلّ الطبقات ويبلّغ «Unsupported opcode:275» زائفًا فوق توليدٍ ناجح
            //      (نفس عرف emitEnvStore الذي يعيد قيمةً غير فارغة إشارةَ نجاح). نعيد
            //      قيمة التعليق (susp) غير الفارغة إشارةَ «عولِج بنجاح».
            // (EN) Yield is a void statement, but the tiered dispatcher reads nullptr
            //      as "unhandled ⇒ try next tier", so it fell through every tier and
            //      spuriously reported "Unsupported opcode:275" over a successful emit
            //      (mirroring emitEnvStore, which returns a non-null value to signal
            //      success). Return the non-null suspend value as the handled sentinel.
            return susp;
        }

        // ============================================================================
        // ============================================================================
        // emitGeneratorConsume - استهلاك مولّد وتجميع القيم المُنتَجة في مصفوفة SadArray
        // Consume a generator and collect all yielded values into a SadArray
        // Returns a pointer to a SadArray { i64 length, i64 capacity, ptr data }
        // ============================================================================
        llvm::Value *CoroutinesCodeGen::emitGeneratorConsume(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "GENERATOR_CONSUME"}});
                return nullptr;
            }

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i1Ty = llvm::Type::getInt1Ty(*cg_.context_);

            // (AR) نوع بنية SadArray: {i64 length, i64 capacity, ptr data}
            // (EN) SadArray struct type: {i64 length, i64 capacity, ptr data}
            llvm::StructType *arrTy = llvm::StructType::getTypeByName(*cg_.context_, "SadArray");
            if (!arrTy)
            {
                // (AR) 4 حقول {طول، سعة، بيانات، وسوم} مطابقةً للتعريف القانونيّ (getArrayStructType)
                //      كي يتّسق التخطيط أيًّا كان أوّلَ مُنشئٍ يُنشئ SadArray في الوحدة.
                // (EN) 4 fields {len, cap, data, tags} matching the canonical getArrayStructType so the
                //      layout is consistent regardless of which creator makes SadArray first.
                auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
                arrTy = llvm::StructType::create(*cg_.context_, {i64Ty, i64Ty, ptrTy, ptrTy, i8Ty}, "SadArray"); // homogKind (option A2): DynKind of a homogeneous array; read only when tags==null
            }

            // (AR) الحصول على مقبض المولّد
            // (EN) Get generator handle
            llvm::Value *genHdl = cg_.resolveOperand(inst->operands[0]);
            if (!genHdl)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "GENERATOR_CONSUME"}});
                return nullptr;
            }

            // (AR) تحويل إلى مؤشر إذا كان i64
            // (EN) Convert to pointer if i64
            if (genHdl->getType()->isIntegerTy())
            {
                genHdl = cg_.builder_->CreateIntToPtr(genHdl, ptrTy, "gen.hdl.ptr");
            }

            auto *curFunc = cg_.context_info_.currentFunction;

            // ================================================================
            // (AR) الخطوة 1: تخصيص SadArray أولية بسعة 8
            // (EN) Step 1: Allocate initial SadArray with capacity 8
            // ================================================================

            // (AR) حساب حجم بنية SadArray
            // (EN) Calculate SadArray struct size
            const llvm::DataLayout &DL = cg_.module_->getDataLayout();
            uint64_t arrStructSize = DL.getTypeAllocSize(arrTy);

            llvm::Value *arrPtr = cg_.emitMalloc(llvm::ConstantInt::get(i64Ty, arrStructSize), "gen.arr");

            // length = 0
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "gen.arr.len");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), lenGep);

            // capacity = 8
            llvm::Value *capGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 1, "gen.arr.cap");
            llvm::Value *initCap = llvm::ConstantInt::get(i64Ty, 8);
            cg_.builder_->CreateStore(initCap, capGep);

            // data = malloc(8 * 8) — 8 عناصر × 8 بايت لكل i64
            // data = malloc(8 * 8) — 8 elements × 8 bytes per i64
            llvm::Value *dataPtr = cg_.emitMalloc(llvm::ConstantInt::get(i64Ty, 64), "gen.arr.data");
            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "gen.arr.datagep");
            cg_.builder_->CreateStore(dataPtr, dataGep);

            // (AR) [وسم زمن-التشغيل] تهيئةُ الحقلين ٣ (tags) و٤ (homogKind): مالُك لا
            //      يُصفّر، فتركُهما قمامةً ⇒ إن قُرئت مصفوفةُ المولّد عبر مسار Any (مثل
            //      «مولّد في فرعِ إرجاعٍ متنافر» يوسّع نوعَ العنصر إلى Any) لتفرّع القارئُ
            //      على tags قمامةً (انهيار) أو قرأ homogKind قمامةً (وسمٌ خاطئ). tags=null
            //      (متجانسة، المسار الساكن)، homogKind=Int (المولّد لا يتتبّع نوعَ العنصر؛
            //      الأعدادُ حالتُه الشائعة، ويطابق المفسّرَ فيها).
            // (EN) [runtime tags] initialize fields 3 (tags) and 4 (homogKind): malloc does
            //      not zero, so leaving them garbage means that if the generator array is read
            //      via the Any path (e.g. a generator in a return-disagreement branch widens
            //      the element type to Any) the reader branches on a garbage tags pointer
            //      (crash) or reads a garbage homogKind (wrong tag). tags=null (homogeneous,
            //      static path); homogKind=Int (generators don't track element type; ints are
            //      the common case and match the interpreter there).
            llvm::Value *tagsGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, "gen.arr.tags");
            cg_.builder_->CreateStore(
                llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*cg_.context_)), tagsGep);
            llvm::Value *hkGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 4, "gen.arr.homogkind");
            cg_.builder_->CreateStore(
                llvm::ConstantInt::get(llvm::Type::getInt8Ty(*cg_.context_), Sad::LLVM::DynKind::Int), hkGep);

            // ================================================================
            // (AR) الخطوة 2: حلقة استئناف المولّد وجمع القيم
            // (EN) Step 2: Resume loop collecting yielded values
            // ================================================================
            auto *loopBB = llvm::BasicBlock::Create(*cg_.context_, "gen.loop", curFunc);
            auto *yieldBB = llvm::BasicBlock::Create(*cg_.context_, "gen.yield", curFunc);
            auto *growBB = llvm::BasicBlock::Create(*cg_.context_, "gen.grow", curFunc);
            auto *storeBB = llvm::BasicBlock::Create(*cg_.context_, "gen.store", curFunc);
            auto *endBB = llvm::BasicBlock::Create(*cg_.context_, "gen.end", curFunc);

            cg_.builder_->CreateBr(loopBB);

            // === Loop: resume generator and check if done ===
            cg_.builder_->SetInsertPoint(loopBB);

            auto coroResumeFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_resume);
            cg_.builder_->CreateCall(coroResumeFn, {genHdl});

            auto coroDoneFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_done);
            llvm::Value *done = cg_.builder_->CreateCall(coroDoneFn, {genHdl}, "gen.isdone");
            cg_.builder_->CreateCondBr(done, endBB, yieldBB);

            // === Yield: read value from promise ===
            cg_.builder_->SetInsertPoint(yieldBB);

            auto coroPromiseFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_promise, {});
            llvm::Value *promisePtr = cg_.builder_->CreateCall(coroPromiseFn, {
                                                                              genHdl, llvm::ConstantInt::get(i32Ty, 8), // alignment
                                                                              llvm::ConstantInt::get(i1Ty, 0)           // from = false
                                                                          },
                                                           "gen.promise");

            llvm::Value *yieldedVal = cg_.builder_->CreateLoad(i64Ty, promisePtr, "gen.val");

            // (AR) فحص: هل الطول == السعة؟ إذا نعم → توسيع المصفوفة
            // (EN) Check: is length == capacity? If yes → grow array
            llvm::Value *curLen = cg_.builder_->CreateLoad(i64Ty,
                                                       cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "gen.len.gep1"), "gen.curlen");
            llvm::Value *curCap = cg_.builder_->CreateLoad(i64Ty,
                                                       cg_.builder_->CreateStructGEP(arrTy, arrPtr, 1, "gen.cap.gep1"), "gen.curcap");
            llvm::Value *needGrow = cg_.builder_->CreateICmpEQ(curLen, curCap, "gen.needgrow");
            cg_.builder_->CreateCondBr(needGrow, growBB, storeBB);

            // === Grow: double capacity and realloc data ===
            cg_.builder_->SetInsertPoint(growBB);

            llvm::Value *newCap = cg_.builder_->CreateMul(curCap, llvm::ConstantInt::get(i64Ty, 2), "gen.newcap");
            cg_.builder_->CreateStore(newCap,
                                  cg_.builder_->CreateStructGEP(arrTy, arrPtr, 1, "gen.cap.gep2"));

            // (AR) realloc(data, newCap * 8)
            // (EN) realloc(data, newCap * 8)
            llvm::Value *oldData = cg_.builder_->CreateLoad(ptrTy,
                                                        cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "gen.data.gep.grow"), "gen.olddata");
            llvm::Value *newSize = cg_.builder_->CreateMul(newCap, llvm::ConstantInt::get(i64Ty, 8), "gen.newsz");
            llvm::Value *newData = cg_.emitRealloc(oldData, newSize, "gen.newdata");
            cg_.builder_->CreateStore(newData,
                                  cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "gen.data.gep.grow2"));

            cg_.builder_->CreateBr(storeBB);

            // === Store: write yielded value into array[length], increment length ===
            cg_.builder_->SetInsertPoint(storeBB);

            // (AR) PHI لمؤشر البيانات (من yieldBB أو growBB)
            // (EN) PHI for data pointer (from yieldBB or growBB)
            // (AR) نعيد قراءة الطول والبيانات من البنية لتجنب PHI المعقد
            // (EN) Re-read length and data from struct to avoid complex PHI
            llvm::Value *storeLen = cg_.builder_->CreateLoad(i64Ty,
                                                         cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "gen.len.gep.store"), "gen.storelen");
            llvm::Value *storeData = cg_.builder_->CreateLoad(ptrTy,
                                                          cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "gen.data.gep.store"), "gen.storedata");

            // data[length] = yieldedVal
            llvm::Value *elemPtr = cg_.builder_->CreateGEP(i64Ty, storeData, {storeLen}, "gen.elemptr");
            cg_.builder_->CreateStore(yieldedVal, elemPtr);

            // length++
            llvm::Value *newLen = cg_.builder_->CreateAdd(storeLen, llvm::ConstantInt::get(i64Ty, 1), "gen.newlen");
            cg_.builder_->CreateStore(newLen,
                                  cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "gen.len.gep.inc"));

            cg_.builder_->CreateBr(loopBB);

            // === End: destroy generator and return array pointer as i64 ===
            cg_.builder_->SetInsertPoint(endBB);

            auto coroDestroyFn = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::coro_destroy);
            cg_.builder_->CreateCall(coroDestroyFn, {genHdl});

            // (AR) تحويل مؤشر المصفوفة إلى i64 (MSB tagging: bit63=0 → ptr)
            // (EN) Convert array pointer to i64 (MSB tagging: bit63=0 → ptr)
            llvm::Value *arrAsI64 = cg_.builder_->CreatePtrToInt(arrPtr, i64Ty, "gen.arr.i64");

            cg_.context_info_.currentBlock = endBB;

            // (AR) تخزين النتيجة في السجل
            // (EN) Store result in register
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = arrAsI64;
            }

            return arrAsI64;
        }
    } // namespace LLVM
} // namespace Sad