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
#include <llvm/TargetParser/Triple.h>

namespace Sad {
namespace LLVM {

namespace {

// ============================================================================
// (AR) أرقام نداءات لينكس وصيغ التجميع — مجمّعة هنا لا متناثرة في البواثّ.
//      الترقيم يختلف بين المعماريّتين، فأيّ خلط = نداء نظام آخر تمامًا.
// (EN) Linux syscall numbers and asm templates — gathered here, not scattered
//      across the emitters. The numbering differs per architecture; mixing them
//      up silently invokes a completely different syscall.
// ============================================================================
struct LinuxSyscallAbi
{
    const char *instruction;    // (AR) التعليمة التي تدخل النواة
    const char *returnReg;      // (AR) قيد سجلّ الإرجاع (وهو أيضًا سجلّ الرقم)
    const char *argRegs[3];     // (AR) قيود سجلّات الوسائط بالترتيب
    const char *clobbers;       // (AR) السجلّات التي تدهسها التعليمة (تُلحَق آخرًا)
    long long   write;
    long long   time;
    long long   exitGroup;
};

// (AR) دهس الرايات إلزاميّ في كلا الواصفين: نداء النظام يعبر حدود النواة
//      فتعود EFLAGS مُعدَّلة (النواة تفرض DF=0، وx86_64 تستعيد rflags من r11).
//      بدونه يُبقي LLVM مقارنةً قبل النداء وفرعَها بعده — أثبتته تجربة على llc —
//      فينقلب الفرع. clang يبثّ هذه الثلاثة لكلّ asm في C.
// (EN) The flags clobber is mandatory in both ABIs: a syscall crosses into the
//      kernel and returns with EFLAGS modified (the kernel forces DF=0; x86_64
//      restores rflags from r11). Without it LLVM keeps a compare before the
//      call and its branch after it — demonstrated on llc — and the branch
//      flips. clang emits these three for every C-level asm.
#define SAD_ASM_FLAGS_CLOBBER_BARE "~{dirflag},~{fpsr},~{flags}"
#define SAD_ASM_FLAGS_CLOBBER "," SAD_ASM_FLAGS_CLOBBER_BARE

// (AR) x86_64: التعليمة `syscall` تدهس rcx وr11 (تحفظ فيهما rip وrflags).
// (EN) x86_64: the `syscall` instruction clobbers rcx and r11 (rip/rflags).
constexpr LinuxSyscallAbi kAbiX86_64 = {
    "syscall", "={rax}", {"{rdi}", "{rsi}", "{rdx}"},
    ",~{rcx},~{r11},~{memory}" SAD_ASM_FLAGS_CLOBBER,
    /*write=*/1, /*time=*/201, /*exitGroup=*/231};

// (AR) i386: البوّابة التقليديّة `int 0x80` ($$ تُنتج $ واحدًا في نصّ التجميع).
//      تستعمل ebx وسيطًا أوّل؛ وهو سجلّ القاعدة في شيفرة PIC — لكنّ LLVM يحفظه
//      ويستعيده تلقائيًّا حول كتلة التجميع (مؤكَّد تجريبيًّا بـllc تحت PIC)، فلا
//      يلزم افتراض بناء ساكن.
// (EN) i386: the classic `int 0x80` gate ($$ yields one literal $). It uses ebx
//      as the first argument — the PIC base register — but LLVM saves and
//      restores it around the asm block (verified with llc under PIC), so no
//      non-PIC assumption is needed.
constexpr LinuxSyscallAbi kAbiI386 = {
    "int $$0x80", "={eax}", {"{ebx}", "{ecx}", "{edx}"},
    ",~{memory}" SAD_ASM_FLAGS_CLOBBER,
    /*write=*/4, /*time=*/13, /*exitGroup=*/252};

// (AR) واصف المخرج القياسيّ في لينكس — ثابت مسمّى لا رقم عارٍ في النداء.
// (EN) The Linux standard-output file descriptor — a named constant.
constexpr long long kStdoutFileDescriptor = 1;
// (AR) رمز خطأ لينكس EINTR كما يعود سالبًا من نداء النظام الخامّ.
// (EN) The Linux EINTR error as a raw syscall returns it (negated).
constexpr long long kErrnoEIntr = -4;
// (AR) حالة خروج بديلة حين يكون رمز سبب الهلع صفرًا: صفرٌ يعني «نجاح».
// (EN) Fallback exit status when the panic reason code is zero (0 means success).
constexpr long long kPanicFallbackStatus = 1;
// (AR) قناع حالة الخروج: النواة تحتفظ بالبايت الأدنى فقط.
// (EN) Exit-status mask: the kernel keeps only the low byte.
constexpr long long kExitStatusMask = 0xFF;

// (AR) الواصف الموافق للمعمارية، أو nullptr إن لم نبثّ لها نداءً مضمَّنًا.
// (EN) The ABI for this architecture, or nullptr if we emit no inline syscall.
const LinuxSyscallAbi *linuxSyscallAbi(const llvm::Triple &triple)
{
    switch (triple.getArch())
    {
    case llvm::Triple::x86_64: return &kAbiX86_64;
    case llvm::Triple::x86:    return &kAbiI386;
    default:                   return nullptr;
    }
}

// (AR) هل تملك هذه المعمارية منافذ دخل/خرج معزولة (inb/outb)؟ x86 وحدها.
// (EN) Does this architecture have isolated port I/O (inb/outb)? x86 only.
bool archHasPortIO(const llvm::Triple &triple)
{
    return triple.getArch() == llvm::Triple::x86 ||
           triple.getArch() == llvm::Triple::x86_64;
}

// (AR) هل للمعمارية تعليمة «انتظر مقاطعة» توقف المعالج بلا امتياز حلقة 0 خاصّ؟
//      تُستعمل في حلقة الهلع على المعدن غير x86 بدل دوران يحرق المعالج.
// (EN) Does the architecture have a wait-for-interrupt instruction usable in a
//      bare-metal halt loop, instead of a spin that burns the CPU?
bool archHasWaitForInterrupt(const llvm::Triple &triple)
{
    switch (triple.getArch())
    {
    case llvm::Triple::aarch64:
    case llvm::Triple::aarch64_be:
    case llvm::Triple::arm:
    case llvm::Triple::thumb:
    case llvm::Triple::riscv32:
    case llvm::Triple::riscv64:
        return true;
    default:
        return false;
    }
}

// (AR) ثالوث الوحدة مُطبَّعًا. التطبيع إلزاميّ: الثالوث ثلاثيّ المكوّنات (مثل
//      "x86_64-linux-gnu") يُقرأ مكوّنه الثاني **بائعًا لا نظامًا**، فيعود
//      getOS() بـUnknownOS ⇒ يُصنَّف لينكس «معدنًا» خطأً. normalize يُدرج البائع
//      الناقص ("x86_64-unknown-linux-gnu").
// (EN) The module triple, normalized. Mandatory: a 3-component triple has its
//      second component read as the *vendor*, not the OS, so getOS() returns
//      UnknownOS and Linux would be misclassified as bare metal.
llvm::Triple normalizedTriple(const llvm::Module &mod)
{
    return llvm::Triple(llvm::Triple::normalize(mod.getTargetTriple()));
}

} // namespace

// ============================================================================
// (AR) المميِّز الواحد لجسور العتاد — التوثيق في الترويسة. دلاليّ عبر llvm::Triple
//      لا مطابقة نصّيّة، ويقرأ **النظام والمعمارية معًا**: نظام غير معروف = معدن
//      عارٍ (i686-unknown-elf لنواة النحلة، *-unknown-none، *-none-elf)، ثمّ
//      تفصل المعمارية بين معدن بمنافذ ومعدن بلا جسر معروف.
// (EN) The single discriminator for hardware bridges — documented in the header.
//      Semantic via llvm::Triple, not a substring match, and it reads *OS and
//      architecture together*: an unknown OS means bare metal, then the arch
//      splits port-I/O bare metal from bare metal with no known bridge.
// ============================================================================
HwBridgeProfile classifyHwBridgeProfile(const llvm::Triple &triple) {
    if (triple.getOS() == llvm::Triple::UnknownOS)
        return archHasPortIO(triple) ? HwBridgeProfile::BareMetalPortIO
                                     : HwBridgeProfile::BareMetalStub;

    if (triple.isOSLinux() && linuxSyscallAbi(triple) != nullptr)
        return HwBridgeProfile::LinuxSyscall;

    return HwBridgeProfile::HostedLibc;
}

HwBridgeProfile FreestandingCodeGen::hwBridgeProfile() const {
    // (AR) بلا وحدة لا ثالوث؛ المستضاف هو الافتراض الآمن (لا تعليمات ممتازة).
    // (EN) No module, no triple; hosted is the safe default (no privileged insns).
    if (!cg_.module_) return HwBridgeProfile::HostedLibc;
    return classifyHwBridgeProfile(normalizedTriple(*cg_.module_));
}

// ============================================================================
// (AR) بثّ نداء نظام لينكس مضمَّن — التوثيق في الترويسة.
//      هذا هو ما يجعل الوضع الحرّ **سياديًّا في الحلقة 3**: بدل ترك `time`
//      و`putchar` للمكتبة القياسيّة (فيلزم libc، ويسقط الربط بـ-nostdlib)،
//      نخاطب النواة مباشرةً كما تفعل libc نفسها.
// (EN) Emit an inline Linux syscall — documented in the header. This is what
//      makes freestanding *sovereign in ring 3*: instead of deferring `time`
//      and `putchar` to libc (which forces libc and breaks a -nostdlib link),
//      we talk to the kernel directly, exactly as libc itself does.
// ============================================================================
llvm::Value *FreestandingCodeGen::emitLinuxSyscall(
    LinuxSyscallId id, llvm::ArrayRef<llvm::Value *> args)
{
    // (AR) لا يُستدعى إلّا تحت LinuxSyscall؛ الحرّاس دفاع عميق لا مسار متوقّع.
    // (EN) Only reachable under LinuxSyscall; these guards are defence in depth.
    if (!cg_.module_ || !cg_.builder_) return nullptr;
    const llvm::Triple triple = normalizedTriple(*cg_.module_);
    const LinuxSyscallAbi *abi = linuxSyscallAbi(triple);
    if (!abi || args.size() > 3) return nullptr;

    auto &B = *cg_.builder_;
    // (AR) عرض الكلمة = عرض المؤشّر: i64 على x86_64، i32 على i386. النداء يمرّر
    //      سجلّات بعرض الكلمة، فأيّ خلط عرض يقطع نصف مؤشّر أو يمرّر قمامة.
    // (EN) Word width = pointer width. The syscall passes word-wide registers,
    //      so any width mismatch truncates a pointer or passes garbage.
    llvm::Type *wordTy = cg_.module_->getDataLayout().getIntPtrType(*cg_.context_);

    const long long number = (id == LinuxSyscallId::Write)   ? abi->write
                           : (id == LinuxSyscallId::Time)    ? abi->time
                                                             : abi->exitGroup;

    llvm::SmallVector<llvm::Type *, 4> paramTys;
    llvm::SmallVector<llvm::Value *, 4> callArgs;
    paramTys.push_back(wordTy);
    callArgs.push_back(llvm::ConstantInt::get(wordTy, number));

    std::string constraints = abi->returnReg;
    constraints += ",";
    constraints += abi->returnReg + 1; // (AR) نفس السجلّ دخلًا: أسقط '=' البادئة

    for (size_t i = 0; i < args.size(); ++i)
    {
        llvm::Value *v = args[i];
        if (v->getType()->isPointerTy())
            v = B.CreatePtrToInt(v, wordTy);
        else
            v = B.CreateZExtOrTrunc(v, wordTy);
        paramTys.push_back(wordTy);
        callArgs.push_back(v);
        constraints += ",";
        constraints += abi->argRegs[i];
    }
    constraints += abi->clobbers;

    llvm::InlineAsm *asmCall = llvm::InlineAsm::get(
        llvm::FunctionType::get(wordTy, paramTys, false),
        abi->instruction, constraints, /*hasSideEffects=*/true, /*isAlignStack=*/false);
    // (AR) الاسم «sys.ret» لا «syscall.ret» عمدًا: الأخير يجعل السلسلة "syscall"
    //      تظهر في IR كلّ هدف لينكس حتّى لو حُذف التجميع، فتصير بصمة اختبار كاذبة.
    // (EN) Named «sys.ret», not «syscall.ret», on purpose: the latter makes the
    //      string "syscall" appear in the IR even if the asm were removed, which
    //      would turn it into a false test fingerprint.
    return B.CreateCall(asmCall, callArgs, "sys.ret");
}

// ============================================================================
// (AR) حلقة الكتابة الكاملة — التوثيق في الترويسة.
// (EN) The full write loop — documented in the header.
// ============================================================================
void FreestandingCodeGen::emitLinuxWriteAll(llvm::Value *buf, llvm::Value *len)
{
    if (!cg_.builder_ || !cg_.module_ || !buf || !len) return;
    auto &B = *cg_.builder_;

    // (AR) الحارس أوّلًا — **قبل إنشاء أيّ كتلة**: إن لم يكن للهدف واصف نداء نظام
    //      فلا نبثّ شيئًا ونترك مؤشّر الإدراج في كتلة المستدعي سليمًا (يُكمل هو
    //      بـret). إنشاء كتل ثمّ الاكتشاف متأخّرًا كان يترك كتلًا بلا مُنهٍ وPHI
    //      بسلف زائف ⇒ IR يرفضه المدقّق. (المستدعون محروسون بـLinuxSyscall فهذا
    //      المسار ميت اليوم، لكن الدفاع العميق يجب أن يبثّ IR صالحًا لا فاسدًا.)
    // (EN) Guard first — *before creating any block*: if the target has no syscall
    //      ABI, emit nothing and leave the insert point in the caller's block
    //      (it continues with its own ret). Creating blocks then bailing late left
    //      unterminated blocks and a PHI with a bogus predecessor ⇒ verifier-
    //      invalid IR. (Callers are LinuxSyscall-guarded so this is dead today,
    //      but defence in depth must emit valid IR, not broken IR.)
    if (linuxSyscallAbi(normalizedTriple(*cg_.module_)) == nullptr) return;

    llvm::Function *fn = B.GetInsertBlock()->getParent();
    llvm::Type *wordTy = cg_.module_->getDataLayout().getIntPtrType(*cg_.context_);
    llvm::Type *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);

