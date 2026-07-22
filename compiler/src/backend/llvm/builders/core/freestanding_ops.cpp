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
    // 10.5 __sad_serial_putc — Polled single-byte serial output (LSR wait)
    //      (AR) يجب إصداره قبل printf/puts/putint — كلّها تمرّ عبره
    // ========================================================================
    emitFreestandingSerialPutc(i8Ty, voidTy);

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

    // ========================================================================
    // 19. __sad_panic — Diagnostic halt (weak; kernels override with real halt)
    // ========================================================================
    emitFreestandingPanic(i64Ty, voidTy);

    // ========================================================================
    // 20. __udivdi3 / __umoddi3 / __divdi3 / __moddi3 — Software 64-bit division
    //     (AR) على i686 الحرّ لا libgcc، فالخلفيّة تُخفِّض «قسمة/باقي i64» إلى هذه
    //          الرموز. يجب إصدار __udivdi3 أوّلًا (تعتمد عليه البقيّة).
    //     (EN) On freestanding i686 there is no libgcc, so the backend lowers i64
    //          div/rem to these symbols. Emit __udivdi3 first (the rest call it).
    // ========================================================================
    emitFreestandingUdivdi3(i64Ty);
    emitFreestandingUmoddi3(i64Ty);
    emitFreestandingDivdi3(i64Ty);
    emitFreestandingModdi3(i64Ty);

    // ========================================================================
    // 24. time — Current wall-clock time (Unix epoch seconds) via CMOS RTC
    //     (AR) جسر عتاد: يقرأ ساعة الوقت الحقيقي (منافذ 0x70/0x71) بلا libc.
    //          يُوفِّر رمز `time` الذي يولّده المترجم لمدمَج «الآن» في وضع --حرّ.
    //     (EN) Hardware bridge: reads the RTC (ports 0x70/0x71) with no libc.
    //          Provides the `time` symbol the compiler emits for the «الآن»
    //          builtin under freestanding (--حرّ) mode.
    // ========================================================================
    llvm::Type* i16Ty = llvm::Type::getInt16Ty(*cg_.context_);
    emitFreestandingTime(i8Ty, i16Ty, i64Ty, ptrTy);

    // ========================================================================
    // 25. sad_file_is_dir — كعب ضعيف لمدمَج «هل_مجلد» في الوضع الحرّ
    //     (AR) لا يوجد نظام ملفّات في المعدن العاري؛ نعيد 0 (ليس مجلدًا) كي يرتبط
    //          البرنامج. النواة المضيفة تتجاوز هذا الرمز بتطبيق VFS حقيقيّ.
    //     (EN) No filesystem on bare metal; return 0 (not a dir) so linking
    //          succeeds. The host kernel overrides this symbol with a real VFS.
    // ========================================================================
    emitFreestandingFileIsDir(i32Ty, ptrTy);
}

// (AR) تصريح مسبق — التعريف أدناه / (EN) Forward declaration — defined below
static llvm::Function* getOrCreateFreestandingFunc(
    llvm::Module* mod, llvm::LLVMContext& ctx,
    const std::string& name, llvm::FunctionType* ft);

