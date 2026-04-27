// ============================================================================
// llvm_codegen_freestanding_conv.cpp — دوال التحويل والمنفذ التسلسلي
// (AR) serial_puts, serial_putint, itoa, ftoa, xtoa
// (EN) Serial output + number-to-string conversion functions
// تم استخراج هذا الملف من llvm_codegen_freestanding_io.cpp وفقاً لقاعدة CW-05
// ============================================================================
// ============================================================================
// llvm_codegen_freestanding_io.cpp — Freestanding I/O & Conversion Functions
// (AR) دوال الإدخال/الإخراج والتحويل للوضع المستقل (freestanding)
// (EN) I/O and conversion functions for freestanding mode
// ============================================================================
// Author: Sad Compiler Team
// Date: January 2026
//
// (AR) هذا الملف يحتوي تطبيقات الدوال المتعلقة بالإدخال/الإخراج والتحويل:
//   - printf: طباعة منسقة عبر المنفذ التسلسلي COM1
//   - sprintf: تنسيق نص إلى مخزن مؤقت
//   - pow: دالة الأُس الرياضية
//   - __sad_serial_puts: إخراج نص مباشر عبر المنفذ التسلسلي
//   - __sad_serial_putint: إخراج رقم عبر المنفذ التسلسلي
//   - __sad_itoa: تحويل رقم صحيح إلى نص
//   - __sad_ftoa: تحويل رقم عشري إلى نص
//   - __sad_xtoa: تحويل رقم سداسي عشري إلى نص
//
//   تم فصل هذه الدوال من llvm_codegen_freestanding.cpp الذي يحتفظ بـ:
//   - نقطة الدخول emitFreestandingRuntime + الدوال المساعدة
//   - دوال الذاكرة (malloc, free, realloc, calloc)
//   - دوال النصوص (memcpy, memset, strlen, strcmp, strcpy, strcat)
// ============================================================================

#include "llvm_codegen.h"
#include "builders/freestanding_codegen.h"
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Intrinsics.h>

namespace Sad
{
    namespace LLVM
    {

        static bool hasDefinition(llvm::Module *mod, const std::string &name)
        {
            llvm::Function *fn = mod->getFunction(name);
            return fn && !fn->isDeclaration();
        }

        static llvm::Function *getOrCreateFreestandingFunc(
            llvm::Module *mod, llvm::LLVMContext &ctx,
            const std::string &name, llvm::FunctionType *ft)
        {
            if (hasDefinition(mod, name))
                return nullptr;
            llvm::Function *fn = mod->getFunction(name);
            if (!fn)
                fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, mod);
            fn->setLinkage(llvm::Function::WeakODRLinkage);
            fn->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
            fn->addFnAttr("no-builtins");
            fn->addFnAttr(llvm::Attribute::NoInline);
            fn->addFnAttr(llvm::Attribute::OptimizeNone);
            return fn;
        }

        void FreestandingCodeGen::emitFreestandingSerialPuts(
            llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*cg_.context_), {ptrTy}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(
                cg_.module_.get(), *cg_.context_, "__sad_serial_puts", ft);
            if (!fn)
                return;

            auto savedIP = cg_.builder_->saveIP();
            llvm::Type *i16Ty = llvm::Type::getInt16Ty(*cg_.context_);

            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *loop = llvm::BasicBlock::Create(*cg_.context_, "loop", fn);
            llvm::BasicBlock *send = llvm::BasicBlock::Create(*cg_.context_, "send", fn);
            llvm::BasicBlock *done = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

            cg_.builder_->SetInsertPoint(entry);
            llvm::Value *str = fn->getArg(0);
            cg_.builder_->CreateBr(loop);

