/*
 * ============================================================================
 * ProcessesBuiltinsCodeGen — توليدُ LLVM IR لبدائيّاتِ وحدةِ «عمليات»
 * ProcessesBuiltinsCodeGen — LLVM IR for the «عمليات» (processes) primitives
 * ============================================================================
 *
 * (AR) نظيرُ المصرِّفِ لِـ interpreter/src/builtins/builtin_module_processes.cpp.
 *      لا مكتبةَ زمنِ تشغيلٍ تُربَط: البدائيّاتُ تُولَّدُ دوالَّ داخليّةً في الوحدةِ
 *      المُصدَرةِ نفسِها (‏`__sad_proc_*`) تنادي libc مباشرةً — نفسُ نهجِ سائرِ
 *      مدمجاتِ المصرِّف، ولأنّ سطرَ الربطِ (‏llvm_target.cpp) لا يضمُّ إلّا
 *      ‎-lm -lpthread، فأيُّ أرشيفٍ جديدٍ كان سيصير مسارَ فشلٍ حين لا يُعثَرُ عليه.
 *
 *      **ثوابتُ الهدفِ لا ثوابتُ المضيف.** كانت أعلامُ `O_*` وأرقامُ الإشاراتِ
 *      وحجمُ `sigset_t` أرقامًا مُثبَّتةً في الملفّ، فكان المولِّدُ لينكسَ حصرًا.
 *      وهي أرقامٌ **تختلفُ فعلًا** بين لينكس وماك: `O_CREAT` ٠١٠٠ هناك و٠x٢٠٠
 *      هنا، و`F_DUPFD_CLOEXEC` ‏١٠٣٠ مقابل ٦٧. صارت جدولًا يُنتقى بمثلّثِ **الهدف**
 *      (‏`ProcPlatform`) — لأنّ التصريفَ المتقاطعَ يجعلَ مضيفَ البناءِ لا يدلُّ على
 *      شيء. وسطرٌ واحدٌ خاطئٌ منها لا يُفشِلُ الترجمةَ بل يفتحُ الملفَّ بعلمٍ آخر.
 *
 * (EN) The compiler counterpart of the interpreter's processes module. No runtime
 *      library is linked: each primitive is emitted as an internal `__sad_proc_*`
 *      function inside the produced module, calling libc directly — the same
 *      approach every other compiler builtin takes, and required because the link
 *      line (llvm_target.cpp) carries only -lm -lpthread.
 *      Constants come from a per-target table keyed on the **target** triple, not
 *      the build host: O_CREAT, F_DUPFD_CLOEXEC and sigset_t genuinely differ
 *      between Linux and Darwin, and a wrong value opens the file with another flag
 *      rather than failing the build.
 * ============================================================================
 */
#ifndef SAD_LLVM_PROCESSES_BUILTINS_CODEGEN_H
#define SAD_LLVM_PROCESSES_BUILTINS_CODEGEN_H

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include "sir_instruction.h"
#include "error_codes.h"

namespace Sad
{
    namespace LLVM
    {

        class LLVMCodeGen;
        using SIRInstruction = Compiler::SIR::SIRInstruction;

        /// (AR) عائلةُ نظامِ **الهدف** — لا المضيف.
        enum class ProcTargetOS
        {
            Linux,
            Darwin,
            Windows,
            Unsupported
        };

        /// (AR) ثوابتُ ABI للهدف. كلُّ حقلٍ هنا رقمٌ يختلفُ فعلًا بين المنصّات،
        ///      ولا يُكشَفُ خطؤه بالبناءِ بل بسلوكٍ خاطئٍ صامت: علمُ فتحٍ مختلفٌ
        ///      يفتحُ الملفَّ فتحًا آخر، وحجمُ `sigset_t` أصغرُ من الحقيقيّ يجعلُ
        ///      `sigemptyset` تكتبُ خارجَ الحجز. فالمصدرُ هنا رؤوسُ كلِّ منصّةٍ
        ///      نفسُها، مذكورةً عند كلِّ سطرٍ كي يُراجَعَ لا يُصدَّق.
        /// (EN) Target ABI constants. Each field genuinely differs across
        ///      platforms and a wrong value produces silent misbehaviour, never a
        ///      build failure — so every line cites the header it came from.
        struct ProcPlatform
        {
            ProcTargetOS os = ProcTargetOS::Unsupported;

            int64_t oRdOnly = 0;
            int64_t oWrOnly = 0;
            int64_t oCreat = 0;
            int64_t oTrunc = 0;
            int64_t oAppend = 0;
            int64_t oCloexec = 0;   ///< POSIX: O_CLOEXEC — ويندوز: _O_NOINHERIT
            int64_t oBinary = 0;    ///< ويندوز وحدَه (‏POSIX لا يترجمُ الأسطر)
            int64_t openMode = 0;   ///< حقوقُ الإنشاء

            int64_t fDupFdCloexec = 0;

            int64_t eIntr = 0;
            int64_t eChild = 0;

