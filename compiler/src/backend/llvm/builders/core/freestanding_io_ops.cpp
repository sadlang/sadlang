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

        // ============================================================================
        // Helper: check if function already has a body (user-provided implementation)
        // ============================================================================
        static bool hasDefinition(llvm::Module *mod, const std::string &name)
        {
            llvm::Function *fn = mod->getFunction(name);
            return fn && !fn->isDeclaration();
        }

        // ============================================================================
        // Helper: get or create function with linkonce_odr linkage
        // If the function already has a definition, return nullptr (skip)
        // If it has only a declaration, replace with definition.
        // When user declared a function with different type (e.g., free(i64) vs free(ptr)),
        // create a bridge function that preserves existing call sites.
        // ============================================================================
        static llvm::Function *getOrCreateFreestandingFunc(
            llvm::Module *mod,
            llvm::LLVMContext &ctx,
            const std::string &name,
            llvm::FunctionType *ft)
        {
            // (AR) إذا كانت الدالة لها تطبيق فعلي، لا نتجاوز تعريف المستخدم
            // (EN) If function already has a body, don't override user's implementation
            if (hasDefinition(mod, name))
                return nullptr;

            llvm::Function *fn = mod->getFunction(name);
            if (fn)
            {
                if (fn->getFunctionType() != ft)
                {
                    // (AR) التوقيع مختلف (مثلاً المستخدم صرّح free(i64) بينما نحتاج free(ptr))
                    //      لا نحذف الدالة القديمة لأن هناك استدعاءات تشير إليها.
                    //      بدلاً من ذلك: ننشئ دالة التطبيق بالتوقيع الصحيح باسم مختلف،
                    //      ثم نضيف جسراً للدالة القديمة يحوّل الأنواع ويستدعي التطبيق.
                    // (EN) Signature differs (e.g., user declared free(i64) but we need free(ptr)).
                    //      DON'T erase — existing call sites reference the old function.
                    //      Instead: create impl with correct type under a different name,
                    //      then bridge the old function to call the impl with type casts.
                    std::string implName = name + ".freestanding";
                    llvm::Function *fnImpl = llvm::Function::Create(
                        ft, llvm::Function::InternalLinkage, implName, mod);
                    fnImpl->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
                    fnImpl->addFnAttr("no-builtins");
                    fnImpl->addFnAttr(llvm::Attribute::NoInline);
                    fnImpl->addFnAttr(llvm::Attribute::OptimizeNone);

                    // (AR) إنشاء جسر في الدالة القديمة: تحويل الأنواع → استدعاء التطبيق → تحويل النتيجة
                    // (EN) Create bridge body in old function: cast params → call impl → cast result
                    llvm::BasicBlock *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
                    llvm::IRBuilder<> bld(entry);

                    llvm::SmallVector<llvm::Value *, 4> args;
                    unsigned idx = 0;
                    for (auto &arg : fn->args())
                    {
                        llvm::Value *val = &arg;
                        if (idx < ft->getNumParams())
                        {
                            llvm::Type *implTy = ft->getParamType(idx);
                            if (val->getType() != implTy)
                            {
                                if (val->getType()->isIntegerTy() && implTy->isPointerTy())
                                    val = bld.CreateIntToPtr(val, implTy);
                                else if (val->getType()->isPointerTy() && implTy->isIntegerTy())
                                    val = bld.CreatePtrToInt(val, implTy);
                            }
                        }
                        args.push_back(val);
                        idx++;
                    }

                    llvm::Value *result = bld.CreateCall(fnImpl, args);

                    if (fn->getReturnType()->isVoidTy())
                    {
                        bld.CreateRetVoid();
                    }
                    else
                    {
                        llvm::Value *retVal = result;
                        if (result->getType() != fn->getReturnType())
                        {
                            if (result->getType()->isPointerTy() && fn->getReturnType()->isIntegerTy())
                                retVal = bld.CreatePtrToInt(result, fn->getReturnType());
                            else if (result->getType()->isIntegerTy() && fn->getReturnType()->isPointerTy())
                                retVal = bld.CreateIntToPtr(result, fn->getReturnType());
                        }
                        bld.CreateRet(retVal);
                    }

                    fn->setLinkage(llvm::Function::WeakODRLinkage);
                    fn->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
                    fn->addFnAttr("no-builtins");
                    fn->addFnAttr(llvm::Attribute::NoInline);
                    fn->addFnAttr(llvm::Attribute::OptimizeNone);

                    return fnImpl; // (AR) نعيد التطبيق ليملأه emit (EN) return impl for emit to populate
                }
                // (AR) نفس التوقيع — تحويل التصريح إلى تعريف
                // (EN) Same signature — convert declaration to definition
            }
            else
            {
                fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, mod);
            }
            // (AR) استخدام weak_odr لمنع الحذف أثناء دمج الوحدات
            // (EN) Use weak_odr: survives llvm-link even without direct references
            fn->setLinkage(llvm::Function::WeakODRLinkage);
            fn->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
            // (AR) منع LLVM من استبدال الحلقات باستدعاءات مكتبية (memcpy, memset, etc.)
            // (EN) Prevent LLVM from replacing loops with library calls (infinite recursion)
            fn->addFnAttr("no-builtins");
            fn->addFnAttr(llvm::Attribute::NoInline);
            fn->addFnAttr(llvm::Attribute::OptimizeNone);
            return fn;
        }

        // ============================================================================
        // 1. malloc — Bump allocator
        //    4MB static heap, 16-byte aligned allocation
        // ============================================================================

        // ============================================================================
        // 11. printf — Full serial output with va_arg format parsing (COM1 0x3F8)
        //     Supports: %lld, %ld, %d, %i, %s, %g, %f, %x, %X, %c, %p, %%
        //     (AR) تنفيذ كامل لـ printf مع تحليل محددات التنسيق عبر va_arg
        //          الإخراج مباشرة إلى المنفذ التسلسلي COM1
        // ============================================================================
        void FreestandingCodeGen::emitFreestandingPrintf(
            llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(i32Ty, {ptrTy}, true);
            llvm::Function *fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "printf", ft);
            if (!fn)
                return;

            auto savedIP = cg_.builder_->saveIP();
            auto *dblTy = llvm::Type::getDoubleTy(*cg_.context_);
            auto *i16Ty = llvm::Type::getInt16Ty(*cg_.context_);
            auto *voidTy = llvm::Type::getVoidTy(*cg_.context_);

            // --- Basic blocks ---
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

            // Helper function refs
            auto itoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            auto ftoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, dblTy}, false);
            auto xtoaFT = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty}, false);
            auto itoaFn = cg_.module_->getOrInsertFunction("__sad_itoa", itoaFT);
            auto ftoaFn = cg_.module_->getOrInsertFunction("__sad_ftoa", ftoaFT);
            auto xtoaFn = cg_.module_->getOrInsertFunction("__sad_xtoa", xtoaFT);
            // __sad_serial_puts for outputting converted strings
            auto putsFT = llvm::FunctionType::get(voidTy, {ptrTy}, false);
            auto putsFn = cg_.module_->getOrInsertFunction("__sad_serial_puts", putsFT);

            // Inline asm for single char serial output
            llvm::InlineAsm *outAsm = llvm::InlineAsm::get(
                llvm::FunctionType::get(voidTy, {i16Ty, i8Ty}, false),
                "outb %al, %dx",
                "{dx},{al}",
                true, false);
            llvm::Value *port3F8 = llvm::ConstantInt::get(i16Ty, 0x3F8);

            // === Entry ===
            cg_.builder_->SetInsertPoint(BB_entry);
            auto *fmt = fn->getArg(0);

            auto *pI = cg_.builder_->CreateAlloca(i64Ty, nullptr, "p.i");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), pI);

            // Temp buffer for itoa/ftoa/xtoa conversions (64 bytes on stack)
            auto *tmpBuf = cg_.builder_->CreateAlloca(
                llvm::ArrayType::get(i8Ty, 64), nullptr, "tmp_buf");

            // va_list
            auto vaTagTy = llvm::StructType::get(*cg_.context_, {i32Ty, i32Ty, ptrTy, ptrTy});
            auto *vaListTy = llvm::ArrayType::get(vaTagTy, 1);
            auto *vaList = cg_.builder_->CreateAlloca(vaListTy, nullptr, "va_list");
            auto vaStartDecl = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::vastart);
            auto vaEndDecl = llvm::Intrinsic::getDeclaration(cg_.module_.get(), llvm::Intrinsic::vaend);
            cg_.builder_->CreateCall(vaStartDecl, {vaList});
            cg_.builder_->CreateBr(BB_loop);

            // Helpers
            auto loadI = [&]()
            { return cg_.builder_->CreateLoad(i64Ty, pI, "i"); };
            auto storeI = [&](llvm::Value *v)
            { cg_.builder_->CreateStore(v, pI); };
            auto loadCh = [&](llvm::Value *base, llvm::Value *idx) -> llvm::Value *
            {
                return cg_.builder_->CreateLoad(i8Ty, cg_.builder_->CreateGEP(i8Ty, base, idx));
            };
            auto C64 = [&](int64_t v)
            { return llvm::ConstantInt::get(i64Ty, v); };
            auto C8 = [&](int8_t v)
            { return llvm::ConstantInt::get(i8Ty, (uint8_t)v); };
            auto emitOutb = [&](llvm::Value *byte)
            {
                cg_.builder_->CreateCall(outAsm, {port3F8, byte});
            };

            // === Main Loop ===
            cg_.builder_->SetInsertPoint(BB_loop);
            auto *curI = loadI();
            auto *ch = loadCh(fmt, curI);
            auto *isNull = cg_.builder_->CreateICmpEQ(ch, C8(0));
            cg_.builder_->CreateCondBr(isNull, BB_done, BB_not_null);

            cg_.builder_->SetInsertPoint(BB_not_null);
            auto *isPct = cg_.builder_->CreateICmpEQ(ch, C8('%'));
            cg_.builder_->CreateCondBr(isPct, BB_pct, BB_normal);

            // === Normal char → serial ===
            cg_.builder_->SetInsertPoint(BB_normal);
            emitOutb(ch);
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
            cg_.builder_->CreateCondBr(
                cg_.builder_->CreateICmpEQ(spec2, C8('l')), BB_chk_ll, BB_chk_ld);

            // === %ll... ===
            cg_.builder_->SetInsertPoint(BB_chk_ll);
            auto *iPlusThree = cg_.builder_->CreateAdd(iPlusTwo, C64(1));
            storeI(iPlusThree);
            auto *spec3 = loadCh(fmt, iPlusThree);
            auto *isLLD = cg_.builder_->CreateOr(
                cg_.builder_->CreateICmpEQ(spec3, C8('d')),
                cg_.builder_->CreateICmpEQ(spec3, C8('i')));
            cg_.builder_->CreateCondBr(isLLD, BB_do_lld, BB_copy_ll);

            // === do %lld → itoa → serial_puts ===
            cg_.builder_->SetInsertPoint(BB_do_lld);
            {
                auto *val = cg_.builder_->CreateVAArg(vaList, i64Ty, "va.lld");
                cg_.builder_->CreateCall(itoaFn, {tmpBuf, val});
                cg_.builder_->CreateCall(putsFn, {tmpBuf});
                storeI(cg_.builder_->CreateAdd(iPlusThree, C64(1)));
                cg_.builder_->CreateBr(BB_loop);
            }

            // === copy %ll literally ===
            cg_.builder_->SetInsertPoint(BB_copy_ll);
            emitOutb(C8('%'));
            emitOutb(C8('l'));
            emitOutb(C8('l'));
            cg_.builder_->CreateBr(BB_loop);

            // === %ld ===
            cg_.builder_->SetInsertPoint(BB_chk_ld);
            cg_.builder_->CreateCondBr(
                cg_.builder_->CreateOr(
                    cg_.builder_->CreateICmpEQ(spec2, C8('d')),
                    cg_.builder_->CreateICmpEQ(spec2, C8('i'))),
                BB_do_ld, BB_copy_l);

            cg_.builder_->SetInsertPoint(BB_do_ld);
            {
                auto *val = cg_.builder_->CreateVAArg(vaList, i64Ty, "va.ld");
                cg_.builder_->CreateCall(itoaFn, {tmpBuf, val});
                cg_.builder_->CreateCall(putsFn, {tmpBuf});
                storeI(cg_.builder_->CreateAdd(iPlusTwo, C64(1)));
                cg_.builder_->CreateBr(BB_loop);
            }

            cg_.builder_->SetInsertPoint(BB_copy_l);
            emitOutb(C8('%'));
            emitOutb(C8('l'));
            cg_.builder_->CreateBr(BB_loop);

            // === %d / %i ===
            cg_.builder_->SetInsertPoint(BB_chk_d);
            cg_.builder_->CreateCondBr(
                cg_.builder_->CreateOr(
                    cg_.builder_->CreateICmpEQ(spec, C8('d')),
                    cg_.builder_->CreateICmpEQ(spec, C8('i'))),
                BB_do_d, BB_chk_s);

            cg_.builder_->SetInsertPoint(BB_do_d);
            {
                auto *val = cg_.builder_->CreateVAArg(vaList, i64Ty, "va.d");
                cg_.builder_->CreateCall(itoaFn, {tmpBuf, val});
                cg_.builder_->CreateCall(putsFn, {tmpBuf});
                storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
                cg_.builder_->CreateBr(BB_loop);
            }

            // === %s → serial_puts ===
            cg_.builder_->SetInsertPoint(BB_chk_s);
            cg_.builder_->CreateCondBr(
                cg_.builder_->CreateICmpEQ(spec, C8('s')), BB_do_s, BB_chk_gf);

            cg_.builder_->SetInsertPoint(BB_do_s);
            {
                auto *strVal = cg_.builder_->CreateVAArg(vaList, ptrTy, "va.s");
                auto *notNull = cg_.builder_->CreateICmpNE(strVal,
                                                       llvm::ConstantPointerNull::get(llvm::PointerType::get(*cg_.context_, 0)));
                // Call serial_puts only if non-null (conditional call via select is not
                // possible — use branch-free approach: call puts with dummy if null)
                // Actually, serial_puts handles reading from null poorly, so we branch:
                auto *BB_s_out = llvm::BasicBlock::Create(*cg_.context_, "s_out", fn);
                auto *BB_s_skip = llvm::BasicBlock::Create(*cg_.context_, "s_skip", fn);
                cg_.builder_->CreateCondBr(notNull, BB_s_out, BB_s_skip);

                cg_.builder_->SetInsertPoint(BB_s_out);
                cg_.builder_->CreateCall(putsFn, {strVal});
                cg_.builder_->CreateBr(BB_s_skip);

                cg_.builder_->SetInsertPoint(BB_s_skip);
                storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
                cg_.builder_->CreateBr(BB_loop);
            }

            // === %g / %f ===
            cg_.builder_->SetInsertPoint(BB_chk_gf);
            cg_.builder_->CreateCondBr(
                cg_.builder_->CreateOr(
                    cg_.builder_->CreateOr(
                        cg_.builder_->CreateICmpEQ(spec, C8('g')),
                        cg_.builder_->CreateICmpEQ(spec, C8('f'))),
                    cg_.builder_->CreateICmpEQ(spec, C8('e'))),
                BB_do_gf, BB_chk_x);

            cg_.builder_->SetInsertPoint(BB_do_gf);
            {
                auto *fval = cg_.builder_->CreateVAArg(vaList, dblTy, "va.gf");
                cg_.builder_->CreateCall(ftoaFn, {tmpBuf, fval});
                cg_.builder_->CreateCall(putsFn, {tmpBuf});
                storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
                cg_.builder_->CreateBr(BB_loop);
            }

            // === %x / %X ===
            cg_.builder_->SetInsertPoint(BB_chk_x);
            cg_.builder_->CreateCondBr(
                cg_.builder_->CreateOr(
                    cg_.builder_->CreateICmpEQ(spec, C8('x')),
                    cg_.builder_->CreateICmpEQ(spec, C8('X'))),
                BB_do_x, BB_chk_c);

            cg_.builder_->SetInsertPoint(BB_do_x);
            {
                auto *val = cg_.builder_->CreateVAArg(vaList, i64Ty, "va.x");
                cg_.builder_->CreateCall(xtoaFn, {tmpBuf, val});
                cg_.builder_->CreateCall(putsFn, {tmpBuf});
                storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
                cg_.builder_->CreateBr(BB_loop);
            }

            // === %c ===
            cg_.builder_->SetInsertPoint(BB_chk_c);
            cg_.builder_->CreateCondBr(
                cg_.builder_->CreateICmpEQ(spec, C8('c')), BB_do_c, BB_chk_p);

            cg_.builder_->SetInsertPoint(BB_do_c);
            {
                auto *val = cg_.builder_->CreateVAArg(vaList, i64Ty, "va.c");
                emitOutb(cg_.builder_->CreateTrunc(val, i8Ty));
                storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
                cg_.builder_->CreateBr(BB_loop);
            }

            // === %p ===
            cg_.builder_->SetInsertPoint(BB_chk_p);
            cg_.builder_->CreateCondBr(
                cg_.builder_->CreateICmpEQ(spec, C8('p')), BB_do_p, BB_chk_pct2);

            cg_.builder_->SetInsertPoint(BB_do_p);
            {
                auto *val = cg_.builder_->CreateVAArg(vaList, ptrTy, "va.p");
                auto *ptrInt = cg_.builder_->CreatePtrToInt(val, i64Ty);
                emitOutb(C8('0'));
                emitOutb(C8('x'));
                cg_.builder_->CreateCall(xtoaFn, {tmpBuf, ptrInt});
                cg_.builder_->CreateCall(putsFn, {tmpBuf});
                storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
                cg_.builder_->CreateBr(BB_loop);
            }

            // === %% ===
            cg_.builder_->SetInsertPoint(BB_chk_pct2);
            cg_.builder_->CreateCondBr(
                cg_.builder_->CreateICmpEQ(spec, C8('%')), BB_do_pct2, BB_default);

            cg_.builder_->SetInsertPoint(BB_do_pct2);
            emitOutb(C8('%'));
            storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === Default ===
            cg_.builder_->SetInsertPoint(BB_default);
            emitOutb(C8('%'));
            emitOutb(spec);
            storeI(cg_.builder_->CreateAdd(iPlusOne, C64(1)));
            cg_.builder_->CreateBr(BB_loop);

            // === Done ===
            cg_.builder_->SetInsertPoint(BB_done);
            cg_.builder_->CreateCall(vaEndDecl, {vaList});
            cg_.builder_->CreateRet(llvm::ConstantInt::get(i32Ty, 0));

            cg_.builder_->restoreIP(savedIP);
        }

        // ============================================================================
        // 12. sprintf — Full implementation with va_arg support
        //     Supports: %lld, %lli, %ld, %li, %d, %i, %s, %g, %f, %e, %x, %X,
        //               %c, %p, %%
        //     (AR) تنفيذ كامل لـ sprintf مع دعم va_arg لجميع محددات التنسيق الشائعة
        //     Uses alloca-based state (mem2reg optimizes to PHI at -O1)
        // ============================================================================
    } // namespace LLVM
} // namespace Sad