// ============================================================================
// 19. __sad_panic(code) — (AR) هلع تشخيصيّ للوضع الحرّ: يطبع الرمز عبر printf
//     الحرّ (منفذ تسلسليّ) ثم يتوقّف في حلقة لا نهائيّة. weak_odr — النواة
//     تتجاوزه بتعريف قويّ (cli/hlt مثلًا). يستبدل exit(1) في مسارات التشخيص.
//     (EN) Freestanding diagnostic panic: print code via freestanding printf
//     (serial port) then spin forever. weak_odr — kernels override with a
//     strong definition (e.g., cli/hlt). Replaces exit(1) on diagnostic paths.
// ============================================================================
void FreestandingCodeGen::emitFreestandingPanic(llvm::Type* i64Ty, llvm::Type* voidTy) {
    llvm::FunctionType* ft = llvm::FunctionType::get(voidTy, {i64Ty}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "__sad_panic", ft);
    if (!fn) return;
    // (AR) عقد التجاوز: النواة قد توفّر تعريفًا قويًّا خاصًّا بها (cli/hlt). مواقع
    //      النداء تُتبِع الاستدعاء بـ unreachable، فأيّ تجاوز يجب ألّا يعود أبدًا
    //      (NoReturn جزء من العقد — تجاوزٌ يعود = سلوك غير معرّف بعد unreachable).
    // (EN) Override contract: a kernel may supply its own strong definition
    //      (cli/hlt). Call sites follow with unreachable, so any override MUST
    //      NOT return (NoReturn is part of the contract).
    fn->addFnAttr(llvm::Attribute::NoReturn);

    auto savedIP = cg_.builder_->saveIP();
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* halt  = llvm::BasicBlock::Create(*cg_.context_, "halt", fn);
    cg_.builder_->SetInsertPoint(entry);

    // (AR) الطباعة عبر printf الحرّ إن وُجد (يُبثّ قبلنا في emitFreestandingRuntime)
    // (EN) Print via freestanding printf if present (emitted earlier in the runtime)
    if (llvm::Function* pf = cg_.module_->getFunction("printf")) {
        llvm::Value* fmt = cg_.builder_->CreateGlobalStringPtr(
            "\n[SAD PANIC %lld]\n", "sad.panic.fmt");
        cg_.builder_->CreateCall(pf, {fmt, fn->getArg(0)});
    }
    cg_.builder_->CreateBr(halt);

    // (AR) التوقّف الافتراضيّ: cli ثم hlt في حلقة — الدوران الفارغ السابق كان
    //      يواصل خدمة المقاطعات بعد الهلع ويحرق المعالج. (يبقى weak_odr —
    //      النواة تتجاوزه بسياستها الخاصّة عند الحاجة.)
    // (EN) Default halt: cli;hlt loop — the previous empty spin kept serving
    //      interrupts after panic and burned the CPU. Still weak_odr.
    cg_.builder_->SetInsertPoint(halt);
    llvm::InlineAsm* haltAsm = llvm::InlineAsm::get(
        llvm::FunctionType::get(voidTy, {}, false),
        "cli\n\thlt", "", true, false);
    cg_.builder_->CreateCall(haltAsm, {});
    cg_.builder_->CreateBr(halt);
    cg_.builder_->restoreIP(savedIP);
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
//
// (AR) عقد المخصّص الحرّ (موثَّق — كان دَينًا):
//   - المحاذاة: المؤشّر المعاد محاذى دائمًا إلى 16 بايت (يكفي أيّ نوع
//     أساسيّ بما فيه fxsave لا — ذاك يتطلّب 16 وهي مضمونة هنا).
//   - الترويسة: قبل كلّ مؤشّر معاد بـ16 بايت تُخزَّن ترويسة تحمل حجم
//     الطلب (i64 في أوّلها والبقيّة حشو محاذاة) — يقرؤها realloc لنسخ
//     الأصغر (لا over-read). free لا-عمليّة، فالترويسة لا تُستردّ أبدًا.
//   - الفشل: تجاوز الكومة (4MB) يعيد null — المستهلكون العلويّون
//     (المصفوفات/الخرائط) يهلعون عبر مساراتهم.
// (EN) Freestanding allocator contract: 16-byte aligned results; a 16-byte
//     header immediately before each returned pointer stores the request
//     size (i64 + padding) so realloc can copy min(old,new); free is a
//     no-op; heap exhaustion returns null.
// ============================================================================
void FreestandingCodeGen::emitFreestandingMalloc(
    llvm::Type* i8Ty, llvm::Type* i64Ty, llvm::Type* ptrTy)
{
    constexpr uint64_t HEAP_SIZE = 4 * 1024 * 1024; // 4MB
    constexpr uint64_t HEADER_SIZE = 16; // (AR) ترويسة الحجم — تحفظ محاذاة 16

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
    // (AR) الحجز = ترويسة الحجم (16) + الطلب — الترويسة تسبق المؤشّر المعاد
    // (EN) Reserve header (16) + request; header precedes the returned pointer
    llvm::Value* withHdr = cg_.builder_->CreateAdd(aligned,
        llvm::ConstantInt::get(i64Ty, HEADER_SIZE), "with_hdr");
    llvm::Value* newOff = cg_.builder_->CreateAdd(withHdr, size, "new_off");

    // (AR) فحص تجاوز الكومة (بما يشمل الترويسة)
    // (EN) Check heap overflow (header included)
    llvm::Value* overflow = cg_.builder_->CreateICmpUGT(newOff,
        llvm::ConstantInt::get(i64Ty, HEAP_SIZE), "overflow");
    cg_.builder_->CreateCondBr(overflow, oomBB, okBB);

    // OOM path — return null
    cg_.builder_->SetInsertPoint(oomBB);
    cg_.builder_->CreateRet(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)));

    // OK path
    cg_.builder_->SetInsertPoint(okBB);
    cg_.builder_->CreateStore(newOff, heapOff);
    // (AR) كتابة حجم الطلب في الترويسة — يقرؤه realloc لنسخ الأصغر
    // (EN) Store request size in the header — realloc reads it to copy min
    llvm::Value* hdrPtr = cg_.builder_->CreateGEP(heapTy, heap,
        {llvm::ConstantInt::get(i64Ty, 0), aligned}, "hdr_ptr");
    cg_.builder_->CreateStore(size, hdrPtr);
    llvm::Value* ptr = cg_.builder_->CreateGEP(heapTy, heap,
        {llvm::ConstantInt::get(i64Ty, 0), withHdr}, "heap_ptr");
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

    // (AR) النسخ فقط إذا كان المؤشران غير فارغين (فشل malloc ⇒ إرجاع null بلا نسخ)
    // (EN) Copy only when both pointers are non-null (malloc failure returns null)
    llvm::Value* nullPtr = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));
    llvm::Value* oldIsNull = cg_.builder_->CreateICmpEQ(oldPtr, nullPtr, "old.is.null");
    llvm::Value* newIsNull = cg_.builder_->CreateICmpEQ(newPtr, nullPtr, "new.is.null");
    llvm::Value* skipCopy = cg_.builder_->CreateOr(oldIsNull, newIsNull, "skip.copy");
    cg_.builder_->CreateCondBr(skipCopy, done, notNull);

    cg_.builder_->SetInsertPoint(notNull);
    // (AR) قراءة حجم الكتلة القديمة من ترويستها (تسبق المؤشّر بـ16 بايت —
    //      انظر عقد malloc أعلاه) والنسخ بالأصغر بين القديم والجديد.
    //      كان النسخ سابقًا بحجم الكتلة الجديدة ⇒ قراءة زائدة (over-read)
    //      من ذيل الكتلة القديمة.
    // (EN) Read old block size from its header (16 bytes before the pointer)
    //      and copy min(old, new) — previously copied newSz (over-read).
    llvm::Type* i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
    llvm::Value* hdrPtr = cg_.builder_->CreateGEP(i8Ty, oldPtr,
        llvm::ConstantInt::get(i64Ty, -16), "old.hdr");
    llvm::Value* oldSz = cg_.builder_->CreateLoad(i64Ty, hdrPtr, "old.size");
    llvm::Value* newSmaller = cg_.builder_->CreateICmpULT(newSz, oldSz, "new.smaller");
    llvm::Value* copySz = cg_.builder_->CreateSelect(newSmaller, newSz, oldSz, "copy.size");
    llvm::Function* memcpyFn = cg_.module_->getFunction("memcpy");
    cg_.builder_->CreateCall(memcpyFn, {newPtr, oldPtr, copySz});
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

    // (AR) التصفير عبر llvm.memset intrinsic لا رمز @memset — يتفادى تعارض
    //      التوقيع حين يعيد المصدر إعلان memset بتوقيع مختلف (خارجي memset).
    // (EN) Zero via llvm.memset intrinsic, not the @memset symbol — avoids the
    //      signature clash when source redeclares memset (خارجي memset).
    (void)i32Ty;
    cg_.builder_->CreateMemSet(ptr, cg_.builder_->getInt8(0), total, llvm::MaybeAlign(8));

    cg_.builder_->CreateRet(ptr);
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 24. time — Unix epoch seconds from the CMOS Real-Time Clock (no libc)
// ============================================================================
// (AR) يُطبّق التوقيع C: time_t time(time_t*). يتجاهل المعامل (يعيد القيمة فقط،
//      لا يكتب عبر المؤشر كما تسمح المواصفة عند تمرير NULL). يقرأ سِجلّات CMOS
//      عبر منفذ الفهرس 0x70 ومنفذ البيانات 0x71، يفكّ ترميز BCD، ثم يحوّل مكوّنات
//      الوقت إلى ثوانٍ منذ 1970 بخوارزمية days-from-civil (سنوات ≥ 2000 موجبة).
//      افتراضات: وضع BCD (افتراض QEMU/العتاد الشائع) وساعة 24، والسنة 2000–2099.
// (EN) Implements C signature: time_t time(time_t*). Ignores the argument
//      (returns the value only; does not write through the pointer — allowed
//      when NULL is passed). Reads CMOS registers via index port 0x70 / data
//      port 0x71, BCD-decodes them, then converts the broken-down time to
//      seconds since 1970 using the days-from-civil algorithm (years ≥ 2000 are
//      positive). Assumes BCD mode (common QEMU/hardware default), 24h, 2000–2099.
// ============================================================================
void FreestandingCodeGen::emitFreestandingTime(
    llvm::Type* i8Ty, llvm::Type* i16Ty, llvm::Type* i64Ty, llvm::Type* ptrTy)
{
    llvm::FunctionType* ft = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "time", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    auto& B = *cg_.builder_;
    llvm::Type* voidTy = llvm::Type::getVoidTy(*cg_.context_);

    // (AR) تجميع مُضمّن لـ inb/outb (يطابق نمط llvm_port_io_intrinsics)
    // (EN) inline asm for inb/outb (mirrors llvm_port_io_intrinsics pattern)
    llvm::InlineAsm* outbAsm = llvm::InlineAsm::get(
        llvm::FunctionType::get(voidTy, {i8Ty, i16Ty}, false),
        "outb $0, $1", "{al},{dx}", true, false);
    llvm::InlineAsm* inbAsm = llvm::InlineAsm::get(
        llvm::FunctionType::get(i8Ty, {i16Ty}, false),
        "inb $1, $0", "={al},{dx}", true, false);

    llvm::Value* idxPort = llvm::ConstantInt::get(i16Ty, 0x70);
    llvm::Value* dataPort = llvm::ConstantInt::get(i16Ty, 0x71);

    // (AR) دالة محلية: قراءة سِجلّ CMOS رقم reg وإرجاعه كـ i64
    // (EN) local helper: read CMOS register `reg`, return as i64
    auto readCmos = [&](uint8_t reg) -> llvm::Value* {
        B.CreateCall(outbAsm, {llvm::ConstantInt::get(i8Ty, reg), idxPort});
        llvm::Value* raw = B.CreateCall(inbAsm, {dataPort});
        return B.CreateZExt(raw, i64Ty);
    };
    // (AR) فكّ BCD: (v & 0x0F) + ((v >> 4) * 10)
    // (EN) BCD decode: (v & 0x0F) + ((v >> 4) * 10)
    auto bcd = [&](llvm::Value* v) -> llvm::Value* {
        llvm::Value* lo = B.CreateAnd(v, llvm::ConstantInt::get(i64Ty, 0x0F));
        llvm::Value* hi = B.CreateMul(
            B.CreateLShr(v, llvm::ConstantInt::get(i64Ty, 4)),
            llvm::ConstantInt::get(i64Ty, 10));
        return B.CreateAdd(lo, hi);
    };

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* wait  = llvm::BasicBlock::Create(*cg_.context_, "uip_wait", fn);
    llvm::BasicBlock* read  = llvm::BasicBlock::Create(*cg_.context_, "read", fn);

    B.SetInsertPoint(entry);
    B.CreateBr(wait);

    // (AR) انتظر حتى ينتهي تحديث RTC: سِجلّ الحالة A (0x0A) بت 7 (0x80)
    // (EN) wait until RTC update finishes: status register A (0x0A) bit 7 (0x80)
    B.SetInsertPoint(wait);
    llvm::Value* statusA = readCmos(0x0A);
    llvm::Value* uip = B.CreateAnd(statusA, llvm::ConstantInt::get(i64Ty, 0x80));
    llvm::Value* busy = B.CreateICmpNE(uip, llvm::ConstantInt::get(i64Ty, 0));
    B.CreateCondBr(busy, wait, read);

    B.SetInsertPoint(read);
    llvm::Value* sec  = bcd(readCmos(0x00));
    llvm::Value* min  = bcd(readCmos(0x02));
    llvm::Value* hour = bcd(readCmos(0x04));
    llvm::Value* day  = bcd(readCmos(0x07));
    llvm::Value* mon  = bcd(readCmos(0x08));
    llvm::Value* yy   = bcd(readCmos(0x09)); // 0..99
    llvm::Value* year = B.CreateAdd(yy, llvm::ConstantInt::get(i64Ty, 2000), "year");

    // days_from_civil (Howard Hinnant); year >= 2000 so all terms are positive.
    //   y   = year - (mon <= 2)
    //   era = y / 400
    //   yoe = y - era*400
    //   mp  = (mon > 2) ? mon-3 : mon+9
    //   doy = (153*mp + 2)/5 + day - 1
    //   doe = yoe*365 + yoe/4 - yoe/100 + doy
    //   days= era*146097 + doe - 719468
    llvm::Value* monLE2 = B.CreateICmpULE(mon, llvm::ConstantInt::get(i64Ty, 2));
    llvm::Value* yAdj = B.CreateSelect(monLE2,
        llvm::ConstantInt::get(i64Ty, 1), llvm::ConstantInt::get(i64Ty, 0));
    llvm::Value* y = B.CreateSub(year, yAdj, "y");
    llvm::Value* era = B.CreateUDiv(y, llvm::ConstantInt::get(i64Ty, 400), "era");
    llvm::Value* yoe = B.CreateSub(y, B.CreateMul(era, llvm::ConstantInt::get(i64Ty, 400)), "yoe");
    llvm::Value* mp = B.CreateSelect(
        B.CreateICmpUGT(mon, llvm::ConstantInt::get(i64Ty, 2)),
        B.CreateSub(mon, llvm::ConstantInt::get(i64Ty, 3)),
        B.CreateAdd(mon, llvm::ConstantInt::get(i64Ty, 9)), "mp");
    llvm::Value* doy = B.CreateAdd(
        B.CreateUDiv(
            B.CreateAdd(B.CreateMul(llvm::ConstantInt::get(i64Ty, 153), mp),
                        llvm::ConstantInt::get(i64Ty, 2)),
            llvm::ConstantInt::get(i64Ty, 5)),
        B.CreateSub(day, llvm::ConstantInt::get(i64Ty, 1)), "doy");
    llvm::Value* doe = B.CreateAdd(
        B.CreateAdd(
            B.CreateSub(
                B.CreateAdd(B.CreateMul(yoe, llvm::ConstantInt::get(i64Ty, 365)),
                            B.CreateUDiv(yoe, llvm::ConstantInt::get(i64Ty, 4))),
                B.CreateUDiv(yoe, llvm::ConstantInt::get(i64Ty, 100))),
            doy),
        llvm::ConstantInt::get(i64Ty, 0), "doe");
    llvm::Value* days = B.CreateSub(
        B.CreateAdd(B.CreateMul(era, llvm::ConstantInt::get(i64Ty, 146097)), doe),
        llvm::ConstantInt::get(i64Ty, 719468), "days");

    // epoch = days*86400 + hour*3600 + min*60 + sec
    llvm::Value* epoch = B.CreateAdd(
        B.CreateAdd(
            B.CreateAdd(B.CreateMul(days, llvm::ConstantInt::get(i64Ty, 86400)),
                        B.CreateMul(hour, llvm::ConstantInt::get(i64Ty, 3600))),
            B.CreateMul(min, llvm::ConstantInt::get(i64Ty, 60))),
        sec, "epoch");
    B.CreateRet(epoch);

    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 25. sad_file_is_dir — weak stub for the «هل_مجلد» builtin in freestanding
//     Returns 0 (not a directory); the host kernel overrides with a real VFS.
//     (getOrCreateFreestandingFunc keeps any user/OS-provided definition.)
// ============================================================================
void FreestandingCodeGen::emitFreestandingFileIsDir(
    llvm::Type* i32Ty, llvm::Type* ptrTy)
{
    llvm::FunctionType* ft = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(
        cg_.module_.get(), *cg_.context_, "sad_file_is_dir", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    cg_.builder_->SetInsertPoint(bb);
    cg_.builder_->CreateRet(llvm::ConstantInt::get(i32Ty, 0));
    cg_.builder_->restoreIP(savedIP);
}


// ============================================================================
// 20. __udivdi3(n, d) — Unsigned 64-bit division via binary long division.
//     (AR) خوارزمية القسمة المطوّلة الثنائيّة: تمرّ على 64 بتًّا من الأعلى للأدنى،
//          تُدخِل كلّ بتّ في الباقي وتطرح المقسوم-عليه عند التجاوز. تستعمل فقط
//          إزاحات/جمع/طرح/مقارنات i64 (تُخفَّض ضمنيًّا على i686 — لا udiv i64 —
//          فلا تتكرّر ذاتيًّا). قسمة على صفر: سلوك غير معرّف (كالعتاد) لكنّها تنتهي.
//     (EN) Binary long division: iterate 64 bits MSB→LSB, shift each bit into the
//          remainder and subtract the divisor on overflow. Uses only i64 shifts/
//          add/sub/compares (lowered inline on i686 — no i64 udiv — hence no
//          self-recursion). Division by zero is UB (like hardware) but terminates.
// ============================================================================
void FreestandingCodeGen::emitFreestandingUdivdi3(llvm::Type* i64Ty) {
    llvm::FunctionType* ft = llvm::FunctionType::get(i64Ty, {i64Ty, i64Ty}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "__udivdi3", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* loop  = llvm::BasicBlock::Create(*cg_.context_, "loop", fn);
    llvm::BasicBlock* exit  = llvm::BasicBlock::Create(*cg_.context_, "exit", fn);

    llvm::Value* n = fn->getArg(0);
    llvm::Value* d = fn->getArg(1);
    llvm::Constant* zero = llvm::ConstantInt::get(i64Ty, 0);
    llvm::Constant* one  = llvm::ConstantInt::get(i64Ty, 1);
    llvm::Constant* i63  = llvm::ConstantInt::get(i64Ty, 63);

    cg_.builder_->SetInsertPoint(entry);
    cg_.builder_->CreateBr(loop);

    // (AR) الحلقة: i من 63 إلى 0، q الحاصل، r الباقي الجاري
    cg_.builder_->SetInsertPoint(loop);
    llvm::PHINode* i = cg_.builder_->CreatePHI(i64Ty, 2, "i");
    llvm::PHINode* q = cg_.builder_->CreatePHI(i64Ty, 2, "q");
    llvm::PHINode* r = cg_.builder_->CreatePHI(i64Ty, 2, "r");
    i->addIncoming(i63, entry);
    q->addIncoming(zero, entry);
    r->addIncoming(zero, entry);

    llvm::Value* sh     = cg_.builder_->CreateLShr(n, i, "sh");        // n >> i
    llvm::Value* bit    = cg_.builder_->CreateAnd(sh, one, "bit");     // & 1
    llvm::Value* rshl   = cg_.builder_->CreateShl(r, one, "rshl");     // r << 1
    llvm::Value* r1     = cg_.builder_->CreateOr(rshl, bit, "r1");     // | bit
    llvm::Value* ge     = cg_.builder_->CreateICmpUGE(r1, d, "ge");    // r1 >= d
    llvm::Value* rsub   = cg_.builder_->CreateSub(r1, d, "rsub");      // r1 - d
    llvm::Value* onebit = cg_.builder_->CreateShl(one, i, "onebit");   // 1 << i
    llvm::Value* qset   = cg_.builder_->CreateOr(q, onebit, "qset");   // q | (1<<i)
    llvm::Value* r2     = cg_.builder_->CreateSelect(ge, rsub, r1, "r2");
    llvm::Value* q2     = cg_.builder_->CreateSelect(ge, qset, q, "q2");
    llvm::Value* isZero = cg_.builder_->CreateICmpEQ(i, zero, "isZero");
    llvm::Value* inext  = cg_.builder_->CreateSub(i, one, "inext");
    i->addIncoming(inext, loop);
    q->addIncoming(q2, loop);
    r->addIncoming(r2, loop);
    cg_.builder_->CreateCondBr(isZero, exit, loop);

    // (AR) exit مسبوقة حصريًّا بـ loop، فـ q2 يهيمن عليها (SSA سليم)
    cg_.builder_->SetInsertPoint(exit);
    cg_.builder_->CreateRet(q2);
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 21. __umoddi3(n, d) — Unsigned 64-bit remainder = n - (n / d) * d.
//     (AR) ضرب i64 يُخفَّض ضمنيًّا على i686 (لا __muldi3)، فلا حاجة لحلقة ثانية.
//     (EN) i64 mul lowers inline on i686 (no __muldi3), so no second loop needed.
// ============================================================================
void FreestandingCodeGen::emitFreestandingUmoddi3(llvm::Type* i64Ty) {
    llvm::FunctionType* ft = llvm::FunctionType::get(i64Ty, {i64Ty, i64Ty}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "__umoddi3", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    cg_.builder_->SetInsertPoint(entry);

    llvm::Value* n = fn->getArg(0);
    llvm::Value* d = fn->getArg(1);
    llvm::Function* udiv = cg_.module_->getFunction("__udivdi3");
    if (!udiv) { cg_.builder_->restoreIP(savedIP); return; }
    llvm::Value* q   = cg_.builder_->CreateCall(udiv, {n, d}, "q");
    llvm::Value* qd  = cg_.builder_->CreateMul(q, d, "qd");
    llvm::Value* rem = cg_.builder_->CreateSub(n, qd, "rem");
    cg_.builder_->CreateRet(rem);
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 22. __divdi3(a, b) — Signed 64-bit division via unsigned core + sign fold.
//     (AR) sa/sb أقنعة الإشارة (ashr 63)؛ القيم المطلقة (a^sa)-sa؛ إشارة الحاصل
//          sa^sb. لا يستعمل sdiv/udiv i64 مباشرةً (يستدعي __udivdi3 البرمجيّ).
//     (EN) sa/sb are sign masks (ashr 63); abs = (a^sa)-sa; quotient sign sa^sb.
// ============================================================================
void FreestandingCodeGen::emitFreestandingDivdi3(llvm::Type* i64Ty) {
    llvm::FunctionType* ft = llvm::FunctionType::get(i64Ty, {i64Ty, i64Ty}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "__divdi3", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    cg_.builder_->SetInsertPoint(entry);

    llvm::Value* a = fn->getArg(0);
    llvm::Value* b = fn->getArg(1);
    llvm::Constant* c63 = llvm::ConstantInt::get(i64Ty, 63);
    llvm::Value* sa = cg_.builder_->CreateAShr(a, c63, "sa");
    llvm::Value* sb = cg_.builder_->CreateAShr(b, c63, "sb");
    llvm::Value* ua = cg_.builder_->CreateSub(cg_.builder_->CreateXor(a, sa), sa, "ua");
    llvm::Value* ub = cg_.builder_->CreateSub(cg_.builder_->CreateXor(b, sb), sb, "ub");
    llvm::Function* udiv = cg_.module_->getFunction("__udivdi3");
    if (!udiv) { cg_.builder_->restoreIP(savedIP); return; }
    llvm::Value* uq = cg_.builder_->CreateCall(udiv, {ua, ub}, "uq");
    llvm::Value* s  = cg_.builder_->CreateXor(sa, sb, "s");
    llvm::Value* res = cg_.builder_->CreateSub(cg_.builder_->CreateXor(uq, s), s, "res");
    cg_.builder_->CreateRet(res);
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 23. __moddi3(a, b) — Signed 64-bit remainder; sign follows the dividend.
//     (AR) الباقي الموقَّع يأخذ إشارة المقسوم (sa): (|a| % |b|) ثمّ (^sa)-sa.
//     (EN) Signed remainder takes the dividend sign (sa): (|a| % |b|) then (^sa)-sa.
// ============================================================================
void FreestandingCodeGen::emitFreestandingModdi3(llvm::Type* i64Ty) {
    llvm::FunctionType* ft = llvm::FunctionType::get(i64Ty, {i64Ty, i64Ty}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "__moddi3", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    cg_.builder_->SetInsertPoint(entry);

    llvm::Value* a = fn->getArg(0);
    llvm::Value* b = fn->getArg(1);
    llvm::Constant* c63 = llvm::ConstantInt::get(i64Ty, 63);
    llvm::Value* sa = cg_.builder_->CreateAShr(a, c63, "sa");
    llvm::Value* sb = cg_.builder_->CreateAShr(b, c63, "sb");
    llvm::Value* ua = cg_.builder_->CreateSub(cg_.builder_->CreateXor(a, sa), sa, "ua");
    llvm::Value* ub = cg_.builder_->CreateSub(cg_.builder_->CreateXor(b, sb), sb, "ub");
    llvm::Function* umod = cg_.module_->getFunction("__umoddi3");
    if (!umod) { cg_.builder_->restoreIP(savedIP); return; }
    llvm::Value* ur = cg_.builder_->CreateCall(umod, {ua, ub}, "ur");
    llvm::Value* res = cg_.builder_->CreateSub(cg_.builder_->CreateXor(ur, sa), sa, "res");
    cg_.builder_->CreateRet(res);
    cg_.builder_->restoreIP(savedIP);
}

} // namespace LLVM
} // namespace Sad
