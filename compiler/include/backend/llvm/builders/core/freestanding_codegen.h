/*
 * ============================================================================
 * FreestandingCodeGen - LLVM IR for freestanding builtins (no-OS environment)
 * Phase 7 Step 16 - 19 methods
 * ============================================================================
 */
#ifndef SAD_LLVM_FREESTANDING_CODEGEN_H
#define SAD_LLVM_FREESTANDING_CODEGEN_H

#include <llvm/IR/Type.h>

namespace Sad { namespace LLVM {

class LLVMCodeGen;

class FreestandingCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit FreestandingCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    FreestandingCodeGen(const FreestandingCodeGen &) = delete;
    FreestandingCodeGen &operator=(const FreestandingCodeGen &) = delete;

    // (AR) هل الهدف «معدن عارٍ» (بلا نظام تشغيل) — أي تُنفَّذ الشيفرة في الحلقة 0؟
    //      المميِّز الواحد لكلّ جسور العتاد المباشرة في وقت التشغيل الحرّ (منافذ
    //      الدخل/الخرج، cli/hlt): على هدف بنظام تشغيل يعمل البرنامج عمليّةَ مستخدم
    //      في الحلقة 3، فالتعليمات الممتازة تُثير خطأ حماية عامّ (#GP) ⇒ SIGSEGV،
    //      فيلزم مسار مستضاف بديل (libc) بدل لمس العتاد.
    // (EN) Is the target bare metal (no OS) — i.e. does the code run in ring 0?
    //      The single discriminator for every direct hardware bridge in the
    //      freestanding runtime (port I/O, cli/hlt): on a target with an OS the
    //      program is a ring-3 userspace process where privileged instructions
    //      raise a general protection fault (#GP) → SIGSEGV, so a hosted (libc)
    //      path must be emitted instead of touching hardware.
    bool targetIsBareMetal() const;

    void emitFreestandingRuntime();
    void emitFreestandingMalloc(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingFree(llvm::Type *ptrTy, llvm::Type *voidTy);
    void emitFreestandingMemcpy(llvm::Type *i8Ty, llvm::Type *sizeTy, llvm::Type *ptrTy);
    void emitFreestandingMemmove(llvm::Type *i8Ty, llvm::Type *sizeTy, llvm::Type *ptrTy);
    void emitFreestandingMemset(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *sizeTy, llvm::Type *ptrTy);
    void emitFreestandingMemcmp(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *sizeTy, llvm::Type *ptrTy);
    void emitFreestandingStrlen(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingStrcmp(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingStrcpy(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingStrcat(llvm::Type *ptrTy);
    void emitFreestandingRealloc(llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingCalloc(llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingSerialPutc(llvm::Type *i8Ty, llvm::Type *voidTy);
    void emitFreestandingPrintf(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingSprintf(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingPow(llvm::Type *dblTy);
    void emitFreestandingSerialPuts(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingSerialPutInt(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingItoa(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingFtoa(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingXtoa(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingPanic(llvm::Type *i64Ty, llvm::Type *voidTy);
    // (AR) مقسِّمات 64-بت البرمجيّة — libgcc/compiler-rt غائبة على i686 الحرّ،
    //      فالخلفيّة تُخفِّض «قسمة/باقي i64» إلى استدعاء __udivdi3/__umoddi3/
    //      __divdi3/__moddi3. نوفّرها بخوارزميّة قسمة مطوّلة ثنائيّة (بلا udiv i64
    //      حتّى لا تتكرّر ذاتيًّا على i686) + مغلِّفات إشارة.
    // (EN) Software 64-bit dividers — libgcc/compiler-rt are absent on freestanding
    //      i686, so the backend lowers «i64 div/rem» to __udivdi3/__umoddi3/
    //      __divdi3/__moddi3 calls. Provide them via binary long-division (no i64
    //      udiv, to avoid self-recursion on i686) + sign wrappers.
    void emitFreestandingUdivdi3(llvm::Type *i64Ty);
    void emitFreestandingUmoddi3(llvm::Type *i64Ty);
    void emitFreestandingDivdi3(llvm::Type *i64Ty);
    void emitFreestandingModdi3(llvm::Type *i64Ty);
    // (AR) time — الوقت الحالي عبر ساعة الوقت الحقيقي (RTC/CMOS) بلا مكتبة C
    // (EN) time — current wall-clock time via the CMOS Real-Time Clock, no libc
    void emitFreestandingTime(llvm::Type *i8Ty, llvm::Type *i16Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    // (AR) sad_file_is_dir — كعب ضعيف لمدمَج «هل_مجلد» (تتجاوزه النواة المضيفة)
    // (EN) sad_file_is_dir — weak stub for the «هل_مجلد» builtin (host kernel overrides)
    void emitFreestandingFileIsDir(llvm::Type *i32Ty, llvm::Type *ptrTy);
};

}} // namespace Sad::LLVM
#endif
