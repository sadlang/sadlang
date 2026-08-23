/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/directives/concurrency_codegen.h"
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
        llvm::Value *ConcurrencyCodeGen::emitAsyncCreateChannel(std::shared_ptr<SIRInstruction> inst)
        {
            // ================================================================
            // (AR) إنشاء قناة متعددة المخزن (Ring Buffer Channel)
            //      التخطيط:
            //        [0]  capacity (i64)  — السعة القصوى
            //        [1]  count    (i64)  — عدد العناصر الحالي
            //        [2]  head     (i64)  — مؤشر القراءة (recv)
            //        [3]  tail     (i64)  — مؤشر الكتابة (send)
            //        [4]  closed   (i64)  — علم الإغلاق
            //        [5+] data pairs: [value, type_tag] × capacity
            //      الحجم الكلي = 40 + capacity * 16 بايت (فتحتان لكل عنصر)
            //      وسم النوع: 0=رقم، 1=نص/مؤشر، 2=منطقي
            // (EN) Create multi-buffered ring buffer channel
            //      Layout: [capacity, count, head, tail, closed, (value,type)×n]
            //      Total size = 40 + capacity * 16 bytes (2 slots per element)
            //      Type tags: 0=Integer, 1=String/Pointer, 2=Boolean
            // ================================================================
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) قراءة السعة من الوسيط (افتراضي = 16)
            // (EN) Read capacity from operand (default = 16)
            llvm::Value *capacity;
            if (!inst->operands.empty())
            {
                capacity = cg_.resolveOperand(inst->operands[0]);
            }
            else
            {
                capacity = llvm::ConstantInt::get(i64Ty, 16);
            }

            // (AR) حساب الحجم الكلي: 40 + capacity * 16 (فتحتان لكل عنصر: قيمة + وسم نوع)
            // (EN) Calculate total size: 40 + capacity * 16 (2 slots per element: value + type tag)
            auto dataSize = cg_.builder_->CreateMul(capacity, llvm::ConstantInt::get(i64Ty, 16));
            auto totalSize = cg_.builder_->CreateAdd(dataSize, llvm::ConstantInt::get(i64Ty, 40));

            // (AR) تخصيص الذاكرة وتصفيرها
            // (EN) Allocate and zero-initialize
            auto chanPtr = cg_.emitMalloc(totalSize);
            cg_.builder_->CreateMemSet(chanPtr, cg_.builder_->getInt8(0), totalSize, llvm::MaybeAlign(8));

            // (AR) تخزين السعة في الموقع [0]
            // (EN) Store capacity at slot [0]
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);
            cg_.builder_->CreateStore(capacity, chanI64Ptr);

            auto result = cg_.builder_->CreatePtrToInt(chanPtr, i64Ty);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelSend(std::shared_ptr<SIRInstruction> inst)
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
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *value = cg_.resolveOperand(inst->operands[1]);

            // (AR) تحويل ptr → i64 إذا كان نصاً (مؤشر)
            // (EN) Convert ptr → i64 if value is a string (pointer)
            if (value->getType()->isPointerTy())
            {
                value = cg_.builder_->CreatePtrToInt(value, i64Ty, "chan.send.p2i");
            }

            auto chanPtr = cg_.builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) قراءة capacity [0] و count [1]
            auto capPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capacity = cg_.builder_->CreateLoad(i64Ty, capPtr);

            // (AR) ع-16: تحميل العد ذريا (acquire) — منذ صارت «أطلق» تطلق خيطا
            //      حقيقيا يتقاسم المرسل والمستقبل هذا العداد عبر خيطين، وكانت
            //      الأحمال العادية سباق بيانات. الترتيب acquire/release يجعل
            //      الحلقة صحيحة قفليا لنمط منتج واحد/مستهلك واحد (نمط أطلق):
            //      الرأس ملك المستهلك والذيل ملك المنتج والعداد وحده مشترك.
            // (EN) ع-16: atomic (acquire) count load — «أطلق» now spawns a real
            //      thread, so producer and consumer share this counter across
            //      threads and the plain loads were a data race. Acquire/release
            //      ordering makes the ring lock-free-correct for the SPSC
            //      pattern (the أطلق shape): head is consumer-owned, tail is
            //      producer-owned, only the counter is shared.
            auto cntPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto countLoad = cg_.builder_->CreateLoad(i64Ty, cntPtr);
            countLoad->setAtomic(llvm::AtomicOrdering::Acquire);
            countLoad->setAlignment(llvm::Align(8));
            llvm::Value *count = countLoad;

            // (AR) فحص: count < capacity
            auto closedPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 4)});
            auto closedVal = cg_.builder_->CreateLoad(i64Ty, closedPtr);
            auto notClosed = cg_.builder_->CreateICmpEQ(closedVal, llvm::ConstantInt::get(i64Ty, 0));
            auto hasSpace = cg_.builder_->CreateICmpULT(count, capacity);
            auto canSend = cg_.builder_->CreateAnd(notClosed, hasSpace);

            // (AR) كتل الشرط: send_ok و send_done
            auto parentFunc = cg_.builder_->GetInsertBlock()->getParent();
            auto sendOkBB = llvm::BasicBlock::Create(*cg_.context_, "chan_send_ok", parentFunc);
            auto sendDoneBB = llvm::BasicBlock::Create(*cg_.context_, "chan_send_done", parentFunc);

            cg_.builder_->CreateCondBr(canSend, sendOkBB, sendDoneBB);

            // ─── كتلة send_ok: تخزين القيمة في data[tail] ───
            cg_.builder_->SetInsertPoint(sendOkBB);

            // (AR) قراءة tail [3]
            auto tailPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 3)});
            auto tail = cg_.builder_->CreateLoad(i64Ty, tailPtr);

            // (AR) حساب عنوان data[tail*2]: ch_i64[5 + tail*2] — فتحتان لكل عنصر
            // (EN) Calculate data slot: ch_i64[5 + tail*2] — 2 slots per element
            auto tailMul2 = cg_.builder_->CreateMul(tail, llvm::ConstantInt::get(i64Ty, 2), "tail.x2");
            auto dataIdx = cg_.builder_->CreateAdd(tailMul2, llvm::ConstantInt::get(i64Ty, 5));
            auto dataSlot = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {dataIdx});
            cg_.builder_->CreateStore(value, dataSlot);

            // (AR) تخزين وسم النوع في الفتحة التالية: ch_i64[5 + tail*2 + 1]
            //      0 = رقم، 1 = نص/مؤشر، 2 = منطقي
            // (EN) Store type tag in next slot: 0=Integer, 1=String/Pointer, 2=Boolean
            {
                int64_t typeTag = 0; // default: integer
                if (inst->operands.size() > 1)
                {
                    auto valType = inst->operands[1].dataType;
                    if (valType == SadTypeKind::String || valType == SadTypeKind::Pointer ||
                        valType == SadTypeKind::Class || valType == SadTypeKind::Array)
                        typeTag = 1;
                    else if (valType == SadTypeKind::Boolean)
                        typeTag = 2;
                }
                auto typeIdx = cg_.builder_->CreateAdd(dataIdx, llvm::ConstantInt::get(i64Ty, 1));
                auto typeSlot = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {typeIdx});
                cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, typeTag), typeSlot);
            }

            // (AR) tail = (tail + 1) % capacity
            auto nextTail = cg_.builder_->CreateAdd(tail, llvm::ConstantInt::get(i64Ty, 1));
            auto newTail = cg_.builder_->CreateURem(nextTail, capacity);
            cg_.builder_->CreateStore(newTail, tailPtr);

            // (AR) count++ ذريا (release): ينشر تخزين القيمة والوسم أعلاه قبل
            //      أن يرى المستهلك العد الجديد بحمل acquire
            // (EN) Atomic count++ (release): publishes the value/tag stores
            //      above before the consumer's acquire load observes the count
            cg_.builder_->CreateAtomicRMW(llvm::AtomicRMWInst::Add, cntPtr,
                                          llvm::ConstantInt::get(i64Ty, 1),
                                          llvm::MaybeAlign(8),
                                          llvm::AtomicOrdering::Release);

            cg_.builder_->CreateBr(sendDoneBB);

            // ─── كتلة send_done ───
            cg_.builder_->SetInsertPoint(sendDoneBB);

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelRecv(std::shared_ptr<SIRInstruction> inst)
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
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = cg_.resolveOperand(inst->operands[0]);
            auto chanPtr = cg_.builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) قراءة count [1] و capacity [0]
            auto capPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capacity = cg_.builder_->CreateLoad(i64Ty, capPtr);

            auto cntPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto closedFlagPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 4)});

            // (AR) ع-16: الاستقبال حاجب — منذ صارت «أطلق» خيطا حقيقيا، الاستقبال
            //      غير الحاجب (حارس عدمي فورا) كان يسبق المرسل فيسابقه (اختبار
            //      098 كان أخضر لأن أطلق كانت متزامنة — «أخضر لأن الشرط لا يكون
            //      خاطئا»). الآن: حلقة انتظار — عد ذري acquire؛ إن وجدت بيانات
            //      خرجنا، وإلا فحصنا الإغلاق (قناة مغلقة فارغة ⇒ الحارس العدمي)،
            //      وإلا نمنا 1م وأعدنا. يطابق دلالة المفسر (الاستقبال يحجب).
            //      المسارات غير الحاجبة لها معالجاتها المستقلة (TRY_RECV/
            //      RECV_TIMEOUT) و«اختر» تفحص HAS_DATA قبل الاستقبال فلا تحجب.
            // (EN) ع-16: recv now BLOCKS — with «أطلق» a real thread, the old
            //      non-blocking recv (immediate null sentinel) raced the sender
            //      (test 098 was green only because أطلق was synchronous). Wait
            //      loop: atomic acquire count; data ⇒ proceed; else closed &&
            //      empty ⇒ null sentinel; else sleep 1ms and retry. Matches the
            //      interpreter's blocking semantics. Non-blocking paths keep
            //      their own emitters (TRY_RECV/RECV_TIMEOUT), and «اختر»
            //      checks HAS_DATA before recv so it never blocks here.
            auto parentFunc = cg_.builder_->GetInsertBlock()->getParent();
            auto recvWaitBB = llvm::BasicBlock::Create(*cg_.context_, "chan_recv_wait", parentFunc);
            auto recvCheckClosedBB = llvm::BasicBlock::Create(*cg_.context_, "chan_recv_chkclosed", parentFunc);
            auto recvSleepBB = llvm::BasicBlock::Create(*cg_.context_, "chan_recv_sleep", parentFunc);
            auto recvOkBB = llvm::BasicBlock::Create(*cg_.context_, "chan_recv_ok", parentFunc);
            auto recvEmptyBB = llvm::BasicBlock::Create(*cg_.context_, "chan_recv_empty", parentFunc);
            auto recvDoneBB = llvm::BasicBlock::Create(*cg_.context_, "chan_recv_done", parentFunc);

            cg_.builder_->CreateBr(recvWaitBB);

            cg_.builder_->SetInsertPoint(recvWaitBB);
            auto countLoad = cg_.builder_->CreateLoad(i64Ty, cntPtr, "chan.count");
            countLoad->setAtomic(llvm::AtomicOrdering::Acquire);
            countLoad->setAlignment(llvm::Align(8));
            llvm::Value *count = countLoad;
            auto hasData = cg_.builder_->CreateICmpUGT(count, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(hasData, recvOkBB, recvCheckClosedBB);

            cg_.builder_->SetInsertPoint(recvCheckClosedBB);
            auto closedLoad = cg_.builder_->CreateLoad(i64Ty, closedFlagPtr, "chan.closed");
            closedLoad->setAtomic(llvm::AtomicOrdering::Acquire);
            closedLoad->setAlignment(llvm::Align(8));
            auto isClosed = cg_.builder_->CreateICmpNE(closedLoad, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(isClosed, recvEmptyBB, recvSleepBB);

            cg_.builder_->SetInsertPoint(recvSleepBB);
            {
                auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
                auto sleepTy = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i32Ty}, false);
                auto sleepFn = cg_.module_->getOrInsertFunction("sad_rt_sleep_ms", sleepTy);
                cg_.builder_->CreateCall(sleepFn, {llvm::ConstantInt::get(i32Ty, 1)});
            }
            cg_.builder_->CreateBr(recvWaitBB);

            // ─── كتلة recv_ok: قراءة data[head] ───
            cg_.builder_->SetInsertPoint(recvOkBB);

            auto headPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 2)});
            auto head = cg_.builder_->CreateLoad(i64Ty, headPtr);

            // (AR) حساب عنوان data[head*2]: ch_i64[5 + head*2] — فتحتان لكل عنصر
            // (EN) Calculate data slot: ch_i64[5 + head*2] — 2 slots per element
            auto headMul2 = cg_.builder_->CreateMul(head, llvm::ConstantInt::get(i64Ty, 2), "head.x2");
            auto dataIdx = cg_.builder_->CreateAdd(headMul2, llvm::ConstantInt::get(i64Ty, 5));
            auto dataSlot = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {dataIdx});
            auto recvValueRaw = cg_.builder_->CreateLoad(i64Ty, dataSlot);

            // (AR) قراءة وسم النوع من الفتحة التالية: ch_i64[5 + head*2 + 1]
            // (EN) Read type tag from next slot
            auto typeIdx = cg_.builder_->CreateAdd(dataIdx, llvm::ConstantInt::get(i64Ty, 1));
            auto typeSlot = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {typeIdx});
            auto typeTag = cg_.builder_->CreateLoad(i64Ty, typeSlot, "chan.typetag");

            // ================================================================
            // (AR) القيمة الخام تُرجع مباشرة بدون وسم MSB
            //      النوع يُحدد في SIR (Integer افتراضياً عند عدم معرفة نوع القناة)
            //      kSadNullSentinel يُستخدم للقناة الفارغة ويُعالج عند الطباعة/الدمج
            // (EN) Raw value returned directly — no MSB tagging
            //      Type determined at SIR level (Integer default when channel type unknown)
            //      kSadNullSentinel used for empty channel, handled at print/concat
            // ================================================================
            llvm::Value *recvValue = recvValueRaw;

            // (AR) head = (head + 1) % capacity
            auto nextHead = cg_.builder_->CreateAdd(head, llvm::ConstantInt::get(i64Ty, 1));
            auto newHead = cg_.builder_->CreateURem(nextHead, capacity);
            cg_.builder_->CreateStore(newHead, headPtr);

            // (AR) count-- ذريا (release) — نظير زيادة المرسل
            // (EN) Atomic count-- (release) — mirror of the sender's increment
            cg_.builder_->CreateAtomicRMW(llvm::AtomicRMWInst::Sub, cntPtr,
                                          llvm::ConstantInt::get(i64Ty, 1),
                                          llvm::MaybeAlign(8),
                                          llvm::AtomicOrdering::Release);

            cg_.builder_->CreateBr(recvDoneBB);

            // ─── كتلة recv_empty: إرجاع sentinel ───
            cg_.builder_->SetInsertPoint(recvEmptyBB);
            cg_.builder_->CreateBr(recvDoneBB);

            // ─── كتلة recv_done: PHI لدمج النتائج ───
            cg_.builder_->SetInsertPoint(recvDoneBB);
            auto phi = cg_.builder_->CreatePHI(i64Ty, 2, "chan_recv_val");
            phi->addIncoming(recvValue, recvOkBB);
            phi->addIncoming(llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel), recvEmptyBB);

            // ================================================================
            // (AR) تحويل القيمة المُستقبلة إلى مؤشر إذا كان النوع نصاً/مؤشراً
            //      عند الإرسال: المؤشرات تُحوّل إلى i64 عبر ptrtoint
            //      عند الاستقبال: نحتاج inttoptr لإعادة التحويل
            //      النوع يُحدد في SIR من خلال channelTypeMap_ (يُنشر عبر حدود الدوال)
            // (EN) Convert received value to pointer if SIR type is String/Pointer
            //      At send: pointers are converted to i64 via ptrtoint
            //      At recv: we need inttoptr to convert back
            //      Type determined at SIR level via channelTypeMap_ (propagated across functions)
            // ================================================================
            llvm::Value *finalValue = phi;
            if (inst->result.has_value() &&
                (inst->result->dataType == SadTypeKind::String ||
                 inst->result->dataType == SadTypeKind::Pointer ||
                 inst->result->dataType == SadTypeKind::Class ||
                 inst->result->dataType == SadTypeKind::Array))
            {
                // (AR) تحويل i64 → ptr مع حماية kSadNullSentinel
                //      إذا كانت القيمة sentinel (قناة فارغة): نُرجع null ptr
                //      إذا كانت قيمة حقيقية: نحوّل إلى مؤشر
                //      بدون هذا: inttoptr(sentinel) ينتج مؤشر وهمي يسبب crash
                // (EN) Convert i64 → ptr with kSadNullSentinel guard
                //      If sentinel (empty channel): return null ptr
                //      If real value: convert to pointer
                //      Without this: inttoptr(sentinel) produces a bogus pointer → crash
                auto sentinelConst = llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel);
                auto isSentinel = cg_.builder_->CreateICmpEQ(phi, sentinelConst);
                auto ptrVal = cg_.builder_->CreateIntToPtr(phi, i8PtrTy);
                auto nullPtr = llvm::ConstantPointerNull::get(
                    llvm::cast<llvm::PointerType>(i8PtrTy));
                finalValue = cg_.builder_->CreateSelect(isSentinel, nullPtr, ptrVal);
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = finalValue;
            }
            return finalValue;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelClose(std::shared_ptr<SIRInstruction> inst)
        {
            // ================================================================
            // (AR) إغلاق القناة — تعيين علم الإغلاق في الموقع [4] (offset 32)
            // (EN) Close channel — set closed flag at slot [4] (offset 32)
            // ================================================================
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = cg_.resolveOperand(inst->operands[0]);
            auto chanPtr = cg_.builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) تخزين 1 في الموقع [4] (closed) — ذريا (release) ليقابل حمل
            //      acquire في حلقة انتظار الاستقبال الحاجب (ع-16)
            // (EN) Store 1 at slot [4] (closed) — atomic release, pairing with
            //      the acquire load in the blocking-recv wait loop (ع-16)
            auto closedPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 4)});
            auto closedStore = cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 1), closedPtr);
            closedStore->setAtomic(llvm::AtomicOrdering::Release);
            closedStore->setAlignment(llvm::Align(8));

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // (AR) فحص إغلاق القناة — يقرأ الموقع [4] (offset 32)
        // (EN) Check if channel is closed — reads slot [4] (offset 32)
        // ═══════════════════════════════════════════════════════════════════════════
        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelIsClosed(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = cg_.resolveOperand(inst->operands[0]);
            auto chanPtr = cg_.builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) قراءة الموقع [4] (closed)
            auto closedPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 4)});
            auto closedVal = cg_.builder_->CreateLoad(i64Ty, closedPtr);

            // (AR) تحويل إلى مقارنة: closedVal != 0
            auto result = cg_.builder_->CreateICmpNE(closedVal, llvm::ConstantInt::get(i64Ty, 0));
            auto resultI64 = cg_.builder_->CreateZExt(result, i64Ty);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = resultI64;
            }
            return resultI64;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // (AR) فحص وجود بيانات في القناة — يقرأ count من الموقع [1]
        // (EN) Check if channel has data — reads count from slot [1]
        // ═══════════════════════════════════════════════════════════════════════════
        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelHasData(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = cg_.resolveOperand(inst->operands[0]);
            auto chanPtr = cg_.builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) قراءة count من الموقع [1] / (EN) Read count from slot [1]
            auto cntPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto countVal = cg_.builder_->CreateLoad(i64Ty, cntPtr);

            // (AR) تحويل إلى مقارنة: count > 0 / (EN) Convert to comparison: count > 0
            auto result = cg_.builder_->CreateICmpUGT(countVal, llvm::ConstantInt::get(i64Ty, 0));
            auto resultI64 = cg_.builder_->CreateZExt(result, i64Ty);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = resultI64;
            }
            return resultI64;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // (AR) حجم القناة — قراءة عداد العناصر count من الموقع [1]
        // (EN) Channel size — read element count from slot [1] of ring buffer
        // ═══════════════════════════════════════════════════════════════════════════
        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelSize(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = cg_.resolveOperand(inst->operands[0]);
            auto chanPtr = cg_.builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) قراءة count من الموقع [1] / (EN) Read count from slot [1]
            auto cntPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto countVal = cg_.builder_->CreateLoad(i64Ty, cntPtr);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = countVal;
            }
            return countVal;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelCapacity(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = cg_.resolveOperand(inst->operands[0]);
            auto chanPtr = cg_.builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);

            // (AR) السعة في الموقع [0]
            auto capPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capVal = cg_.builder_->CreateLoad(i64Ty, capPtr);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = capVal;
            }
            return capVal;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelTrySend(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) try_send: يرسل إذا كان هناك مكان، ويرجع 1 عند النجاح و0 عند الفشل.
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *value = cg_.resolveOperand(inst->operands[1]);

            // (AR) تحويل ptr → i64 إذا كان نصاً
            if (value->getType()->isPointerTy())
            {
                value = cg_.builder_->CreatePtrToInt(value, i64Ty, "trysend.p2i");
            }

            auto chanPtr = cg_.builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);

            auto capPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capacity = cg_.builder_->CreateLoad(i64Ty, capPtr);
            auto cntPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto count = cg_.builder_->CreateLoad(i64Ty, cntPtr);
            auto closedPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 4)});
            auto isClosed = cg_.builder_->CreateLoad(i64Ty, closedPtr);

            auto hasSpace = cg_.builder_->CreateICmpULT(count, capacity);
            auto notClosed = cg_.builder_->CreateICmpEQ(isClosed, llvm::ConstantInt::get(i64Ty, 0));
            auto canSend = cg_.builder_->CreateAnd(hasSpace, notClosed);

            auto parentFunc = cg_.builder_->GetInsertBlock()->getParent();
            auto okBB = llvm::BasicBlock::Create(*cg_.context_, "chan_try_send_ok", parentFunc);
            auto failBB = llvm::BasicBlock::Create(*cg_.context_, "chan_try_send_fail", parentFunc);
            auto doneBB = llvm::BasicBlock::Create(*cg_.context_, "chan_try_send_done", parentFunc);
            cg_.builder_->CreateCondBr(canSend, okBB, failBB);

            cg_.builder_->SetInsertPoint(okBB);
            auto tailPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 3)});
            auto tail = cg_.builder_->CreateLoad(i64Ty, tailPtr);
            // (AR) فتحتان لكل عنصر: data[tail*2] = قيمة, data[tail*2+1] = وسم النوع
            auto tailMul2 = cg_.builder_->CreateMul(tail, llvm::ConstantInt::get(i64Ty, 2), "ts.tail.x2");
            auto dataIdx = cg_.builder_->CreateAdd(tailMul2, llvm::ConstantInt::get(i64Ty, 5));
            auto dataSlot = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {dataIdx});
            cg_.builder_->CreateStore(value, dataSlot);
            // (AR) تخزين وسم النوع
            {
                int64_t typeTag = 0;
                if (inst->operands.size() > 1)
                {
                    auto valType = inst->operands[1].dataType;
                    if (valType == SadTypeKind::String || valType == SadTypeKind::Pointer ||
                        valType == SadTypeKind::Class || valType == SadTypeKind::Array)
                        typeTag = 1;
                    else if (valType == SadTypeKind::Boolean)
                        typeTag = 2;
                }
                auto typeIdx = cg_.builder_->CreateAdd(dataIdx, llvm::ConstantInt::get(i64Ty, 1));
                auto typeSlot = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {typeIdx});
                cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, typeTag), typeSlot);
            }
            auto nextTail = cg_.builder_->CreateAdd(tail, llvm::ConstantInt::get(i64Ty, 1));
            auto newTail = cg_.builder_->CreateURem(nextTail, capacity);
            cg_.builder_->CreateStore(newTail, tailPtr);
            auto newCount = cg_.builder_->CreateAdd(count, llvm::ConstantInt::get(i64Ty, 1));
            cg_.builder_->CreateStore(newCount, cntPtr);
            cg_.builder_->CreateBr(doneBB);

            cg_.builder_->SetInsertPoint(failBB);
            cg_.builder_->CreateBr(doneBB);

            cg_.builder_->SetInsertPoint(doneBB);
            auto resultPhi = cg_.builder_->CreatePHI(i64Ty, 2, "chan_try_send_res");
            resultPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 1), okBB);
            resultPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), failBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = resultPhi;
            }
            return resultPhi;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelTryRecv(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) try_recv: يستقبل إذا وُجدت بيانات، وإلا يرجع 0 (قيمة فشل غير حاجبة).
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = cg_.resolveOperand(inst->operands[0]);
            auto chanPtr = cg_.builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);

            auto capPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capacity = cg_.builder_->CreateLoad(i64Ty, capPtr);
            auto cntPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto count = cg_.builder_->CreateLoad(i64Ty, cntPtr);

            auto hasData = cg_.builder_->CreateICmpUGT(count, llvm::ConstantInt::get(i64Ty, 0));

            auto parentFunc = cg_.builder_->GetInsertBlock()->getParent();
            auto recvOkBB = llvm::BasicBlock::Create(*cg_.context_, "chan_try_recv_ok", parentFunc);
            auto recvEmptyBB = llvm::BasicBlock::Create(*cg_.context_, "chan_try_recv_empty", parentFunc);
            auto recvDoneBB = llvm::BasicBlock::Create(*cg_.context_, "chan_try_recv_done", parentFunc);

            cg_.builder_->CreateCondBr(hasData, recvOkBB, recvEmptyBB);

            cg_.builder_->SetInsertPoint(recvOkBB);
            auto headPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 2)});
            auto head = cg_.builder_->CreateLoad(i64Ty, headPtr);
            // (AR) فتحتان لكل عنصر: data[head*2] = قيمة, data[head*2+1] = وسم النوع
            auto headMul2 = cg_.builder_->CreateMul(head, llvm::ConstantInt::get(i64Ty, 2), "tr.head.x2");
            auto dataIdx = cg_.builder_->CreateAdd(headMul2, llvm::ConstantInt::get(i64Ty, 5));
            auto dataSlot = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {dataIdx});
            auto recvValueRaw = cg_.builder_->CreateLoad(i64Ty, dataSlot);

            // (AR) قراءة وسم النوع من الفتحة التالية
            auto typeIdx = cg_.builder_->CreateAdd(dataIdx, llvm::ConstantInt::get(i64Ty, 1));
            auto typeSlot = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {typeIdx});
            auto typeTag = cg_.builder_->CreateLoad(i64Ty, typeSlot, "tr.typetag");

            auto nextHead = cg_.builder_->CreateAdd(head, llvm::ConstantInt::get(i64Ty, 1));
            auto newHead = cg_.builder_->CreateURem(nextHead, capacity);
            cg_.builder_->CreateStore(newHead, headPtr);
            auto newCount = cg_.builder_->CreateSub(count, llvm::ConstantInt::get(i64Ty, 1));
            cg_.builder_->CreateStore(newCount, cntPtr);

            // (AR) القيمة الخام تُرجع مباشرة بدون وسم MSB
            //      try_recv الآن يُرجع Integer — kSadNullSentinel للقناة الفارغة
            //      يُعالج sentinel عند الطباعة/الدمج
            llvm::Value *recvValue = recvValueRaw;

            cg_.builder_->CreateBr(recvDoneBB);

            cg_.builder_->SetInsertPoint(recvEmptyBB);
            cg_.builder_->CreateBr(recvDoneBB);

            cg_.builder_->SetInsertPoint(recvDoneBB);
            auto phi = cg_.builder_->CreatePHI(i64Ty, 2, "chan_try_recv_val");
            phi->addIncoming(recvValue, recvOkBB);
            // (AR) عند فشل try_recv نعيد null-sentinel ليتطابق مع لاشيء في اللغة.
            phi->addIncoming(llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel), recvEmptyBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = phi;
            }
            return phi;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelSendTimeout(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) send_timeout: في المرحلة الحالية يُعامل كـ try_send (غير حاجب).
            //      operands: [channel, value, timeout_ms]
            if (inst->operands.size() < 2)
            {
                auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
                auto zero = llvm::ConstantInt::get(i64Ty, 0);
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = zero;
                }
                return zero;
            }

            auto lowered = std::make_shared<SIRInstruction>(SIROpcode::ASYNC_CHANNEL_TRY_SEND);
            lowered->result = inst->result;
            lowered->operands.push_back(inst->operands[0]);
            lowered->operands.push_back(inst->operands[1]);
            return emitAsyncChannelTrySend(lowered);
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncChannelRecvTimeout(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) recv_timeout: غير حاجب حالياً؛ إذا القناة فارغة نعيد null-sentinel.
            //      operands: [channel, timeout_ms]
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            if (inst->operands.empty())
            {
                auto nullSentinel = llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel);
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = nullSentinel;
                }
                return nullSentinel;
            }

            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *chanId = cg_.resolveOperand(inst->operands[0]);
            auto chanPtr = cg_.builder_->CreateIntToPtr(chanId, i8PtrTy);
            auto chanI64Ptr = cg_.builder_->CreateBitCast(chanPtr, i64PtrTy);

            auto capPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 0)});
            auto capacity = cg_.builder_->CreateLoad(i64Ty, capPtr);
            auto cntPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto count = cg_.builder_->CreateLoad(i64Ty, cntPtr);

            auto hasData = cg_.builder_->CreateICmpUGT(count, llvm::ConstantInt::get(i64Ty, 0));

            auto parentFunc = cg_.builder_->GetInsertBlock()->getParent();
            auto recvOkBB = llvm::BasicBlock::Create(*cg_.context_, "chan_recv_to_ok", parentFunc);
            auto recvEmptyBB = llvm::BasicBlock::Create(*cg_.context_, "chan_recv_to_empty", parentFunc);
            auto recvDoneBB = llvm::BasicBlock::Create(*cg_.context_, "chan_recv_to_done", parentFunc);
            cg_.builder_->CreateCondBr(hasData, recvOkBB, recvEmptyBB);

            cg_.builder_->SetInsertPoint(recvOkBB);
            auto headPtr = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {llvm::ConstantInt::get(i64Ty, 2)});
            auto head = cg_.builder_->CreateLoad(i64Ty, headPtr);
            // (AR) فتحتان لكل عنصر: data[head*2] = قيمة — تتوافق مع تخطيط send
            auto headMul2 = cg_.builder_->CreateMul(head, llvm::ConstantInt::get(i64Ty, 2), "rto.head.x2");
            auto dataIdx = cg_.builder_->CreateAdd(headMul2, llvm::ConstantInt::get(i64Ty, 5));
            auto dataSlot = cg_.builder_->CreateGEP(i64Ty, chanI64Ptr, {dataIdx});
            auto recvValue = cg_.builder_->CreateLoad(i64Ty, dataSlot);
            auto nextHead = cg_.builder_->CreateAdd(head, llvm::ConstantInt::get(i64Ty, 1));
            auto newHead = cg_.builder_->CreateURem(nextHead, capacity);
            cg_.builder_->CreateStore(newHead, headPtr);
            auto newCount = cg_.builder_->CreateSub(count, llvm::ConstantInt::get(i64Ty, 1));
            cg_.builder_->CreateStore(newCount, cntPtr);
            cg_.builder_->CreateBr(recvDoneBB);

            cg_.builder_->SetInsertPoint(recvEmptyBB);
            cg_.builder_->CreateBr(recvDoneBB);

            cg_.builder_->SetInsertPoint(recvDoneBB);
            auto phi = cg_.builder_->CreatePHI(i64Ty, 2, "chan_recv_to_val");
            phi->addIncoming(recvValue, recvOkBB);
            phi->addIncoming(llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel), recvEmptyBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = phi;
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