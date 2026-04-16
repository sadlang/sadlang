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
// (AR) عمليات القنوات (Channels) - إنشاء، إرسال، استقبال، إغلاق، إلخ
// (EN) Channel operations - create, send, recv, close, etc.
// (AR) تم فصل هذا الملف عن llvm_codegen_concurrency.cpp وفق قاعدة CW-05
// ============================================================================
        llvm::Value *LLVMCodeGen::emitAsyncCreateChannel(std::shared_ptr<SIRInstruction> inst)
        {
            // ================================================================
            // (AR) إنشاء قناة متعددة المخزن (Ring Buffer Channel)
            //      التخطيط:
            //        [0]  capacity (i64)  — السعة القصوى
            //        [1]  count    (i64)  — عدد العناصر الحالي
            //        [2]  head     (i64)  — مؤشر القراءة (recv)
            //        [3]  tail     (i64)  — مؤشر الكتابة (send)
            //        [4]  closed   (i64)  — علم الإغلاق
            //        [5+] data[i]  (i64)  — مصفوفة البيانات الدائرية
            //      الحجم الكلي = 40 + capacity * 8 بايت
            // (EN) Create multi-buffered ring buffer channel
            //      Layout: [capacity, count, head, tail, closed, data[0..n]]
            //      Total size = 40 + capacity * 8 bytes
            // ================================================================
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) قراءة السعة من الوسيط (افتراضي = 16)
            // (EN) Read capacity from operand (default = 16)
            llvm::Value *capacity;
            if (!inst->operands.empty())
            {
                capacity = resolveOperand(inst->operands[0]);
            }
            else
            {
                capacity = llvm::ConstantInt::get(i64Ty, 16);
            }

            // (AR) حساب الحجم الكلي: 40 + capacity * 8
            // (EN) Calculate total size: 40 + capacity * 8
            auto dataSize = builder_->CreateMul(capacity, llvm::ConstantInt::get(i64Ty, 8));
            auto totalSize = builder_->CreateAdd(dataSize, llvm::ConstantInt::get(i64Ty, 40));

            // (AR) تخصيص الذاكرة وتصفيرها
            // (EN) Allocate and zero-initialize
            auto mallocTy = llvm::FunctionType::get(i8PtrTy, {i64Ty}, false);
            auto mallocFn = module_->getOrInsertFunction("malloc", mallocTy);
            auto chanPtr = builder_->CreateCall(mallocFn, {totalSize});
            builder_->CreateMemSet(chanPtr, builder_->getInt8(0), totalSize, llvm::MaybeAlign(8));

            // (AR) تخزين السعة في الموقع [0]
            // (EN) Store capacity at slot [0]
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);
            builder_->CreateStore(capacity, chanI64Ptr);

            auto result = builder_->CreatePtrToInt(chanPtr, i64Ty);
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitAsyncChannelSend(std::shared_ptr<SIRInstruction> inst)
        {
            // ================================================================
            // (AR) إرسال قيمة إلى القناة (Ring Buffer Send)
            //      1. قراءة count و capacity
            //      2. إذا count < capacity: تخزين القيمة في data[tail]
            //         tail = (tail + 1) % capacity
            //         count++
            //      3. إذا count >= capacity: إسقاط القيمة (لا حجب)
            // (EN) Send value to channel (Ring Buffer)
            //      If count < capacity: store at data[tail], advance tail, increment count
            //      If full: drop value (non-blocking)
            // ================================================================
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = resolveOperand(inst->operands[0]);
            llvm::Value *value = resolveOperand(inst->operands[1]);
            auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) قراءة capacity [0] و count [1]
            auto capPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capacity = builder_->CreateLoad(i64Ty, capPtr);

            auto cntPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto count = builder_->CreateLoad(i64Ty, cntPtr);

            // (AR) فحص: count < capacity
            auto closedPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 4)});
            auto closedVal = builder_->CreateLoad(i64Ty, closedPtr);
            auto notClosed = builder_->CreateICmpEQ(closedVal, llvm::ConstantInt::get(i64Ty, 0));
            auto hasSpace = builder_->CreateICmpULT(count, capacity);
            auto canSend = builder_->CreateAnd(notClosed, hasSpace);

            // (AR) كتل الشرط: send_ok و send_done
            auto parentFunc = builder_->GetInsertBlock()->getParent();
            auto sendOkBB = llvm::BasicBlock::Create(*context_, "chan_send_ok", parentFunc);
            auto sendDoneBB = llvm::BasicBlock::Create(*context_, "chan_send_done", parentFunc);

            builder_->CreateCondBr(canSend, sendOkBB, sendDoneBB);

            // ─── كتلة send_ok: تخزين القيمة في data[tail] ───
            builder_->SetInsertPoint(sendOkBB);

            // (AR) قراءة tail [3]
            auto tailPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 3)});
            auto tail = builder_->CreateLoad(i64Ty, tailPtr);

            // (AR) حساب عنوان data[tail]: ch_i64[5 + tail]
            auto dataIdx = builder_->CreateAdd(tail, llvm::ConstantInt::get(i64Ty, 5));
            auto dataSlot = builder_->CreateGEP(i64Ty, chanI64Ptr, {dataIdx});
            builder_->CreateStore(value, dataSlot);

            // (AR) tail = (tail + 1) % capacity
            auto nextTail = builder_->CreateAdd(tail, llvm::ConstantInt::get(i64Ty, 1));
            auto newTail = builder_->CreateURem(nextTail, capacity);
            builder_->CreateStore(newTail, tailPtr);

            // (AR) count++
            auto newCount = builder_->CreateAdd(count, llvm::ConstantInt::get(i64Ty, 1));
            builder_->CreateStore(newCount, cntPtr);

            builder_->CreateBr(sendDoneBB);

            // ─── كتلة send_done ───
            builder_->SetInsertPoint(sendDoneBB);

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitAsyncChannelRecv(std::shared_ptr<SIRInstruction> inst)
        {
            // ================================================================
            // (AR) استقبال قيمة من القناة (Ring Buffer Recv)
            //      1. قراءة count
            //      2. إذا count > 0: قراءة data[head]
            //         head = (head + 1) % capacity
            //         count--
            //         إرجاع القيمة
            //      3. إذا count == 0: إرجاع 0
            // (EN) Receive value from channel (Ring Buffer)
            //      If count > 0: read data[head], advance head, decrement count
            //      If empty: return 0
            // ================================================================
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = resolveOperand(inst->operands[0]);
            auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) قراءة count [1] و capacity [0]
            auto capPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capacity = builder_->CreateLoad(i64Ty, capPtr);

            auto cntPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto count = builder_->CreateLoad(i64Ty, cntPtr);

            // (AR) فحص: count > 0
            auto hasData = builder_->CreateICmpUGT(count, llvm::ConstantInt::get(i64Ty, 0));

            // (AR) كتل الشرط
            auto parentFunc = builder_->GetInsertBlock()->getParent();
            auto recvOkBB = llvm::BasicBlock::Create(*context_, "chan_recv_ok", parentFunc);
            auto recvEmptyBB = llvm::BasicBlock::Create(*context_, "chan_recv_empty", parentFunc);
            auto recvDoneBB = llvm::BasicBlock::Create(*context_, "chan_recv_done", parentFunc);

            builder_->CreateCondBr(hasData, recvOkBB, recvEmptyBB);

            // ─── كتلة recv_ok: قراءة data[head] ───
            builder_->SetInsertPoint(recvOkBB);

            auto headPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 2)});
            auto head = builder_->CreateLoad(i64Ty, headPtr);

            // (AR) حساب عنوان data[head]: ch_i64[5 + head]
            auto dataIdx = builder_->CreateAdd(head, llvm::ConstantInt::get(i64Ty, 5));
            auto dataSlot = builder_->CreateGEP(i64Ty, chanI64Ptr, {dataIdx});
            auto recvValue = builder_->CreateLoad(i64Ty, dataSlot);

            // (AR) head = (head + 1) % capacity
            auto nextHead = builder_->CreateAdd(head, llvm::ConstantInt::get(i64Ty, 1));
            auto newHead = builder_->CreateURem(nextHead, capacity);
            builder_->CreateStore(newHead, headPtr);

            // (AR) count--
            auto newCount = builder_->CreateSub(count, llvm::ConstantInt::get(i64Ty, 1));
            builder_->CreateStore(newCount, cntPtr);

            builder_->CreateBr(recvDoneBB);

            // ─── كتلة recv_empty: إرجاع 0 ───
            builder_->SetInsertPoint(recvEmptyBB);
            builder_->CreateBr(recvDoneBB);

            // ─── كتلة recv_done: PHI لدمج النتائج ───
            builder_->SetInsertPoint(recvDoneBB);
            auto phi = builder_->CreatePHI(i64Ty, 2, "chan_recv_val");
            phi->addIncoming(recvValue, recvOkBB);
            phi->addIncoming(llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel), recvEmptyBB);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = phi;
            }
            return phi;
        }

        llvm::Value *LLVMCodeGen::emitAsyncChannelClose(std::shared_ptr<SIRInstruction> inst)
        {
            // ================================================================
            // (AR) إغلاق القناة — تعيين علم الإغلاق في الموقع [4] (offset 32)
            // (EN) Close channel — set closed flag at slot [4] (offset 32)
            // ================================================================
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = resolveOperand(inst->operands[0]);
            auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) تخزين 1 في الموقع [4] (closed)
            auto closedPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 4)});
            builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 1), closedPtr);

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // (AR) فحص إغلاق القناة — يقرأ الموقع [4] (offset 32)
        // (EN) Check if channel is closed — reads slot [4] (offset 32)
        // ═══════════════════════════════════════════════════════════════════════════
        llvm::Value *LLVMCodeGen::emitAsyncChannelIsClosed(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = resolveOperand(inst->operands[0]);
            auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) قراءة الموقع [4] (closed)
            auto closedPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 4)});
            auto closedVal = builder_->CreateLoad(i64Ty, closedPtr);

            // (AR) تحويل إلى مقارنة: closedVal != 0
            auto result = builder_->CreateICmpNE(closedVal, llvm::ConstantInt::get(i64Ty, 0));
            auto resultI64 = builder_->CreateZExt(result, i64Ty);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = resultI64;
            }
            return resultI64;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // (AR) فحص وجود بيانات في القناة — يقرأ count من الموقع [1]
        // (EN) Check if channel has data — reads count from slot [1]
        // ═══════════════════════════════════════════════════════════════════════════
        llvm::Value *LLVMCodeGen::emitAsyncChannelHasData(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = resolveOperand(inst->operands[0]);
            auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) قراءة count من الموقع [1] / (EN) Read count from slot [1]
            auto cntPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto countVal = builder_->CreateLoad(i64Ty, cntPtr);

            // (AR) تحويل إلى مقارنة: count > 0 / (EN) Convert to comparison: count > 0
            auto result = builder_->CreateICmpUGT(countVal, llvm::ConstantInt::get(i64Ty, 0));
            auto resultI64 = builder_->CreateZExt(result, i64Ty);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = resultI64;
            }
            return resultI64;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // (AR) حجم القناة — قراءة عداد العناصر count من الموقع [1]
        // (EN) Channel size — read element count from slot [1] of ring buffer
        // ═══════════════════════════════════════════════════════════════════════════
        llvm::Value *LLVMCodeGen::emitAsyncChannelSize(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = resolveOperand(inst->operands[0]);
            auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) قراءة count من الموقع [1] / (EN) Read count from slot [1]
            auto cntPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto countVal = builder_->CreateLoad(i64Ty, cntPtr);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = countVal;
            }
            return countVal;
        }

        llvm::Value *LLVMCodeGen::emitAsyncChannelCapacity(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = resolveOperand(inst->operands[0]);
            auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) السعة في الموقع [0]
            auto capPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capVal = builder_->CreateLoad(i64Ty, capPtr);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = capVal;
            }
            return capVal;
        }

        llvm::Value *LLVMCodeGen::emitAsyncChannelTrySend(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) try_send: يرسل إذا كان هناك مكان، ويرجع 1 عند النجاح و0 عند الفشل.
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = resolveOperand(inst->operands[0]);
            llvm::Value *value = resolveOperand(inst->operands[1]);
            auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);

            auto capPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capacity = builder_->CreateLoad(i64Ty, capPtr);
            auto cntPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto count = builder_->CreateLoad(i64Ty, cntPtr);
            auto closedPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 4)});
            auto isClosed = builder_->CreateLoad(i64Ty, closedPtr);

            auto hasSpace = builder_->CreateICmpULT(count, capacity);
            auto notClosed = builder_->CreateICmpEQ(isClosed, llvm::ConstantInt::get(i64Ty, 0));
            auto canSend = builder_->CreateAnd(hasSpace, notClosed);

            auto parentFunc = builder_->GetInsertBlock()->getParent();
            auto okBB = llvm::BasicBlock::Create(*context_, "chan_try_send_ok", parentFunc);
            auto failBB = llvm::BasicBlock::Create(*context_, "chan_try_send_fail", parentFunc);
            auto doneBB = llvm::BasicBlock::Create(*context_, "chan_try_send_done", parentFunc);
            builder_->CreateCondBr(canSend, okBB, failBB);

            builder_->SetInsertPoint(okBB);
            auto tailPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 3)});
            auto tail = builder_->CreateLoad(i64Ty, tailPtr);
            auto dataIdx = builder_->CreateAdd(tail, llvm::ConstantInt::get(i64Ty, 5));
            auto dataSlot = builder_->CreateGEP(i64Ty, chanI64Ptr, {dataIdx});
            builder_->CreateStore(value, dataSlot);
            auto nextTail = builder_->CreateAdd(tail, llvm::ConstantInt::get(i64Ty, 1));
            auto newTail = builder_->CreateURem(nextTail, capacity);
            builder_->CreateStore(newTail, tailPtr);
            auto newCount = builder_->CreateAdd(count, llvm::ConstantInt::get(i64Ty, 1));
            builder_->CreateStore(newCount, cntPtr);
            builder_->CreateBr(doneBB);

            builder_->SetInsertPoint(failBB);
            builder_->CreateBr(doneBB);

            builder_->SetInsertPoint(doneBB);
            auto resultPhi = builder_->CreatePHI(i64Ty, 2, "chan_try_send_res");
            resultPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 1), okBB);
            resultPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), failBB);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = resultPhi;
            }
            return resultPhi;
        }

        llvm::Value *LLVMCodeGen::emitAsyncChannelTryRecv(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) try_recv: يستقبل إذا وُجدت بيانات، وإلا يرجع 0 (قيمة فشل غير حاجبة).
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = resolveOperand(inst->operands[0]);
            auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);

            auto capPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capacity = builder_->CreateLoad(i64Ty, capPtr);
            auto cntPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto count = builder_->CreateLoad(i64Ty, cntPtr);

            auto hasData = builder_->CreateICmpUGT(count, llvm::ConstantInt::get(i64Ty, 0));

            auto parentFunc = builder_->GetInsertBlock()->getParent();
            auto recvOkBB = llvm::BasicBlock::Create(*context_, "chan_try_recv_ok", parentFunc);
            auto recvEmptyBB = llvm::BasicBlock::Create(*context_, "chan_try_recv_empty", parentFunc);
            auto recvDoneBB = llvm::BasicBlock::Create(*context_, "chan_try_recv_done", parentFunc);

            builder_->CreateCondBr(hasData, recvOkBB, recvEmptyBB);

            builder_->SetInsertPoint(recvOkBB);
            auto headPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 2)});
            auto head = builder_->CreateLoad(i64Ty, headPtr);
            auto dataIdx = builder_->CreateAdd(head, llvm::ConstantInt::get(i64Ty, 5));
            auto dataSlot = builder_->CreateGEP(i64Ty, chanI64Ptr, {dataIdx});
            auto recvValue = builder_->CreateLoad(i64Ty, dataSlot);
            auto nextHead = builder_->CreateAdd(head, llvm::ConstantInt::get(i64Ty, 1));
            auto newHead = builder_->CreateURem(nextHead, capacity);
            builder_->CreateStore(newHead, headPtr);
            auto newCount = builder_->CreateSub(count, llvm::ConstantInt::get(i64Ty, 1));
            builder_->CreateStore(newCount, cntPtr);
            builder_->CreateBr(recvDoneBB);

            builder_->SetInsertPoint(recvEmptyBB);
            builder_->CreateBr(recvDoneBB);

            builder_->SetInsertPoint(recvDoneBB);
            auto phi = builder_->CreatePHI(i64Ty, 2, "chan_try_recv_val");
            phi->addIncoming(recvValue, recvOkBB);
            // (AR) عند فشل try_recv نعيد null-sentinel ليتطابق مع لاشيء في اللغة.
            phi->addIncoming(llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel), recvEmptyBB);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = phi;
            }
            return phi;
        }

        llvm::Value *LLVMCodeGen::emitAsyncChannelSendTimeout(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) send_timeout: في المرحلة الحالية يُعامل كـ try_send (غير حاجب).
            //      operands: [channel, value, timeout_ms]
            if (inst->operands.size() < 2)
            {
                auto i64Ty = llvm::Type::getInt64Ty(*context_);
                auto zero = llvm::ConstantInt::get(i64Ty, 0);
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = zero;
                }
                return zero;
            }

            auto lowered = std::make_shared<SIRInstruction>(SIROpcode::ASYNC_CHANNEL_TRY_SEND);
            lowered->result = inst->result;
            lowered->operands.push_back(inst->operands[0]);
            lowered->operands.push_back(inst->operands[1]);
            return emitAsyncChannelTrySend(lowered);
        }

        llvm::Value *LLVMCodeGen::emitAsyncChannelRecvTimeout(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) recv_timeout: غير حاجب حالياً؛ إذا القناة فارغة نعيد null-sentinel.
            //      operands: [channel, timeout_ms]
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            if (inst->operands.empty())
            {
                auto nullSentinel = llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel);
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = nullSentinel;
                }
                return nullSentinel;
            }

            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = resolveOperand(inst->operands[0]);
            auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = builder_->CreateBitCast(chanPtr, i64PtrTy);

            auto capPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capacity = builder_->CreateLoad(i64Ty, capPtr);
            auto cntPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto count = builder_->CreateLoad(i64Ty, cntPtr);

            auto hasData = builder_->CreateICmpUGT(count, llvm::ConstantInt::get(i64Ty, 0));

            auto parentFunc = builder_->GetInsertBlock()->getParent();
            auto recvOkBB = llvm::BasicBlock::Create(*context_, "chan_recv_to_ok", parentFunc);
            auto recvEmptyBB = llvm::BasicBlock::Create(*context_, "chan_recv_to_empty", parentFunc);
            auto recvDoneBB = llvm::BasicBlock::Create(*context_, "chan_recv_to_done", parentFunc);
            builder_->CreateCondBr(hasData, recvOkBB, recvEmptyBB);

            builder_->SetInsertPoint(recvOkBB);
            auto headPtr = builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 2)});
            auto head = builder_->CreateLoad(i64Ty, headPtr);
            auto dataIdx = builder_->CreateAdd(head, llvm::ConstantInt::get(i64Ty, 5));
            auto dataSlot = builder_->CreateGEP(i64Ty, chanI64Ptr, {dataIdx});
            auto recvValue = builder_->CreateLoad(i64Ty, dataSlot);
            auto nextHead = builder_->CreateAdd(head, llvm::ConstantInt::get(i64Ty, 1));
            auto newHead = builder_->CreateURem(nextHead, capacity);
            builder_->CreateStore(newHead, headPtr);
            auto newCount = builder_->CreateSub(count, llvm::ConstantInt::get(i64Ty, 1));
            builder_->CreateStore(newCount, cntPtr);
            builder_->CreateBr(recvDoneBB);

            builder_->SetInsertPoint(recvEmptyBB);
            builder_->CreateBr(recvDoneBB);

            builder_->SetInsertPoint(recvDoneBB);
            auto phi = builder_->CreatePHI(i64Ty, 2, "chan_recv_to_val");
            phi->addIncoming(recvValue, recvOkBB);
            phi->addIncoming(llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel), recvEmptyBB);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = phi;
            }
            return phi;
        }

        // ================================================================
        // (AR) بنية القفل (Mutex): مصفوفة بسيطة [handle, locked_flag]
        //      slot[0] = HANDLE (كمؤشر محوّل لـ i64)
        //      slot[1] = علم القفل (0 = مفتوح، 1 = مقفل)
        //      المرجع المُخزن = مؤشر i64* لأول slot
        // (EN) Mutex structure: simple array [handle, locked_flag]
        //      slot[0] = HANDLE (as ptr-to-i64)
        //      slot[1] = lock flag (0 = unlocked, 1 = locked)
        //      stored reference = i64* pointer to first slot

    } // namespace LLVM
} // namespace Sad