    llvm::BasicBlock *entry = B.GetInsertBlock();
    llvm::BasicBlock *loop = llvm::BasicBlock::Create(*cg_.context_, "write.loop", fn);
    llvm::BasicBlock *retry = llvm::BasicBlock::Create(*cg_.context_, "write.retry", fn);
    llvm::BasicBlock *advance = llvm::BasicBlock::Create(*cg_.context_, "write.advance", fn);
    llvm::BasicBlock *done = llvm::BasicBlock::Create(*cg_.context_, "write.done", fn);

    llvm::Value *len0 = B.CreateZExtOrTrunc(len, wordTy, "write.len");
    B.CreateBr(loop);

    // (AR) الحلقة تحمل (المؤشّر الجاري، المتبقّي).
    // (EN) The loop carries (current pointer, remaining).
    B.SetInsertPoint(loop);
    llvm::PHINode *ptr = B.CreatePHI(buf->getType(), 3, "write.ptr");
    llvm::PHINode *rem = B.CreatePHI(wordTy, 3, "write.rem");
    ptr->addIncoming(buf, entry);
    rem->addIncoming(len0, entry);

    // (AR) الواصف مضمون الوجود بالحارس أعلاه ⇒ ret غير عدميّ (3 وسائط لا تتجاوز 3).
    // (EN) The ABI is guaranteed by the guard above ⇒ ret is non-null (3 args ≤ 3).
    llvm::Value *ret = emitLinuxSyscall(
        LinuxSyscallId::Write,
        {llvm::ConstantInt::get(wordTy, kStdoutFileDescriptor), ptr, rem});