            int64_t sigInt = 0;
            int64_t sigQuit = 0;
            int64_t sigKill = 0;
            int64_t sigPipe = 0;
            int64_t sigIgn = 0;
            int64_t sigSetMask = 0;
            int64_t sigsetBytes = 0; ///< sizeof(sigset_t) على الهدف

            /// (AR) اسمُ دالّةِ موضعِ `errno`. لا اسمَ موحّدًا لها: glibc
            ///      `__errno_location`، وdarwin `__error`، وUCRT `_errno`.
            const char *errnoLocation = "";

            /// (AR) هل عند الهدفِ `pipe2`؟ ماك لا يملكها، فيُصطنَع بديلُها
            ///      `pipe` + `fcntl(F_SETFD, FD_CLOEXEC)` — وهو **غيرُ ذرّيّ**:
            ///      بين النداءين نافذةٌ يرثُ فيها `fork` من خيطٍ آخر طرفًا بلا
            ///      CLOEXEC. حدٌّ معلَنٌ للمنصّة لا اختيارٌ لنا (المفسّرُ يفعلُ
            ///      الشيءَ نفسَه هناك).
            bool hasPipe2 = false;
        };

        // ────────────────────────────────────────────────────────────────────
        // (AR) وسمُ نوعِ المقبضِ في البتّاتِ العليا — **يجبُ أن يطابقَ المفسّرَ
        //      حرفيًّا** (‏builtin_module_processes.cpp)، وإلّا قُرئ مقبضٌ من
        //      محرّكٍ في الآخرِ من نوعٍ خاطئ. وهو هنا في الرأسِ لا في ملفٍّ
        //      واحدٍ كي لا يُنسَخَ بين ملفَّي المولِّد فينحرفَ أحدُهما وحدَه.
        // (EN) The handle kind tag, in the header so the two generator files
        //      cannot drift apart from each other.
        // ────────────────────────────────────────────────────────────────────
        inline constexpr int64_t kKindShift = 56;
        inline constexpr int64_t kKindMask = static_cast<int64_t>(0xFF) << kKindShift;
        inline constexpr int64_t kKindDescriptor = static_cast<int64_t>(0xD0) << kKindShift;
        inline constexpr int64_t kKindProcess = static_cast<int64_t>(0xC1) << kKindShift;

        /// (AR) سعةُ جدولِ المقابضِ الحيّة — دَينٌ معلَن: المفسّرُ بلا حدّ.
        inline constexpr int64_t kHandleSlots = 1024;

        class ProcessesBuiltinsCodeGen
        {
            LLVMCodeGen &cg_;

        public:
            explicit ProcessesBuiltinsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
            ProcessesBuiltinsCodeGen(const ProcessesBuiltinsCodeGen &) = delete;
            ProcessesBuiltinsCodeGen &operator=(const ProcessesBuiltinsCodeGen &) = delete;

            /// (AR) الموزِّع: يُرجع nullptr إن لم يكن الرمزُ من هذه الوحدة.
            /// (EN) Dispatcher: returns nullptr when the opcode is not ours.
            llvm::Value *emitProcessesBuiltin(std::shared_ptr<SIRInstruction> inst);

        private:
            // ────────────────────────────────────────────────────────────────
            // (AR) بناءُ الدوالِّ المساعدةِ الداخليّة (تُنشَأُ مرّةً لكلِّ وحدة)
            // (EN) Internal helper builders (created once per module)
            // ────────────────────────────────────────────────────────────────
            llvm::Function *getOrCreateFail();     ///< __sad_proc_fail(fmt,a,b,c) — يطبع ويخرج
            llvm::Function *getOrCreateNumToStr(); ///< __sad_proc_num2str(i64) → ptr
            llvm::Function *getOrCreateStore();    ///< __sad_proc_store(kind,val) → مقبض
            llvm::Function *getOrCreateFind();     ///< __sad_proc_find(handle,take) → val | -1
            llvm::Function *getOrCreateKindGuard();///< __sad_proc_kind_guard(handle,kind,expected)
            llvm::Function *getOrCreateOpenFd();
            llvm::Function *getOrCreateClose();
            llvm::Function *getOrCreateWait();
            llvm::Function *getOrCreatePipe();
            llvm::Function *getOrCreateSpawn();

