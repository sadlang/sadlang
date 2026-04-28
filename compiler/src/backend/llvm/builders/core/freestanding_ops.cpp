// ============================================================================
// llvm_codegen_freestanding.cpp — Freestanding Runtime Implementation
// (AR) توليد وقت التشغيل المستقل — تطبيقات مدمجة لدوال C الأساسية
// (EN) Freestanding runtime generation — built-in C runtime function implementations
// ============================================================================
// Author: Sad Compiler Team
// Date: January 2026
// Version: 1.0
//
// (AR) عندما يكون المترجم في وضع freestanding (--freestanding), لا توجد مكتبة C
//      قياسية. هذا الملف يوفر تطبيقات مدمجة لدوال C الأساسية التي يولدها المترجم:
//      malloc, free, realloc, calloc, memcpy, memset, strlen, strcmp, strcpy, printf, pow
//      التطبيقات تُضاف بربط linkonce_odr لتجنب التكرار عند دمج عدة وحدات.
//      يمكن للمستخدم تجاوز أي دالة بتوفير تطبيقه الخاص.
//
// (EN) When the compiler is in freestanding mode (--freestanding), no C standard
//      library is available. This file provides built-in implementations for
//      essential C runtime functions that the compiler generates calls to:
//      malloc, free, realloc, calloc, memcpy, memset, strlen, strcmp, strcpy, printf, pow
//      Implementations use linkonce_odr linkage to avoid duplication when linking
//      multiple modules. Users can override any function by providing their own.
// ============================================================================

#include "llvm_codegen.h"
#include "builders/core/freestanding_codegen.h"
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Intrinsics.h>