    // (AR) تقدّم فقط عند ret > 0؛ خلاف ذلك افحص السبب.
    // (EN) Advance only when ret > 0; otherwise inspect the cause.
    llvm::Value *wrote = B.CreateICmpSGT(ret, llvm::ConstantInt::get(wordTy, 0), "write.ok");
    B.CreateCondBr(wrote, advance, retry);

    // (AR) المقاطعة بإشارة (EINTR) تعيد المحاولة بنفس الحالة؛ أيّ خطأ آخر ينهي
    //      الحلقة كي لا تدور أبدًا على واصف تالف.
    // (EN) EINTR retries with the same state; any other error ends the loop so a
    //      broken descriptor cannot spin forever.
    B.SetInsertPoint(retry);
    llvm::Value *interrupted = B.CreateICmpEQ(
        ret, llvm::ConstantInt::get(wordTy, kErrnoEIntr), "write.eintr");
    ptr->addIncoming(ptr, retry);
    rem->addIncoming(rem, retry);
    B.CreateCondBr(interrupted, loop, done);

    B.SetInsertPoint(advance);
    llvm::Value *nextPtr = B.CreateGEP(i8Ty, ptr, ret, "write.next");
    llvm::Value *nextRem = B.CreateSub(rem, ret, "write.left");
    llvm::Value *finished = B.CreateICmpEQ(nextRem, llvm::ConstantInt::get(wordTy, 0), "write.end");
    ptr->addIncoming(nextPtr, advance);
    rem->addIncoming(nextRem, advance);
    B.CreateCondBr(finished, done, loop);

    B.SetInsertPoint(done);
}

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
    // (AR) نوع الحجم = ‎size_t‎ الهدف (بحجم المؤشّر): ‎i32‎ على i686، ‎i64‎ على x86-64.
    //      ⚠️ حرِج لدوالّ الذاكرة الأربع: الخلفيّة تخفّض ‎llvm.memset/memcpy/memmove‎
    //      (و‎expandMemCmp‎) إلى **نداءات مكتبيّة بوسيط ‎size_t‎**. تعريفٌ بـ‎i64‎ ثابت
    //      يقرأ 8 بايت بينما النداء يدفع 4 على هدف 32-بت ⇒ النصف الأعلى قمامة
    //      ⇒ حجم هائل ⇒ دوس ذاكرة صامت. (كان تعريف C خارجيّ يقنّع العيب في
    //      نواة النحلة حتى حُذف في لبنة «صفر C».)
    // (EN) Size type = target size_t (pointer-sized). The backend lowers the
    //      mem* intrinsics to libcalls taking size_t; a hardcoded i64 length
    //      mismatches on 32-bit targets (callee reads 8, caller pushes 4).
    llvm::Type* sizeTy  = cg_.module_->getDataLayout().getIntPtrType(*cg_.context_);
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
    emitFreestandingMemcpy(i8Ty, sizeTy, ptrTy);

    // ========================================================================
    // 3b. memmove — Overlap-safe directional copy (clang -ffreestanding needs it)
    // ========================================================================
    emitFreestandingMemmove(i8Ty, sizeTy, ptrTy);

    // ========================================================================
    // 4. memset — Byte-by-byte set loop
    // ========================================================================
    emitFreestandingMemset(i8Ty, i32Ty, sizeTy, ptrTy);