            // ────────────────────────────────────────────────────────────────
            // (AR) نظائرُ ويندوز — Win32 + CRT، في processes_builtins_win.cpp.
            //      **ثلاثٌ لا خمس**: `اغلق` و`انبوب` بنيتُهما واحدةٌ في المنصّتين
            //      ولا تختلفان إلّا بالأسماءِ والأعلام، فهما مُعادتا الاستعمالِ
            //      لا منسوختين. والمنسوخُ هو ما يتباعدُ عند أوّلِ تصليحٍ يقعُ في
            //      نسخةٍ دون أخرى. ويفترقُ بنيويًّا الانتظارُ والشوكةُ (لا `waitpid`
            //      ولا `fork` هناك أصلًا) **والفتحُ**: «إلحاق» في CRT مسلكٌ تسلكه
            //      CRT عند الكتابةِ لا خاصّيّةٌ في المقبض، فابنٌ يكتبُ بالمقبضِ
            //      الخامِّ يبدأ من الصفرِ ويدهس. فيُفتَحُ بـ`CreateFileA` بـ
            //      `FILE_APPEND_DATA` كي تكون الدلالةُ في المقبضِ نفسِه.
            //      والمساعداتُ المشتركة (‏fail/num2str/store/find/kind_guard) واحدةٌ
            //      للمنصّتين، وهي موضعُ **دلالةِ المقبض** كلِّها.
            // (EN) Only wait and spawn differ structurally; open/close/pipe differ
            //      merely in names and flags, so they are parameterised, not copied.
            // ────────────────────────────────────────────────────────────────
            llvm::Function *getOrCreateWinErrText();  ///< __sad_proc_winerr(i32) → ptr
            llvm::Function *getOrCreateWinWiden();    ///< __sad_proc_widen(utf8) → UTF-16
            llvm::Function *getOrCreateWinOpenFd();
            llvm::Function *getOrCreateWinQuoteArg(); ///< __sad_proc_winquote(dst,arg) → dst'
            llvm::Function *getOrCreateWinCtrlHandler();
            llvm::Function *getOrCreateWinWait();
            llvm::Function *getOrCreateWinSpawn();

            /// (AR) يبني سلسلةَ تنسيقٍ من نصِّ الخطأِ المولَّد: كلُّ `{اسم}` يصير `%s`
            ///      بالترتيب. الترتيبُ مُعادٌ في `order` كي تُمرَّرَ القيمُ في محلِّها
            ///      ولا تُقلَبَ الرسالةُ حين يختلفُ ترتيبُ الفجواتِ عن ترتيبِ الوسائط.
            /// (EN) Builds a printf format from the generated error text: each
            ///      `{name}` becomes `%s`, and their order is returned in `order`.
            std::string errorFormat(::Sad::Errors::ErrorCode code,
                                    std::vector<std::string> &order);

            /// (AR) يُصدِرُ نداءَ الفشل: القيمُ تُعطى **بأسمائها** لا بترتيبها، ثمّ
            ///      تُرتَّبُ حسبَ ورودِ الفجواتِ في النصِّ المولَّد. أيُّ فجوةٍ بلا قيمةٍ
            ///      تُملأُ بنصِّها الحرفيّ كي يُرى النقصُ في الرسالةِ لا يُخفى.
            /// (EN) Emits the failure call: values are supplied **by name**, then
            ///      ordered by their placeholders' order in the generated text.
            void emitFail(llvm::IRBuilder<> &b, ::Sad::Errors::ErrorCode code,
                          std::vector<std::pair<std::string, llvm::Value *>> named);

            /// (AR) ثابتٌ نصّيٌّ عالميّ (يُعادُ استعمالُه بالاسم).
            llvm::Value *cstr(llvm::IRBuilder<> &b, const std::string &text);

            /// (AR) عائلةُ نظامِ الهدفِ من مثلّثِ الوحدة (‏فارغٌ ⇒ المضيف).
            ProcTargetOS targetOS() const;

            /// (AR) جدولُ ثوابتِ الهدف. يُحسَبُ عند أوّلِ طلبٍ ويُخبَّأ: المثلّثُ
            ///      لا يتغيّرُ داخلَ وحدةٍ واحدة.
            const ProcPlatform &plat() const;

            /// (AR) `errno` الحاليّ بوصفه i32 — الاسمُ من جدولِ الهدفِ لا مُثبَّتًا.
            llvm::Value *errnoValue(llvm::IRBuilder<> &b, const char *name);

            /// (AR) أنبوبٌ بطرفين CLOEXEC، محمولًا: `pipe2` حيث توجد، وإلّا
            ///      `pipe` + `fcntl(F_SETFD)`. و`fds` تُملأُ بـ‎-1 قبل النداءِ
            ///      كي لا تصيبَ `fcntl` — عند فشلِ `pipe` — وصفًا عشوائيًّا
            ///      يصادفُ وجودَه في الحجزِ غيرِ المهيّأ فتُغلقه على مالكِه.
            /// (EN) A CLOEXEC pipe, portably. fds is pre-filled with -1 so that a
            ///      failed pipe() cannot leave fcntl aiming at whatever integer
            ///      happened to sit in the uninitialised slot.
            /// @return rc كما تُرجعه `pipe`/`pipe2` (‏0 نجاح).
            llvm::Value *emitCloexecPipe(llvm::IRBuilder<> &b, llvm::Value *fds);

            mutable ProcPlatform platCache_{};
            mutable bool platReady_ = false;
        };

    } // namespace LLVM
} // namespace Sad

#endif // SAD_LLVM_PROCESSES_BUILTINS_CODEGEN_H
