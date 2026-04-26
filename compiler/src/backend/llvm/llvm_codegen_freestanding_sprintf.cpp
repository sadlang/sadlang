// ============================================================================
// llvm_codegen_freestanding_sprintf.cpp — sprintf و pow للوضع المستقل
// (AR) تنسيق نص إلى مخزن + دالة الأُس الرياضية
// (EN) Freestanding sprintf + pow implementations
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

        void LLVMCodeGen::emitFreestandingSprintf(
            llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, true);
            llvm::Function *fn = getOrCreateFreestandingFunc(module_.get(), *context_, "sprintf", ft);
            if (!fn)
                return;

            auto savedIP = builder_->saveIP();
            auto *dblTy = llvm::Type::getDoubleTy(*context_);

            // --- Create all basic blocks ---
            auto *BB_entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            auto *BB_loop = llvm::BasicBlock::Create(*context_, "loop", fn);
            auto *BB_not_null = llvm::BasicBlock::Create(*context_, "not_null", fn);
            auto *BB_normal = llvm::BasicBlock::Create(*context_, "normal", fn);
            auto *BB_pct = llvm::BasicBlock::Create(*context_, "pct", fn);
            auto *BB_chk_l = llvm::BasicBlock::Create(*context_, "chk_l", fn);
            auto *BB_chk_ll = llvm::BasicBlock::Create(*context_, "chk_ll", fn);
            auto *BB_do_lld = llvm::BasicBlock::Create(*context_, "do_lld", fn);
            auto *BB_copy_ll = llvm::BasicBlock::Create(*context_, "copy_ll", fn);
            auto *BB_chk_ld = llvm::BasicBlock::Create(*context_, "chk_ld", fn);
            auto *BB_do_ld = llvm::BasicBlock::Create(*context_, "do_ld", fn);
            auto *BB_copy_l = llvm::BasicBlock::Create(*context_, "copy_l", fn);
            auto *BB_chk_d = llvm::BasicBlock::Create(*context_, "chk_d", fn);
            auto *BB_do_d = llvm::BasicBlock::Create(*context_, "do_d", fn);
            auto *BB_chk_s = llvm::BasicBlock::Create(*context_, "chk_s", fn);
            auto *BB_do_s = llvm::BasicBlock::Create(*context_, "do_s", fn);
            auto *BB_s_loop = llvm::BasicBlock::Create(*context_, "s_loop", fn);
            auto *BB_s_body = llvm::BasicBlock::Create(*context_, "s_body", fn);
            auto *BB_s_done = llvm::BasicBlock::Create(*context_, "s_done", fn);
            auto *BB_chk_gf = llvm::BasicBlock::Create(*context_, "chk_gf", fn);
            auto *BB_do_gf = llvm::BasicBlock::Create(*context_, "do_gf", fn);
            auto *BB_chk_x = llvm::BasicBlock::Create(*context_, "chk_x", fn);
            auto *BB_do_x = llvm::BasicBlock::Create(*context_, "do_x", fn);
            auto *BB_chk_c = llvm::BasicBlock::Create(*context_, "chk_c", fn);
            auto *BB_do_c = llvm::BasicBlock::Create(*context_, "do_c", fn);
            auto *BB_chk_p = llvm::BasicBlock::Create(*context_, "chk_p", fn);
            auto *BB_do_p = llvm::BasicBlock::Create(*context_, "do_p", fn);
            auto *BB_chk_pct2 = llvm::BasicBlock::Create(*context_, "chk_pct2", fn);
            auto *BB_do_pct2 = llvm::BasicBlock::Create(*context_, "do_pct2", fn);
            auto *BB_default = llvm::BasicBlock::Create(*context_, "do_default", fn);
            auto *BB_done = llvm::BasicBlock::Create(*context_, "done", fn);

            // --- Get helper function references ---
            auto itoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            auto ftoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, dblTy}, false);
            auto xtoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            auto itoaFn = module_->getOrInsertFunction("__sad_itoa", itoaFT);
            auto ftoaFn = module_->getOrInsertFunction("__sad_ftoa", ftoaFT);
            auto xtoaFn = module_->getOrInsertFunction("__sad_xtoa", xtoaFT);

            // === Entry: init state vars, va_start ===
            builder_->SetInsertPoint(BB_entry);
            auto *buf = fn->getArg(0);
            auto *fmt = fn->getArg(1);

            auto *pI = builder_->CreateAlloca(i64Ty, nullptr, "p.i");
            auto *pJ = builder_->CreateAlloca(i64Ty, nullptr, "p.j");
            builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), pI);
            builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), pJ);

            auto vaTagTy = llvm::StructType::get(*context_, {i32Ty, i32Ty, ptrTy, ptrTy});
            auto *vaListTy = llvm::ArrayType::get(vaTagTy, 1);
            auto *vaList = builder_->CreateAlloca(vaListTy, nullptr, "va_list");
            auto vaStartDecl = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::vastart);
            auto vaEndDecl = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::vaend);
            builder_->CreateCall(vaStartDecl, {vaList});
            builder_->CreateBr(BB_loop);

            // Helper lambdas
            auto loadI = [&]()
            { return builder_->CreateLoad(i64Ty, pI, "i"); };
            auto loadJ = [&]()
            { return builder_->CreateLoad(i64Ty, pJ, "j"); };
            auto storeI = [&](llvm::Value *v)
            { builder_->CreateStore(v, pI); };
            auto storeJ = [&](llvm::Value *v)
            { builder_->CreateStore(v, pJ); };
            auto loadCh = [&](llvm::Value *base, llvm::Value *idx) -> llvm::Value *
            {
                return builder_->CreateLoad(i8Ty, builder_->CreateGEP(i8Ty, base, idx));
            };
            auto storeCh = [&](llvm::Value *base, llvm::Value *idx, llvm::Value *val)
            {
                builder_->CreateStore(val, builder_->CreateGEP(i8Ty, base, idx));
            };
            auto C64 = [&](int64_t v)
            { return llvm::ConstantInt::get(i64Ty, v); };
            auto C8 = [&](int8_t v)
            { return llvm::ConstantInt::get(i8Ty, (uint8_t)v); };

            // === Main Loop ===
            builder_->SetInsertPoint(BB_loop);
            auto *curI = loadI();
            auto *ch = loadCh(fmt, curI);
            auto *isNull = builder_->CreateICmpEQ(ch, C8(0));
            builder_->CreateCondBr(isNull, BB_done, BB_not_null);

            builder_->SetInsertPoint(BB_not_null);
            auto *isPct = builder_->CreateICmpEQ(ch, C8('%'));
            builder_->CreateCondBr(isPct, BB_pct, BB_normal);

            // === Normal char ===
            builder_->SetInsertPoint(BB_normal);
            storeCh(buf, loadJ(), ch);
            storeJ(builder_->CreateAdd(loadJ(), C64(1)));
            storeI(builder_->CreateAdd(curI, C64(1)));
            builder_->CreateBr(BB_loop);

            // === Handle '%' ===
            builder_->SetInsertPoint(BB_pct);
            auto *iPlusOne = builder_->CreateAdd(curI, C64(1));
            storeI(iPlusOne);
            auto *spec = loadCh(fmt, iPlusOne);
            auto *isL = builder_->CreateICmpEQ(spec, C8('l'));
            builder_->CreateCondBr(isL, BB_chk_l, BB_chk_d);

            // === %l... ===
            builder_->SetInsertPoint(BB_chk_l);
            auto *iPlusTwo = builder_->CreateAdd(iPlusOne, C64(1));
            storeI(iPlusTwo);
            auto *spec2 = loadCh(fmt, iPlusTwo);
            auto *isLL = builder_->CreateICmpEQ(spec2, C8('l'));
            builder_->CreateCondBr(isLL, BB_chk_ll, BB_chk_ld);

            // === %ll... ===
            builder_->SetInsertPoint(BB_chk_ll);
            auto *iPlusThree = builder_->CreateAdd(iPlusTwo, C64(1));
            storeI(iPlusThree);
            auto *spec3 = loadCh(fmt, iPlusThree);
            auto *isD3 = builder_->CreateICmpEQ(spec3, C8('d'));
            auto *isI3 = builder_->CreateICmpEQ(spec3, C8('i'));
            auto *isLLD = builder_->CreateOr(isD3, isI3);
            builder_->CreateCondBr(isLLD, BB_do_lld, BB_copy_ll);

            // === do %lld ===
            builder_->SetInsertPoint(BB_do_lld);
            auto *lldVal = builder_->CreateVAArg(vaList, i64Ty, "va.lld");
            auto *lldBuf = builder_->CreateGEP(i8Ty, buf, loadJ(), "buf.j");
            auto *lldLen = builder_->CreateCall(itoaFn, {lldBuf, lldVal}, "len");
            storeJ(builder_->CreateAdd(loadJ(), builder_->CreateSExt(lldLen, i64Ty)));
            storeI(builder_->CreateAdd(iPlusThree, C64(1)));
            builder_->CreateBr(BB_loop);

            // === copy %ll ===
            builder_->SetInsertPoint(BB_copy_ll);
            auto *llJ0 = loadJ();
            storeCh(buf, llJ0, C8('%'));
            storeCh(buf, builder_->CreateAdd(llJ0, C64(1)), C8('l'));
            storeCh(buf, builder_->CreateAdd(llJ0, C64(2)), C8('l'));
            storeJ(builder_->CreateAdd(llJ0, C64(3)));
            builder_->CreateBr(BB_loop);

            // === %ld ===
            builder_->SetInsertPoint(BB_chk_ld);
            auto *isD2 = builder_->CreateICmpEQ(spec2, C8('d'));
            auto *isI2 = builder_->CreateICmpEQ(spec2, C8('i'));
            builder_->CreateCondBr(builder_->CreateOr(isD2, isI2), BB_do_ld, BB_copy_l);

            builder_->SetInsertPoint(BB_do_ld);
            auto *ldVal = builder_->CreateVAArg(vaList, i64Ty, "va.ld");
            auto *ldBuf = builder_->CreateGEP(i8Ty, buf, loadJ(), "buf.j");
            auto *ldLen = builder_->CreateCall(itoaFn, {ldBuf, ldVal}, "len");
            storeJ(builder_->CreateAdd(loadJ(), builder_->CreateSExt(ldLen, i64Ty)));
            storeI(builder_->CreateAdd(iPlusTwo, C64(1)));
            builder_->CreateBr(BB_loop);

            // === copy %l ===
            builder_->SetInsertPoint(BB_copy_l);
            auto *lJ0 = loadJ();
            storeCh(buf, lJ0, C8('%'));
            storeCh(buf, builder_->CreateAdd(lJ0, C64(1)), C8('l'));
            storeJ(builder_->CreateAdd(lJ0, C64(2)));
            builder_->CreateBr(BB_loop);

            // === %d / %i ===
            builder_->SetInsertPoint(BB_chk_d);
            auto *isDSpec = builder_->CreateICmpEQ(spec, C8('d'));
            auto *isISpec = builder_->CreateICmpEQ(spec, C8('i'));
            builder_->CreateCondBr(builder_->CreateOr(isDSpec, isISpec), BB_do_d, BB_chk_s);

            builder_->SetInsertPoint(BB_do_d);
            auto *dVal = builder_->CreateVAArg(vaList, i64Ty, "va.d");
            auto *dBuf = builder_->CreateGEP(i8Ty, buf, loadJ(), "buf.j");
            auto *dLen = builder_->CreateCall(itoaFn, {dBuf, dVal}, "len");
            storeJ(builder_->CreateAdd(loadJ(), builder_->CreateSExt(dLen, i64Ty)));
            storeI(builder_->CreateAdd(iPlusOne, C64(1)));
            builder_->CreateBr(BB_loop);

            // === %s ===
            builder_->SetInsertPoint(BB_chk_s);
            auto *isS = builder_->CreateICmpEQ(spec, C8('s'));
            builder_->CreateCondBr(isS, BB_do_s, BB_chk_gf);

            // %s: va_arg(ptr), copy string char by char
            builder_->SetInsertPoint(BB_do_s);
            auto *strVal = builder_->CreateVAArg(vaList, ptrTy, "va.s");
            auto *isNullStr = builder_->CreateICmpEQ(strVal,
                                                     llvm::ConstantPointerNull::get(llvm::PointerType::get(*context_, 0)));
            builder_->CreateCondBr(isNullStr, BB_s_done, BB_s_loop);

            // String copy loop (strVal from BB_do_s dominates here)
            builder_->SetInsertPoint(BB_s_loop);
            auto *sK = builder_->CreatePHI(i64Ty, 2, "s.k");
            sK->addIncoming(C64(0), BB_do_s);
            auto *sCh = loadCh(strVal, sK);
            auto *sEnd = builder_->CreateICmpEQ(sCh, C8(0));
            builder_->CreateCondBr(sEnd, BB_s_done, BB_s_body);

            builder_->SetInsertPoint(BB_s_body);
            storeCh(buf, loadJ(), sCh);
            storeJ(builder_->CreateAdd(loadJ(), C64(1)));
            auto *nextK = builder_->CreateAdd(sK, C64(1));
            sK->addIncoming(nextK, BB_s_body);
            builder_->CreateBr(BB_s_loop);

            builder_->SetInsertPoint(BB_s_done);
            storeI(builder_->CreateAdd(iPlusOne, C64(1)));
            builder_->CreateBr(BB_loop);

            // === %g / %f / %e ===
            builder_->SetInsertPoint(BB_chk_gf);
            auto *isG = builder_->CreateICmpEQ(spec, C8('g'));
            auto *isF = builder_->CreateICmpEQ(spec, C8('f'));
            auto *isE = builder_->CreateICmpEQ(spec, C8('e'));
            builder_->CreateCondBr(builder_->CreateOr(builder_->CreateOr(isG, isF), isE),
                                   BB_do_gf, BB_chk_x);

            builder_->SetInsertPoint(BB_do_gf);
            auto *fVal = builder_->CreateVAArg(vaList, dblTy, "va.gf");
            auto *fBuf = builder_->CreateGEP(i8Ty, buf, loadJ(), "buf.j");
            auto *fLen = builder_->CreateCall(ftoaFn, {fBuf, fVal}, "len");
            storeJ(builder_->CreateAdd(loadJ(), builder_->CreateSExt(fLen, i64Ty)));
            storeI(builder_->CreateAdd(iPlusOne, C64(1)));
            builder_->CreateBr(BB_loop);

            // === %x / %X ===
            builder_->SetInsertPoint(BB_chk_x);
            auto *isXL = builder_->CreateICmpEQ(spec, C8('x'));
            auto *isXU = builder_->CreateICmpEQ(spec, C8('X'));
            builder_->CreateCondBr(builder_->CreateOr(isXL, isXU), BB_do_x, BB_chk_c);

            builder_->SetInsertPoint(BB_do_x);
            auto *xVal = builder_->CreateVAArg(vaList, i64Ty, "va.x");
            auto *xBuf = builder_->CreateGEP(i8Ty, buf, loadJ(), "buf.j");
            auto *xLen = builder_->CreateCall(xtoaFn, {xBuf, xVal}, "len");
            storeJ(builder_->CreateAdd(loadJ(), builder_->CreateSExt(xLen, i64Ty)));
            storeI(builder_->CreateAdd(iPlusOne, C64(1)));
            builder_->CreateBr(BB_loop);

            // === %c ===
            builder_->SetInsertPoint(BB_chk_c);
            auto *isC = builder_->CreateICmpEQ(spec, C8('c'));
            builder_->CreateCondBr(isC, BB_do_c, BB_chk_p);

            builder_->SetInsertPoint(BB_do_c);
            auto *cVal = builder_->CreateVAArg(vaList, i64Ty, "va.c");
            storeCh(buf, loadJ(), builder_->CreateTrunc(cVal, i8Ty));
            storeJ(builder_->CreateAdd(loadJ(), C64(1)));
            storeI(builder_->CreateAdd(iPlusOne, C64(1)));
            builder_->CreateBr(BB_loop);

            // === %p ===
            builder_->SetInsertPoint(BB_chk_p);
            auto *isP = builder_->CreateICmpEQ(spec, C8('p'));
            builder_->CreateCondBr(isP, BB_do_p, BB_chk_pct2);

            builder_->SetInsertPoint(BB_do_p);
            auto *pVal = builder_->CreateVAArg(vaList, ptrTy, "va.p");
            auto *pInt = builder_->CreatePtrToInt(pVal, i64Ty, "ptr.int");
            auto *pJ0 = loadJ();
            storeCh(buf, pJ0, C8('0'));
            storeCh(buf, builder_->CreateAdd(pJ0, C64(1)), C8('x'));
            auto *pJ2 = builder_->CreateAdd(pJ0, C64(2));
            storeJ(pJ2);
            auto *pBuf = builder_->CreateGEP(i8Ty, buf, pJ2, "buf.jp");
            auto *pLen = builder_->CreateCall(xtoaFn, {pBuf, pInt}, "len");
            storeJ(builder_->CreateAdd(pJ2, builder_->CreateSExt(pLen, i64Ty)));
            storeI(builder_->CreateAdd(iPlusOne, C64(1)));
            builder_->CreateBr(BB_loop);

            // === %% ===
            builder_->SetInsertPoint(BB_chk_pct2);
            auto *isPct2 = builder_->CreateICmpEQ(spec, C8('%'));
            builder_->CreateCondBr(isPct2, BB_do_pct2, BB_default);

            builder_->SetInsertPoint(BB_do_pct2);
            storeCh(buf, loadJ(), C8('%'));
            storeJ(builder_->CreateAdd(loadJ(), C64(1)));
            storeI(builder_->CreateAdd(iPlusOne, C64(1)));
            builder_->CreateBr(BB_loop);

            // === Default: copy %<spec> literally ===
            builder_->SetInsertPoint(BB_default);
            auto *defJ = loadJ();
            storeCh(buf, defJ, C8('%'));
            storeCh(buf, builder_->CreateAdd(defJ, C64(1)), spec);
            storeJ(builder_->CreateAdd(defJ, C64(2)));
            storeI(builder_->CreateAdd(iPlusOne, C64(1)));
            builder_->CreateBr(BB_loop);

            // === Done: null terminate, va_end, return length ===
            builder_->SetInsertPoint(BB_done);
            auto *finalJ = loadJ();
            storeCh(buf, finalJ, C8(0));
            builder_->CreateCall(vaEndDecl, {vaList});
            builder_->CreateRet(builder_->CreateTrunc(finalJ, i32Ty, "ret.len"));

            builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 13. pow — Simple double power function
        //     For integer exponents: repeated multiplication
        //     For negative exponents: 1.0 / pow(base, -exp)
        // ============================================================================
        void LLVMCodeGen::emitFreestandingPow(llvm::Type *dblTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(dblTy, {dblTy, dblTy}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(module_.get(), *context_, "pow", ft);
            if (!fn)
                return;

            auto savedIP = builder_->saveIP();
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*context_);

            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            llvm::BasicBlock *loop = llvm::BasicBlock::Create(*context_, "loop", fn);
            llvm::BasicBlock *done = llvm::BasicBlock::Create(*context_, "done", fn);

            builder_->SetInsertPoint(entry);
            llvm::Value *base = fn->getArg(0);
            llvm::Value *exp = fn->getArg(1);

            // (AR) تحويل الأس إلى عدد صحيح (تقريب)
            // (EN) Convert exponent to integer (truncation)
            llvm::Value *expI = builder_->CreateFPToSI(exp, i64Ty, "exp.i");
            llvm::Value *isZero = builder_->CreateICmpEQ(expI, llvm::ConstantInt::get(i64Ty, 0));
            builder_->CreateCondBr(isZero, done, loop);

            builder_->SetInsertPoint(loop);
            llvm::PHINode *i = builder_->CreatePHI(i64Ty, 2, "i");
            llvm::PHINode *result = builder_->CreatePHI(dblTy, 2, "result");
            i->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            result->addIncoming(llvm::ConstantFP::get(dblTy, 1.0), entry);

            llvm::Value *newResult = builder_->CreateFMul(result, base, "mul");
            llvm::Value *nextI = builder_->CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1));
            i->addIncoming(nextI, loop);
            result->addIncoming(newResult, loop);

            // (AR) مقارنة مع القيمة المطلقة للأس
            // (EN) Compare with absolute value of exponent
            llvm::Value *absExp = builder_->CreateSelect(
                builder_->CreateICmpSLT(expI, llvm::ConstantInt::get(i64Ty, 0)),
                builder_->CreateNeg(expI),
                expI, "abs.exp");
            llvm::Value *cond = builder_->CreateICmpULT(nextI, absExp);
            builder_->CreateCondBr(cond, loop, done);

            builder_->SetInsertPoint(done);
            llvm::PHINode *finalResult = builder_->CreatePHI(dblTy, 2, "final");
            finalResult->addIncoming(llvm::ConstantFP::get(dblTy, 1.0), entry);
            finalResult->addIncoming(newResult, loop);

            // (AR) إذا كان الأس سالباً، أرجع 1/النتيجة
            // (EN) If exponent is negative, return 1/result
            llvm::Value *isNeg = builder_->CreateICmpSLT(expI, llvm::ConstantInt::get(i64Ty, 0));
            llvm::Value *inv = builder_->CreateFDiv(llvm::ConstantFP::get(dblTy, 1.0), finalResult, "inv");
            llvm::Value *ret = builder_->CreateSelect(isNeg, inv, finalResult, "pow.ret");
            builder_->CreateRet(ret);

            builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 14. __sad_serial_puts — Output a null-terminated string to serial port 0x3F8
        // ============================================================================
    } // namespace LLVM
} // namespace Sad