    // ========================================================================
    // 4b. memcmp — Byte-by-byte unsigned comparison (clang -ffreestanding needs it)
    // ========================================================================
    emitFreestandingMemcmp(i8Ty, i32Ty, sizeTy, ptrTy);

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
    // 24. time — الوقت الحاليّ (ثوانٍ منذ 1970) بجسر يناسب البيئة
    //     (AR) مصدر الوقت يختلف بحلقة الامتياز والمعمارية، لا بوجود libc:
    //          • معدن x86 ⇒ ساعة CMOS عبر منفذَي 0x70/0x71 (نواة النحلة).
    //          • لينكس ⇒ نداء النظام مباشرةً (يعمل مع libc وبدونها؛ منافذ CMOS
    //            ممتازة في الحلقة 3 ⇒ #GP ⇒ SIGSEGV).
    //          • معدن بمعمارية أخرى ⇒ كعب ضعيف يتجاوزه دعم اللوحة.
    //          • نظام آخر ⇒ لا نبثّ شيئًا: يحلّه CRT.
    //     (EN) The time source varies by privilege ring and architecture, not by
    //          the presence of libc: x86 bare metal ⇒ CMOS RTC; Linux ⇒ the
    //          syscall directly (works with and without libc — CMOS ports are
    //          privileged in ring 3 ⇒ #GP ⇒ SIGSEGV); other bare metal ⇒ a weak
    //          stub for the board; another OS ⇒ nothing, the CRT resolves it.
    // ========================================================================
    {
        llvm::Type* i16Ty = llvm::Type::getInt16Ty(*cg_.context_);
        emitFreestandingTime(i8Ty, i16Ty, i64Ty, ptrTy);
    }

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

    // (AR) التوقّف يختلف بالبيئة — الحلقة الفارغة وحدها لم تعد كافية:
    //      • معدن x86 ⇒ cli ثمّ hlt في حلقة. (الدوران الفارغ السابق كان يواصل
    //        خدمة المقاطعات بعد الهلع ويحرق المعالج.)
    //      • لينكس ⇒ exit_group(رمز السبب): إنهاء **نظيف** بحالة خروج تحمل رمز
    //        سبب الهلع، بلا تعليمة ممتازة وبلا رمز libc. هذا يحسم دَين «الهلع في
    //        الحلقة 3» الذي كان مقبولًا اضطرارًا: cli/hlt كانتا تنهيان الهلع
    //        بـ#GP ⇒ SIGSEGV، وطريق abort كان مرفوضًا لأنّه يكسر عقد «حرًّا:
    //        abort ⇒ __sad_panic». نداء النظام لا يفعل أيًّا من الاثنين، ويُبقي
    //        رمز السبب (#248) مرئيًّا في حالة الخروج لا في الطبع وحده.
    //      • معدن بمعمارية لها «انتظر مقاطعة» ⇒ حلقة wfi: توقف المعالج بدل حرقه.
    //      • ما تبقّى (نظام آخر، معمارية مجهولة، wasm) ⇒ llvm.trap: إنهاء **فوريّ**
    //        محايد معماريًّا وغير ممتاز وبلا رمز libc. حلقةٌ فارغة هنا كانت ستبدّل
    //        انهيارًا صاخبًا بتعليقٍ صامت بنسبة معالج ١٠٠٪ — أسوأ للأدوات وللـCI.
    //      (الدالّة تبقى weak_odr — النواة تتجاوزها بسياستها الخاصّة.)
    // (EN) The halt varies by environment: x86 bare metal ⇒ cli;hlt loop (an empty
    //      spin kept serving interrupts after panic and burned the CPU); Linux ⇒
    //      exit_group(reason), a *clean* exit whose status carries the panic reason
    //      — no privileged instruction, no libc symbol. This settles the «ring-3
    //      panic» debt accepted under duress (cli/hlt ended a panic in #GP →
    //      SIGSEGV; routing to abort was rejected as it breaks the gate-tested
    //      «freestanding: abort ⇒ __sad_panic» contract) and keeps the reason code
    //      (#248) visible in the exit status, not only in the printout. Bare metal
    //      with a wait-for-interrupt ⇒ a wfi loop that idles rather than burns.
    //      Everything else (another OS, unknown arch, wasm) ⇒ llvm.trap: an
    //      immediate, arch-neutral, unprivileged, dependency-free stop. An empty
    //      spin there would have traded a loud crash for a silent 100%-CPU hang —
    //      worse for tooling and CI. (Still weak_odr — a kernel overrides it.)
    cg_.builder_->SetInsertPoint(halt);
    const HwBridgeProfile profile = hwBridgeProfile();
    const llvm::Triple triple =
        cg_.module_ ? normalizedTriple(*cg_.module_) : llvm::Triple();

    if (profile == HwBridgeProfile::BareMetalPortIO)
    {
        llvm::InlineAsm* haltAsm = llvm::InlineAsm::get(
            llvm::FunctionType::get(voidTy, {}, false),
            "cli\n\thlt", SAD_ASM_FLAGS_CLOBBER_BARE, true, false);
        cg_.builder_->CreateCall(haltAsm, {});
        cg_.builder_->CreateBr(halt);
    }
    else if (profile == HwBridgeProfile::LinuxSyscall)
    {
        // (AR) حالة الخروج = البايت الأدنى من رمز السبب (النواة تقنّعه)، وإن كان
        //      صفرًا استُبدل بـ1: صفرٌ يعني «نجاح» فيُخفي الهلع عن المستدعي.
        // (EN) Exit status = the low byte of the reason (the kernel masks it), or
        //      1 when that is zero — a zero status means success and would hide
        //      the panic from the caller.
        llvm::Value* masked = cg_.builder_->CreateAnd(
            fn->getArg(0), llvm::ConstantInt::get(i64Ty, kExitStatusMask), "panic.code");
        llvm::Value* status = cg_.builder_->CreateSelect(
            cg_.builder_->CreateICmpEQ(masked, llvm::ConstantInt::get(i64Ty, 0)),
            llvm::ConstantInt::get(i64Ty, kPanicFallbackStatus), masked, "panic.status");
        emitLinuxSyscall(LinuxSyscallId::ExitGroup, {status});
        // (AR) النداء لا يعود؛ القفزة تُبقي الكتلة منتهية بفرع سليم.
        // (EN) The call does not return; the branch keeps the block terminated.
        cg_.builder_->CreateBr(halt);
    }
    else if (profile == HwBridgeProfile::BareMetalStub &&
             archHasWaitForInterrupt(triple))
    {
        // (AR) «wfi» هي التسمية نفسها على ARM/AArch64 وRISC-V.
        // (EN) «wfi» is spelled the same on ARM/AArch64 and RISC-V.
        llvm::InlineAsm* wfiAsm = llvm::InlineAsm::get(
            llvm::FunctionType::get(voidTy, {}, false), "wfi", "", true, false);
        cg_.builder_->CreateCall(wfiAsm, {});
        cg_.builder_->CreateBr(halt);
    }
    else
    {
        cg_.builder_->CreateIntrinsic(llvm::Intrinsic::trap, {}, {});
        cg_.builder_->CreateUnreachable();
    }
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
                        // (AR) صحيح↔صحيح بعرضين مختلفين: يقع حين يصرّح المصدر
                        //      حجمًا i64 بينما التطبيق يأخذ size_t الهدف (i32).
                        //      بدونه يخرج الجسر بنداء مخالف النوع ⇒ verifyModule يسقط.
                        else if (val->getType()->isIntegerTy() && implTy->isIntegerTy())
                            val = bld.CreateZExtOrTrunc(val, implTy);
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
                    else if (result->getType()->isIntegerTy() && fn->getReturnType()->isIntegerTy())
                        retVal = bld.CreateZExtOrTrunc(result, fn->getReturnType());
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