namespace Sad {
namespace LLVM {

// ============================================================================
// (AR) نقطة الدخول الرئيسية — تُستدعى من emitModule عند freestanding
// (EN) Main entry point — called from emitModule when freestanding is true
// ============================================================================
void FreestandingCodeGen::emitFreestandingRuntime() {
    if (!cg_.module_ || !cg_.context_ || !cg_.builder_) return;
    
    // (AR) أنواع مشتركة نستخدمها في جميع الدوال
    // (EN) Common types used across all functions
    llvm::Type* i8Ty    = llvm::Type::getInt8Ty(*cg_.context_);
    llvm::Type* i32Ty   = llvm::Type::getInt32Ty(*cg_.context_);
    llvm::Type* i64Ty   = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Type* ptrTy   = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Type* voidTy  = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Type* dblTy   = llvm::Type::getDoubleTy(*cg_.context_);

    // ========================================================================
    // 1. malloc — Bump allocator (4MB heap, 16-byte aligned)
    // ========================================================================
    emitFreestandingMalloc(i8Ty, i64Ty, ptrTy);

    // ========================================================================
    // 2. free — No-op for bump allocator
    // ========================================================================
    emitFreestandingFree(ptrTy, voidTy);

    // ========================================================================
    // 3. memcpy — Byte-by-byte copy loop
    // ========================================================================
    emitFreestandingMemcpy(i8Ty, i64Ty, ptrTy);

    // ========================================================================
    // 4. memset — Byte-by-byte set loop
    // ========================================================================
    emitFreestandingMemset(i8Ty, i32Ty, i64Ty, ptrTy);

    // ========================================================================
    // 5. strlen — Scan for null byte
    // ========================================================================
    emitFreestandingStrlen(i8Ty, i64Ty, ptrTy);

    // ========================================================================
    // 6. strcmp — Byte-by-byte comparison
    // ========================================================================
    emitFreestandingStrcmp(i8Ty, i32Ty, i64Ty, ptrTy);

    // ========================================================================
    // 7. strcpy — Byte-by-byte copy until null
    // ========================================================================
    emitFreestandingStrcpy(i8Ty, i64Ty, ptrTy);

    // ========================================================================
    // 8. strcat — strlen + memcpy
    // ========================================================================
    emitFreestandingStrcat(ptrTy);

    // ========================================================================
    // 9. realloc — malloc + memcpy + free
    // ========================================================================
    emitFreestandingRealloc(i64Ty, ptrTy);

    // ========================================================================
    // 10. calloc — malloc + memset
    // ========================================================================
    emitFreestandingCalloc(i64Ty, ptrTy);

    // ========================================================================
    // 11. printf — Serial port output (0x3F8)
    // ========================================================================
    emitFreestandingPrintf(i8Ty, i32Ty, i64Ty, ptrTy);

    // ========================================================================
    // 12. sprintf — Minimal format to buffer
    // ========================================================================
    emitFreestandingSprintf(i8Ty, i32Ty, i64Ty, ptrTy);

    // ========================================================================
    // 13. pow — Simple power function
    // ========================================================================
    emitFreestandingPow(dblTy);

    // ========================================================================
    // 14. __sad_serial_puts — Direct serial string output (for freestanding print)
    // ========================================================================
    emitFreestandingSerialPuts(i8Ty, i64Ty, ptrTy);

    // ========================================================================
    // 15. __sad_serial_putint — Direct serial integer output (for freestanding print)
    // ========================================================================
    emitFreestandingSerialPutInt(i8Ty, i64Ty, ptrTy);

    // ========================================================================
    // 16. __sad_itoa — Convert i64 to decimal string in buffer (non-variadic)
    // ========================================================================
    emitFreestandingItoa(i8Ty, i32Ty, i64Ty, ptrTy);

    // ========================================================================
    // 17. __sad_ftoa — Convert f64 to decimal string in buffer (non-variadic)
    // ========================================================================
    emitFreestandingFtoa(i8Ty, i32Ty, i64Ty, ptrTy);

    // ========================================================================
    // 18. __sad_xtoa — Convert i64 to hex string in buffer (non-variadic)
    // ========================================================================
    emitFreestandingXtoa(i8Ty, i32Ty, i64Ty, ptrTy);
}

// ============================================================================
// Helper: check if function already has a body (user-provided implementation)
// ============================================================================
static bool hasDefinition(llvm::Module* mod, const std::string& name) {
    llvm::Function* fn = mod->getFunction(name);
    return fn && !fn->isDeclaration();
}

// ============================================================================
// Helper: get or create function with linkonce_odr linkage
// If the function already has a definition, return nullptr (skip)
// If it has only a declaration, replace with definition.
// When user declared a function with different type (e.g., free(i64) vs free(ptr)),
// create a bridge function that preserves existing call sites.
// ============================================================================
static llvm::Function* getOrCreateFreestandingFunc(
    llvm::Module* mod,
    llvm::LLVMContext& ctx,
    const std::string& name,
    llvm::FunctionType* ft)
{
    // (AR) إذا كانت الدالة لها تطبيق فعلي، لا نتجاوز تعريف المستخدم
    // (EN) If function already has a body, don't override user's implementation
    if (hasDefinition(mod, name)) return nullptr;

    llvm::Function* fn = mod->getFunction(name);
    if (fn) {
        if (fn->getFunctionType() != ft) {
            // (AR) التوقيع مختلف (مثلاً المستخدم صرّح free(i64) بينما نحتاج free(ptr))
            //      لا نحذف الدالة القديمة لأن هناك استدعاءات تشير إليها.
            //      بدلاً من ذلك: ننشئ دالة التطبيق بالتوقيع الصحيح باسم مختلف،
            //      ثم نضيف جسراً للدالة القديمة يحوّل الأنواع ويستدعي التطبيق.
            // (EN) Signature differs (e.g., user declared free(i64) but we need free(ptr)).
            //      DON'T erase — existing call sites reference the old function.
            //      Instead: create impl with correct type under a different name,
            //      then bridge the old function to call the impl with type casts.
            std::string implName = name + ".freestanding";
            llvm::Function* fnImpl = llvm::Function::Create(
                ft, llvm::Function::InternalLinkage, implName, mod);
            fnImpl->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
            fnImpl->addFnAttr("no-builtins");
            fnImpl->addFnAttr(llvm::Attribute::NoInline);
            fnImpl->addFnAttr(llvm::Attribute::OptimizeNone);

            // (AR) إنشاء جسر في الدالة القديمة: تحويل الأنواع → استدعاء التطبيق → تحويل النتيجة
            // (EN) Create bridge body in old function: cast params → call impl → cast result
            llvm::BasicBlock* entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            llvm::IRBuilder<> bld(entry);

            llvm::SmallVector<llvm::Value*, 4> args;
            unsigned idx = 0;
            for (auto& arg : fn->args()) {
                llvm::Value* val = &arg;
                if (idx < ft->getNumParams()) {
                    llvm::Type* implTy = ft->getParamType(idx);
                    if (val->getType() != implTy) {
                        if (val->getType()->isIntegerTy() && implTy->isPointerTy())
                            val = bld.CreateIntToPtr(val, implTy);
                        else if (val->getType()->isPointerTy() && implTy->isIntegerTy())
                            val = bld.CreatePtrToInt(val, implTy);
                    }
                }
                args.push_back(val);
                idx++;
            }

            llvm::Value* result = bld.CreateCall(fnImpl, args);

            if (fn->getReturnType()->isVoidTy()) {
                bld.CreateRetVoid();
            } else {
                llvm::Value* retVal = result;
                if (result->getType() != fn->getReturnType()) {
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
    } else {
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
void FreestandingCodeGen::emitFreestandingMalloc(
    llvm::Type* i8Ty, llvm::Type* i64Ty, llvm::Type* ptrTy)
{
    constexpr uint64_t HEAP_SIZE = 4 * 1024 * 1024; // 4MB

    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "malloc", ft);
    if (!fn) return;

    // (AR) إنشاء كومة ثابتة ومؤشر الموضع
    // (EN) Create static heap and offset pointer
    llvm::ArrayType* heapTy = llvm::ArrayType::get(i8Ty, HEAP_SIZE);
    auto* heap = new llvm::GlobalVariable(
        *cg_.module_, heapTy, false,
        llvm::GlobalValue::InternalLinkage,
        llvm::ConstantAggregateZero::get(heapTy),
        "__sad_heap");
    heap->setAlignment(llvm::Align(16));

    auto* heapOff = new llvm::GlobalVariable(
        *cg_.module_, i64Ty, false,
        llvm::GlobalValue::InternalLinkage,
        llvm::ConstantInt::get(i64Ty, 0),
        "__sad_heap_offset");

    // Save/restore builder state
    auto savedIP = cg_.builder_->saveIP();
    
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* oomBB = llvm::BasicBlock::Create(*cg_.context_, "oom", fn);
    llvm::BasicBlock* okBB  = llvm::BasicBlock::Create(*cg_.context_, "ok", fn);

    cg_.builder_->SetInsertPoint(entry);
    llvm::Value* size = fn->getArg(0);

    // (AR) محاذاة إلى 16 بايت: aligned = (offset + 15) & ~15
    // (EN) Align to 16 bytes
    llvm::Value* offset = cg_.builder_->CreateLoad(i64Ty, heapOff, "offset");
    llvm::Value* plus15 = cg_.builder_->CreateAdd(offset, llvm::ConstantInt::get(i64Ty, 15));
    llvm::Value* aligned = cg_.builder_->CreateAnd(plus15, llvm::ConstantInt::get(i64Ty, ~15ULL), "aligned");
    llvm::Value* newOff = cg_.builder_->CreateAdd(aligned, size, "new_off");

    // (AR) فحص تجاوز الكومة
    // (EN) Check heap overflow
    llvm::Value* overflow = cg_.builder_->CreateICmpUGT(newOff,
        llvm::ConstantInt::get(i64Ty, HEAP_SIZE), "overflow");
    cg_.builder_->CreateCondBr(overflow, oomBB, okBB);

    // OOM path — return null
    cg_.builder_->SetInsertPoint(oomBB);
    cg_.builder_->CreateRet(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)));

    // OK path
    cg_.builder_->SetInsertPoint(okBB);
    cg_.builder_->CreateStore(newOff, heapOff);
    llvm::Value* ptr = cg_.builder_->CreateGEP(heapTy, heap,
        {llvm::ConstantInt::get(i64Ty, 0), aligned}, "heap_ptr");
    cg_.builder_->CreateRet(ptr);

    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 2. free — No-op (bump allocator doesn't free)
// ============================================================================
void FreestandingCodeGen::emitFreestandingFree(llvm::Type* ptrTy, llvm::Type* voidTy) {
    llvm::FunctionType* ft = llvm::FunctionType::get(voidTy, {ptrTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "free", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    cg_.builder_->SetInsertPoint(entry);
    cg_.builder_->CreateRetVoid();
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 3. memcpy — Fast 8-byte (qword) forward copy with byte tail
// ============================================================================
void FreestandingCodeGen::emitFreestandingMemcpy(
    llvm::Type* i8Ty, llvm::Type* i64Ty, llvm::Type* ptrTy)
{
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "memcpy", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();

    llvm::BasicBlock* entry    = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* qLoop    = llvm::BasicBlock::Create(*cg_.context_, "qloop", fn);
    llvm::BasicBlock* qDone    = llvm::BasicBlock::Create(*cg_.context_, "qdone", fn);
    llvm::BasicBlock* tailLoop = llvm::BasicBlock::Create(*cg_.context_, "tail", fn);
    llvm::BasicBlock* done     = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

    cg_.builder_->SetInsertPoint(entry);
    llvm::Value* dst = fn->getArg(0);
    llvm::Value* src = fn->getArg(1);
    llvm::Value* n   = fn->getArg(2);

    // qword count = n / 8, tail start = qcount * 8
    llvm::Value* qCount = cg_.builder_->CreateLShr(n, llvm::ConstantInt::get(i64Ty, 3), "qcount");
    llvm::Value* tStart = cg_.builder_->CreateShl(qCount, llvm::ConstantInt::get(i64Ty, 3), "tstart");

    llvm::Value* hasQ = cg_.builder_->CreateICmpNE(qCount, llvm::ConstantInt::get(i64Ty, 0));
    cg_.builder_->CreateCondBr(hasQ, qLoop, qDone);

    // 8-byte copy loop
    cg_.builder_->SetInsertPoint(qLoop);
    llvm::PHINode* qi = cg_.builder_->CreatePHI(i64Ty, 2, "qi");
    qi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
    llvm::Value* qOff = cg_.builder_->CreateShl(qi, llvm::ConstantInt::get(i64Ty, 3));
    llvm::Value* srcQ = cg_.builder_->CreateGEP(i8Ty, src, qOff, "srcq");
    llvm::Value* qVal = cg_.builder_->CreateLoad(i64Ty, srcQ, "qval");
    llvm::Value* dstQ = cg_.builder_->CreateGEP(i8Ty, dst, qOff, "dstq");
    cg_.builder_->CreateStore(qVal, dstQ);
    llvm::Value* qNext = cg_.builder_->CreateAdd(qi, llvm::ConstantInt::get(i64Ty, 1));
    qi->addIncoming(qNext, qLoop);
    llvm::Value* qCond = cg_.builder_->CreateICmpULT(qNext, qCount);
    cg_.builder_->CreateCondBr(qCond, qLoop, qDone);

    // Tail byte loop
    cg_.builder_->SetInsertPoint(qDone);
    llvm::Value* hasTail = cg_.builder_->CreateICmpULT(tStart, n);
    cg_.builder_->CreateCondBr(hasTail, tailLoop, done);

    cg_.builder_->SetInsertPoint(tailLoop);
    llvm::PHINode* ti = cg_.builder_->CreatePHI(i64Ty, 2, "ti");
    ti->addIncoming(tStart, qDone);
    llvm::Value* srcP = cg_.builder_->CreateGEP(i8Ty, src, ti, "src.p");
    llvm::Value* byte = cg_.builder_->CreateLoad(i8Ty, srcP, "byte");
    llvm::Value* dstP = cg_.builder_->CreateGEP(i8Ty, dst, ti, "dst.p");
    cg_.builder_->CreateStore(byte, dstP);
    llvm::Value* tNext = cg_.builder_->CreateAdd(ti, llvm::ConstantInt::get(i64Ty, 1));
    ti->addIncoming(tNext, tailLoop);
    llvm::Value* tCond = cg_.builder_->CreateICmpULT(tNext, n);
    cg_.builder_->CreateCondBr(tCond, tailLoop, done);

    cg_.builder_->SetInsertPoint(done);
    cg_.builder_->CreateRet(dst);

    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 4. memset — Fast 8-byte (qword) set with byte tail
// ============================================================================
void FreestandingCodeGen::emitFreestandingMemset(
    llvm::Type* i8Ty, llvm::Type* i32Ty, llvm::Type* i64Ty, llvm::Type* ptrTy)
{
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {ptrTy, i32Ty, i64Ty}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "memset", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();

    llvm::BasicBlock* entry    = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* qLoop    = llvm::BasicBlock::Create(*cg_.context_, "qloop", fn);
    llvm::BasicBlock* qDone    = llvm::BasicBlock::Create(*cg_.context_, "qdone", fn);
    llvm::BasicBlock* tailLoop = llvm::BasicBlock::Create(*cg_.context_, "tail", fn);
    llvm::BasicBlock* done     = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

    cg_.builder_->SetInsertPoint(entry);
    llvm::Value* dst = fn->getArg(0);
    llvm::Value* val = cg_.builder_->CreateTrunc(fn->getArg(1), i8Ty, "val8");
    llvm::Value* n   = fn->getArg(2);

    // Build 8-byte fill pattern: broadcast byte to all 8 positions in i64
    llvm::Value* v64 = cg_.builder_->CreateZExt(val, i64Ty, "v64");
    llvm::Value* fill = v64;
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(i64Ty, 8)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(i64Ty, 16)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(i64Ty, 24)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(i64Ty, 32)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(i64Ty, 40)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(i64Ty, 48)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(i64Ty, 56)));

    // qword count = n / 8, tail count = n % 8
    llvm::Value* qCount = cg_.builder_->CreateLShr(n, llvm::ConstantInt::get(i64Ty, 3), "qcount");
    llvm::Value* tStart = cg_.builder_->CreateShl(qCount, llvm::ConstantInt::get(i64Ty, 3), "tstart");

    llvm::Value* hasQ = cg_.builder_->CreateICmpNE(qCount, llvm::ConstantInt::get(i64Ty, 0));
    cg_.builder_->CreateCondBr(hasQ, qLoop, qDone);

    // 8-byte loop
    cg_.builder_->SetInsertPoint(qLoop);
    llvm::PHINode* qi = cg_.builder_->CreatePHI(i64Ty, 2, "qi");
    qi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
    llvm::Value* qOff = cg_.builder_->CreateShl(qi, llvm::ConstantInt::get(i64Ty, 3));
    llvm::Value* qPtr = cg_.builder_->CreateGEP(i8Ty, dst, qOff, "qptr");
    cg_.builder_->CreateStore(fill, qPtr);
    llvm::Value* qNext = cg_.builder_->CreateAdd(qi, llvm::ConstantInt::get(i64Ty, 1));
    qi->addIncoming(qNext, qLoop);
    llvm::Value* qCond = cg_.builder_->CreateICmpULT(qNext, qCount);
    cg_.builder_->CreateCondBr(qCond, qLoop, qDone);

    // Tail bytes
    cg_.builder_->SetInsertPoint(qDone);
    llvm::Value* hasTail = cg_.builder_->CreateICmpULT(tStart, n);
    cg_.builder_->CreateCondBr(hasTail, tailLoop, done);

    cg_.builder_->SetInsertPoint(tailLoop);
    llvm::PHINode* ti = cg_.builder_->CreatePHI(i64Ty, 2, "ti");
    ti->addIncoming(tStart, qDone);
    llvm::Value* tPtr = cg_.builder_->CreateGEP(i8Ty, dst, ti, "tptr");
    cg_.builder_->CreateStore(val, tPtr);
    llvm::Value* tNext = cg_.builder_->CreateAdd(ti, llvm::ConstantInt::get(i64Ty, 1));
    ti->addIncoming(tNext, tailLoop);
    llvm::Value* tCond = cg_.builder_->CreateICmpULT(tNext, n);
    cg_.builder_->CreateCondBr(tCond, tailLoop, done);

    cg_.builder_->SetInsertPoint(done);
    cg_.builder_->CreateRet(dst);
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 5. strlen — Scan for null byte
// ============================================================================
void FreestandingCodeGen::emitFreestandingStrlen(
    llvm::Type* i8Ty, llvm::Type* i64Ty, llvm::Type* ptrTy)
{
    llvm::FunctionType* ft = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "strlen", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* loop  = llvm::BasicBlock::Create(*cg_.context_, "loop", fn);
    llvm::BasicBlock* done  = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

    cg_.builder_->SetInsertPoint(entry);
    llvm::Value* str = fn->getArg(0);
    cg_.builder_->CreateBr(loop);

    cg_.builder_->SetInsertPoint(loop);
    llvm::PHINode* i = cg_.builder_->CreatePHI(i64Ty, 2, "i");
    i->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
    llvm::Value* p = cg_.builder_->CreateGEP(i8Ty, str, i, "p");
    llvm::Value* ch = cg_.builder_->CreateLoad(i8Ty, p, "ch");
    llvm::Value* isNull = cg_.builder_->CreateICmpEQ(ch, llvm::ConstantInt::get(i8Ty, 0));
    llvm::Value* next = cg_.builder_->CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1));
    i->addIncoming(next, loop);
    cg_.builder_->CreateCondBr(isNull, done, loop);

    cg_.builder_->SetInsertPoint(done);
    cg_.builder_->CreateRet(i);
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 6. strcmp — Byte-by-byte comparison
// ============================================================================
void FreestandingCodeGen::emitFreestandingStrcmp(
    llvm::Type* i8Ty, llvm::Type* i32Ty, llvm::Type* i64Ty, llvm::Type* ptrTy)
{
    llvm::FunctionType* ft = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "strcmp", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();

    llvm::BasicBlock* entry  = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* loop   = llvm::BasicBlock::Create(*cg_.context_, "loop", fn);
    llvm::BasicBlock* differ = llvm::BasicBlock::Create(*cg_.context_, "differ", fn);
    llvm::BasicBlock* eqNull = llvm::BasicBlock::Create(*cg_.context_, "eq_null", fn);
    llvm::BasicBlock* next   = llvm::BasicBlock::Create(*cg_.context_, "next", fn);

    cg_.builder_->SetInsertPoint(entry);
    llvm::Value* s1 = fn->getArg(0);
    llvm::Value* s2 = fn->getArg(1);
    cg_.builder_->CreateBr(loop);

    cg_.builder_->SetInsertPoint(loop);
    llvm::PHINode* i = cg_.builder_->CreatePHI(i64Ty, 2, "i");
    i->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);

    llvm::Value* p1 = cg_.builder_->CreateGEP(i8Ty, s1, i);
    llvm::Value* c1 = cg_.builder_->CreateLoad(i8Ty, p1);
    llvm::Value* p2 = cg_.builder_->CreateGEP(i8Ty, s2, i);
    llvm::Value* c2 = cg_.builder_->CreateLoad(i8Ty, p2);
    llvm::Value* neq = cg_.builder_->CreateICmpNE(c1, c2);
    cg_.builder_->CreateCondBr(neq, differ, eqNull);

    // Characters differ — return difference
    cg_.builder_->SetInsertPoint(differ);
    llvm::Value* e1 = cg_.builder_->CreateZExt(c1, i32Ty);
    llvm::Value* e2 = cg_.builder_->CreateZExt(c2, i32Ty);
    llvm::Value* diff = cg_.builder_->CreateSub(e1, e2, "diff");
    cg_.builder_->CreateRet(diff);

    // Characters equal — check if null terminator
    cg_.builder_->SetInsertPoint(eqNull);
    llvm::Value* nextI = cg_.builder_->CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1), "next.i");
    llvm::Value* isNull = cg_.builder_->CreateICmpEQ(c1, llvm::ConstantInt::get(i8Ty, 0));
    cg_.builder_->CreateCondBr(isNull, next, loop);

    cg_.builder_->SetInsertPoint(next);
    cg_.builder_->CreateRet(llvm::ConstantInt::get(i32Ty, 0));

    // Fix phi — incoming from eqNull since that's where we increment
    i->addIncoming(nextI, eqNull);

    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 7. strcpy — Copy bytes including null terminator
// ============================================================================
void FreestandingCodeGen::emitFreestandingStrcpy(
    llvm::Type* i8Ty, llvm::Type* i64Ty, llvm::Type* ptrTy)
{
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "strcpy", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* loop  = llvm::BasicBlock::Create(*cg_.context_, "loop", fn);
    llvm::BasicBlock* done  = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

    cg_.builder_->SetInsertPoint(entry);
    llvm::Value* dst = fn->getArg(0);
    llvm::Value* src = fn->getArg(1);
    cg_.builder_->CreateBr(loop);

    cg_.builder_->SetInsertPoint(loop);
    llvm::PHINode* i = cg_.builder_->CreatePHI(i64Ty, 2, "i");
    i->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);

    llvm::Value* srcP = cg_.builder_->CreateGEP(i8Ty, src, i);
    llvm::Value* ch = cg_.builder_->CreateLoad(i8Ty, srcP);
    llvm::Value* dstP = cg_.builder_->CreateGEP(i8Ty, dst, i);
    cg_.builder_->CreateStore(ch, dstP);

    llvm::Value* isNull = cg_.builder_->CreateICmpEQ(ch, llvm::ConstantInt::get(i8Ty, 0));
    llvm::Value* next = cg_.builder_->CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1));
    i->addIncoming(next, loop);
    cg_.builder_->CreateCondBr(isNull, done, loop);

    cg_.builder_->SetInsertPoint(done);
    cg_.builder_->CreateRet(dst);
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 8. strcat — Concatenate strings: strlen(dst) + strcpy
// ============================================================================
void FreestandingCodeGen::emitFreestandingStrcat(llvm::Type* ptrTy) {
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "strcat", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    llvm::Type* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    cg_.builder_->SetInsertPoint(entry);

    llvm::Value* dst = fn->getArg(0);
    llvm::Value* src = fn->getArg(1);

    // (AR) استدعاء strlen للحصول على طول dst
    // (EN) Call strlen to get dst length
    llvm::Function* strlenFn = cg_.module_->getFunction("strlen");
    llvm::Value* dstLen = cg_.builder_->CreateCall(strlenFn, {dst}, "dst.len");

    // (AR) استدعاء strcpy لنسخ src إلى dst+len
    // (EN) Call strcpy to copy src to dst+len
    llvm::Type* i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
    llvm::Value* dstEnd = cg_.builder_->CreateGEP(i8Ty, dst, dstLen, "dst.end");
    llvm::Function* strcpyFn = cg_.module_->getFunction("strcpy");
    cg_.builder_->CreateCall(strcpyFn, {dstEnd, src});

    cg_.builder_->CreateRet(dst);
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 9. realloc — malloc new block, memcpy old data, free old block
// ============================================================================
void FreestandingCodeGen::emitFreestandingRealloc(llvm::Type* i64Ty, llvm::Type* ptrTy) {
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "realloc", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* notNull = llvm::BasicBlock::Create(*cg_.context_, "not_null", fn);
    llvm::BasicBlock* done    = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

    cg_.builder_->SetInsertPoint(entry);
    llvm::Value* oldPtr = fn->getArg(0);
    llvm::Value* newSz  = fn->getArg(1);

    // (AR) تخصيص كتلة جديدة
    // (EN) Allocate new block
    llvm::Function* mallocFn = cg_.module_->getFunction("malloc");
    llvm::Value* newPtr = cg_.builder_->CreateCall(mallocFn, {newSz}, "new.ptr");

    // (AR) إذا كان المؤشر القديم غير null، انسخ البيانات
    // (EN) If old pointer is non-null, copy data
    llvm::Value* isNull = cg_.builder_->CreateICmpEQ(oldPtr,
        llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)));
    cg_.builder_->CreateCondBr(isNull, done, notNull);

    cg_.builder_->SetInsertPoint(notNull);
    // (AR) نسخ الحجم الجديد (قد يكون أكثر من القديم لكن آمن للكومة)
    // (EN) Copy new size bytes (may be more than old but safe for bump allocator)
    llvm::Function* memcpyFn = cg_.module_->getFunction("memcpy");
    cg_.builder_->CreateCall(memcpyFn, {newPtr, oldPtr, newSz});
    llvm::Function* freeFn = cg_.module_->getFunction("free");
    cg_.builder_->CreateCall(freeFn, {oldPtr});
    cg_.builder_->CreateBr(done);

    cg_.builder_->SetInsertPoint(done);
    cg_.builder_->CreateRet(newPtr);
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 10. calloc — malloc + memset to zero
// ============================================================================
void FreestandingCodeGen::emitFreestandingCalloc(llvm::Type* i64Ty, llvm::Type* ptrTy) {
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {i64Ty, i64Ty}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "calloc", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    llvm::Type* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    cg_.builder_->SetInsertPoint(entry);

    llvm::Value* count = fn->getArg(0);
    llvm::Value* size  = fn->getArg(1);
    llvm::Value* total = cg_.builder_->CreateMul(count, size, "total");

    llvm::Function* mallocFn = cg_.module_->getFunction("malloc");
    llvm::Value* ptr = cg_.builder_->CreateCall(mallocFn, {total}, "ptr");

    llvm::Function* memsetFn = cg_.module_->getFunction("memset");
    cg_.builder_->CreateCall(memsetFn, {ptr, llvm::ConstantInt::get(i32Ty, 0), total});

    cg_.builder_->CreateRet(ptr);
    cg_.builder_->restoreIP(savedIP);
}


} // namespace LLVM
} // namespace Sad
