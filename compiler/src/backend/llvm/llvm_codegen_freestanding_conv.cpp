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

        void LLVMCodeGen::emitFreestandingSerialPuts(
            llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*context_), {ptrTy}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(
                module_.get(), *context_, "__sad_serial_puts", ft);
            if (!fn)
                return;

            auto savedIP = builder_->saveIP();
            llvm::Type *i16Ty = llvm::Type::getInt16Ty(*context_);

            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            llvm::BasicBlock *loop = llvm::BasicBlock::Create(*context_, "loop", fn);
            llvm::BasicBlock *send = llvm::BasicBlock::Create(*context_, "send", fn);
            llvm::BasicBlock *done = llvm::BasicBlock::Create(*context_, "done", fn);

            builder_->SetInsertPoint(entry);
            llvm::Value *str = fn->getArg(0);
            builder_->CreateBr(loop);

            builder_->SetInsertPoint(loop);
            llvm::PHINode *idx = builder_->CreatePHI(i64Ty, 2, "idx");
            idx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);

            llvm::Value *ptr = builder_->CreateGEP(i8Ty, str, idx, "ch.ptr");
            llvm::Value *ch = builder_->CreateLoad(i8Ty, ptr, "ch");
            llvm::Value *isEnd = builder_->CreateICmpEQ(ch, llvm::ConstantInt::get(i8Ty, 0));
            builder_->CreateCondBr(isEnd, done, send);

            builder_->SetInsertPoint(send);
            llvm::InlineAsm *outAsm = llvm::InlineAsm::get(
                llvm::FunctionType::get(llvm::Type::getVoidTy(*context_),
                                        {i16Ty, i8Ty}, false),
                "outb %al, %dx", "{dx},{al}", true, false);
            llvm::Value *port = llvm::ConstantInt::get(i16Ty, 0x3F8);
            builder_->CreateCall(outAsm, {port, ch});

            llvm::Value *nextIdx = builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1));
            idx->addIncoming(nextIdx, send);
            builder_->CreateBr(loop);

            builder_->SetInsertPoint(done);
            builder_->CreateRetVoid();

            builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 15. __sad_serial_putint — Output a 64-bit signed integer to serial port
        //     Handles negative numbers and zero. No leading zeros.
        // ============================================================================
        void LLVMCodeGen::emitFreestandingSerialPutInt(
            llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*context_), {i64Ty}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(
                module_.get(), *context_, "__sad_serial_putint", ft);
            if (!fn)
                return;

            auto savedIP = builder_->saveIP();
            llvm::Type *i16Ty = llvm::Type::getInt16Ty(*context_);

            // Blocks
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            llvm::BasicBlock *isNegBB = llvm::BasicBlock::Create(*context_, "is_neg", fn);
            llvm::BasicBlock *posStart = llvm::BasicBlock::Create(*context_, "pos_start", fn);
            llvm::BasicBlock *revLoop = llvm::BasicBlock::Create(*context_, "rev_loop", fn);
            llvm::BasicBlock *revDone = llvm::BasicBlock::Create(*context_, "rev_done", fn);
            llvm::BasicBlock *outLoop = llvm::BasicBlock::Create(*context_, "out_loop", fn);
            llvm::BasicBlock *outBody = llvm::BasicBlock::Create(*context_, "out_body", fn);
            llvm::BasicBlock *done = llvm::BasicBlock::Create(*context_, "done", fn);

            // Inline asm for outb
            llvm::InlineAsm *outAsm = llvm::InlineAsm::get(
                llvm::FunctionType::get(llvm::Type::getVoidTy(*context_),
                                        {i16Ty, i8Ty}, false),
                "outb %al, %dx", "{dx},{al}", true, false);
            llvm::Value *port = llvm::ConstantInt::get(i16Ty, 0x3F8);

            // Use a 21-byte stack buffer for digits (max 20 digits for int64 + null)
            builder_->SetInsertPoint(entry);
            llvm::Value *val = fn->getArg(0);
            llvm::Value *buf = builder_->CreateAlloca(
                llvm::ArrayType::get(i8Ty, 21), nullptr, "buf");
            llvm::Value *bufPtr = builder_->CreateBitCast(buf, ptrTy, "buf.ptr");

            // Check negative
            llvm::Value *isNeg = builder_->CreateICmpSLT(val, llvm::ConstantInt::get(i64Ty, 0));
            builder_->CreateCondBr(isNeg, isNegBB, posStart);

            // Negative: output '-' and negate
            builder_->SetInsertPoint(isNegBB);
            builder_->CreateCall(outAsm, {port, llvm::ConstantInt::get(i8Ty, '-')});
            llvm::Value *negVal = builder_->CreateNeg(val, "neg");
            builder_->CreateBr(posStart);

            // Start with positive value
            builder_->SetInsertPoint(posStart);
            llvm::PHINode *absVal = builder_->CreatePHI(i64Ty, 2, "abs_val");
            absVal->addIncoming(val, entry);
            absVal->addIncoming(negVal, isNegBB);
            builder_->CreateBr(revLoop);

            // Reverse loop: extract digits into buffer (least significant first)
            builder_->SetInsertPoint(revLoop);
            llvm::PHINode *num = builder_->CreatePHI(i64Ty, 2, "num");
            llvm::PHINode *pos = builder_->CreatePHI(i64Ty, 2, "pos");
            num->addIncoming(absVal, posStart);
            pos->addIncoming(llvm::ConstantInt::get(i64Ty, 0), posStart);

            // digit = num % 10 + '0'
            llvm::Value *digit = builder_->CreateURem(num, llvm::ConstantInt::get(i64Ty, 10));
            llvm::Value *digitCh = builder_->CreateTrunc(
                builder_->CreateAdd(digit, llvm::ConstantInt::get(i64Ty, 48)), i8Ty, "digit_ch");
            llvm::Value *bufSlot = builder_->CreateGEP(i8Ty, bufPtr, pos, "slot");
            builder_->CreateStore(digitCh, bufSlot);

            llvm::Value *nextNum = builder_->CreateUDiv(num, llvm::ConstantInt::get(i64Ty, 10));
            llvm::Value *nextPos = builder_->CreateAdd(pos, llvm::ConstantInt::get(i64Ty, 1));
            num->addIncoming(nextNum, revLoop);
            pos->addIncoming(nextPos, revLoop);

            // Continue if num/10 > 0
            llvm::Value *more = builder_->CreateICmpUGT(nextNum, llvm::ConstantInt::get(i64Ty, 0));
            builder_->CreateCondBr(more, revLoop, revDone);

            // Prepare to output in reverse order
            builder_->SetInsertPoint(revDone);
            // pos now holds the count of digits. Output from pos-1 down to 0
            llvm::Value *startIdx = builder_->CreateSub(nextPos, llvm::ConstantInt::get(i64Ty, 1));
            builder_->CreateBr(outLoop);

            builder_->SetInsertPoint(outLoop);
            llvm::PHINode *outIdx = builder_->CreatePHI(i64Ty, 2, "out_idx");
            outIdx->addIncoming(startIdx, revDone);
            // Check if outIdx >= 0 (since i64 is unsigned, check outIdx < count)
            llvm::Value *gez = builder_->CreateICmpSGE(outIdx, llvm::ConstantInt::get(i64Ty, 0));
            builder_->CreateCondBr(gez, outBody, done);

            builder_->SetInsertPoint(outBody);
            llvm::Value *outSlot = builder_->CreateGEP(i8Ty, bufPtr, outIdx, "out.slot");
            llvm::Value *outCh = builder_->CreateLoad(i8Ty, outSlot, "out.ch");
            builder_->CreateCall(outAsm, {port, outCh});
            llvm::Value *prevIdx = builder_->CreateSub(outIdx, llvm::ConstantInt::get(i64Ty, 1));
            outIdx->addIncoming(prevIdx, outBody);
            builder_->CreateBr(outLoop);

            builder_->SetInsertPoint(done);
            builder_->CreateRetVoid();

            builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 16. __sad_itoa — Convert i64 to decimal string in buffer, return length
        //     Signature: i32 __sad_itoa(ptr buf, i64 val)
        //     Non-variadic replacement for sprintf(buf, "%lld", val) in freestanding
        // ============================================================================
        void LLVMCodeGen::emitFreestandingItoa(
            llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(
                module_.get(), *context_, "__sad_itoa", ft);
            if (!fn)
                return;

            auto savedIP = builder_->saveIP();

            // Block layout:
            //   entry -> isNeg? -> posStart -> digitLoop -> digitDone -> revLoop? -> revDone (ret)
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            llvm::BasicBlock *isNegBB = llvm::BasicBlock::Create(*context_, "is_neg", fn);
            llvm::BasicBlock *posStart = llvm::BasicBlock::Create(*context_, "pos_start", fn);
            llvm::BasicBlock *zeroCase = llvm::BasicBlock::Create(*context_, "zero_case", fn);
            llvm::BasicBlock *nonZero = llvm::BasicBlock::Create(*context_, "non_zero", fn);
            llvm::BasicBlock *digitLoop = llvm::BasicBlock::Create(*context_, "digit_loop", fn);
            llvm::BasicBlock *digitDone = llvm::BasicBlock::Create(*context_, "digit_done", fn);
            llvm::BasicBlock *revLoop = llvm::BasicBlock::Create(*context_, "rev_loop", fn);
            llvm::BasicBlock *revDone = llvm::BasicBlock::Create(*context_, "rev_done", fn);

            builder_->SetInsertPoint(entry);
            llvm::Value *buf = fn->getArg(0);
            llvm::Value *val = fn->getArg(1);

            // (AR) تحقق من القيمة السالبة
            // (EN) Check for negative value
            llvm::Value *isNeg = builder_->CreateICmpSLT(val, llvm::ConstantInt::get(i64Ty, 0));
            builder_->CreateCondBr(isNeg, isNegBB, posStart);

            // Negative: store '-' at buf[0], negate value
            builder_->SetInsertPoint(isNegBB);
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '-'), buf);
            llvm::Value *negVal = builder_->CreateNeg(val, "neg");
            builder_->CreateBr(posStart);

            // Merge absolute value and starting offset (0 or 1)
            builder_->SetInsertPoint(posStart);
            llvm::PHINode *absVal = builder_->CreatePHI(i64Ty, 2, "abs_val");
            absVal->addIncoming(val, entry);
            absVal->addIncoming(negVal, isNegBB);
            llvm::PHINode *startOff = builder_->CreatePHI(i64Ty, 2, "start_off");
            startOff->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            startOff->addIncoming(llvm::ConstantInt::get(i64Ty, 1), isNegBB);

            // (AR) حالة خاصة: القيمة صفر
            // (EN) Special case: value is zero
            llvm::Value *isZero = builder_->CreateICmpEQ(absVal, llvm::ConstantInt::get(i64Ty, 0));
            builder_->CreateCondBr(isZero, zeroCase, nonZero);

            // Zero: write '0' + null, return length
            builder_->SetInsertPoint(zeroCase);
            llvm::Value *zeroSlot = builder_->CreateGEP(i8Ty, buf, startOff, "zero.slot");
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '0'), zeroSlot);
            llvm::Value *zeroNull = builder_->CreateGEP(i8Ty, buf,
                                                        builder_->CreateAdd(startOff, llvm::ConstantInt::get(i64Ty, 1)), "zero.null");
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), zeroNull);
            llvm::Value *zeroLen = builder_->CreateTrunc(
                builder_->CreateAdd(startOff, llvm::ConstantInt::get(i64Ty, 1)), i32Ty, "zero.len");
            builder_->CreateRet(zeroLen);

            // Non-zero: extract digits in reverse order into buf[startOff..]
            builder_->SetInsertPoint(nonZero);
            builder_->CreateBr(digitLoop);

            builder_->SetInsertPoint(digitLoop);
            llvm::PHINode *num = builder_->CreatePHI(i64Ty, 2, "num");
            llvm::PHINode *pos = builder_->CreatePHI(i64Ty, 2, "pos");
            num->addIncoming(absVal, nonZero);
            pos->addIncoming(llvm::ConstantInt::get(i64Ty, 0), nonZero);

            // digit = num % 10 + '0'
            llvm::Value *digit = builder_->CreateURem(num, llvm::ConstantInt::get(i64Ty, 10));
            llvm::Value *digitCh = builder_->CreateTrunc(
                builder_->CreateAdd(digit, llvm::ConstantInt::get(i64Ty, 48)), i8Ty, "digit_ch");

            // Store at buf[startOff + pos]
            llvm::Value *writeIdx = builder_->CreateAdd(startOff, pos, "write_idx");
            llvm::Value *slot = builder_->CreateGEP(i8Ty, buf, writeIdx, "slot");
            builder_->CreateStore(digitCh, slot);

            llvm::Value *nextNum = builder_->CreateUDiv(num, llvm::ConstantInt::get(i64Ty, 10));
            llvm::Value *nextPos = builder_->CreateAdd(pos, llvm::ConstantInt::get(i64Ty, 1));
            num->addIncoming(nextNum, digitLoop);
            pos->addIncoming(nextPos, digitLoop);

            llvm::Value *more = builder_->CreateICmpUGT(nextNum, llvm::ConstantInt::get(i64Ty, 0));
            builder_->CreateCondBr(more, digitLoop, digitDone);

            // (AR) انتهينا من استخراج الأرقام. أنهِ بـ null ثم اعكس ترتيب الأرقام
            // (EN) Done extracting digits. Null-terminate then reverse the digit portion
            builder_->SetInsertPoint(digitDone);
            llvm::Value *totalLen = builder_->CreateAdd(startOff, nextPos, "total_len");
            // Null terminate
            llvm::Value *nullSlot = builder_->CreateGEP(i8Ty, buf, totalLen, "null.slot");
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), nullSlot);

            // Reverse digits buf[startOff .. totalLen-1]
            llvm::Value *lo = startOff;
            llvm::Value *hi = builder_->CreateSub(totalLen, llvm::ConstantInt::get(i64Ty, 1), "hi.init");
            llvm::Value *needRev = builder_->CreateICmpSLT(lo, hi);
            builder_->CreateCondBr(needRev, revLoop, revDone);

            builder_->SetInsertPoint(revLoop);
            llvm::PHINode *loP = builder_->CreatePHI(i64Ty, 2, "lo_p");
            llvm::PHINode *hiP = builder_->CreatePHI(i64Ty, 2, "hi_p");
            loP->addIncoming(lo, digitDone);
            hiP->addIncoming(hi, digitDone);

            // swap buf[lo] and buf[hi]
            llvm::Value *loSlot = builder_->CreateGEP(i8Ty, buf, loP, "lo.slot");
            llvm::Value *hiSlot = builder_->CreateGEP(i8Ty, buf, hiP, "hi.slot");
            llvm::Value *loVal = builder_->CreateLoad(i8Ty, loSlot, "lo.val");
            llvm::Value *hiVal = builder_->CreateLoad(i8Ty, hiSlot, "hi.val");
            builder_->CreateStore(hiVal, loSlot);
            builder_->CreateStore(loVal, hiSlot);

            llvm::Value *newLo = builder_->CreateAdd(loP, llvm::ConstantInt::get(i64Ty, 1));
            llvm::Value *newHi = builder_->CreateSub(hiP, llvm::ConstantInt::get(i64Ty, 1));
            loP->addIncoming(newLo, revLoop);
            hiP->addIncoming(newHi, revLoop);

            llvm::Value *stillNeed = builder_->CreateICmpSLT(newLo, newHi);
            builder_->CreateCondBr(stillNeed, revLoop, revDone);

            // Return total length
            builder_->SetInsertPoint(revDone);
            llvm::PHINode *finalLen = builder_->CreatePHI(i64Ty, 2, "final_len");
            finalLen->addIncoming(totalLen, digitDone);
            finalLen->addIncoming(totalLen, revLoop);
            builder_->CreateRet(builder_->CreateTrunc(finalLen, i32Ty, "ret.len"));

            builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 17. __sad_ftoa — Convert f64 to decimal string in buffer, return length
        //     Signature: i32 __sad_ftoa(ptr buf, f64 val)
        //     Writes: [-]integer_part.fractional_digits
        // ============================================================================
        void LLVMCodeGen::emitFreestandingFtoa(
            llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            auto *dblTy = llvm::Type::getDoubleTy(*context_);
            llvm::FunctionType *ft = llvm::FunctionType::get(i32Ty, {ptrTy, dblTy}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(
                module_.get(), *context_, "__sad_ftoa", ft);
            if (!fn)
                return;

            auto savedIP = builder_->saveIP();

            auto *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            auto *negBB = llvm::BasicBlock::Create(*context_, "neg", fn);
            auto *posBB = llvm::BasicBlock::Create(*context_, "pos", fn);
            auto *writeFrac = llvm::BasicBlock::Create(*context_, "write_frac", fn);
            auto *stripLoop = llvm::BasicBlock::Create(*context_, "strip_loop", fn);
            auto *stripChk = llvm::BasicBlock::Create(*context_, "strip_chk", fn);
            auto *stripDo = llvm::BasicBlock::Create(*context_, "strip_do", fn);
            auto *finalize = llvm::BasicBlock::Create(*context_, "finalize", fn);

            builder_->SetInsertPoint(entry);
            auto *buf = fn->getArg(0);
            auto *val = fn->getArg(1);

            // (AR) مؤشر الإزاحة الحالية في المخزن (alloca لتبسيط التحكم)
            // (EN) Current offset pointer in buffer (alloca for simpler control flow)
            auto *pOff = builder_->CreateAlloca(i64Ty, nullptr, "p.off");
            builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), pOff);

            auto *isNeg = builder_->CreateFCmpOLT(val, llvm::ConstantFP::get(dblTy, 0.0));
            builder_->CreateCondBr(isNeg, negBB, posBB);

            // Negative: store '-', negate
            builder_->SetInsertPoint(negBB);
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '-'),
                                  builder_->CreateGEP(i8Ty, buf, llvm::ConstantInt::get(i64Ty, 0)));
            builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 1), pOff);
            auto *negVal = builder_->CreateFNeg(val, "negval");
            builder_->CreateBr(posBB);

            // Positive path
            builder_->SetInsertPoint(posBB);
            auto *absVal = builder_->CreatePHI(dblTy, 2, "abs_val");
            absVal->addIncoming(val, entry);
            absVal->addIncoming(negVal, negBB);

            // Integer part
            auto *intPart = builder_->CreateFPToSI(absVal, i64Ty, "int_part");
            auto *intPartF = builder_->CreateSIToFP(intPart, dblTy, "int_part_f");

            // Write integer part via __sad_itoa
            auto itoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            auto itoaFn = module_->getOrInsertFunction("__sad_itoa", itoaFT);
            auto *curOff = builder_->CreateLoad(i64Ty, pOff, "cur_off");
            auto *bufOff = builder_->CreateGEP(i8Ty, buf, curOff, "buf.off");
            auto *intLen = builder_->CreateCall(itoaFn, {bufOff, intPart}, "int_len");
            auto *intLen64 = builder_->CreateSExt(intLen, i64Ty, "int_len64");
            auto *newOff = builder_->CreateAdd(curOff, intLen64, "new_off");
            builder_->CreateStore(newOff, pOff);

            // Write '.'
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '.'),
                                  builder_->CreateGEP(i8Ty, buf, newOff));
            auto *dotOff = builder_->CreateAdd(newOff, llvm::ConstantInt::get(i64Ty, 1));
            builder_->CreateStore(dotOff, pOff);
            builder_->CreateBr(writeFrac);

            // Fractional part: 6 digits with leading zeros
            builder_->SetInsertPoint(writeFrac);
            auto *fracF = builder_->CreateFSub(absVal, intPartF, "frac_f");
            auto *fracMul = builder_->CreateFMul(fracF,
                                                 llvm::ConstantFP::get(dblTy, 1000000.0), "frac_mul");
            auto *fracInt = builder_->CreateFPToSI(fracMul, i64Ty, "frac_int");
            // Handle rounding error: abs(fracInt)
            auto *fracNeg = builder_->CreateICmpSLT(fracInt, llvm::ConstantInt::get(i64Ty, 0));
            auto *fracNegVal = builder_->CreateNeg(fracInt, "frac_neg");
            auto *fracAbs = builder_->CreateSelect(fracNeg, fracNegVal, fracInt, "frac_abs");

            auto *digitBase = builder_->CreateLoad(i64Ty, pOff, "digit_base");

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
                auto *digit = builder_->CreateURem(
                    builder_->CreateUDiv(fracAbs, divisors[d]), llvm::ConstantInt::get(i64Ty, 10));
                auto *digitCh = builder_->CreateTrunc(
                    builder_->CreateAdd(digit, llvm::ConstantInt::get(i64Ty, '0')), i8Ty);
                auto *slot = builder_->CreateGEP(i8Ty, buf,
                                                 builder_->CreateAdd(digitBase, llvm::ConstantInt::get(i64Ty, d)));
                builder_->CreateStore(digitCh, slot);
            }

            auto *endOff = builder_->CreateAdd(digitBase, llvm::ConstantInt::get(i64Ty, 6), "end_off");
            builder_->CreateStore(endOff, pOff);

            // (AR) حذف الأصفار الزائدة من نهاية الجزء العشري مع إبقاء رقم واحد على الأقل
            // (EN) Strip trailing zeros from fractional part, keeping at least 1 digit
            auto *minOff = builder_->CreateAdd(digitBase, llvm::ConstantInt::get(i64Ty, 1), "min_off");
            builder_->CreateBr(stripLoop);

            builder_->SetInsertPoint(stripLoop);
            auto *curEnd = builder_->CreateLoad(i64Ty, pOff, "cur_end");
            auto *above = builder_->CreateICmpSGT(curEnd, minOff);
            builder_->CreateCondBr(above, stripChk, finalize);

            builder_->SetInsertPoint(stripChk);
            auto *prevSlot = builder_->CreateGEP(i8Ty, buf,
                                                 builder_->CreateSub(curEnd, llvm::ConstantInt::get(i64Ty, 1)));
            auto *lastCh = builder_->CreateLoad(i8Ty, prevSlot, "last_ch");
            auto *isZero = builder_->CreateICmpEQ(lastCh, llvm::ConstantInt::get(i8Ty, '0'));
            builder_->CreateCondBr(isZero, stripDo, finalize);

            builder_->SetInsertPoint(stripDo);
            builder_->CreateStore(
                builder_->CreateSub(curEnd, llvm::ConstantInt::get(i64Ty, 1)), pOff);
            builder_->CreateBr(stripLoop);

            builder_->SetInsertPoint(finalize);
            auto *finalOff = builder_->CreateLoad(i64Ty, pOff, "final_off");
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0),
                                  builder_->CreateGEP(i8Ty, buf, finalOff));
            builder_->CreateRet(builder_->CreateTrunc(finalOff, i32Ty, "ret.len"));

            builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 18. __sad_xtoa — Convert i64 to hexadecimal string in buffer, return length
        //     Signature: i32 __sad_xtoa(ptr buf, i64 val)
        // ============================================================================
        void LLVMCodeGen::emitFreestandingXtoa(
            llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(
                module_.get(), *context_, "__sad_xtoa", ft);
            if (!fn)
                return;

            auto savedIP = builder_->saveIP();

            auto *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            auto *zeroBB = llvm::BasicBlock::Create(*context_, "zero", fn);
            auto *nonZero = llvm::BasicBlock::Create(*context_, "non_zero", fn);
            auto *hexLoop = llvm::BasicBlock::Create(*context_, "hex_loop", fn);
            auto *hexDone = llvm::BasicBlock::Create(*context_, "hex_done", fn);
            auto *revLoop = llvm::BasicBlock::Create(*context_, "rev_loop", fn);
            auto *revDone = llvm::BasicBlock::Create(*context_, "rev_done", fn);

            builder_->SetInsertPoint(entry);
            auto *buf = fn->getArg(0);
            auto *val = fn->getArg(1);

            auto *isZero = builder_->CreateICmpEQ(val, llvm::ConstantInt::get(i64Ty, 0));
            builder_->CreateCondBr(isZero, zeroBB, nonZero);

            // Zero case
            builder_->SetInsertPoint(zeroBB);
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '0'), buf);
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0),
                                  builder_->CreateGEP(i8Ty, buf, llvm::ConstantInt::get(i64Ty, 1)));
            builder_->CreateRet(llvm::ConstantInt::get(i32Ty, 1));

            // Non-zero: extract hex digits in reverse
            builder_->SetInsertPoint(nonZero);
            builder_->CreateBr(hexLoop);

            builder_->SetInsertPoint(hexLoop);
            auto *num = builder_->CreatePHI(i64Ty, 2, "num");
            auto *pos = builder_->CreatePHI(i64Ty, 2, "pos");
            num->addIncoming(val, nonZero);
            pos->addIncoming(llvm::ConstantInt::get(i64Ty, 0), nonZero);

            // nibble = num & 0xF
            auto *nibble = builder_->CreateAnd(num, llvm::ConstantInt::get(i64Ty, 0xF), "nibble");
            // hexCh = nibble < 10 ? nibble + '0' : nibble - 10 + 'a'
            auto *isDigit = builder_->CreateICmpULT(nibble, llvm::ConstantInt::get(i64Ty, 10));
            auto *chDigit = builder_->CreateAdd(nibble, llvm::ConstantInt::get(i64Ty, '0'));
            auto *chAlpha = builder_->CreateAdd(
                builder_->CreateSub(nibble, llvm::ConstantInt::get(i64Ty, 10)),
                llvm::ConstantInt::get(i64Ty, 'a'));
            auto *hexCh = builder_->CreateSelect(isDigit, chDigit, chAlpha, "hex_ch");
            auto *hexByte = builder_->CreateTrunc(hexCh, i8Ty, "hex_byte");
            builder_->CreateStore(hexByte, builder_->CreateGEP(i8Ty, buf, pos));

            auto *nextNum = builder_->CreateLShr(num, llvm::ConstantInt::get(i64Ty, 4), "next_num");
            auto *nextPos = builder_->CreateAdd(pos, llvm::ConstantInt::get(i64Ty, 1));
            num->addIncoming(nextNum, hexLoop);
            pos->addIncoming(nextPos, hexLoop);

            auto *more = builder_->CreateICmpNE(nextNum, llvm::ConstantInt::get(i64Ty, 0));
            builder_->CreateCondBr(more, hexLoop, hexDone);

            // Null terminate and reverse
            builder_->SetInsertPoint(hexDone);
            // nextPos is the count of digits
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0),
                                  builder_->CreateGEP(i8Ty, buf, nextPos));
            auto *lo = llvm::ConstantInt::get(i64Ty, 0);
            auto *hi = builder_->CreateSub(nextPos, llvm::ConstantInt::get(i64Ty, 1), "hi.init");
            auto *needRev = builder_->CreateICmpSLT(lo, hi);
            builder_->CreateCondBr(needRev, revLoop, revDone);

            builder_->SetInsertPoint(revLoop);
            auto *loP = builder_->CreatePHI(i64Ty, 2, "lo_p");
            auto *hiP = builder_->CreatePHI(i64Ty, 2, "hi_p");
            loP->addIncoming(lo, hexDone);
            hiP->addIncoming(hi, hexDone);
            auto *loSlot = builder_->CreateGEP(i8Ty, buf, loP);
            auto *hiSlot = builder_->CreateGEP(i8Ty, buf, hiP);
            auto *loVal = builder_->CreateLoad(i8Ty, loSlot);
            auto *hiVal = builder_->CreateLoad(i8Ty, hiSlot);
            builder_->CreateStore(hiVal, loSlot);
            builder_->CreateStore(loVal, hiSlot);
            auto *newLo = builder_->CreateAdd(loP, llvm::ConstantInt::get(i64Ty, 1));
            auto *newHi = builder_->CreateSub(hiP, llvm::ConstantInt::get(i64Ty, 1));
            loP->addIncoming(newLo, revLoop);
            hiP->addIncoming(newHi, revLoop);
            auto *stillRev = builder_->CreateICmpSLT(newLo, newHi);
            builder_->CreateCondBr(stillRev, revLoop, revDone);

            builder_->SetInsertPoint(revDone);
            auto *retLen = builder_->CreatePHI(i64Ty, 2, "ret_len");
            retLen->addIncoming(nextPos, hexDone);
            retLen->addIncoming(nextPos, revLoop);
            builder_->CreateRet(builder_->CreateTrunc(retLen, i32Ty));

            builder_->restoreIP(savedIP);
        }

    } // namespace LLVM
} // namespace Sad
