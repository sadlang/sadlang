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
#include "builders/core/freestanding_codegen.h"
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

        void FreestandingCodeGen::emitFreestandingSprintf(
            llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, true);
            llvm::Function *fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "sprintf", ft);
            if (!fn)
                return;

            auto savedIP = cg_.builder_->saveIP();
            auto *dblTy = llvm::Type::getDoubleTy(*cg_.context_);

            // --- Create all basic blocks ---
            auto *BB_entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            auto *BB_loop = llvm::BasicBlock::Create(*cg_.context_, "loop", fn);
            auto *BB_not_null = llvm::BasicBlock::Create(*cg_.context_, "not_null", fn);
            auto *BB_normal = llvm::BasicBlock::Create(*cg_.context_, "normal", fn);
            auto *BB_pct = llvm::BasicBlock::Create(*cg_.context_, "pct", fn);
            auto *BB_chk_l = llvm::BasicBlock::Create(*cg_.context_, "chk_l", fn);
            auto *BB_chk_ll = llvm::BasicBlock::Create(*cg_.context_, "chk_ll", fn);
            auto *BB_do_lld = llvm::BasicBlock::Create(*cg_.context_, "do_lld", fn);
            auto *BB_copy_ll = llvm::BasicBlock::Create(*cg_.context_, "copy_ll", fn);
            auto *BB_chk_ld = llvm::BasicBlock::Create(*cg_.context_, "chk_ld", fn);
            auto *BB_do_ld = llvm::BasicBlock::Create(*cg_.context_, "do_ld", fn);
            auto *BB_copy_l = llvm::BasicBlock::Create(*cg_.context_, "copy_l", fn);
            auto *BB_chk_d = llvm::BasicBlock::Create(*cg_.context_, "chk_d", fn);
            auto *BB_do_d = llvm::BasicBlock::Create(*cg_.context_, "do_d", fn);
            auto *BB_chk_s = llvm::BasicBlock::Create(*cg_.context_, "chk_s", fn);
            auto *BB_do_s = llvm::BasicBlock::Create(*cg_.context_, "do_s", fn);
            auto *BB_s_loop = llvm::BasicBlock::Create(*cg_.context_, "s_loop", fn);
            auto *BB_s_body = llvm::BasicBlock::Create(*cg_.context_, "s_body", fn);
            auto *BB_s_done = llvm::BasicBlock::Create(*cg_.context_, "s_done", fn);
            auto *BB_chk_gf = llvm::BasicBlock::Create(*cg_.context_, "chk_gf", fn);
            auto *BB_do_gf = llvm::BasicBlock::Create(*cg_.context_, "do_gf", fn);
            auto *BB_chk_x = llvm::BasicBlock::Create(*cg_.context_, "chk_x", fn);
            auto *BB_do_x = llvm::BasicBlock::Create(*cg_.context_, "do_x", fn);
            auto *BB_chk_c = llvm::BasicBlock::Create(*cg_.context_, "chk_c", fn);
            auto *BB_do_c = llvm::BasicBlock::Create(*cg_.context_, "do_c", fn);
            auto *BB_chk_p = llvm::BasicBlock::Create(*cg_.context_, "chk_p", fn);
            auto *BB_do_p = llvm::BasicBlock::Create(*cg_.context_, "do_p", fn);
            auto *BB_chk_pct2 = llvm::BasicBlock::Create(*cg_.context_, "chk_pct2", fn);
            auto *BB_do_pct2 = llvm::BasicBlock::Create(*cg_.context_, "do_pct2", fn);
            auto *BB_default = llvm::BasicBlock::Create(*cg_.context_, "do_default", fn);
            auto *BB_done = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

            // --- Get helper function references ---
            auto itoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            auto ftoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, dblTy}, false);
            auto xtoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            auto itoaFn = cg_.module_->getOrInsertFunction("__sad_itoa", itoaFT);
            auto ftoaFn = cg_.module_->getOrInsertFunction("__sad_ftoa", ftoaFT);
            auto xtoaFn = cg_.module_->getOrInsertFunction("__sad_xtoa", xtoaFT);

            // === Entry: init state vars, va_start ===
            cg_.builder_->SetInsertPoint(BB_entry);
            auto *buf = fn->getArg(0);
            auto *fmt = fn->getArg(1);

            auto *pI = cg_.builder_->CreateAlloca(i64Ty, nullptr, "p.i");
            auto *pJ = cg_.builder_->CreateAlloca(i64Ty, nullptr, "p.j");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), pI);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), pJ);

            auto vaTagTy = llvm::StructType::get(*cg_.context_, {i32Ty, i32Ty, ptrTy, ptrTy});
            auto *vaListTy = llvm::ArrayType::get(vaTagTy, 1);
            auto *vaList = cg_.builder_->CreateAlloca(vaListTy, nullptr, "va_list");
            auto vaStartDecl = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::vastart);
            auto vaEndDecl = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::vaend);
            cg_.builder_->CreateCall(vaStartDecl, {vaList});
            cg_.builder_->CreateBr(BB_loop);

            // Helper lambdas
            auto loadI = [&]()
            { return cg_.builder_->CreateLoad(i64Ty, pI, "i"); };
            auto loadJ = [&]()
            { return cg_.builder_->CreateLoad(i64Ty, pJ, "j"); };
            auto storeI = [&](llvm::Value *v)
            { cg_.builder_->CreateStore(v, pI); };
            auto storeJ = [&](llvm::Value *v)
            { cg_.builder_->CreateStore(v, pJ); };
            auto loadCh = [&](llvm::Value *base, llvm::Value *idx) -> llvm::Value *
            {
                return cg_.builder_->CreateLoad(i8Ty, cg_.builder_->CreateGEP(i8Ty, base, idx));
            };
            auto storeCh = [&](llvm::Value *base, llvm::Value *idx, llvm::Value *val)
            {
                cg_.builder_->CreateStore(val, cg_.builder_->CreateGEP(i8Ty, base, idx));
            };
            auto C64 = [&](int64_t v)
            { return llvm::ConstantInt::get(i64Ty, v); };
            auto C8 = [&](int8_t v)
            { return llvm::ConstantInt::get(i8Ty, (uint8_t)v); };

            // === Main Loop ===
            cg_.builder_->SetInsertPoint(BB_loop);
            auto *curI = loadI();
            auto *ch = loadCh(fmt, curI);
            auto *isNull = cg_.builder_->CreateICmpEQ(ch, C8(0));
            cg_.builder_->CreateCondBr(isNull, BB_done, BB_not_null);

            cg_.builder_->SetInsertPoint(BB_not_null);
            auto *isPct = cg_.builder_->CreateICmpEQ(ch, C8('%'));
            cg_.builder_->CreateCondBr(isPct, BB_pct, BB_normal);

            // === Normal char ===
            cg_.builder_->SetInsertPoint(BB_normal);
            storeCh(buf, loadJ(), ch);
            storeJ(cg_.builder_->CreateAdd(loadJ(), C64(1)));
            storeI(cg_.builder_->CreateAdd(curI, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === Handle '%' ===
            cg_.builder_->SetInsertPoint(BB_pct);
            auto *iPlusOne = cg_.builder_->CreateAdd(curI, C64(1));
            storeI(iPlusOne);
            auto *spec = loadCh(fmt, iPlusOne);
            auto *isL = cg_.builder_->CreateICmpEQ(spec, C8('l'));
            cg_.builder_->CreateCondBr(isL, BB_chk_l, BB_chk_d);

            // === %l... ===
            cg_.builder_->SetInsertPoint(BB_chk_l);
            auto *iPlusTwo = cg_.builder_->CreateAdd(iPlusOne, C64(1));
            storeI(iPlusTwo);
            auto *spec2 = loadCh(fmt, iPlusTwo);
            auto *isLL = cg_.builder_->CreateICmpEQ(spec2, C8('l'));
            cg_.builder_->CreateCondBr(isLL, BB_chk_ll, BB_chk_ld);

            // === %ll... ===
            cg_.builder_->SetInsertPoint(BB_chk_ll);
            auto *iPlusThree = cg_.builder_->CreateAdd(iPlusTwo, C64(1));
            storeI(iPlusThree);
            auto *spec3 = loadCh(fmt, iPlusThree);
            auto *isD3 = cg_.builder_->CreateICmpEQ(spec3, C8('d'));
            auto *isI3 = cg_.builder_->CreateICmpEQ(spec3, C8('i'));
            auto *isLLD = cg_.builder_->CreateOr(isD3, isI3);
            cg_.builder_->CreateCondBr(isLLD, BB_do_lld, BB_copy_ll);

            // === do %lld ===
            cg_.builder_->SetInsertPoint(BB_do_lld);
            auto *lldVal = cg_.builder_->CreateVAArg(vaList, i64Ty, "va.lld");
            auto *lldBuf = cg_.builder_->CreateGEP(i8Ty, buf, loadJ(), "buf.j");
            auto *lldLen = cg_.builder_->CreateCall(itoaFn, {lldBuf, lldVal}, "len");
            storeJ(cg_.builder_->CreateAdd(loadJ(), cg_.builder_->CreateSExt(lldLen, i64Ty)));
            storeI(cg_.builder_->CreateAdd(iPlusThree, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === copy %ll ===
            cg_.builder_->SetInsertPoint(BB_copy_ll);
            auto *llJ0 = loadJ();
            storeCh(buf, llJ0, C8('%'));
            storeCh(buf, cg_.builder_->CreateAdd(llJ0, C64(1)), C8('l'));
            storeCh(buf, cg_.builder_->CreateAdd(llJ0, C64(2)), C8('l'));
            storeJ(cg_.builder_->CreateAdd(llJ0, C64(3)));
            cg_.builder_->CreateBr(BB_loop);

            // === %ld ===
            cg_.builder_->SetInsertPoint(BB_chk_ld);
            auto *isD2 = cg_.builder_->CreateICmpEQ(spec2, C8('d'));
            auto *isI2 = cg_.builder_->CreateICmpEQ(spec2, C8('i'));
            cg_.builder_->CreateCondBr(cg_.builder_->CreateOr(isD2, isI2), BB_do_ld, BB_copy_l);

            cg_.builder_->SetInsertPoint(BB_do_ld);
            auto *ldVal = cg_.builder_->CreateVAArg(vaList, i64Ty, "va.ld");
            auto *ldBuf = cg_.builder_->CreateGEP(i8Ty, buf, loadJ(), "buf.j");
            auto *ldLen = cg_.builder_->CreateCall(itoaFn, {ldBuf, ldVal}, "len");
            storeJ(cg_.builder_->CreateAdd(loadJ(), cg_.builder_->CreateSExt(ldLen, i64Ty)));
            storeI(cg_.builder_->CreateAdd(iPlusTwo, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === copy %l ===
            cg_.builder_->SetInsertPoint(BB_copy_l);
            auto *lJ0 = loadJ();
            storeCh(buf, lJ0, C8('%'));
            storeCh(buf, cg_.builder_->CreateAdd(lJ0, C64(1)), C8('l'));
            storeJ(cg_.builder_->CreateAdd(lJ0, C64(2)));
            cg_.builder_->CreateBr(BB_loop);

            // === %d / %i ===
            cg_.builder_->SetInsertPoint(BB_chk_d);
            auto *isDSpec = cg_.builder_->CreateICmpEQ(spec, C8('d'));
            auto *isISpec = cg_.builder_->CreateICmpEQ(spec, C8('i'));
            cg_.builder_->CreateCondBr(cg_.builder_->CreateOr(isDSpec, isISpec), BB_do_d, BB_chk_s);

            cg_.builder_->SetInsertPoint(BB_do_d);
            auto *dVal = cg_.builder_->CreateVAArg(vaList, i64Ty, "va.d");
            auto *dBuf = cg_.builder_->CreateGEP(i8Ty, buf, loadJ(), "buf.j");
            auto *dLen = cg_.builder_->CreateCall(itoaFn, {dBuf, dVal}, "len");
            storeJ(cg_.builder_->CreateAdd(loadJ(), cg_.builder_->CreateSExt(dLen, i64Ty)));
            storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === %s ===
            cg_.builder_->SetInsertPoint(BB_chk_s);
            auto *isS = cg_.builder_->CreateICmpEQ(spec, C8('s'));
            cg_.builder_->CreateCondBr(isS, BB_do_s, BB_chk_gf);

            // %s: va_arg(ptr), copy string char by char
            cg_.builder_->SetInsertPoint(BB_do_s);
            auto *strVal = cg_.builder_->CreateVAArg(vaList, ptrTy, "va.s");
            auto *isNullStr = cg_.builder_->CreateICmpEQ(strVal,
                                                     llvm::ConstantPointerNull::get(llvm::PointerType::get(*cg_.context_, 0)));
            cg_.builder_->CreateCondBr(isNullStr, BB_s_done, BB_s_loop);

            // String copy loop (strVal from BB_do_s dominates here)
            cg_.builder_->SetInsertPoint(BB_s_loop);
            auto *sK = cg_.builder_->CreatePHI(i64Ty, 2, "s.k");
            sK->addIncoming(C64(0), BB_do_s);
            auto *sCh = loadCh(strVal, sK);
            auto *sEnd = cg_.builder_->CreateICmpEQ(sCh, C8(0));
            cg_.builder_->CreateCondBr(sEnd, BB_s_done, BB_s_body);

            cg_.builder_->SetInsertPoint(BB_s_body);
            storeCh(buf, loadJ(), sCh);
            storeJ(cg_.builder_->CreateAdd(loadJ(), C64(1)));
            auto *nextK = cg_.builder_->CreateAdd(sK, C64(1));
            sK->addIncoming(nextK, BB_s_body);
            cg_.builder_->CreateBr(BB_s_loop);

            cg_.builder_->SetInsertPoint(BB_s_done);
            storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === %g / %f / %e ===
            cg_.builder_->SetInsertPoint(BB_chk_gf);
            auto *isG = cg_.builder_->CreateICmpEQ(spec, C8('g'));
            auto *isF = cg_.builder_->CreateICmpEQ(spec, C8('f'));
            auto *isE = cg_.builder_->CreateICmpEQ(spec, C8('e'));
            cg_.builder_->CreateCondBr(cg_.builder_->CreateOr(cg_.builder_->CreateOr(isG, isF), isE),
                                   BB_do_gf, BB_chk_x);

            cg_.builder_->SetInsertPoint(BB_do_gf);
            auto *fVal = cg_.builder_->CreateVAArg(vaList, dblTy, "va.gf");
            auto *fBuf = cg_.builder_->CreateGEP(i8Ty, buf, loadJ(), "buf.j");
            auto *fLen = cg_.builder_->CreateCall(ftoaFn, {fBuf, fVal}, "len");
            storeJ(cg_.builder_->CreateAdd(loadJ(), cg_.builder_->CreateSExt(fLen, i64Ty)));
            storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === %x / %X ===
            cg_.builder_->SetInsertPoint(BB_chk_x);
            auto *isXL = cg_.builder_->CreateICmpEQ(spec, C8('x'));
            auto *isXU = cg_.builder_->CreateICmpEQ(spec, C8('X'));
            cg_.builder_->CreateCondBr(cg_.builder_->CreateOr(isXL, isXU), BB_do_x, BB_chk_c);

            cg_.builder_->SetInsertPoint(BB_do_x);
            auto *xVal = cg_.builder_->CreateVAArg(vaList, i64Ty, "va.x");
            auto *xBuf = cg_.builder_->CreateGEP(i8Ty, buf, loadJ(), "buf.j");
            auto *xLen = cg_.builder_->CreateCall(xtoaFn, {xBuf, xVal}, "len");
            storeJ(cg_.builder_->CreateAdd(loadJ(), cg_.builder_->CreateSExt(xLen, i64Ty)));
            storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === %c ===
            cg_.builder_->SetInsertPoint(BB_chk_c);
            auto *isC = cg_.builder_->CreateICmpEQ(spec, C8('c'));
            cg_.builder_->CreateCondBr(isC, BB_do_c, BB_chk_p);

            cg_.builder_->SetInsertPoint(BB_do_c);
            auto *cVal = cg_.builder_->CreateVAArg(vaList, i64Ty, "va.c");
            storeCh(buf, loadJ(), cg_.builder_->CreateTrunc(cVal, i8Ty));
            storeJ(cg_.builder_->CreateAdd(loadJ(), C64(1)));
            storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === %p ===
            cg_.builder_->SetInsertPoint(BB_chk_p);
            auto *isP = cg_.builder_->CreateICmpEQ(spec, C8('p'));
            cg_.builder_->CreateCondBr(isP, BB_do_p, BB_chk_pct2);

            cg_.builder_->SetInsertPoint(BB_do_p);
            auto *pVal = cg_.builder_->CreateVAArg(vaList, ptrTy, "va.p");
            auto *pInt = cg_.builder_->CreatePtrToInt(pVal, i64Ty, "ptr.int");
            auto *pJ0 = loadJ();
            storeCh(buf, pJ0, C8('0'));
            storeCh(buf, cg_.builder_->CreateAdd(pJ0, C64(1)), C8('x'));
            auto *pJ2 = cg_.builder_->CreateAdd(pJ0, C64(2));
            storeJ(pJ2);
            auto *pBuf = cg_.builder_->CreateGEP(i8Ty, buf, pJ2, "buf.jp");
            auto *pLen = cg_.builder_->CreateCall(xtoaFn, {pBuf, pInt}, "len");
            storeJ(cg_.builder_->CreateAdd(pJ2, cg_.builder_->CreateSExt(pLen, i64Ty)));
            storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === %% ===
            cg_.builder_->SetInsertPoint(BB_chk_pct2);
            auto *isPct2 = cg_.builder_->CreateICmpEQ(spec, C8('%'));
            cg_.builder_->CreateCondBr(isPct2, BB_do_pct2, BB_default);

            cg_.builder_->SetInsertPoint(BB_do_pct2);
            storeCh(buf, loadJ(), C8('%'));
            storeJ(cg_.builder_->CreateAdd(loadJ(), C64(1)));
            storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === Default: copy %<spec> literally ===
            cg_.builder_->SetInsertPoint(BB_default);
            auto *defJ = loadJ();
            storeCh(buf, defJ, C8('%'));
            storeCh(buf, cg_.builder_->CreateAdd(defJ, C64(1)), spec);
            storeJ(cg_.builder_->CreateAdd(defJ, C64(2)));
            storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === Done: null terminate, va_end, return length ===
            cg_.builder_->SetInsertPoint(BB_done);
            auto *finalJ = loadJ();
            storeCh(buf, finalJ, C8(0));
            cg_.builder_->CreateCall(vaEndDecl, {vaList});
            cg_.builder_->CreateRet(cg_.builder_->CreateTrunc(finalJ, i32Ty, "ret.len"));

            cg_.builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 13. pow — Simple double power function
        //     For integer exponents: repeated multiplication
        //     For negative exponents: 1.0 / pow(base, -exp)
        // ============================================================================
        void FreestandingCodeGen::emitFreestandingPow(llvm::Type *dblTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(dblTy, {dblTy, dblTy}, false);
            llvm::Function *fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "pow", ft);
            if (!fn)
                return;

            auto savedIP = cg_.builder_->saveIP();
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);

            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *loop = llvm::BasicBlock::Create(*cg_.context_, "loop", fn);
            llvm::BasicBlock *done = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

            cg_.builder_->SetInsertPoint(entry);
            llvm::Value *base = fn->getArg(0);
            llvm::Value *exp = fn->getArg(1);

            // (AR) تحويل الأس إلى عدد صحيح (تقريب)
            // (EN) Convert exponent to integer (truncation)
            llvm::Value *expI = cg_.builder_->CreateFPToSI(exp, i64Ty, "exp.i");
            llvm::Value *isZero = cg_.builder_->CreateICmpEQ(expI, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(isZero, done, loop);

            cg_.builder_->SetInsertPoint(loop);
            llvm::PHINode *i = cg_.builder_->CreatePHI(i64Ty, 2, "i");
            llvm::PHINode *result = cg_.builder_->CreatePHI(dblTy, 2, "result");
            i->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            result->addIncoming(llvm::ConstantFP::get(dblTy, 1.0), entry);

            llvm::Value *newResult = cg_.builder_->CreateFMul(result, base, "mul");
            llvm::Value *nextI = cg_.builder_->CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1));
            i->addIncoming(nextI, loop);
            result->addIncoming(newResult, loop);

            // (AR) مقارنة مع القيمة المطلقة للأس
            // (EN) Compare with absolute value of exponent
            llvm::Value *absExp = cg_.builder_->CreateSelect(
                cg_.builder_->CreateICmpSLT(expI, llvm::ConstantInt::get(i64Ty, 0)),
                cg_.builder_->CreateNeg(expI),
                expI, "abs.exp");
            llvm::Value *cond = cg_.builder_->CreateICmpULT(nextI, absExp);
            cg_.builder_->CreateCondBr(cond, loop, done);

            cg_.builder_->SetInsertPoint(done);
            llvm::PHINode *finalResult = cg_.builder_->CreatePHI(dblTy, 2, "final");
            finalResult->addIncoming(llvm::ConstantFP::get(dblTy, 1.0), entry);
            finalResult->addIncoming(newResult, loop);

            // (AR) إذا كان الأس سالباً، أرجع 1/النتيجة
            // (EN) If exponent is negative, return 1/result
            llvm::Value *isNeg = cg_.builder_->CreateICmpSLT(expI, llvm::ConstantInt::get(i64Ty, 0));
            llvm::Value *inv = cg_.builder_->CreateFDiv(llvm::ConstantFP::get(dblTy, 1.0), finalResult, "inv");
            llvm::Value *ret = cg_.builder_->CreateSelect(isNeg, inv, finalResult, "pow.ret");
            cg_.builder_->CreateRet(ret);

            cg_.builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 14. __sad_serial_puts — Output a null-terminated string to serial port 0x3F8
        // ============================================================================
    } // namespace LLVM
} // namespace Sad