            cg_.builder_->SetInsertPoint(loop);
            llvm::PHINode *idx = cg_.builder_->CreatePHI(i64Ty, 2, "idx");
            idx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);

            llvm::Value *ptr = cg_.builder_->CreateGEP(i8Ty, str, idx, "ch.ptr");
            llvm::Value *ch = cg_.builder_->CreateLoad(i8Ty, ptr, "ch");
            llvm::Value *isEnd = cg_.builder_->CreateICmpEQ(ch, llvm::ConstantInt::get(i8Ty, 0));
            cg_.builder_->CreateCondBr(isEnd, done, send);

            cg_.builder_->SetInsertPoint(send);
            llvm::InlineAsm *outAsm = llvm::InlineAsm::get(
                llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_),
                                        {i16Ty, i8Ty}, false),
                "outb %al, %dx", "{dx},{al}", true, false);
            llvm::Value *port = llvm::ConstantInt::get(i16Ty, 0x3F8);
            cg_.builder_->CreateCall(outAsm, {port, ch});

            llvm::Value *nextIdx = cg_.builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1));
            idx->addIncoming(nextIdx, send);
            cg_.builder_->CreateBr(loop);

            cg_.builder_->SetInsertPoint(done);
            cg_.builder_->CreateRetVoid();

            cg_.builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 15. __sad_serial_putint — Output a 64-bit signed integer to serial port
        //     Handles negative numbers and zero. No leading zeros.
        // ============================================================================
        void FreestandingCodeGen::emitFreestandingSerialPutInt(
            llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*cg_.context_), {i64Ty}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(
                cg_.module_.get(), *cg_.context_, "__sad_serial_putint", ft);
            if (!fn)
                return;

            auto savedIP = cg_.builder_->saveIP();
            llvm::Type *i16Ty = llvm::Type::getInt16Ty(*cg_.context_);

            // Blocks
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *isNegBB = llvm::BasicBlock::Create(*cg_.context_, "is_neg", fn);
            llvm::BasicBlock *posStart = llvm::BasicBlock::Create(*cg_.context_, "pos_start", fn);
            llvm::BasicBlock *revLoop = llvm::BasicBlock::Create(*cg_.context_, "rev_loop", fn);
            llvm::BasicBlock *revDone = llvm::BasicBlock::Create(*cg_.context_, "rev_done", fn);
            llvm::BasicBlock *outLoop = llvm::BasicBlock::Create(*cg_.context_, "out_loop", fn);
            llvm::BasicBlock *outBody = llvm::BasicBlock::Create(*cg_.context_, "out_body", fn);
            llvm::BasicBlock *done = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

            // Inline asm for outb
            llvm::InlineAsm *outAsm = llvm::InlineAsm::get(
                llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_),
                                        {i16Ty, i8Ty}, false),
                "outb %al, %dx", "{dx},{al}", true, false);
            llvm::Value *port = llvm::ConstantInt::get(i16Ty, 0x3F8);

            // Use a 21-byte stack buffer for digits (max 20 digits for int64 + null)
            cg_.builder_->SetInsertPoint(entry);
            llvm::Value *val = fn->getArg(0);
            llvm::Value *buf = cg_.builder_->CreateAlloca(
                llvm::ArrayType::get(i8Ty, 21), nullptr, "buf");
            llvm::Value *bufPtr = cg_.builder_->CreateBitCast(buf, ptrTy, "buf.ptr");

            // Check negative
            llvm::Value *isNeg = cg_.builder_->CreateICmpSLT(val, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(isNeg, isNegBB, posStart);

            // Negative: output '-' and negate
            cg_.builder_->SetInsertPoint(isNegBB);
            cg_.builder_->CreateCall(outAsm, {port, llvm::ConstantInt::get(i8Ty, '-')});
            llvm::Value *negVal = cg_.builder_->CreateNeg(val, "neg");
            cg_.builder_->CreateBr(posStart);

            // Start with positive value
            cg_.builder_->SetInsertPoint(posStart);
            llvm::PHINode *absVal = cg_.builder_->CreatePHI(i64Ty, 2, "abs_val");
            absVal->addIncoming(val, entry);
            absVal->addIncoming(negVal, isNegBB);
            cg_.builder_->CreateBr(revLoop);

            // Reverse loop: extract digits into buffer (least significant first)
            cg_.builder_->SetInsertPoint(revLoop);
            llvm::PHINode *num = cg_.builder_->CreatePHI(i64Ty, 2, "num");
            llvm::PHINode *pos = cg_.builder_->CreatePHI(i64Ty, 2, "pos");
            num->addIncoming(absVal, posStart);
            pos->addIncoming(llvm::ConstantInt::get(i64Ty, 0), posStart);

            // digit = num % 10 + '0'
            llvm::Value *digit = cg_.builder_->CreateURem(num, llvm::ConstantInt::get(i64Ty, 10));
            llvm::Value *digitCh = cg_.builder_->CreateTrunc(
                cg_.builder_->CreateAdd(digit, llvm::ConstantInt::get(i64Ty, 48)), i8Ty, "digit_ch");
            llvm::Value *bufSlot = cg_.builder_->CreateGEP(i8Ty, bufPtr, pos, "slot");
            cg_.builder_->CreateStore(digitCh, bufSlot);

            llvm::Value *nextNum = cg_.builder_->CreateUDiv(num, llvm::ConstantInt::get(i64Ty, 10));
            llvm::Value *nextPos = cg_.builder_->CreateAdd(pos, llvm::ConstantInt::get(i64Ty, 1));
            num->addIncoming(nextNum, revLoop);
            pos->addIncoming(nextPos, revLoop);

            // Continue if num/10 > 0
            llvm::Value *more = cg_.builder_->CreateICmpUGT(nextNum, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(more, revLoop, revDone);

            // Prepare to output in reverse order
            cg_.builder_->SetInsertPoint(revDone);
            // pos now holds the count of digits. Output from pos-1 down to 0
            llvm::Value *startIdx = cg_.builder_->CreateSub(nextPos, llvm::ConstantInt::get(i64Ty, 1));
            cg_.builder_->CreateBr(outLoop);

            cg_.builder_->SetInsertPoint(outLoop);
            llvm::PHINode *outIdx = cg_.builder_->CreatePHI(i64Ty, 2, "out_idx");
            outIdx->addIncoming(startIdx, revDone);
            // Check if outIdx >= 0 (since i64 is unsigned, check outIdx < count)
            llvm::Value *gez = cg_.builder_->CreateICmpSGE(outIdx, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(gez, outBody, done);

            cg_.builder_->SetInsertPoint(outBody);
            llvm::Value *outSlot = cg_.builder_->CreateGEP(i8Ty, bufPtr, outIdx, "out.slot");
            llvm::Value *outCh = cg_.builder_->CreateLoad(i8Ty, outSlot, "out.ch");
            cg_.builder_->CreateCall(outAsm, {port, outCh});
            llvm::Value *prevIdx = cg_.builder_->CreateSub(outIdx, llvm::ConstantInt::get(i64Ty, 1));
            outIdx->addIncoming(prevIdx, outBody);
            cg_.builder_->CreateBr(outLoop);

            cg_.builder_->SetInsertPoint(done);
            cg_.builder_->CreateRetVoid();

            cg_.builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 16. __sad_itoa — Convert i64 to decimal string in buffer, return length
        //     Signature: i32 __sad_itoa(ptr buf, i64 val)
        //     Non-variadic replacement for sprintf(buf, "%lld", val) in freestanding
        // ============================================================================
        void FreestandingCodeGen::emitFreestandingItoa(
            llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(
                cg_.module_.get(), *cg_.context_, "__sad_itoa", ft);
            if (!fn)
                return;

            auto savedIP = cg_.builder_->saveIP();

            // Block layout:
            //   entry -> isNeg? -> posStart -> digitLoop -> digitDone -> revLoop? -> revDone (ret)
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *isNegBB = llvm::BasicBlock::Create(*cg_.context_, "is_neg", fn);
            llvm::BasicBlock *posStart = llvm::BasicBlock::Create(*cg_.context_, "pos_start", fn);
            llvm::BasicBlock *zeroCase = llvm::BasicBlock::Create(*cg_.context_, "zero_case", fn);
            llvm::BasicBlock *nonZero = llvm::BasicBlock::Create(*cg_.context_, "non_zero", fn);
            llvm::BasicBlock *digitLoop = llvm::BasicBlock::Create(*cg_.context_, "digit_loop", fn);
            llvm::BasicBlock *digitDone = llvm::BasicBlock::Create(*cg_.context_, "digit_done", fn);
            llvm::BasicBlock *revLoop = llvm::BasicBlock::Create(*cg_.context_, "rev_loop", fn);
            llvm::BasicBlock *revDone = llvm::BasicBlock::Create(*cg_.context_, "rev_done", fn);

            cg_.builder_->SetInsertPoint(entry);
            llvm::Value *buf = fn->getArg(0);
            llvm::Value *val = fn->getArg(1);

            // (AR) تحقق من القيمة السالبة
            // (EN) Check for negative value
            llvm::Value *isNeg = cg_.builder_->CreateICmpSLT(val, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(isNeg, isNegBB, posStart);

            // Negative: store '-' at buf[0], negate value
            cg_.builder_->SetInsertPoint(isNegBB);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '-'), buf);
            llvm::Value *negVal = cg_.builder_->CreateNeg(val, "neg");
            cg_.builder_->CreateBr(posStart);

            // Merge absolute value and starting offset (0 or 1)
            cg_.builder_->SetInsertPoint(posStart);
            llvm::PHINode *absVal = cg_.builder_->CreatePHI(i64Ty, 2, "abs_val");
            absVal->addIncoming(val, entry);
            absVal->addIncoming(negVal, isNegBB);
            llvm::PHINode *startOff = cg_.builder_->CreatePHI(i64Ty, 2, "start_off");
            startOff->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            startOff->addIncoming(llvm::ConstantInt::get(i64Ty, 1), isNegBB);

            // (AR) حالة خاصة: القيمة صفر
            // (EN) Special case: value is zero
            llvm::Value *isZero = cg_.builder_->CreateICmpEQ(absVal, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(isZero, zeroCase, nonZero);

            // Zero: write '0' + null, return length
            cg_.builder_->SetInsertPoint(zeroCase);
            llvm::Value *zeroSlot = cg_.builder_->CreateGEP(i8Ty, buf, startOff, "zero.slot");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '0'), zeroSlot);
            llvm::Value *zeroNull = cg_.builder_->CreateGEP(i8Ty, buf,
                                                        cg_.builder_->CreateAdd(startOff, llvm::ConstantInt::get(i64Ty, 1)), "zero.null");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), zeroNull);
            llvm::Value *zeroLen = cg_.builder_->CreateTrunc(
                cg_.builder_->CreateAdd(startOff, llvm::ConstantInt::get(i64Ty, 1)), i32Ty, "zero.len");
            cg_.builder_->CreateRet(zeroLen);

            // Non-zero: extract digits in reverse order into buf[startOff..]
            cg_.builder_->SetInsertPoint(nonZero);
            cg_.builder_->CreateBr(digitLoop);

            cg_.builder_->SetInsertPoint(digitLoop);
            llvm::PHINode *num = cg_.builder_->CreatePHI(i64Ty, 2, "num");
            llvm::PHINode *pos = cg_.builder_->CreatePHI(i64Ty, 2, "pos");
            num->addIncoming(absVal, nonZero);
            pos->addIncoming(llvm::ConstantInt::get(i64Ty, 0), nonZero);

            // digit = num % 10 + '0'
            llvm::Value *digit = cg_.builder_->CreateURem(num, llvm::ConstantInt::get(i64Ty, 10));
            llvm::Value *digitCh = cg_.builder_->CreateTrunc(
                cg_.builder_->CreateAdd(digit, llvm::ConstantInt::get(i64Ty, 48)), i8Ty, "digit_ch");

            // Store at buf[startOff + pos]
            llvm::Value *writeIdx = cg_.builder_->CreateAdd(startOff, pos, "write_idx");
            llvm::Value *slot = cg_.builder_->CreateGEP(i8Ty, buf, writeIdx, "slot");
            cg_.builder_->CreateStore(digitCh, slot);

            llvm::Value *nextNum = cg_.builder_->CreateUDiv(num, llvm::ConstantInt::get(i64Ty, 10));
            llvm::Value *nextPos = cg_.builder_->CreateAdd(pos, llvm::ConstantInt::get(i64Ty, 1));
            num->addIncoming(nextNum, digitLoop);
            pos->addIncoming(nextPos, digitLoop);

            llvm::Value *more = cg_.builder_->CreateICmpUGT(nextNum, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(more, digitLoop, digitDone);

            // (AR) انتهينا من استخراج الأرقام. أنهِ بـ null ثم اعكس ترتيب الأرقام
            // (EN) Done extracting digits. Null-terminate then reverse the digit portion
            cg_.builder_->SetInsertPoint(digitDone);
            llvm::Value *totalLen = cg_.builder_->CreateAdd(startOff, nextPos, "total_len");
            // Null terminate
            llvm::Value *nullSlot = cg_.builder_->CreateGEP(i8Ty, buf, totalLen, "null.slot");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), nullSlot);

            // Reverse digits buf[startOff .. totalLen-1]
            llvm::Value *lo = startOff;
            llvm::Value *hi = cg_.builder_->CreateSub(totalLen, llvm::ConstantInt::get(i64Ty, 1), "hi.init");
            llvm::Value *needRev = cg_.builder_->CreateICmpSLT(lo, hi);
            cg_.builder_->CreateCondBr(needRev, revLoop, revDone);

            cg_.builder_->SetInsertPoint(revLoop);
            llvm::PHINode *loP = cg_.builder_->CreatePHI(i64Ty, 2, "lo_p");
            llvm::PHINode *hiP = cg_.builder_->CreatePHI(i64Ty, 2, "hi_p");
            loP->addIncoming(lo, digitDone);
            hiP->addIncoming(hi, digitDone);

            // swap buf[lo] and buf[hi]
            llvm::Value *loSlot = cg_.builder_->CreateGEP(i8Ty, buf, loP, "lo.slot");
            llvm::Value *hiSlot = cg_.builder_->CreateGEP(i8Ty, buf, hiP, "hi.slot");
            llvm::Value *loVal = cg_.builder_->CreateLoad(i8Ty, loSlot, "lo.val");
            llvm::Value *hiVal = cg_.builder_->CreateLoad(i8Ty, hiSlot, "hi.val");
            cg_.builder_->CreateStore(hiVal, loSlot);
            cg_.builder_->CreateStore(loVal, hiSlot);

            llvm::Value *newLo = cg_.builder_->CreateAdd(loP, llvm::ConstantInt::get(i64Ty, 1));
            llvm::Value *newHi = cg_.builder_->CreateSub(hiP, llvm::ConstantInt::get(i64Ty, 1));
            loP->addIncoming(newLo, revLoop);
            hiP->addIncoming(newHi, revLoop);

            llvm::Value *stillNeed = cg_.builder_->CreateICmpSLT(newLo, newHi);
            cg_.builder_->CreateCondBr(stillNeed, revLoop, revDone);

            // Return total length
            cg_.builder_->SetInsertPoint(revDone);
            llvm::PHINode *finalLen = cg_.builder_->CreatePHI(i64Ty, 2, "final_len");
            finalLen->addIncoming(totalLen, digitDone);
            finalLen->addIncoming(totalLen, revLoop);
            cg_.builder_->CreateRet(cg_.builder_->CreateTrunc(finalLen, i32Ty, "ret.len"));

            cg_.builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 17. __sad_ftoa — Convert f64 to decimal string in buffer, return length
        //     Signature: i32 __sad_ftoa(ptr buf, f64 val)
        //     Writes: [-]integer_part.fractional_digits
        // ============================================================================
        void FreestandingCodeGen::emitFreestandingFtoa(
            llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            auto *dblTy = llvm::Type::getDoubleTy(*cg_.context_);
            llvm::FunctionType *ft = llvm::FunctionType::get(i32Ty, {ptrTy, dblTy}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(
                cg_.module_.get(), *cg_.context_, "__sad_ftoa", ft);
            if (!fn)
                return;

            auto savedIP = cg_.builder_->saveIP();

            auto *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            auto *negBB = llvm::BasicBlock::Create(*cg_.context_, "neg", fn);
            auto *posBB = llvm::BasicBlock::Create(*cg_.context_, "pos", fn);
            auto *writeFrac = llvm::BasicBlock::Create(*cg_.context_, "write_frac", fn);
            auto *stripLoop = llvm::BasicBlock::Create(*cg_.context_, "strip_loop", fn);
            auto *stripChk = llvm::BasicBlock::Create(*cg_.context_, "strip_chk", fn);
            auto *stripDo = llvm::BasicBlock::Create(*cg_.context_, "strip_do", fn);
            auto *finalize = llvm::BasicBlock::Create(*cg_.context_, "finalize", fn);

            cg_.builder_->SetInsertPoint(entry);
            auto *buf = fn->getArg(0);
            auto *val = fn->getArg(1);

            // (AR) مؤشر الإزاحة الحالية في المخزن (alloca لتبسيط التحكم)
            // (EN) Current offset pointer in buffer (alloca for simpler control flow)
            auto *pOff = cg_.builder_->CreateAlloca(i64Ty, nullptr, "p.off");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), pOff);

            auto *isNeg = cg_.builder_->CreateFCmpOLT(val, llvm::ConstantFP::get(dblTy, 0.0));
            cg_.builder_->CreateCondBr(isNeg, negBB, posBB);

            // Negative: store '-', negate
            cg_.builder_->SetInsertPoint(negBB);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '-'),
                                  cg_.builder_->CreateGEP(i8Ty, buf, llvm::ConstantInt::get(i64Ty, 0)));
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 1), pOff);
            auto *negVal = cg_.builder_->CreateFNeg(val, "negval");
            cg_.builder_->CreateBr(posBB);

            // Positive path
            cg_.builder_->SetInsertPoint(posBB);
            auto *absVal = cg_.builder_->CreatePHI(dblTy, 2, "abs_val");
            absVal->addIncoming(val, entry);
            absVal->addIncoming(negVal, negBB);

            // Integer part
            auto *intPart = cg_.builder_->CreateFPToSI(absVal, i64Ty, "int_part");
            auto *intPartF = cg_.builder_->CreateSIToFP(intPart, dblTy, "int_part_f");

            // Write integer part via __sad_itoa
            auto itoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            auto itoaFn = cg_.module_->getOrInsertFunction("__sad_itoa", itoaFT);
            auto *curOff = cg_.builder_->CreateLoad(i64Ty, pOff, "cur_off");
            auto *bufOff = cg_.builder_->CreateGEP(i8Ty, buf, curOff, "buf.off");
            auto *intLen = cg_.builder_->CreateCall(itoaFn, {bufOff, intPart}, "int_len");
            auto *intLen64 = cg_.builder_->CreateSExt(intLen, i64Ty, "int_len64");
            auto *newOff = cg_.builder_->CreateAdd(curOff, intLen64, "new_off");
            cg_.builder_->CreateStore(newOff, pOff);

            // Write '.'
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '.'),
                                  cg_.builder_->CreateGEP(i8Ty, buf, newOff));
            auto *dotOff = cg_.builder_->CreateAdd(newOff, llvm::ConstantInt::get(i64Ty, 1));
            cg_.builder_->CreateStore(dotOff, pOff);
            cg_.builder_->CreateBr(writeFrac);

            // Fractional part: 6 digits with leading zeros
            cg_.builder_->SetInsertPoint(writeFrac);
            auto *fracF = cg_.builder_->CreateFSub(absVal, intPartF, "frac_f");
            auto *fracMul = cg_.builder_->CreateFMul(fracF,
                                                 llvm::ConstantFP::get(dblTy, 1000000.0), "frac_mul");
            auto *fracInt = cg_.builder_->CreateFPToSI(fracMul, i64Ty, "frac_int");
            // Handle rounding error: abs(fracInt)
            auto *fracNeg = cg_.builder_->CreateICmpSLT(fracInt, llvm::ConstantInt::get(i64Ty, 0));
            auto *fracNegVal = cg_.builder_->CreateNeg(fracInt, "frac_neg");
            auto *fracAbs = cg_.builder_->CreateSelect(fracNeg, fracNegVal, fracInt, "frac_abs");

            auto *digitBase = cg_.builder_->CreateLoad(i64Ty, pOff, "digit_base");

            // Write 6 digits (d5..d0) with leading zeros using sequential div/mod
            llvm::Value *divisors[] = {
                llvm::ConstantInt::get(i64Ty, 100000),
                llvm::ConstantInt::get(i64Ty, 10000),
                llvm::ConstantInt::get(i64Ty, 1000),
                llvm::ConstantInt::get(i64Ty, 100),
                llvm::ConstantInt::get(i64Ty, 10),
                llvm::ConstantInt::get(i64Ty, 1)};

            for (int d = 0; d < 6; d++)
            {
                auto *digit = cg_.builder_->CreateURem(
                    cg_.builder_->CreateUDiv(fracAbs, divisors[d]), llvm::ConstantInt::get(i64Ty, 10));
                auto *digitCh = cg_.builder_->CreateTrunc(
                    cg_.builder_->CreateAdd(digit, llvm::ConstantInt::get(i64Ty, '0')), i8Ty);
                auto *slot = cg_.builder_->CreateGEP(i8Ty, buf,
                                                 cg_.builder_->CreateAdd(digitBase, llvm::ConstantInt::get(i64Ty, d)));
                cg_.builder_->CreateStore(digitCh, slot);
            }

            auto *endOff = cg_.builder_->CreateAdd(digitBase, llvm::ConstantInt::get(i64Ty, 6), "end_off");
            cg_.builder_->CreateStore(endOff, pOff);

            // (AR) حذف الأصفار الزائدة من نهاية الجزء العشري مع إبقاء رقم واحد على الأقل
            // (EN) Strip trailing zeros from fractional part, keeping at least 1 digit
            auto *minOff = cg_.builder_->CreateAdd(digitBase, llvm::ConstantInt::get(i64Ty, 1), "min_off");
            cg_.builder_->CreateBr(stripLoop);

            cg_.builder_->SetInsertPoint(stripLoop);
            auto *curEnd = cg_.builder_->CreateLoad(i64Ty, pOff, "cur_end");
            auto *above = cg_.builder_->CreateICmpSGT(curEnd, minOff);
            cg_.builder_->CreateCondBr(above, stripChk, finalize);

            cg_.builder_->SetInsertPoint(stripChk);
            auto *prevSlot = cg_.builder_->CreateGEP(i8Ty, buf,
                                                 cg_.builder_->CreateSub(curEnd, llvm::ConstantInt::get(i64Ty, 1)));
            auto *lastCh = cg_.builder_->CreateLoad(i8Ty, prevSlot, "last_ch");
            auto *isZero = cg_.builder_->CreateICmpEQ(lastCh, llvm::ConstantInt::get(i8Ty, '0'));
            cg_.builder_->CreateCondBr(isZero, stripDo, finalize);

            cg_.builder_->SetInsertPoint(stripDo);
            cg_.builder_->CreateStore(
                cg_.builder_->CreateSub(curEnd, llvm::ConstantInt::get(i64Ty, 1)), pOff);
            cg_.builder_->CreateBr(stripLoop);

            cg_.builder_->SetInsertPoint(finalize);
            auto *finalOff = cg_.builder_->CreateLoad(i64Ty, pOff, "final_off");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0),
                                  cg_.builder_->CreateGEP(i8Ty, buf, finalOff));
            cg_.builder_->CreateRet(cg_.builder_->CreateTrunc(finalOff, i32Ty, "ret.len"));

            cg_.builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 18. __sad_xtoa — Convert i64 to hexadecimal string in buffer, return length
        //     Signature: i32 __sad_xtoa(ptr buf, i64 val)
        // ============================================================================
        void FreestandingCodeGen::emitFreestandingXtoa(
            llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(
                cg_.module_.get(), *cg_.context_, "__sad_xtoa", ft);
            if (!fn)
                return;

            auto savedIP = cg_.builder_->saveIP();

            auto *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            auto *zeroBB = llvm::BasicBlock::Create(*cg_.context_, "zero", fn);
            auto *nonZero = llvm::BasicBlock::Create(*cg_.context_, "non_zero", fn);
            auto *hexLoop = llvm::BasicBlock::Create(*cg_.context_, "hex_loop", fn);
            auto *hexDone = llvm::BasicBlock::Create(*cg_.context_, "hex_done", fn);
            auto *revLoop = llvm::BasicBlock::Create(*cg_.context_, "rev_loop", fn);
            auto *revDone = llvm::BasicBlock::Create(*cg_.context_, "rev_done", fn);

            cg_.builder_->SetInsertPoint(entry);
            auto *buf = fn->getArg(0);
            auto *val = fn->getArg(1);

            auto *isZero = cg_.builder_->CreateICmpEQ(val, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(isZero, zeroBB, nonZero);

            // Zero case
            cg_.builder_->SetInsertPoint(zeroBB);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '0'), buf);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0),
                                  cg_.builder_->CreateGEP(i8Ty, buf, llvm::ConstantInt::get(i64Ty, 1)));
            cg_.builder_->CreateRet(llvm::ConstantInt::get(i32Ty, 1));

            // Non-zero: extract hex digits in reverse
            cg_.builder_->SetInsertPoint(nonZero);
            cg_.builder_->CreateBr(hexLoop);

            cg_.builder_->SetInsertPoint(hexLoop);
            auto *num = cg_.builder_->CreatePHI(i64Ty, 2, "num");
            auto *pos = cg_.builder_->CreatePHI(i64Ty, 2, "pos");
            num->addIncoming(val, nonZero);
            pos->addIncoming(llvm::ConstantInt::get(i64Ty, 0), nonZero);

            // nibble = num & 0xF
            auto *nibble = cg_.builder_->CreateAnd(num, llvm::ConstantInt::get(i64Ty, 0xF), "nibble");
            // hexCh = nibble < 10 ? nibble + '0' : nibble - 10 + 'a'
            auto *isDigit = cg_.builder_->CreateICmpULT(nibble, llvm::ConstantInt::get(i64Ty, 10));
            auto *chDigit = cg_.builder_->CreateAdd(nibble, llvm::ConstantInt::get(i64Ty, '0'));
            auto *chAlpha = cg_.builder_->CreateAdd(
                cg_.builder_->CreateSub(nibble, llvm::ConstantInt::get(i64Ty, 10)),
                llvm::ConstantInt::get(i64Ty, 'a'));
            auto *hexCh = cg_.builder_->CreateSelect(isDigit, chDigit, chAlpha, "hex_ch");
            auto *hexByte = cg_.builder_->CreateTrunc(hexCh, i8Ty, "hex_byte");
            cg_.builder_->CreateStore(hexByte, cg_.builder_->CreateGEP(i8Ty, buf, pos));

            auto *nextNum = cg_.builder_->CreateLShr(num, llvm::ConstantInt::get(i64Ty, 4), "next_num");
            auto *nextPos = cg_.builder_->CreateAdd(pos, llvm::ConstantInt::get(i64Ty, 1));
            num->addIncoming(nextNum, hexLoop);
            pos->addIncoming(nextPos, hexLoop);

            auto *more = cg_.builder_->CreateICmpNE(nextNum, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(more, hexLoop, hexDone);

            // Null terminate and reverse
            cg_.builder_->SetInsertPoint(hexDone);
            // nextPos is the count of digits
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0),
                                  cg_.builder_->CreateGEP(i8Ty, buf, nextPos));
            auto *lo = llvm::ConstantInt::get(i64Ty, 0);
            auto *hi = cg_.builder_->CreateSub(nextPos, llvm::ConstantInt::get(i64Ty, 1), "hi.init");
            auto *needRev = cg_.builder_->CreateICmpSLT(lo, hi);
            cg_.builder_->CreateCondBr(needRev, revLoop, revDone);

            cg_.builder_->SetInsertPoint(revLoop);
            auto *loP = cg_.builder_->CreatePHI(i64Ty, 2, "lo_p");
            auto *hiP = cg_.builder_->CreatePHI(i64Ty, 2, "hi_p");
            loP->addIncoming(lo, hexDone);
            hiP->addIncoming(hi, hexDone);
            auto *loSlot = cg_.builder_->CreateGEP(i8Ty, buf, loP);
            auto *hiSlot = cg_.builder_->CreateGEP(i8Ty, buf, hiP);
            auto *loVal = cg_.builder_->CreateLoad(i8Ty, loSlot);
            auto *hiVal = cg_.builder_->CreateLoad(i8Ty, hiSlot);
            cg_.builder_->CreateStore(hiVal, loSlot);
            cg_.builder_->CreateStore(loVal, hiSlot);
            auto *newLo = cg_.builder_->CreateAdd(loP, llvm::ConstantInt::get(i64Ty, 1));
            auto *newHi = cg_.builder_->CreateSub(hiP, llvm::ConstantInt::get(i64Ty, 1));
            loP->addIncoming(newLo, revLoop);
            hiP->addIncoming(newHi, revLoop);
            auto *stillRev = cg_.builder_->CreateICmpSLT(newLo, newHi);
            cg_.builder_->CreateCondBr(stillRev, revLoop, revDone);

            cg_.builder_->SetInsertPoint(revDone);
            auto *retLen = cg_.builder_->CreatePHI(i64Ty, 2, "ret_len");
            retLen->addIncoming(nextPos, hexDone);
            retLen->addIncoming(nextPos, revLoop);
            cg_.builder_->CreateRet(cg_.builder_->CreateTrunc(retLen, i32Ty));

            cg_.builder_->restoreIP(savedIP);
        }

    } // namespace LLVM
} // namespace Sad