    // (AR) ⚠️ الوسائط/العائد بنوع ‎size_t‎ الهدف (i32 على 32-بت) ليطابق عقد C
    //      ومواقع الاستدعاء المولَّدة؛ الحساب الداخليّ يبقى i64.
    llvm::Type* szTy = cg_.getSizeType();
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {szTy}, false);
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
    llvm::Value* size = cg_.builder_->CreateZExtOrTrunc(fn->getArg(0), i64Ty, "size.i64");

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
    llvm::Type* i8Ty, llvm::Type* sizeTy, llvm::Type* ptrTy)
{
    // (AR) ⚠️ تمييزٌ إلزاميّ بين نوعين لا يجوز خلطهما:
    //      • ‎sizeTy‎ = ‎size_t‎ الهدف (i32 على i686) — للطول والفهارس والإزاحات،
    //        ليطابق النداء المكتبيّ الذي تولّده الخلفيّة من ‎llvm.memcpy‎.
    //      • ‎wordTy‎ = i64 ثابتًا — **عرض بيانات** حلقة الكلمة المضاعفة (8 بايت).
    //      خلطُهما (استعمال sizeTy للحمل/الخزن) ينسخ 4 بايت بخطوة 8 على i686.
    llvm::Type* wordTy = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, sizeTy}, false);
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
    llvm::Value* qCount = cg_.builder_->CreateLShr(n, llvm::ConstantInt::get(sizeTy, 3), "qcount");
    llvm::Value* tStart = cg_.builder_->CreateShl(qCount, llvm::ConstantInt::get(sizeTy, 3), "tstart");

    llvm::Value* hasQ = cg_.builder_->CreateICmpNE(qCount, llvm::ConstantInt::get(sizeTy, 0));
    cg_.builder_->CreateCondBr(hasQ, qLoop, qDone);

    // 8-byte copy loop
    cg_.builder_->SetInsertPoint(qLoop);
    llvm::PHINode* qi = cg_.builder_->CreatePHI(sizeTy, 2, "qi");
    qi->addIncoming(llvm::ConstantInt::get(sizeTy, 0), entry);
    llvm::Value* qOff = cg_.builder_->CreateShl(qi, llvm::ConstantInt::get(sizeTy, 3));
    llvm::Value* srcQ = cg_.builder_->CreateGEP(i8Ty, src, qOff, "srcq");
    llvm::Value* qVal = cg_.builder_->CreateLoad(wordTy, srcQ, "qval");
    llvm::Value* dstQ = cg_.builder_->CreateGEP(i8Ty, dst, qOff, "dstq");
    cg_.builder_->CreateStore(qVal, dstQ);
    llvm::Value* qNext = cg_.builder_->CreateAdd(qi, llvm::ConstantInt::get(sizeTy, 1));
    qi->addIncoming(qNext, qLoop);
    llvm::Value* qCond = cg_.builder_->CreateICmpULT(qNext, qCount);
    cg_.builder_->CreateCondBr(qCond, qLoop, qDone);

    // Tail byte loop
    cg_.builder_->SetInsertPoint(qDone);
    llvm::Value* hasTail = cg_.builder_->CreateICmpULT(tStart, n);
    cg_.builder_->CreateCondBr(hasTail, tailLoop, done);

    cg_.builder_->SetInsertPoint(tailLoop);
    llvm::PHINode* ti = cg_.builder_->CreatePHI(sizeTy, 2, "ti");
    ti->addIncoming(tStart, qDone);
    llvm::Value* srcP = cg_.builder_->CreateGEP(i8Ty, src, ti, "src.p");
    llvm::Value* byte = cg_.builder_->CreateLoad(i8Ty, srcP, "byte");
    llvm::Value* dstP = cg_.builder_->CreateGEP(i8Ty, dst, ti, "dst.p");
    cg_.builder_->CreateStore(byte, dstP);
    llvm::Value* tNext = cg_.builder_->CreateAdd(ti, llvm::ConstantInt::get(sizeTy, 1));
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
    llvm::Type* i8Ty, llvm::Type* i32Ty, llvm::Type* sizeTy, llvm::Type* ptrTy)
{
    // (AR) ⚠️ نوعان لا يجوز خلطهما (انظر ‎emitFreestandingMemcpy‎):
    //      • ‎sizeTy‎ = ‎size_t‎ الهدف — للطول والفهارس والإزاحات.
    //      • ‎wordTy‎ = i64 ثابتًا — عرض نمط التعبئة وخزن حلقة الكلمة المضاعفة.
    //      استعمال sizeTy للنمط على i686 يجعل الإزاحات 32/40/48/56 ‎poison‎
    //      (‎shl‎ بعدد ≥ عرض النوع) ويعبّئ 4 بايت من كلّ 8.
    llvm::Type* wordTy = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {ptrTy, i32Ty, sizeTy}, false);
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
    llvm::Value* v64 = cg_.builder_->CreateZExt(val, wordTy, "v64");
    llvm::Value* fill = v64;
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(wordTy, 8)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(wordTy, 16)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(wordTy, 24)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(wordTy, 32)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(wordTy, 40)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(wordTy, 48)));
    fill = cg_.builder_->CreateOr(fill, cg_.builder_->CreateShl(v64, llvm::ConstantInt::get(wordTy, 56)));

    // qword count = n / 8, tail count = n % 8
    llvm::Value* qCount = cg_.builder_->CreateLShr(n, llvm::ConstantInt::get(sizeTy, 3), "qcount");
    llvm::Value* tStart = cg_.builder_->CreateShl(qCount, llvm::ConstantInt::get(sizeTy, 3), "tstart");

    llvm::Value* hasQ = cg_.builder_->CreateICmpNE(qCount, llvm::ConstantInt::get(sizeTy, 0));
    cg_.builder_->CreateCondBr(hasQ, qLoop, qDone);

    // 8-byte loop
    cg_.builder_->SetInsertPoint(qLoop);
    llvm::PHINode* qi = cg_.builder_->CreatePHI(sizeTy, 2, "qi");
    qi->addIncoming(llvm::ConstantInt::get(sizeTy, 0), entry);
    llvm::Value* qOff = cg_.builder_->CreateShl(qi, llvm::ConstantInt::get(sizeTy, 3));
    llvm::Value* qPtr = cg_.builder_->CreateGEP(i8Ty, dst, qOff, "qptr");
    cg_.builder_->CreateStore(fill, qPtr);
    llvm::Value* qNext = cg_.builder_->CreateAdd(qi, llvm::ConstantInt::get(sizeTy, 1));
    qi->addIncoming(qNext, qLoop);
    llvm::Value* qCond = cg_.builder_->CreateICmpULT(qNext, qCount);
    cg_.builder_->CreateCondBr(qCond, qLoop, qDone);

    // Tail bytes
    cg_.builder_->SetInsertPoint(qDone);
    llvm::Value* hasTail = cg_.builder_->CreateICmpULT(tStart, n);
    cg_.builder_->CreateCondBr(hasTail, tailLoop, done);

    cg_.builder_->SetInsertPoint(tailLoop);
    llvm::PHINode* ti = cg_.builder_->CreatePHI(sizeTy, 2, "ti");
    ti->addIncoming(tStart, qDone);
    llvm::Value* tPtr = cg_.builder_->CreateGEP(i8Ty, dst, ti, "tptr");
    cg_.builder_->CreateStore(val, tPtr);
    llvm::Value* tNext = cg_.builder_->CreateAdd(ti, llvm::ConstantInt::get(sizeTy, 1));
    ti->addIncoming(tNext, tailLoop);
    llvm::Value* tCond = cg_.builder_->CreateICmpULT(tNext, n);
    cg_.builder_->CreateCondBr(tCond, tailLoop, done);

    cg_.builder_->SetInsertPoint(done);
    cg_.builder_->CreateRet(dst);
    cg_.builder_->restoreIP(savedIP);
}

