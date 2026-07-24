/*
 * ============================================================================
 * FreestandingCodeGen - LLVM IR for freestanding builtins (no-OS environment)
 * Phase 7 Step 16 - 19 methods
 * ============================================================================
 */
#ifndef SAD_LLVM_FREESTANDING_CODEGEN_H
#define SAD_LLVM_FREESTANDING_CODEGEN_H

#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/TargetParser/Triple.h>

#include <cstdint>

namespace Sad { namespace LLVM {

class LLVMCodeGen;

// ============================================================================
// (AR) بيئة جسور العتاد — المميِّز الوحيد لكلّ توليد يلمس العتاد أو نظام التشغيل
//      في الوضع الحرّ. راية «--حرّ» وحدها لا تكفي: هي تصف **غياب المكتبة
//      القياسيّة**، لا **حلقة الامتياز** ولا **المعمارية**. الجسور تختلف بالثلاثة.
// (EN) Hardware-bridge environment — the single discriminator for every emission
//      that touches hardware or the OS in freestanding mode. The «--حرّ» flag
//      alone is not enough: it describes the *absence of a standard library*,
//      not the *privilege ring* nor the *architecture*. Bridges differ by all.
// ============================================================================
enum class HwBridgeProfile
{
    // (AR) معدن عارٍ على x86/x86_64: الحلقة 0 — منافذ الدخل/الخرج وcli/hlt متاحة.
    // (EN) Bare metal on x86/x86_64: ring 0 — port I/O and cli/hlt are available.
    BareMetalPortIO,
    // (AR) معدن عارٍ على معمارية بلا جسر معروف (aarch64/riscv64/...): تعليمات
    //      inb/outb غير موجودة أصلًا، والمنفذ التسلسليّ يُخاطَب بذاكرة مُهيَّأة
    //      خاصّة باللوحة. نبثّ أكعابًا ضعيفة محايدة يتجاوزها دعم اللوحة (BSP).
    // (EN) Bare metal on an architecture with no known bridge: inb/outb do not
    //      exist and the UART is board-specific MMIO. Emit neutral weak stubs
    //      for the board support package to override.
    BareMetalStub,
    // (AR) لينكس بمعمارية لها نداء نظام مبثوث (x86_64: syscall، i386: int 0x80):
    //      الحلقة 3 — نخاطب النواة مباشرةً، فيعمل الوضع الحرّ **مع libc وبدونها**.
    // (EN) Linux on an architecture with an inline syscall (x86_64: syscall,
    //      i386: int 0x80): ring 3 — talk to the kernel directly, so freestanding
    //      works both with and without libc.
    LinuxSyscall,
    // (AR) نظام تشغيل آخر (ويندوز/ماك) أو لينكس بمعمارية لا نبثّ لها نداء نظام:
    //      نترك الرموز للمكتبة القياسيّة/CRT يحلّها الرابط.
    // (EN) Another OS (Windows/macOS), or Linux on an architecture we do not emit
    //      a syscall for: leave the symbols for the libc/CRT linker resolution.
    HostedLibc
};

// ============================================================================
// (AR) نداءات النظام المستعملة في الجسور — التعداد يمنع تناثر الأرقام السحريّة،
//      والترجمة إلى رقم تتمّ في مكان واحد لأنّ الترقيم يختلف بين x86_64 وi386.
// (EN) The syscalls used by the bridges — the enum keeps magic numbers out of
//      the emitters; the mapping lives in one place because x86_64 and i386
//      number them differently.
// ============================================================================
enum class LinuxSyscallId
{
    Write,     // (AR) write(fd, buf, len)
    Time,      // (AR) time(tloc) — ثوانٍ منذ 1970
    ExitGroup  // (AR) exit_group(status) — إنهاء العمليّة بكاملها
};

// ============================================================================
// (AR) تصنيف ثالوث هدف إلى بيئة جسور. **دالّة حرّة عمدًا**: يستشيرها السائق أيضًا
//      ليطابق تشخيصُه ما تفعله الخلفيّة فعلًا؛ تكرار المحكّ في موضعين يُنتج
//      تحذيرًا يناقض التوليد.
//      ⚠️ يجب أن يكون الثالوث **مُطبَّعًا** (llvm::Triple::normalize): الثالوث
//      ثلاثيّ المكوّنات مثل "x86_64-linux-gnu" يُقرأ مكوّنه الثاني بائعًا لا
//      نظامًا، فيعود getOS() بـUnknownOS ⇒ لينكس يُصنَّف «معدنًا» خطأً.
// (EN) Classify a target triple into a bridge profile. Deliberately a free
//      function: the driver consults it too, so its diagnostics match what the
//      backend actually emits — duplicating the test would let a warning
//      contradict codegen. ⚠️ The triple must be normalized: a 3-component
//      triple reads its second field as vendor, not OS.
// ============================================================================
HwBridgeProfile classifyHwBridgeProfile(const llvm::Triple &normalizedTriple);

class FreestandingCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit FreestandingCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    FreestandingCodeGen(const FreestandingCodeGen &) = delete;
    FreestandingCodeGen &operator=(const FreestandingCodeGen &) = delete;

    // (AR) بيئة الجسور المستنتَجة من ثالوث الوحدة (نظامًا **ومعمارية**).
    // (EN) The bridge profile inferred from the module triple (OS *and* arch).
    HwBridgeProfile hwBridgeProfile() const;

    // (AR) يبثّ نداء نظام لينكس مضمَّنًا ويعيد قيمة الإرجاع بعرض كلمة الهدف.
    //      يقبل حتّى ثلاثة وسائط (كافية لكلّ جسورنا). الوسائط تُحوَّل تلقائيًّا إلى
    //      عرض الكلمة (i64 على x86_64، i32 على i386) — المؤشّرات عبر ptrtoint.
    //      يعيد nullptr إن لم يكن للهدف واصف نداء نظام.
    // (EN) Emits an inline Linux syscall and returns its result at target word
    //      width. Takes up to three arguments (enough for every bridge here);
    //      arguments are coerced to the word type (pointers via ptrtoint).
    //      Returns nullptr when the target has no syscall ABI.
    llvm::Value *emitLinuxSyscall(LinuxSyscallId id,
                                  llvm::ArrayRef<llvm::Value *> args);

    // (AR) يبثّ حلقة كتابة كاملة على المخرج القياسيّ: تعالج **الكتابة الجزئيّة**
    //      و**المقاطعة بإشارة** (EINTR). نداء write خامّ بلا حلقة يُسقط بايتات
    //      صامتةً إن وصلت إشارة — وهو ما كانت putchar المخزَّنة تخفيه.
    //      يفترض المستدعي أنّ البيئة LinuxSyscall، ويترك مؤشّر الإدراج في كتلة
    //      «انتهى» صالحة للمتابعة.
    // (EN) Emits a full write loop to stdout handling partial writes and EINTR.
    //      A raw one-shot write silently drops bytes when a signal lands — which
    //      buffered putchar used to hide. Assumes a LinuxSyscall environment and
    //      leaves the insert point in a valid «done» block.
    void emitLinuxWriteAll(llvm::Value *buf, llvm::Value *len);

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