// ============================================================================
// 3b. memmove — Overlap-safe copy: forward if dst<src, else backward.
//     Byte-by-byte (correctness over speed; memmove is rarely a hot path).
//     getOrCreateFreestandingFunc marks it weak_odr + no-builtins + NoInline +
//     OptimizeNone ⇒ LLVM's loop-idiom pass cannot rewrite the loop into a
//     self-referential memmove call (would recurse infinitely).
// ============================================================================
void FreestandingCodeGen::emitFreestandingMemmove(
    llvm::Type* i8Ty, llvm::Type* sizeTy, llvm::Type* ptrTy)
{
    // (AR) ⚠️ الطول ‎size_t‎ الهدف (يطابق النداء المكتبيّ من ‎llvm.memmove‎).
    //      الجسم بايتيّ خالص فلا نوع بيانات منفصل هنا (خلاف memcpy/memset).
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, sizeTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "memmove", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();

    llvm::BasicBlock* entry    = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* checkDir = llvm::BasicBlock::Create(*cg_.context_, "checkdir", fn);
    llvm::BasicBlock* fwdLoop  = llvm::BasicBlock::Create(*cg_.context_, "fwd", fn);
    llvm::BasicBlock* bwdLoop  = llvm::BasicBlock::Create(*cg_.context_, "bwd", fn);
    llvm::BasicBlock* done     = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

    cg_.builder_->SetInsertPoint(entry);
    llvm::Value* dst = fn->getArg(0);
    llvm::Value* src = fn->getArg(1);
    llvm::Value* n   = fn->getArg(2);
    // n == 0 → nothing to copy (also guards the backward loop's n-1 underflow)
    llvm::Value* isZero = cg_.builder_->CreateICmpEQ(n, llvm::ConstantInt::get(sizeTy, 0));
    cg_.builder_->CreateCondBr(isZero, done, checkDir);

    // dst < src ⇒ forward copy is overlap-safe; otherwise copy backward.
    cg_.builder_->SetInsertPoint(checkDir);
    llvm::Value* fwdSafe = cg_.builder_->CreateICmpULT(dst, src, "fwdsafe");
    cg_.builder_->CreateCondBr(fwdSafe, fwdLoop, bwdLoop);

    // Forward loop: i = 0 .. n-1
    cg_.builder_->SetInsertPoint(fwdLoop);
    llvm::PHINode* fi = cg_.builder_->CreatePHI(sizeTy, 2, "fi");
    fi->addIncoming(llvm::ConstantInt::get(sizeTy, 0), checkDir);
    llvm::Value* fSrc  = cg_.builder_->CreateGEP(i8Ty, src, fi, "fsrc");
    llvm::Value* fByte = cg_.builder_->CreateLoad(i8Ty, fSrc, "fbyte");
    llvm::Value* fDst  = cg_.builder_->CreateGEP(i8Ty, dst, fi, "fdst");
    cg_.builder_->CreateStore(fByte, fDst);
    llvm::Value* fNext = cg_.builder_->CreateAdd(fi, llvm::ConstantInt::get(sizeTy, 1));
    fi->addIncoming(fNext, fwdLoop);
    llvm::Value* fCond = cg_.builder_->CreateICmpULT(fNext, n);
    cg_.builder_->CreateCondBr(fCond, fwdLoop, done);

    // Backward loop: index = n-1 .. 0 (phi starts at n, decrement-first)
    cg_.builder_->SetInsertPoint(bwdLoop);
    llvm::PHINode* bj = cg_.builder_->CreatePHI(sizeTy, 2, "bj");
    bj->addIncoming(n, checkDir);
    llvm::Value* bIdx  = cg_.builder_->CreateSub(bj, llvm::ConstantInt::get(sizeTy, 1), "bidx");
    llvm::Value* bSrc  = cg_.builder_->CreateGEP(i8Ty, src, bIdx, "bsrc");
    llvm::Value* bByte = cg_.builder_->CreateLoad(i8Ty, bSrc, "bbyte");
    llvm::Value* bDst  = cg_.builder_->CreateGEP(i8Ty, dst, bIdx, "bdst");
    cg_.builder_->CreateStore(bByte, bDst);
    bj->addIncoming(bIdx, bwdLoop);
    llvm::Value* bCond = cg_.builder_->CreateICmpNE(bIdx, llvm::ConstantInt::get(sizeTy, 0));
    cg_.builder_->CreateCondBr(bCond, bwdLoop, done);

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
    // (AR) ⚠️ العائد ‎size_t‎ الهدف (عقد C): على 32-بت يعيد eax وحده، فلو
    //      أُعلن i64 قرأ المستدعي edx قمامةً. العدّاد الداخليّ i64.
    llvm::Type* szTy = cg_.getSizeType();
    llvm::FunctionType* ft = llvm::FunctionType::get(szTy, {ptrTy}, false);
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
    cg_.builder_->CreateRet(cg_.builder_->CreateZExtOrTrunc(i, szTy, "len.sz"));
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
// 6b. memcmp — Return (unsigned char)a[i] − (unsigned char)b[i] at the first
//     differing byte, else 0. Unsigned per the C memcmp contract (zext, not
//     sext). Length-bounded sibling of strcmp. weak_odr + no-builtins guard
//     against loop-idiom rewriting the loop into a self memcmp call.
// ============================================================================
void FreestandingCodeGen::emitFreestandingMemcmp(
    llvm::Type* i8Ty, llvm::Type* i32Ty, llvm::Type* sizeTy, llvm::Type* ptrTy)
{
    // (AR) ⚠️ الطول ‎size_t‎ الهدف (يطابق نداء ‎memcmp‎ الذي يولّده ‎expandMemCmp‎)
    llvm::FunctionType* ft = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy, sizeTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "memcmp", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();

    llvm::BasicBlock* entry   = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* loop    = llvm::BasicBlock::Create(*cg_.context_, "loop", fn);
    llvm::BasicBlock* differ  = llvm::BasicBlock::Create(*cg_.context_, "differ", fn);
    llvm::BasicBlock* cont    = llvm::BasicBlock::Create(*cg_.context_, "cont", fn);
    llvm::BasicBlock* retZero = llvm::BasicBlock::Create(*cg_.context_, "ret_zero", fn);

    cg_.builder_->SetInsertPoint(entry);
    llvm::Value* a = fn->getArg(0);
    llvm::Value* b = fn->getArg(1);
    llvm::Value* n = fn->getArg(2);
    // n == 0 → equal (also guards the loop's iNext == n exit)
    llvm::Value* isZero = cg_.builder_->CreateICmpEQ(n, llvm::ConstantInt::get(sizeTy, 0));
    cg_.builder_->CreateCondBr(isZero, retZero, loop);

    cg_.builder_->SetInsertPoint(loop);
    llvm::PHINode* i = cg_.builder_->CreatePHI(sizeTy, 2, "i");
    i->addIncoming(llvm::ConstantInt::get(sizeTy, 0), entry);
    llvm::Value* pa = cg_.builder_->CreateGEP(i8Ty, a, i, "pa");
    llvm::Value* ca = cg_.builder_->CreateLoad(i8Ty, pa, "ca");
    llvm::Value* pb = cg_.builder_->CreateGEP(i8Ty, b, i, "pb");
    llvm::Value* cb = cg_.builder_->CreateLoad(i8Ty, pb, "cb");
    llvm::Value* neq = cg_.builder_->CreateICmpNE(ca, cb);
    cg_.builder_->CreateCondBr(neq, differ, cont);

    // Bytes differ — return unsigned difference
    cg_.builder_->SetInsertPoint(differ);
    llvm::Value* ea = cg_.builder_->CreateZExt(ca, i32Ty);
    llvm::Value* eb = cg_.builder_->CreateZExt(cb, i32Ty);
    llvm::Value* diff = cg_.builder_->CreateSub(ea, eb, "diff");
    cg_.builder_->CreateRet(diff);

    // Bytes equal — advance; stop after n bytes
    cg_.builder_->SetInsertPoint(cont);
    llvm::Value* iNext = cg_.builder_->CreateAdd(i, llvm::ConstantInt::get(sizeTy, 1), "inext");
    i->addIncoming(iNext, cont);
    llvm::Value* atEnd = cg_.builder_->CreateICmpEQ(iNext, n);
    cg_.builder_->CreateCondBr(atEnd, retZero, loop);

    cg_.builder_->SetInsertPoint(retZero);
    cg_.builder_->CreateRet(llvm::ConstantInt::get(i32Ty, 0));
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
    // (AR) ⚠️ الوسائط بنوع ‎size_t‎ الهدف (عقد C)؛ الحساب الداخليّ i64.
    llvm::Type* szTy = cg_.getSizeType();
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {ptrTy, szTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "realloc", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    llvm::BasicBlock* notNull = llvm::BasicBlock::Create(*cg_.context_, "not_null", fn);
    llvm::BasicBlock* done    = llvm::BasicBlock::Create(*cg_.context_, "done", fn);

    cg_.builder_->SetInsertPoint(entry);
    llvm::Value* oldPtr = fn->getArg(0);
    llvm::Value* newSz  = cg_.builder_->CreateZExtOrTrunc(fn->getArg(1), i64Ty, "new.size.i64");

    // (AR) تخصيص كتلة جديدة
    // (EN) Allocate new block
    llvm::Function* mallocFn = cg_.module_->getFunction("malloc");
    llvm::Value* newPtr = cg_.builder_->CreateCall(mallocFn,
        {cg_.builder_->CreateZExtOrTrunc(newSz, mallocFn->getFunctionType()->getParamType(0),
                                         "new.size.sz")}, "new.ptr");

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
    // (AR) ‎memcpy‎ يأخذ الطول بنوع ‎size_t‎ الهدف (‎i32‎ على i686)، بينما أحجام
    //      ‎realloc‎ داخليًّا ‎i64‎ ⇒ نطابق النوع صراحةً (اقتطاع آمن: الحجم محصور
    //      بسعة الكومة). بدونها ينهار التحقّق من الأنواع في LLVM.
    // (EN) memcpy takes target size_t; realloc's sizes are i64 — coerce.
    llvm::Type* memcpySizeTy = memcpyFn->getFunctionType()->getParamType(2);
    llvm::Value* copySzArg = (copySz->getType() == memcpySizeTy)
        ? copySz
        : cg_.builder_->CreateZExtOrTrunc(copySz, memcpySizeTy, "copy.size.sz");
    cg_.builder_->CreateCall(memcpyFn, {newPtr, oldPtr, copySzArg});
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
    // (AR) ⚠️ الوسائط بنوع ‎size_t‎ الهدف (عقد C)؛ الحساب الداخليّ i64.
    llvm::Type* szTy = cg_.getSizeType();
    llvm::FunctionType* ft = llvm::FunctionType::get(ptrTy, {szTy, szTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "calloc", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    llvm::Type* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
    cg_.builder_->SetInsertPoint(entry);

    llvm::Value* count = cg_.builder_->CreateZExtOrTrunc(fn->getArg(0), i64Ty, "count.i64");
    llvm::Value* size  = cg_.builder_->CreateZExtOrTrunc(fn->getArg(1), i64Ty, "size.i64");
    llvm::Value* total = cg_.builder_->CreateMul(count, size, "total");

    llvm::Function* mallocFn = cg_.module_->getFunction("malloc");
    llvm::Value* ptr = cg_.builder_->CreateCall(mallocFn,
        {cg_.builder_->CreateZExtOrTrunc(total, mallocFn->getFunctionType()->getParamType(0),
                                         "total.sz")}, "ptr");

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
// 24. time — (AR) ثوانٍ منذ 1970 بالمصدر المناسب للبيئة (CMOS / نداء نظام / كعب)
// ============================================================================
// (AR) يُطبّق التوقيع C: time_t time(time_t*)، **بعقده كاملًا**: إن كان المؤشّر
//      غير عدميّ كُتبت النتيجة عبره أيضًا (كانت النسخة المعدنيّة تتجاهله، فيتباعد
//      سلوك المحرّكين: كود يقرأ ‎*tloc‎ يعمل مستضافًا ويقرأ قمامة حرًّا).
//      المصادر: معدن x86 ⇒ سِجلّات CMOS عبر منفذ الفهرس 0x70 ومنفذ البيانات 0x71
//      بفكّ BCD ثمّ days-from-civil؛ لينكس ⇒ نداء النظام؛ معدن آخر ⇒ صفر.
//      افتراضات CMOS: وضع BCD (افتراض QEMU/العتاد الشائع) وساعة 24، 2000–2099.
// (EN) Implements the C signature time_t time(time_t*) with its *full contract*:
//      when the pointer is non-null the result is stored through it too (the
//      bare-metal version used to ignore it, diverging from hosted behaviour —
//      code reading *tloc worked hosted and read garbage freestanding).
//      Sources: x86 bare metal ⇒ CMOS registers via index port 0x70 / data port
//      0x71, BCD-decoded then days-from-civil; Linux ⇒ the syscall; other bare
//      metal ⇒ zero. CMOS assumes BCD mode, 24h, years 2000–2099.
// ============================================================================
void FreestandingCodeGen::emitFreestandingTime(
    llvm::Type* i8Ty, llvm::Type* i16Ty, llvm::Type* i64Ty, llvm::Type* ptrTy)
{
    const HwBridgeProfile profile = hwBridgeProfile();
    // (AR) نظام تشغيل بلا نداء مبثوث: `time` يبقى تصريحًا خارجيًّا يحلّه CRT.
    // (EN) An OS with no inline syscall: leave `time` external for the CRT.
    if (profile == HwBridgeProfile::HostedLibc) return;

    llvm::FunctionType* ft = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    llvm::Function* fn = getOrCreateFreestandingFunc(cg_.module_.get(), *cg_.context_, "time", ft);
    if (!fn) return;

    auto savedIP = cg_.builder_->saveIP();
    auto& B = *cg_.builder_;
    llvm::Type* voidTy = llvm::Type::getVoidTy(*cg_.context_);

    // ------------------------------------------------------------------------
    // (AR) الجزء المشترك في العقد: إن كان tloc غير عدميّ فاكتب النتيجة عبره ثمّ
    //      أرجِعها. يُستدعى من كلّ فرع بقيمته الخاصّة.
    // (EN) The shared contract tail: if tloc is non-null store the result through
    //      it, then return it. Called by every branch with its own value.
    // ------------------------------------------------------------------------
    auto returnHonouringTloc = [&](llvm::Value* seconds) {
        llvm::BasicBlock* store = llvm::BasicBlock::Create(*cg_.context_, "tloc.store", fn);
        llvm::BasicBlock* done  = llvm::BasicBlock::Create(*cg_.context_, "tloc.done", fn);
        llvm::Value* tloc = fn->getArg(0);
        llvm::Value* isNull = B.CreateICmpEQ(
            tloc, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "tloc.null");
        B.CreateCondBr(isNull, done, store);

        B.SetInsertPoint(store);
        // (AR) time_t بعرض الكلمة على الهدف: i32 على i686، i64 على x86_64.
        //      كتابة i64 ثابتة تدوس 4 بايتات وراء المتغيّر على هدف 32-بت.
        // (EN) time_t is word-sized: i32 on i686, i64 on x86_64. A hardcoded i64
        //      store would smash 4 bytes past the variable on a 32-bit target.
        //      والامتداد بإشارة لا بدونها: قيمة النداء قد تعود سالبة (خطأ)، فامتدادٌ
        //      بلا إشارة يحوّلها إلى عدد ضخم موجب على هدف 32-بت.
        // (EN) Sign-extend, never zero-extend: the syscall may return a negative
        //      error, which zero-extension would turn into a huge positive value.
        llvm::Type* timeTy = cg_.module_->getDataLayout().getIntPtrType(*cg_.context_);
        B.CreateStore(B.CreateSExtOrTrunc(seconds, timeTy, "tloc.val"), tloc);
        B.CreateBr(done);

        B.SetInsertPoint(done);
        B.CreateRet(seconds);
    };

    // ------------------------------------------------------------------------
    // (AR) لينكس: نداء النظام مباشرةً — لا CMOS (ممتاز) ولا libc (تبعيّة).
    //      نمرّر صفرًا لا tloc: تخزين النواة عبر المؤشّر يكرّر ما نفعله أدناه،
    //      ونريد المسار واحدًا لكلّ الجسور (والصفر يعمل ولو كان المؤشّر عدميًّا).
    // (EN) Linux: the syscall directly — no CMOS (privileged), no libc (a dep).
    //      Pass zero rather than tloc: letting the kernel store through the
    //      pointer would duplicate the tail below; one path for every bridge.
    // ------------------------------------------------------------------------
    if (profile == HwBridgeProfile::LinuxSyscall)
    {
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
        B.SetInsertPoint(entry);
        llvm::Value* raw = emitLinuxSyscall(
            LinuxSyscallId::Time,
            {llvm::ConstantInt::get(
                cg_.module_->getDataLayout().getIntPtrType(*cg_.context_), 0)});
        // (AR) دفاع عميق: الفرع مشروط بـLinuxSyscall فالواصف موجود قطعًا، لكن
        //      تمرير nullptr إلى CreateSExtOrTrunc انهيارٌ لا تشخيص.
        // (EN) Defence in depth: this branch implies a syscall ABI exists, but
        //      passing nullptr into CreateSExtOrTrunc would crash, not diagnose.
        if (!raw) raw = llvm::ConstantInt::get(i64Ty, 0);
        returnHonouringTloc(B.CreateSExtOrTrunc(raw, i64Ty, "epoch"));
        cg_.builder_->restoreIP(savedIP);
        return;
    }

    // ------------------------------------------------------------------------
    // (AR) معدن عارٍ بمعمارية بلا جسر معروف: لا inb/outb ولا نداء نظام. كعب ضعيف
    //      يعيد صفرًا كي يرتبط البرنامج، وحزمة دعم اللوحة تتجاوزه بساعتها.
    // (EN) Bare metal with no known bridge: no inb/outb and no syscall. A weak
    //      stub returning zero so the program links; the BSP overrides it.
    // ------------------------------------------------------------------------
    if (profile == HwBridgeProfile::BareMetalStub)
    {
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
        B.SetInsertPoint(entry);
        returnHonouringTloc(llvm::ConstantInt::get(i64Ty, 0));
        cg_.builder_->restoreIP(savedIP);
        return;
    }

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
    returnHonouringTloc(epoch);

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
