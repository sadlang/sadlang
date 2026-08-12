// ============================================================================
// (AR) ملف: processes_builtins_ops.cpp
// (EN) File: processes_builtins_ops.cpp
// المكون: Sad::LLVM
// ============================================================================
// (AR) توليدُ LLVM IR لبدائيّاتِ وحدةِ «عمليات» — نظيرُ المصرِّفِ لِـ
//      interpreter/src/builtins/builtin_module_processes.cpp.
//
//      **لماذا دوالٌّ مولَّدةٌ في الوحدةِ لا مكتبةُ زمنِ تشغيل**: سطرُ الربطِ
//      (‏llvm_target.cpp) لا يضمُّ إلّا `-lm -lpthread`، فأيُّ أرشيفٍ جديدٍ يحتاجُ
//      بحثًا عن مسارٍ ⇒ مسارَ فشلٍ جديدًا حين لا يوجد. وسائرُ مدمجاتِ المصرِّفِ
//      تُولِّدُ نداءاتِ libc مباشرةً، فهذا اتّساقٌ لا استثناء.
//
//      **مطابقةُ المفسّرِ في الدلالةِ لا في نصِّ التشخيص**: القيمُ المُرجَعةُ وشروطُ
//      الفشلِ ورموزُ الخطأِ واحدة، والرسالةُ تُبنى من نفسِ كتالوجِ الأخطاءِ المولَّد
//      (‏`briefAr` من language-truth/errors/*.yaml) — لكنّ المفسّرَ يُحيطُها بإطارِ
//      تشخيصٍ كاملٍ لا يُحاكيه الثنائيُّ المُصدَر. فاختباراتُ التطابقِ السالبةُ
//      تُطابِقُ نصًّا مشتركًا لا الإطارَ كلَّه.
//
//      **دَينٌ معلَنٌ ١ — لا تمييزَ لفشلِ التوجيه**: المفسّرُ يميّزُ فشلَ `dup2` من
//      فشلِ `execvp` بنوعِ البلاغِ في الأنبوبِ الذاتيّ، وهذا المولِّدُ يميّزهما كذلك
//      (نفسُ البنية). أُبقيَ التمييزُ لأنّه بالضبطِ ما وُضع الأنبوبُ لأجله.
//
//      **دَينٌ معلَنٌ ٢ — سعةُ جدولِ المقابضِ ثابتة**: ١٠٢٤ مقبضًا حيًّا. المفسّرُ
//      يستعملُ `unordered_map` بلا حدّ. تجاوزُ السعةِ يفشلُ صاخبًا لا صامتًا.
//
//      **ثوابتُ الهدفِ لا ثوابتُ المضيف**: أعلامُ `O_*` وأرقامُ الإشاراتِ وحجمُ
//      `sigset_t` كانت مُثبَّتةً هنا فكان المولِّدُ لينكسَ حصرًا. صارت جدولًا
//      يُنتقى بمثلّثِ الهدف (‏`plat()`)، لأنّ التصريفَ المتقاطعَ يجعلُ مضيفَ
//      البناءِ لا يدلُّ على شيء. وقيمةٌ واحدةٌ خاطئةٌ منها لا تُفشِلُ البناءَ بل
//      تفتحُ الملفَّ بعلمٍ آخر — عطبٌ صامتٌ لا خطأٌ يُرى. وويندوزُ في
//      processes_builtins_win.cpp: يفترقُ بنيويًّا في الانتظارِ والشوكةِ وحدَهما.
//
// (EN) LLVM IR for the «عمليات» (processes) primitives — the compiler counterpart
//      of the interpreter's builtin_module_processes.cpp. Emitted as module-local
//      functions calling libc, because the link line carries only -lm -lpthread and
//      every other compiler builtin does the same. Semantics (return values, failure
//      conditions, error codes) mirror the interpreter; diagnostic text is built from
//      the same generated catalog but without the interpreter's diagnostic frame.
//      Constants come from a per-target table keyed on the target triple, not the
//      build host; Windows lives in processes_builtins_win.cpp.
// ============================================================================

#include "llvm_codegen.h"
#include "builders/builtins/processes_builtins_codegen.h"
#include "sad_dyn_repr.h"
#include "error_messages_generated.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalVariable.h>

#include <string>
#include <vector>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        namespace
        {
            constexpr int64_t kStderrFd = 2;

            // (AR) وسمُ النوعِ وسعةُ الجدولِ في الرأسِ (‏processes_builtins_codegen.h)
            //      كي يتشاركَهما ملفّا المولِّدِ نصًّا واحدًا لا نسختين.

            // (AR) نوعُ بلاغِ الابنِ في الأنبوبِ الذاتيّ — يطابقُ `ChildError` في المفسّر.
            constexpr int64_t kChildFailRedirect = 1;
            constexpr int64_t kChildFailExec = 2;
            constexpr int64_t kChildReportBytes = 8; // (AR) {i32 kind, i32 err}

            const char *kGlobIds = "__sad_proc_ids";
            const char *kGlobVals = "__sad_proc_vals";
            const char *kGlobNext = "__sad_proc_next";
        } // namespace

        // ════════════════════════════════════════════════════════════════════
        // (AR) أدواتٌ صغيرةٌ مشتركة
        // ════════════════════════════════════════════════════════════════════

        ProcTargetOS ProcessesBuiltinsCodeGen::targetOS() const
        {
            const std::string triple = cg_.module_->getTargetTriple();
            // (AR) ثالوثٌ فارغٌ يعني «هدفُ المضيف» ولم يُثبَّت بعد. لا يُخمَّنُ
            //      بلينكس: تخمينٌ خاطئٌ هنا يبني ثوابتَ لينكسَ في ثنائيٍّ ماكيّ
            //      فيفتحُ `افتح_وصفا` الملفَّ بعلمٍ آخرَ بلا أيِّ خطأ.
            if (triple.empty())
            {
#if defined(_WIN32)
                return ProcTargetOS::Windows;
#elif defined(__APPLE__)
                return ProcTargetOS::Darwin;
#elif defined(__linux__)
                return ProcTargetOS::Linux;
#else
                return ProcTargetOS::Unsupported;
#endif
            }
            if (triple.find("linux") != std::string::npos)
                return ProcTargetOS::Linux;
            if (triple.find("darwin") != std::string::npos ||
                triple.find("macos") != std::string::npos ||
                triple.find("apple") != std::string::npos)
                return ProcTargetOS::Darwin;
            if (triple.find("windows") != std::string::npos ||
                triple.find("mingw") != std::string::npos ||
                triple.find("msvc") != std::string::npos)
                return ProcTargetOS::Windows;
            return ProcTargetOS::Unsupported;
        }

        const ProcPlatform &ProcessesBuiltinsCodeGen::plat() const
        {
            if (platReady_)
                return platCache_;
            platReady_ = true;
            ProcPlatform p;
            p.os = targetOS();
            switch (p.os)
            {
            case ProcTargetOS::Linux:
                // (AR) المصدر: bits/fcntl-linux.h, asm-generic/signal.h, glibc sigset_t
                p.oRdOnly = 0;
                p.oWrOnly = 1;
                p.oCreat = 0100;
                p.oTrunc = 01000;
                p.oAppend = 02000;
                p.oCloexec = 02000000;
                p.openMode = 0644;
                p.fDupFdCloexec = 1030;
                p.eIntr = 4;
                p.eChild = 10;
                p.sigInt = 2;
                p.sigQuit = 3;
                p.sigKill = 9;
                p.sigPipe = 13;
                p.sigIgn = 1;
                p.sigSetMask = 2;
                p.sigsetBytes = 128;
                p.errnoLocation = "__errno_location";
                p.hasPipe2 = true;
                break;

            case ProcTargetOS::Darwin:
                // (AR) المصدر: sys/fcntl.h, sys/signal.h على macOS. لاحظ أنّ
                //      **أربعةً** من هذه تخالفُ لينكسَ عددًا لا تسميةً فقط:
                //      O_CREAT/O_TRUNC/O_APPEND/O_CLOEXEC وF_DUPFD_CLOEXEC،
                //      وsigset_t هناك أربعةُ بايتاتٍ لا ١٢٨. ونقلُ قيمةٍ واحدةٍ
                //      من لينكس يفتحُ الملفَّ فتحًا آخرَ بلا خطأ.
                p.oRdOnly = 0x0000;
                p.oWrOnly = 0x0001;
                p.oCreat = 0x0200;
                p.oTrunc = 0x0400;
                p.oAppend = 0x0008;
                p.oCloexec = 0x1000000;
                p.openMode = 0644;
                p.fDupFdCloexec = 67;
                p.eIntr = 4;
                p.eChild = 10;
                p.sigInt = 2;
                p.sigQuit = 3;
                p.sigKill = 9;
                p.sigPipe = 13;
                p.sigIgn = 1;
                p.sigSetMask = 3; // (AR) darwin: SIG_SETMASK = 3 لا 2
                p.sigsetBytes = 4;
                p.errnoLocation = "__error";
                p.hasPipe2 = false; // (AR) لا pipe2 على ماك — يُصطنَع بـfcntl
                break;

            case ProcTargetOS::Windows:
                // (AR) المصدر: UCRT fcntl.h. الأعلامُ هنا أعلامُ CRT لا POSIX،
                //      و`_O_NOINHERIT` هو نظيرُ CLOEXEC، و`_O_BINARY` بلا نظيرٍ
                //      POSIX لأنّه لا ترجمةَ أسطرٍ هناك أصلًا.
                p.oRdOnly = 0x0000;
                p.oWrOnly = 0x0001;
                p.oCreat = 0x0100;
                p.oTrunc = 0x0200;
                p.oAppend = 0x0008;
                p.oCloexec = 0x0080; // _O_NOINHERIT
                p.oBinary = 0x8000;  // _O_BINARY
                p.openMode = 0x0180; // _S_IREAD | _S_IWRITE
                p.eIntr = 4;         // EINTR — معرَّفٌ ولا يقعُ عمليًّا
                p.errnoLocation = "_errno";
                break;

            case ProcTargetOS::Unsupported:
            default:
                break;
            }
            platCache_ = p;
            return platCache_;
        }

        llvm::Value *ProcessesBuiltinsCodeGen::emitCloexecPipe(llvm::IRBuilder<> &b,
                                                               llvm::Value *fds)
        {
            auto *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto *minusOne = llvm::ConstantInt::get(i32Ty, -1);
            for (int i = 0; i < 2; ++i)
                b.CreateStore(minusOne, b.CreateConstInBoundsGEP1_32(i32Ty, fds, i));

            if (plat().os == ProcTargetOS::Windows)
            {
                // (AR) `_pipe(fds, size, textmode)` — و«وضعُ النصّ» هنا يحملُ
                //      `_O_NOINHERIT` أيضًا، فهو موضعُ CLOEXEC لا وسيطٌ ثالثٌ منفصل.
                //      وسعةُ ٦٥٥٣٦ تطابقُ المفسّرَ: سعةٌ أصغرُ تجعلُ كاتبًا كبيرًا
                //      يتوقّفُ حتّى يقرأَ القارئ، فيختلفُ **توقيتُ** التعلّقِ بين
                //      المحرّكين لا سلوكُهما — وهو أسوأُ ما يُقاس.
                auto *pipeTy = llvm::FunctionType::get(i32Ty, {ptrTy, i32Ty, i32Ty}, false);
                auto pipeFn = cg_.module_->getOrInsertFunction("_pipe", pipeTy);
                return b.CreateCall(
                    pipeFn,
                    {fds, llvm::ConstantInt::get(i32Ty, 65536),
                     llvm::ConstantInt::get(i32Ty, plat().oBinary | plat().oCloexec)},
                    "rc");
            }

            if (plat().hasPipe2)
            {
                auto *pipe2Ty = llvm::FunctionType::get(i32Ty, {ptrTy, i32Ty}, false);
                auto pipe2Fn = cg_.module_->getOrInsertFunction("pipe2", pipe2Ty);
                return b.CreateCall(pipe2Fn,
                                    {fds, llvm::ConstantInt::get(i32Ty, plat().oCloexec)}, "rc");
            }

            // (AR) بديلُ ماك: `pipe` ثمّ `fcntl(F_SETFD, FD_CLOEXEC)` على الطرفين.
            //      و`F_SETFD`=2 و`FD_CLOEXEC`=1 في كلِّ POSIX بلا اختلاف.
            //      **غيرُ ذرّيّ** — النافذةُ بين النداءين حدُّ المنصّةِ لا حدُّنا،
            //      والمفسّرُ يسلكُ المسلكَ نفسَه هناك (‏makeCloexecPipe).
            auto *pipeTy = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto pipeFn = cg_.module_->getOrInsertFunction("pipe", pipeTy);
            llvm::Value *rc = b.CreateCall(pipeFn, {fds}, "rc");

            // (AR) **مُعاملان ثابتان لا ثلاثة**: توقيعُ `fcntl` الحقيقيّ
            //      `int fcntl(int, int, ...)`. وإعلانُه بثلاثةٍ ثابتةٍ يمرُّ على
            //      x86-64 صدفةً (الوسائطُ في السجلّات في الحالين)، ويكسرُ ABI على
            //      arm64 من آبل: الوسائطُ المتغيّرةُ تُمرَّرُ هناك **في المكدَّس** لا
            //      في السجلّات، فيقرأ `fcntl` وسيطًا ثالثًا عشوائيًّا.
            //
            //      وأثرُه ليس فشلًا يُرى بل **تعلّقًا**: لا يُضبَط CLOEXEC، فيتسرّب
            //      طرفُ الكتابةِ إلى ابنٍ لم يُوجَّه إليه، فلا يرى القارئُ نهايةَ
            //      ملفٍّ أبدًا. وهذا بعينه ما كُتب له اختبارُ ١٧٢، وقد أمسكه:
            //      المفسّر ٤١ مِلّي والمُصرَّف مهلةٌ كاملة، على ماك وحدَه.
            //      وسطرُ ١١٦٨ (‏F_DUPFD_CLOEXEC) كان صحيحًا أصلًا — فالخطأ كان في
            //      نسخةٍ واحدةٍ من إعلانين للدالّة نفسِها في الوحدة نفسِها،
            //      وهو سببٌ كافٍ لتوحيدهما.
            // (EN) Two fixed parameters, not three: fcntl is int fcntl(int, int, ...).
            //      Declaring three passes on x86-64 by accident (all in registers)
            //      and breaks the Apple arm64 ABI, where variadic arguments go on
            //      the stack — so fcntl reads a garbage third argument, CLOEXEC is
            //      never set, and the reader hangs forever. Test 172 caught it.
            auto *fcntlTy = llvm::FunctionType::get(i32Ty, {i32Ty, i32Ty}, true);
            auto fcntlFn = cg_.module_->getOrInsertFunction("fcntl", fcntlTy);
            for (int i = 0; i < 2; ++i)
            {
                llvm::Value *fd =
                    b.CreateLoad(i32Ty, b.CreateConstInBoundsGEP1_32(i32Ty, fds, i), "pfd");
                b.CreateCall(fcntlFn, {fd, llvm::ConstantInt::get(i32Ty, 2),
                                       llvm::ConstantInt::get(i32Ty, 1)});
            }
            return rc;
        }

        llvm::Value *ProcessesBuiltinsCodeGen::errnoValue(llvm::IRBuilder<> &b, const char *name)
        {
            auto *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto *fnTy = llvm::FunctionType::get(ptrTy, {}, false);
            auto fn = cg_.module_->getOrInsertFunction(plat().errnoLocation, fnTy);
            llvm::Value *slot = b.CreateCall(fn, {}, "errno.slot");
            return b.CreateLoad(i32Ty, slot, name);
        }

        llvm::Value *ProcessesBuiltinsCodeGen::cstr(llvm::IRBuilder<> &b, const std::string &text)
        {
            return b.CreateGlobalStringPtr(text, "proc.str");
        }

        std::string ProcessesBuiltinsCodeGen::errorFormat(::Sad::Errors::ErrorCode code,
                                                          std::vector<std::string> &order)
        {
            order.clear();
            const auto *entry = ::Sad::Errors::Generated::findByCode(code);
            // (AR) رمزٌ بلا إدخالٍ مولَّد: لا نصمتُ ولا نخترعُ نصًّا — نطبعُ الرمزَ
            //      نفسَه كي يقودَ البحثَ إلى الكتالوج.
            if (!entry || !entry->briefAr)
                return std::string("خطأ في وحدة عمليات (رمز ")
                       + (entry && entry->id ? entry->id : "?") + ")";

            const std::string src = entry->briefAr;
            std::string out;
            out.reserve(src.size() + 16);
            for (size_t i = 0; i < src.size();)
            {
                if (src[i] == '{')
                {
                    const size_t end = src.find('}', i);
                    if (end != std::string::npos)
                    {
                        order.push_back(src.substr(i + 1, end - i - 1));
                        out += "%s";
                        i = end + 1;
                        continue;
                    }
                }
                // (AR) `%` حرفيّةٌ في النصِّ ستُفسَّرُ تنسيقًا في snprintf — تُضاعَف.
                if (src[i] == '%')
                    out += '%';
                out += src[i];
                ++i;
            }
            const char *id = entry->id ? entry->id : "?";
            return std::string("[") + id + "] " + out;
        }

        void ProcessesBuiltinsCodeGen::emitFail(
            llvm::IRBuilder<> &b, ::Sad::Errors::ErrorCode code,
            std::vector<std::pair<std::string, llvm::Value *>> named)
        {
            std::vector<std::string> order;
            const std::string fmt = errorFormat(code, order);

            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            std::vector<llvm::Value *> args;
            for (const std::string &key : order)
            {
                llvm::Value *found = nullptr;
                for (auto &kv : named)
                    if (kv.first == key)
                    {
                        found = kv.second;
                        break;
                    }
                // (AR) فجوةٌ بلا قيمة: تُملأُ باسمِها بين قوسين فيُرى النقصُ في
                //      الرسالةِ ولا يُبتلَعُ سطرًا فارغًا.
                args.push_back(found ? found : cstr(b, "{" + key + "}"));
            }
            while (args.size() < 3)
                args.push_back(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)));

            llvm::Function *fail = getOrCreateFail();
            b.CreateCall(fail, {cstr(b, fmt), args[0], args[1], args[2]});
            b.CreateUnreachable();
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) __sad_proc_fail(fmt, a, b, c) — يُنسِّقُ ويكتبُ على 2 ثمّ يخرجُ بـ1
        //      لا `stderr` ولا `fprintf`: `stderr` رمزٌ عامٌّ يختلفُ تصديرُه بين
        //      مكتبات C، و`write(2,…)` نداءُ نظامٍ لا يعتمدُ على شيء.
        // (EN) Formats into a stack buffer and write(2, …), then exit(1). No
        //      dependence on the `stderr` symbol.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateFail()
        {
            const char *name = "__sad_proc_fail";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i8Ty = llvm::Type::getInt8Ty(ctx);
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *voidTy = llvm::Type::getVoidTy(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            auto *fnTy = llvm::FunctionType::get(voidTy, {ptrTy, ptrTy, ptrTy, ptrTy}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            fn->addFnAttr(llvm::Attribute::NoReturn);

            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            llvm::IRBuilder<> b(entry);

            constexpr int64_t kBufBytes = 2048;
            llvm::Value *buf = b.CreateAlloca(i8Ty, llvm::ConstantInt::get(i64Ty, kBufBytes), "msg");

            auto *snprintfTy = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty, ptrTy}, true);
            auto snprintfFn = cg_.module_->getOrInsertFunction("snprintf", snprintfTy);
            llvm::Value *n = b.CreateCall(
                snprintfFn,
                {buf, llvm::ConstantInt::get(i64Ty, kBufBytes),
                 fn->getArg(0), fn->getArg(1), fn->getArg(2), fn->getArg(3)},
                "msg.len");
            llvm::Value *n64 = b.CreateSExt(n, i64Ty, "msg.len64");

            // (AR) قصُّ الطولِ إلى [0، الحجم-1]: snprintf يُرجعُ ما **كان** سيُكتَب.
            llvm::Value *neg = b.CreateICmpSLT(n64, llvm::ConstantInt::get(i64Ty, 0), "msg.neg");
            llvm::Value *clamped = b.CreateSelect(neg, llvm::ConstantInt::get(i64Ty, 0), n64, "msg.c0");
            llvm::Value *over = b.CreateICmpSGT(clamped, llvm::ConstantInt::get(i64Ty, kBufBytes - 1), "msg.over");
            clamped = b.CreateSelect(over, llvm::ConstantInt::get(i64Ty, kBufBytes - 1), clamped, "msg.c1");

            // (AR) الكتابةُ على الوصفِ ٢ مباشرةً لا على `stderr`: الرمزُ الأخيرُ
            //      كائنٌ في CRT يختلفُ تصديرُه بين المنصّاتِ والإصدارات. و`write`
            //      نفسُها ليست موحّدة: UCRT لا تصدّرُ إلّا `_write`، وتوقيعُها
            //      **بعرضٍ ٣٢** لا ٦٤ — فتثبيتُ i64 كان سيمرّرُ نصفَ سجلٍّ عشوائيًّا
            //      طولًا للكتابة، أي رسالةَ خطأٍ مقطوعةً أو زائدة، وهي آخرُ ما
            //      يُشكُّ فيه لأنّ الرسالةَ نفسَها هي أداةُ التشخيص.
            // (EN) write is not portable: UCRT exports only _write, and with a
            //      32-bit length — pinning i64 would pass half a stale register as
            //      the length, corrupting the very message used to diagnose.
            const bool win = plat().os == ProcTargetOS::Windows;
            auto *lenTy = win ? i32Ty : i64Ty;
            auto *writeTy = llvm::FunctionType::get(lenTy, {i32Ty, ptrTy, lenTy}, false);
            auto writeFn =
                cg_.module_->getOrInsertFunction(win ? "_write" : "write", writeTy);
            b.CreateCall(writeFn, {llvm::ConstantInt::get(i32Ty, kStderrFd), buf,
                                   b.CreateSExtOrTrunc(clamped, lenTy, "msg.len")});
            llvm::Value *nl = b.CreateGlobalStringPtr("\n", "proc.nl");
            b.CreateCall(writeFn, {llvm::ConstantInt::get(i32Ty, kStderrFd), nl,
                                   llvm::ConstantInt::get(lenTy, 1)});

            auto *exitTy = llvm::FunctionType::get(voidTy, {i32Ty}, false);
            auto exitFn = cg_.module_->getOrInsertFunction("exit", exitTy);
            b.CreateCall(exitFn, {llvm::ConstantInt::get(i32Ty, 1)});
            b.CreateUnreachable();
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) __sad_proc_num2str(i64) → ptr — لِفجواتِ `{handle}` العدديّة
        //
        //      المخزنُ من الكومةِ لا من المكدَّس، وقد كان `alloca` فيُعادُ مؤشّرٌ
        //      إلى إطارٍ ميّت: تُبنى الرسالةُ صحيحةً حين لا يقعُ بين النداءِ
        //      والطباعةِ نداءٌ آخر، وتُطبَعُ محارفَ مهملةً حين يقع. كشفَه إثباتُ
        //      الحمرةِ حين تخلّل `close` بينهما فصار المقبضُ `'\x01'` — عطبٌ
        //      يظهرُ في نصِّ التشخيصِ وحدَه، أي في آخرِ موضعٍ يُشكُّ فيه.
        //      يُسرَّبُ ٣٢ بايتًا لكلِّ رسالةٍ، والبرنامجُ يخرجُ عقبَها فورًا.
        // (EN) Heap, not stack: this was an alloca whose pointer outlived its frame —
        //      correct output whenever no call intervened, garbage when one did.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateNumToStr()
        {
            const char *name = "__sad_proc_num2str";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i8Ty = llvm::Type::getInt8Ty(ctx);
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            auto *fnTy = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            llvm::IRBuilder<> b(entry);

            constexpr int64_t kNumBytes = 32;
            auto *mallocTy = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocTy);
            llvm::Value *buf = b.CreateCall(mallocFn, {llvm::ConstantInt::get(i64Ty, kNumBytes)}, "num");
            (void)i8Ty;
            auto *snprintfTy = llvm::FunctionType::get(i32Ty, {ptrTy, i64Ty, ptrTy}, true);
            auto snprintfFn = cg_.module_->getOrInsertFunction("snprintf", snprintfTy);
            b.CreateCall(snprintfFn, {buf, llvm::ConstantInt::get(i64Ty, kNumBytes),
                                      b.CreateGlobalStringPtr("%lld", "proc.numfmt"), fn->getArg(0)});
            b.CreateRet(buf);
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) جدولُ المقابض: مصفوفتانِ متوازيتانِ + عدّادٌ **لا يُعادُ استعمالُه**.
        //      المعرّفُ تصاعديٌّ أبدًا كما في المفسّر، فمقبضٌ بائتٌ لا يصيرُ صالحًا
        //      لأنّ النواةَ أعادت استعمالَ الوصف — وهو الفرقُ بين خطأٍ صريحٍ ونجاحٍ
        //      كاذبٍ يكتبُ في ملفٍّ غيرِ الذي قصده الكاتب.
        // (EN) Handle table: parallel arrays + a monotonically increasing id, never
        //      reused — exactly as in the interpreter. Encoding the raw fd in the
        //      handle would let a stale handle silently become valid again after the
        //      kernel recycled the descriptor.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateStore()
        {
            const char *name = "__sad_proc_store";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);
            auto *slotsTy = llvm::ArrayType::get(i64Ty, kHandleSlots);

            auto *ids = new llvm::GlobalVariable(
                *cg_.module_, slotsTy, false, llvm::GlobalValue::InternalLinkage,
                llvm::ConstantAggregateZero::get(slotsTy), kGlobIds);
            auto *vals = new llvm::GlobalVariable(
                *cg_.module_, slotsTy, false, llvm::GlobalValue::InternalLinkage,
                llvm::ConstantAggregateZero::get(slotsTy), kGlobVals);
            auto *next = new llvm::GlobalVariable(
                *cg_.module_, i64Ty, false, llvm::GlobalValue::InternalLinkage,
                llvm::ConstantInt::get(i64Ty, 1), kGlobNext);
            (void)ptrTy;

            auto *fnTy = llvm::FunctionType::get(i64Ty, {i64Ty, i64Ty}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *loop = llvm::BasicBlock::Create(ctx, "scan", fn);
            auto *test = llvm::BasicBlock::Create(ctx, "test", fn);
            auto *step = llvm::BasicBlock::Create(ctx, "step", fn);
            auto *full = llvm::BasicBlock::Create(ctx, "full", fn);
            auto *found = llvm::BasicBlock::Create(ctx, "found", fn);

            llvm::IRBuilder<> b(entry);
            llvm::Value *iSlot = b.CreateAlloca(i64Ty, nullptr, "i");
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iSlot);
            b.CreateBr(loop);

            b.SetInsertPoint(loop);
            llvm::Value *i = b.CreateLoad(i64Ty, iSlot, "i.v");
            b.CreateCondBr(b.CreateICmpSGE(i, llvm::ConstantInt::get(i64Ty, kHandleSlots), "i.end"),
                           full, test);

            b.SetInsertPoint(test);
            llvm::Value *idGep = b.CreateGEP(i64Ty, ids, {i}, "id.gep");
            llvm::Value *idCur = b.CreateLoad(i64Ty, idGep, "id.cur");
            b.CreateCondBr(b.CreateICmpEQ(idCur, llvm::ConstantInt::get(i64Ty, 0), "id.free"),
                           found, step);

            b.SetInsertPoint(step);
            b.CreateStore(b.CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1)), iSlot);
            b.CreateBr(loop);

            // (AR) لا رمزَ في الكتالوجِ لامتلاءِ الجدول (‏قيدٌ خاصٌّ بالمصرِّفِ لا
            //      نظيرَ له في المفسّرِ الذي يستعملُ خريطةً بلا حدّ)، فلا نستعيرُ
            //      رمزًا يصفُ شيئًا آخر: نصٌّ صريحٌ يقولُ الحدَّ وسببَه.
            b.SetInsertPoint(full);
            {
                auto *nullPtr = llvm::ConstantPointerNull::get(
                    llvm::cast<llvm::PointerType>(llvm::PointerType::getUnqual(ctx)));
                b.CreateCall(getOrCreateFail(),
                             {cstr(b, "[عمليات] امتلأ جدول المقابض (١٠٢٤ مقبضًا حيًّا) — أغلق ما لم يعد يُستعمل"),
                              nullPtr, nullPtr, nullPtr});
                b.CreateUnreachable();
            }

            b.SetInsertPoint(found);
            llvm::Value *idNew = b.CreateLoad(i64Ty, next, "id.new");
            b.CreateStore(b.CreateAdd(idNew, llvm::ConstantInt::get(i64Ty, 1)), next);
            b.CreateStore(idNew, idGep);
            b.CreateStore(fn->getArg(1), b.CreateGEP(i64Ty, vals, {i}, "val.gep"));
            b.CreateRet(b.CreateOr(fn->getArg(0), idNew, "handle"));
            return fn;
        }

        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateFind()
        {
            const char *name = "__sad_proc_find";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            getOrCreateStore(); // (AR) يضمنُ وجودَ العوالم قبل الإشارةِ إليها
            auto &ctx = *cg_.context_;
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *slotsTy = llvm::ArrayType::get(i64Ty, kHandleSlots);
            auto *ids = cg_.module_->getNamedGlobal(kGlobIds);
            auto *vals = cg_.module_->getNamedGlobal(kGlobVals);
            (void)slotsTy;

            auto *fnTy = llvm::FunctionType::get(i64Ty, {i64Ty, i64Ty}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *loop = llvm::BasicBlock::Create(ctx, "scan", fn);
            auto *test = llvm::BasicBlock::Create(ctx, "test", fn);
            auto *step = llvm::BasicBlock::Create(ctx, "step", fn);
            auto *miss = llvm::BasicBlock::Create(ctx, "miss", fn);
            auto *hit = llvm::BasicBlock::Create(ctx, "hit", fn);
            auto *take = llvm::BasicBlock::Create(ctx, "take", fn);
            auto *done = llvm::BasicBlock::Create(ctx, "done", fn);

            llvm::IRBuilder<> b(entry);
            // (AR) المعرّفُ هو المقبضُ مجرَّدًا من وسمِ النوع.
            llvm::Value *id = b.CreateAnd(fn->getArg(0),
                                          llvm::ConstantInt::get(i64Ty, ~kKindMask), "id");
            llvm::Value *iSlot = b.CreateAlloca(i64Ty, nullptr, "i");
            llvm::Value *out = b.CreateAlloca(i64Ty, nullptr, "out");
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iSlot);
            b.CreateStore(llvm::ConstantInt::get(i64Ty, -1), out);
            b.CreateBr(loop);

            b.SetInsertPoint(loop);
            llvm::Value *i = b.CreateLoad(i64Ty, iSlot, "i.v");
            b.CreateCondBr(b.CreateICmpSGE(i, llvm::ConstantInt::get(i64Ty, kHandleSlots), "i.end"),
                           miss, test);

            b.SetInsertPoint(test);
            llvm::Value *idGep = b.CreateGEP(i64Ty, ids, {i}, "id.gep");
            llvm::Value *idCur = b.CreateLoad(i64Ty, idGep, "id.cur");
            b.CreateCondBr(b.CreateICmpEQ(idCur, id, "id.match"), hit, step);

            b.SetInsertPoint(step);
            b.CreateStore(b.CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1)), iSlot);
            b.CreateBr(loop);

            b.SetInsertPoint(hit);
            b.CreateStore(b.CreateLoad(i64Ty, b.CreateGEP(i64Ty, vals, {i}, "val.gep"), "val"), out);
            b.CreateCondBr(b.CreateICmpNE(fn->getArg(1), llvm::ConstantInt::get(i64Ty, 0), "do.take"),
                           take, done);

            b.SetInsertPoint(take);
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), idGep);
            b.CreateBr(done);

            b.SetInsertPoint(miss);
            b.CreateBr(done);

            b.SetInsertPoint(done);
            b.CreateRet(b.CreateLoad(i64Ty, out, "found"));
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) __sad_proc_kind_guard(handle, kindWanted, expectedText)
        //      حارسُ النوعِ عند كلِّ مدخل: مقبضُ عمليّةٍ مرَّ إلى `اغلق` كان يُغلقُ
        //      وصفًا أجنبيًّا ويُرجعُ «صحيح» — نجاحٌ كاذبٌ صامت.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateKindGuard()
        {
            const char *name = "__sad_proc_kind_guard";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *voidTy = llvm::Type::getVoidTy(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            auto *fnTy = llvm::FunctionType::get(voidTy, {i64Ty, i64Ty, ptrTy}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *bad = llvm::BasicBlock::Create(ctx, "bad", fn);
            auto *ok = llvm::BasicBlock::Create(ctx, "ok", fn);

            llvm::IRBuilder<> b(entry);
            llvm::Value *kind = b.CreateAnd(fn->getArg(0), llvm::ConstantInt::get(i64Ty, kKindMask), "kind");
            b.CreateCondBr(b.CreateICmpEQ(kind, fn->getArg(1), "kind.ok"), ok, bad);

            b.SetInsertPoint(ok);
            b.CreateRetVoid();

            // (AR) وصفُ ما وُجد فعلًا — لا «مجهول» فحسب: الفرقُ بين «أخطأتَ الدالّة»
            //      و«ضاعَ المقبض» فرقٌ في اتّجاهِ البحثِ كلِّه (نفسُ نصِّ المفسّر).
            b.SetInsertPoint(bad);
            llvm::Value *isDesc = b.CreateICmpEQ(kind, llvm::ConstantInt::get(i64Ty, kKindDescriptor), "is.desc");
            llvm::Value *isProc = b.CreateICmpEQ(kind, llvm::ConstantInt::get(i64Ty, kKindProcess), "is.proc");
            llvm::Value *foundName = b.CreateSelect(
                isDesc, cstr(b, "وصفُ ملفّ"),
                b.CreateSelect(isProc, cstr(b, "مقبضُ عمليّة"),
                               cstr(b, "ليس مقبضًا تُنتجه وحدة عمليات"), "found.proc"),
                "found.name");
            llvm::Value *handleStr = b.CreateCall(getOrCreateNumToStr(), {fn->getArg(0)}, "handle.str");
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_HANDLE_KIND,
                     {{"handle", handleStr},
                      {"expected", fn->getArg(2)},
                      {"found", foundName}});
            (void)ptrTy;
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) افتح_وصفا — وضعٌ من مجموعةٍ مغلقةٍ، ووصفٌ يحمل CLOEXEC
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateOpenFd()
        {
            // (AR) ويندوز يفتحُ بـCreateFileA لا `_open`: «إلحاق» في CRT مسلكٌ
            //      عند الكتابةِ لا خاصّيّةٌ في المقبض، فالابنُ يدهسُ من الصفر.
            if (plat().os == ProcTargetOS::Windows)
                return getOrCreateWinOpenFd();

            const char *name = "__sad_proc_open_fd";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            auto *fnTy = llvm::FunctionType::get(i64Ty, {ptrTy, ptrTy}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *badMode = llvm::BasicBlock::Create(ctx, "bad.mode", fn);
            auto *doOpen = llvm::BasicBlock::Create(ctx, "open", fn);
            auto *openFail = llvm::BasicBlock::Create(ctx, "open.fail", fn);
            auto *openRetry = llvm::BasicBlock::Create(ctx, "open.retry", fn);
            auto *openOk = llvm::BasicBlock::Create(ctx, "open.ok", fn);

            llvm::IRBuilder<> b(entry);
            auto *strcmpTy = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto strcmpFn = cg_.module_->getOrInsertFunction("strcmp", strcmpTy);
            auto isMode = [&](const char *text, const char *nm) {
                return b.CreateICmpEQ(
                    b.CreateCall(strcmpFn, {fn->getArg(1), b.CreateGlobalStringPtr(text, "proc.mode")}, nm),
                    llvm::ConstantInt::get(i32Ty, 0), std::string(nm) + ".eq");
            };
            llvm::Value *isRead = isMode("قراءة", "mode.read");
            llvm::Value *isWrite = isMode("كتابة", "mode.write");
            llvm::Value *isAppend = isMode("إلحاق", "mode.append");

            // (AR) `oBinary` صفرٌ على POSIX فلا أثرَ له، وعلى ويندوز شرطُ تطابقٍ لا
            //      زينة: الوضعُ النصّيُّ هناك يُقحمُ `\r` في كلِّ كتابةٍ فيختلفُ
            //      **محتوى الملفّ** بين المحرّكين والمنصّتين بلا أيِّ خطأ.
            llvm::Value *flags = llvm::ConstantInt::get(
                i64Ty, plat().oRdOnly | plat().oCloexec | plat().oBinary);
            flags = b.CreateSelect(isWrite,
                                   llvm::ConstantInt::get(i64Ty, plat().oWrOnly | plat().oCreat | plat().oTrunc | plat().oCloexec | plat().oBinary),
                                   flags, "flags.w");
            flags = b.CreateSelect(isAppend,
                                   llvm::ConstantInt::get(i64Ty, plat().oWrOnly | plat().oCreat | plat().oAppend | plat().oCloexec | plat().oBinary),
                                   flags, "flags.a");
            llvm::Value *known = b.CreateOr(b.CreateOr(isRead, isWrite), isAppend, "mode.known");
            b.CreateCondBr(known, doOpen, badMode);

            b.SetInsertPoint(badMode);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_BAD_MODE, {{"mode", fn->getArg(1)}});

            // (AR) EINTR تُعاد المحاولة — إشارةٌ أثناء الفتحِ ليست فشلَ فتح.
            b.SetInsertPoint(doOpen);
            auto *openTy = llvm::FunctionType::get(i32Ty, {ptrTy, i32Ty}, true);
            auto openFn = cg_.module_->getOrInsertFunction(
                plat().os == ProcTargetOS::Windows ? "_open" : "open", openTy);
            llvm::Value *fd = b.CreateCall(openFn,
                                           {fn->getArg(0), b.CreateTrunc(flags, i32Ty, "flags.i32"),
                                            llvm::ConstantInt::get(i32Ty, plat().openMode)},
                                           "fd");
            b.CreateCondBr(b.CreateICmpSLT(fd, llvm::ConstantInt::get(i32Ty, 0), "fd.bad"),
                           openRetry, openOk);

            b.SetInsertPoint(openRetry);
            auto *errnoTy = llvm::FunctionType::get(ptrTy, {}, false);
            auto errnoFn = cg_.module_->getOrInsertFunction(plat().errnoLocation, errnoTy);
            llvm::Value *err = b.CreateLoad(i32Ty, b.CreateCall(errnoFn, {}, "errno.p"), "errno.v");
            b.CreateCondBr(b.CreateICmpEQ(err, llvm::ConstantInt::get(i32Ty, plat().eIntr), "is.eintr"),
                           doOpen, openFail);

            b.SetInsertPoint(openFail);
            llvm::Value *err2 = b.CreateLoad(i32Ty, b.CreateCall(errnoFn, {}, "errno.p2"), "errno.v2");
            auto *strerrorTy = llvm::FunctionType::get(ptrTy, {i32Ty}, false);
            auto strerrorFn = cg_.module_->getOrInsertFunction("strerror", strerrorTy);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_OPEN_FAILED,
                     {{"path", fn->getArg(0)},
                      {"reason", b.CreateCall(strerrorFn, {err2}, "reason")}});

            b.SetInsertPoint(openOk);
            llvm::Function *store = getOrCreateStore();
            b.CreateRet(b.CreateCall(store,
                                     {llvm::ConstantInt::get(i64Ty, kKindDescriptor),
                                      b.CreateSExt(fd, i64Ty, "fd64")},
                                     "handle"));
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) اغلق — السحبُ قبل الإغلاق، وEINTR على close **لا يُعاد**
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateClose()
        {
            const char *name = "__sad_proc_close";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            auto *fnTy = llvm::FunctionType::get(i64Ty, {i64Ty}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *unknown = llvm::BasicBlock::Create(ctx, "unknown", fn);
            auto *doClose = llvm::BasicBlock::Create(ctx, "close", fn);
            auto *checkErr = llvm::BasicBlock::Create(ctx, "close.err", fn);
            auto *closeFail = llvm::BasicBlock::Create(ctx, "close.fail", fn);
            auto *ok = llvm::BasicBlock::Create(ctx, "ok", fn);

            llvm::IRBuilder<> b(entry);
            b.CreateCall(getOrCreateKindGuard(),
                         {fn->getArg(0), llvm::ConstantInt::get(i64Ty, kKindDescriptor),
                          cstr(b, "وصفًا من انبوب أو افتح_وصفا")});
            llvm::Value *fd = b.CreateCall(getOrCreateFind(),
                                           {fn->getArg(0), llvm::ConstantInt::get(i64Ty, 1)}, "fd");
            b.CreateCondBr(b.CreateICmpSLT(fd, llvm::ConstantInt::get(i64Ty, 0), "fd.miss"),
                           unknown, doClose);

            b.SetInsertPoint(unknown);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_BAD_DESCRIPTOR,
                     {{"handle", b.CreateCall(getOrCreateNumToStr(), {fn->getArg(0)}, "h.str")}});

            b.SetInsertPoint(doClose);
            auto *closeTy = llvm::FunctionType::get(i32Ty, {i32Ty}, false);
            auto closeFn = cg_.module_->getOrInsertFunction(plat().os == ProcTargetOS::Windows ? "_close" : "close", closeTy);
            llvm::Value *rc = b.CreateCall(closeFn, {b.CreateTrunc(fd, i32Ty, "fd32")}, "rc");
            b.CreateCondBr(b.CreateICmpSLT(rc, llvm::ConstantInt::get(i32Ty, 0), "rc.bad"),
                           checkErr, ok);

            // (AR) EINTR على close لا يُعادُ النداءُ عليه في لينكس: الوصفُ مُغلَقٌ
            //      أصلًا، وإعادةُ النداءِ قد تُغلقُ وصفًا آخرَ أعادت النواةُ استعماله.
            b.SetInsertPoint(checkErr);
            auto *errnoTy = llvm::FunctionType::get(ptrTy, {}, false);
            auto errnoFn = cg_.module_->getOrInsertFunction(plat().errnoLocation, errnoTy);
            llvm::Value *err = b.CreateLoad(i32Ty, b.CreateCall(errnoFn, {}, "errno.p"), "errno.v");
            b.CreateCondBr(b.CreateICmpEQ(err, llvm::ConstantInt::get(i32Ty, plat().eIntr), "is.eintr"),
                           ok, closeFail);

            b.SetInsertPoint(closeFail);
            auto *strerrorTy = llvm::FunctionType::get(ptrTy, {i32Ty}, false);
            auto strerrorFn = cg_.module_->getOrInsertFunction("strerror", strerrorTy);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_CLOSE_FAILED,
                     {{"handle", b.CreateCall(getOrCreateNumToStr(), {fn->getArg(0)}, "h.str2")},
                      {"reason", b.CreateCall(strerrorFn, {err}, "reason")}});

            b.SetInsertPoint(ok);
            b.CreateRet(llvm::ConstantInt::get(i64Ty, 1));
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) انتظر_عملية — SIGINT/SIGQUIT مُحيَّدتانِ أثناء الانتظار، وEINTR يُعاد
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateWait()
        {
            // (AR) الانتظارُ والشوكةُ وحدَهما يفترقان بالمنصّة افتراقًا بنيويًّا:
            //      لا `waitpid` على ويندوز ولا فكَّ حالةٍ ولا شوكة. أمّا الفتحُ
            //      والإغلاقُ والأنبوبُ فبنيةٌ واحدةٌ بأسماءٍ مختلفة، فتُعادُ
            //      استعمالًا لا تُنسَخ — والنسخُ هو ما يجعل المسارَين يتباعدان
            //      عند أوّلِ تصليحٍ يقعُ في أحدهما.
            if (plat().os == ProcTargetOS::Windows)
                return getOrCreateWinWait();

            const char *name = "__sad_proc_wait";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            auto *fnTy = llvm::FunctionType::get(i64Ty, {i64Ty}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *notChild = llvm::BasicBlock::Create(ctx, "not.child", fn);
            auto *loop = llvm::BasicBlock::Create(ctx, "wait", fn);
            auto *waitErr = llvm::BasicBlock::Create(ctx, "wait.err", fn);
            auto *waitFail = llvm::BasicBlock::Create(ctx, "wait.fail", fn);
            auto *restore = llvm::BasicBlock::Create(ctx, "restore", fn);
            auto *decode = llvm::BasicBlock::Create(ctx, "decode", fn);

            llvm::IRBuilder<> b(entry);
            b.CreateCall(getOrCreateKindGuard(),
                         {fn->getArg(0), llvm::ConstantInt::get(i64Ty, kKindProcess),
                          cstr(b, "مقبضَ عمليّة من شغل_برنامجا")});
            // (AR) بحثٌ **بلا سحب**: المقبضُ يُسحب بعد نجاحِ الانتظارِ لا قبله. المفسّرُ
            //      يسحبُ ثمّ يُعيدُ القيدَ بمعرّفه نفسِه عند فشلٍ عابر، ولا سبيلَ هنا إلى
            //      إعادةِ **المعرّفِ نفسِه** (‏العدّادُ تصاعديٌّ لا يُعادُ استعماله) — فإعادةُ
            //      التخزينِ كانت تُنتج مقبضًا جديدًا ويموتُ الذي في يدِ المستخدم، فيصيرُ
            //      فشلٌ عابرٌ خسارةً دائمةً للابن. التأجيلُ يحفظُ المقبضَ سليمًا بلا حيلة.
            // (EN) Look up **without taking**: the handle is removed only after a
            //      successful wait. The interpreter takes-then-restores under the same id;
            //      re-storing here would mint a new id and kill the caller's handle, turning
            //      a transient failure into permanent loss of the child.
            llvm::Value *pid = b.CreateCall(getOrCreateFind(),
                                            {fn->getArg(0), llvm::ConstantInt::get(i64Ty, 0)}, "pid");
            llvm::Value *statusSlot = b.CreateAlloca(i32Ty, nullptr, "status");
            b.CreateStore(llvm::ConstantInt::get(i32Ty, 0), statusSlot);
            b.CreateCondBr(b.CreateICmpSLT(pid, llvm::ConstantInt::get(i64Ty, 0), "pid.miss"),
                           notChild, loop);

            b.SetInsertPoint(notChild);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_NOT_CHILD,
                     {{"handle", b.CreateCall(getOrCreateNumToStr(), {fn->getArg(0)}, "h.str")}});

            // (AR) تحييدُ SIGINT/SIGQUIT: بلا هذا يقتلُ Ctrl-C **البرنامجَ نفسَه** مع
            //      الابن — سلوكٌ خاطئٌ لصدَفة: الصدَفةُ تبقى والمقاطَعُ هو الأمرُ الجاري.
            b.SetInsertPoint(loop);
            auto *signalTy = llvm::FunctionType::get(ptrTy, {i32Ty, ptrTy}, false);
            auto signalFn = cg_.module_->getOrInsertFunction("signal", signalTy);
            llvm::Value *ignPtr = b.CreateIntToPtr(llvm::ConstantInt::get(i64Ty, plat().sigIgn), ptrTy, "sig.ign");
            llvm::Value *oldInt = b.CreateCall(signalFn,
                                               {llvm::ConstantInt::get(i32Ty, plat().sigInt), ignPtr}, "old.int");
            llvm::Value *oldQuit = b.CreateCall(signalFn,
                                                {llvm::ConstantInt::get(i32Ty, plat().sigQuit), ignPtr}, "old.quit");

            auto *waitpidTy = llvm::FunctionType::get(i32Ty, {i32Ty, ptrTy, i32Ty}, false);
            auto waitpidFn = cg_.module_->getOrInsertFunction("waitpid", waitpidTy);
            llvm::Value *rc = b.CreateCall(waitpidFn,
                                           {b.CreateTrunc(pid, i32Ty, "pid32"), statusSlot,
                                            llvm::ConstantInt::get(i32Ty, 0)},
                                           "rc");
            // (AR) تُعادُ المعالِجاتُ قبلَ أيِّ فرعٍ يخرجُ من الدالّة — وإلّا بقي
            //      البرنامجُ أصمَّ عن Ctrl-C إلى الأبد بعد أوّلِ فشلِ انتظار.
            b.CreateCall(signalFn, {llvm::ConstantInt::get(i32Ty, plat().sigInt), oldInt});
            b.CreateCall(signalFn, {llvm::ConstantInt::get(i32Ty, plat().sigQuit), oldQuit});
            b.CreateCondBr(b.CreateICmpSLT(rc, llvm::ConstantInt::get(i32Ty, 0), "rc.bad"),
                           waitErr, decode);

            b.SetInsertPoint(waitErr);
            auto *errnoTy = llvm::FunctionType::get(ptrTy, {}, false);
            auto errnoFn = cg_.module_->getOrInsertFunction(plat().errnoLocation, errnoTy);
            llvm::Value *err = b.CreateLoad(i32Ty, b.CreateCall(errnoFn, {}, "errno.p"), "errno.v");
            b.CreateCondBr(b.CreateICmpEQ(err, llvm::ConstantInt::get(i32Ty, plat().eIntr), "is.eintr"),
                           loop, restore);

            // (AR) ECHILD وحدَه يعني «ليس ابنًا» فيُسحبُ المقبض؛ وما عداه فشلٌ عابرٌ
            //      يُبقي المقبضَ صالحًا كي لا يبقى الابنُ زومبيًّا بلا سبيلٍ إلى حصاده.
            // (EN) Only ECHILD means "not a child" and takes the handle; any other
            //      failure is transient and leaves the handle valid.
            b.SetInsertPoint(restore);
            auto *gone = llvm::BasicBlock::Create(ctx, "gone", fn);
            b.CreateCondBr(b.CreateICmpEQ(err, llvm::ConstantInt::get(i32Ty, plat().eChild), "is.echild"),
                           gone, waitFail);

            b.SetInsertPoint(gone);
            b.CreateCall(getOrCreateFind(), {fn->getArg(0), llvm::ConstantInt::get(i64Ty, 1)});
            b.CreateBr(notChild);

            b.SetInsertPoint(waitFail);
            auto *strerrorTy = llvm::FunctionType::get(ptrTy, {i32Ty}, false);
            auto strerrorFn = cg_.module_->getOrInsertFunction("strerror", strerrorTy);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_WAIT_FAILED,
                     {{"handle", b.CreateCall(getOrCreateNumToStr(), {fn->getArg(0)}, "h.str2")},
                      {"reason", b.CreateCall(strerrorFn, {err}, "reason")}});

            // (AR) فكُّ الحالة: WIFEXITED ⇒ WEXITSTATUS · WIFSIGNALED ⇒ 128+الإشارة
            //      (اصطلاحُ الصدَفات) · غيرُهما ⇒ -1. حرفيًّا كما في المفسّر.
            b.SetInsertPoint(decode);
            // (AR) نجح الانتظار ⇒ الآن يُسحب المقبض: انتظارٌ ثانٍ عليه يجب أن يرمي.
            b.CreateCall(getOrCreateFind(), {fn->getArg(0), llvm::ConstantInt::get(i64Ty, 1)});
            llvm::Value *st = b.CreateSExt(b.CreateLoad(i32Ty, statusSlot, "st32"), i64Ty, "st");
            llvm::Value *low7 = b.CreateAnd(st, llvm::ConstantInt::get(i64Ty, 0x7f), "st.low7");
            llvm::Value *exited = b.CreateICmpEQ(low7, llvm::ConstantInt::get(i64Ty, 0), "wifexited");
            llvm::Value *exitCode = b.CreateAnd(b.CreateLShr(st, llvm::ConstantInt::get(i64Ty, 8)),
                                                llvm::ConstantInt::get(i64Ty, 0xff), "wexitstatus");
            // (AR) WIFSIGNALED: ((signed char)(((st & 0x7f) + 1) >> 1)) > 0
            llvm::Value *sigProbe = b.CreateLShr(b.CreateAdd(low7, llvm::ConstantInt::get(i64Ty, 1)),
                                                 llvm::ConstantInt::get(i64Ty, 1), "sig.probe");
            llvm::Value *sigProbeSc = b.CreateSExt(
                b.CreateTrunc(sigProbe, llvm::Type::getInt8Ty(ctx), "sig.i8"), i64Ty, "sig.sc");
            llvm::Value *signaled = b.CreateICmpSGT(sigProbeSc, llvm::ConstantInt::get(i64Ty, 0), "wifsignaled");
            llvm::Value *sigCode = b.CreateAdd(llvm::ConstantInt::get(i64Ty, 128), low7, "sig.code");
            llvm::Value *code = b.CreateSelect(
                exited, exitCode,
                b.CreateSelect(signaled, sigCode, llvm::ConstantInt::get(i64Ty, -1), "code.sig"),
                "code");
            b.CreateRet(code);
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) انبوب — طرفانِ بـCLOEXEC، والنتيجةُ تُملأُ في `out[0]`/`out[1]`
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreatePipe()
        {
            const char *name = "__sad_proc_pipe";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *voidTy = llvm::Type::getVoidTy(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            auto *fnTy = llvm::FunctionType::get(voidTy, {ptrTy}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *fail = llvm::BasicBlock::Create(ctx, "fail", fn);
            auto *ok = llvm::BasicBlock::Create(ctx, "ok", fn);

            llvm::IRBuilder<> b(entry);
            llvm::Value *fds = b.CreateAlloca(i32Ty, llvm::ConstantInt::get(i64Ty, 2), "fds");
            llvm::Value *rc = emitCloexecPipe(b, fds);
            b.CreateCondBr(b.CreateICmpSLT(rc, llvm::ConstantInt::get(i32Ty, 0), "rc.bad"), fail, ok);

            b.SetInsertPoint(fail);
            auto *errnoTy = llvm::FunctionType::get(ptrTy, {}, false);
            auto errnoFn = cg_.module_->getOrInsertFunction(plat().errnoLocation, errnoTy);
            llvm::Value *err = b.CreateLoad(i32Ty, b.CreateCall(errnoFn, {}, "errno.p"), "errno.v");
            auto *strerrorTy = llvm::FunctionType::get(ptrTy, {i32Ty}, false);
            auto strerrorFn = cg_.module_->getOrInsertFunction("strerror", strerrorTy);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_PIPE_FAILED,
                     {{"reason", b.CreateCall(strerrorFn, {err}, "reason")}});

            b.SetInsertPoint(ok);
            llvm::Function *store = getOrCreateStore();
            for (int i = 0; i < 2; ++i)
            {
                llvm::Value *fdGep = b.CreateGEP(i32Ty, fds, {llvm::ConstantInt::get(i64Ty, i)}, "fd.gep");
                llvm::Value *fd = b.CreateSExt(b.CreateLoad(i32Ty, fdGep, "fd"), i64Ty, "fd64");
                llvm::Value *h = b.CreateCall(store, {llvm::ConstantInt::get(i64Ty, kKindDescriptor), fd}, "h");
                b.CreateStore(h, b.CreateGEP(i64Ty, fn->getArg(0), {llvm::ConstantInt::get(i64Ty, i)}, "out.gep"));
            }
            b.CreateRetVoid();
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) شغل_برنامجا(path, argv, redirectMapOrNull) → مقبضُ عمليّة
        //      fork + execvp + أنبوبٌ ذاتيٌّ يميّزُ فشلَ التوجيهِ من فشلِ التنفيذ.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateSpawn()
        {
            if (plat().os == ProcTargetOS::Windows)
                return getOrCreateWinSpawn();

            const char *name = "__sad_proc_spawn";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i8Ty = llvm::Type::getInt8Ty(ctx);
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            // (AR) (مسار، مؤشّرُ بياناتِ الوسائط، عددُها، مؤشّرُ خريطةِ التوجيه أو عدم)
            auto *fnTy = llvm::FunctionType::get(i64Ty, {ptrTy, ptrTy, i64Ty, ptrTy}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());

            auto *errnoTy = llvm::FunctionType::get(ptrTy, {}, false);
            auto errnoFn = cg_.module_->getOrInsertFunction(plat().errnoLocation, errnoTy);
            auto *strerrorTy = llvm::FunctionType::get(ptrTy, {i32Ty}, false);
            auto strerrorFn = cg_.module_->getOrInsertFunction("strerror", strerrorTy);
            auto *closeTy = llvm::FunctionType::get(i32Ty, {i32Ty}, false);
            auto closeFn = cg_.module_->getOrInsertFunction(plat().os == ProcTargetOS::Windows ? "_close" : "close", closeTy);

            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            llvm::IRBuilder<> b(entry);

            auto errnoNow = [&](const char *nm) {
                return b.CreateLoad(i32Ty, b.CreateCall(errnoFn, {}, "errno.p"), nm);
            };

            // ── (AR) بناءُ argv: [المسار، الوسائط…، عدم] ──────────────────────
            // (AR) `argv[0]` هو المسارُ نفسُه — الوسائطُ في ص لا تتضمّنُ اسمَ البرنامج.
            llvm::Value *n = fn->getArg(2);
            llvm::Value *bytes = b.CreateMul(b.CreateAdd(n, llvm::ConstantInt::get(i64Ty, 2)),
                                             llvm::ConstantInt::get(i64Ty, 8), "argv.bytes");
            auto *mallocTy = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocTy);
            llvm::Value *argv = b.CreateCall(mallocFn, {bytes}, "argv");
            b.CreateStore(fn->getArg(0), b.CreateGEP(ptrTy, argv, {llvm::ConstantInt::get(i64Ty, 0)}, "argv0"));

            auto *copyLoop = llvm::BasicBlock::Create(ctx, "argv.loop", fn);
            auto *copyBody = llvm::BasicBlock::Create(ctx, "argv.body", fn);
            auto *copyDone = llvm::BasicBlock::Create(ctx, "argv.done", fn);
            llvm::Value *iSlot = b.CreateAlloca(i64Ty, nullptr, "i");
            // (AR) كلُّ التخصيصاتِ في كتلةِ الدخول: `alloca` داخلَ حلقةٍ ينمو بالمكدَّس
            //      كلَّ دورةٍ ولا يُحرَّرُ إلّا بالعودةِ من الدالّة.
            // (EN) Every alloca in the entry block: an alloca inside a loop grows the
            //      stack every iteration and is only released on return.
            llvm::Value *kSlot = b.CreateAlloca(i64Ty, nullptr, "k");
            llvm::Value *slotIdx = b.CreateAlloca(i64Ty, nullptr, "slotidx");
            llvm::Value *finalFd = b.CreateAlloca(i32Ty, nullptr, "fd.final");
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iSlot);
            b.CreateBr(copyLoop);

            b.SetInsertPoint(copyLoop);
            llvm::Value *i = b.CreateLoad(i64Ty, iSlot, "i.v");
            b.CreateCondBr(b.CreateICmpSLT(i, n, "i.lt"), copyBody, copyDone);

            b.SetInsertPoint(copyBody);
            llvm::Value *slot = b.CreateLoad(i64Ty, b.CreateGEP(i64Ty, fn->getArg(1), {i}, "arg.gep"), "arg.i64");
            b.CreateStore(b.CreateIntToPtr(slot, ptrTy, "arg.ptr"),
                          b.CreateGEP(ptrTy, argv, {b.CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1))}, "argv.gep"));
            b.CreateStore(b.CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1)), iSlot);
            b.CreateBr(copyLoop);

            b.SetInsertPoint(copyDone);
            b.CreateStore(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                          b.CreateGEP(ptrTy, argv, {b.CreateAdd(n, llvm::ConstantInt::get(i64Ty, 1))}, "argv.end"));

            // ── (AR) خريطةُ التوجيه ⇒ ثلاثةُ وصفاتٍ (‏-1 = لا توجيه) ──────────
            llvm::Value *redir = b.CreateAlloca(i32Ty, llvm::ConstantInt::get(i64Ty, 3), "redir");
            // (AR) `redirOwned[s]` = هل الوصفُ في الخانة **نسخةٌ صنعناها** بـF_DUPFD؟
            //      نسخةُ الأبِ مِلكُنا لا مِلكُ المستخدم، فتُغلق بعد fork وإلّا تسرّبت
            //      وصفةً كلَّ تشغيلٍ — وفي حلقةِ صدَفةٍ استنزافٌ مؤكَّد. المفسّرُ يُغلقها
            //      بـRAII، فتركُها هنا كان سيصير خلافًا صامتًا لا يظهر إلّا بعد ألفِ أمر.
            // (EN) Track which redirect fds are OUR F_DUPFD copies; the parent's copy is
            //      ours, not the user's, and must be closed after fork — the interpreter
            //      closes them via RAII, so leaving them would leak one fd per spawn.
            llvm::Value *redirOwned = b.CreateAlloca(i32Ty, llvm::ConstantInt::get(i64Ty, 3), "redir.owned");
            for (int s = 0; s < 3; ++s)
            {
                b.CreateStore(llvm::ConstantInt::get(i32Ty, -1),
                              b.CreateGEP(i32Ty, redir, {llvm::ConstantInt::get(i64Ty, s)}, "redir.init"));
                b.CreateStore(llvm::ConstantInt::get(i32Ty, 0),
                              b.CreateGEP(i32Ty, redirOwned, {llvm::ConstantInt::get(i64Ty, s)}, "owned.init"));
            }

            auto *hasMap = llvm::BasicBlock::Create(ctx, "map", fn);
            auto *mapLoop = llvm::BasicBlock::Create(ctx, "map.loop", fn);
            auto *mapBody = llvm::BasicBlock::Create(ctx, "map.body", fn);
            auto *mapSkip = llvm::BasicBlock::Create(ctx, "map.skip", fn);
            auto *mapKey = llvm::BasicBlock::Create(ctx, "map.key", fn);
            auto *mapBadKey = llvm::BasicBlock::Create(ctx, "map.badkey", fn);
            auto *mapVal = llvm::BasicBlock::Create(ctx, "map.val", fn);
            auto *mapMiss = llvm::BasicBlock::Create(ctx, "map.miss", fn);
            auto *mapNext = llvm::BasicBlock::Create(ctx, "map.next", fn);
            auto *afterMap = llvm::BasicBlock::Create(ctx, "after.map", fn);

            llvm::Value *mapPtr = fn->getArg(3);
            b.CreateCondBr(b.CreateICmpNE(mapPtr,
                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                                          "has.map"),
                           hasMap, afterMap);

            // (AR) تخطيطُ الخريطة: {عدد، سعة، مفاتيح، قيم، أنواع} — نفسُ ما يبنيه
            //      map_ops.cpp. المفاتيحُ نصوصٌ، والخانةُ الفارغةُ مفتاحُها عدم.
            b.SetInsertPoint(hasMap);
            llvm::Value *cap = b.CreateLoad(i64Ty, b.CreateGEP(i64Ty, mapPtr, {llvm::ConstantInt::get(i64Ty, 1)}, "cap.gep"), "cap");
            llvm::Value *keys = b.CreateIntToPtr(
                b.CreateLoad(i64Ty, b.CreateGEP(i64Ty, mapPtr, {llvm::ConstantInt::get(i64Ty, 2)}, "keys.gep"), "keys.i64"),
                ptrTy, "keys");
            llvm::Value *valsArr = b.CreateIntToPtr(
                b.CreateLoad(i64Ty, b.CreateGEP(i64Ty, mapPtr, {llvm::ConstantInt::get(i64Ty, 3)}, "vals.gep"), "vals.i64"),
                ptrTy, "vals");
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), kSlot);
            b.CreateBr(mapLoop);

            b.SetInsertPoint(mapLoop);
            llvm::Value *k = b.CreateLoad(i64Ty, kSlot, "k.v");
            b.CreateCondBr(b.CreateICmpSLT(k, cap, "k.lt"), mapBody, afterMap);

            b.SetInsertPoint(mapBody);
            llvm::Value *keyPtr = b.CreateLoad(ptrTy, b.CreateGEP(ptrTy, keys, {k}, "key.gep"), "key");
            b.CreateCondBr(b.CreateICmpEQ(keyPtr,
                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                                          "key.empty"),
                           mapSkip, mapKey);

            b.SetInsertPoint(mapSkip);
            b.CreateBr(mapNext);

            b.SetInsertPoint(mapKey);
            auto *strcmpTy = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto strcmpFn = cg_.module_->getOrInsertFunction("strcmp", strcmpTy);
            auto keyIs = [&](const char *text, const char *nm) {
                return b.CreateICmpEQ(
                    b.CreateCall(strcmpFn, {keyPtr, b.CreateGlobalStringPtr(text, "proc.key")}, nm),
                    llvm::ConstantInt::get(i32Ty, 0), std::string(nm) + ".eq");
            };
            llvm::Value *is0 = keyIs("0", "key0");
            llvm::Value *is1 = keyIs("1", "key1");
            llvm::Value *is2 = keyIs("2", "key2");
            llvm::Value *idx = b.CreateSelect(is0, llvm::ConstantInt::get(i64Ty, 0),
                                              b.CreateSelect(is1, llvm::ConstantInt::get(i64Ty, 1),
                                                             llvm::ConstantInt::get(i64Ty, 2), "idx.12"),
                                              "idx");
            b.CreateStore(idx, slotIdx);
            // (AR) رمزٌ خاصٌّ بالمفتاح: العلّةُ فيه لا في الوصف. كان يُبلَّغُ «الوصف
            //      غير معروف» — كذبٌ مرّتين: الوصفُ معروفٌ، والمذكورُ ليس وصفًا.
            b.CreateCondBr(b.CreateOr(b.CreateOr(is0, is1), is2, "key.known"), mapVal, mapBadKey);

            b.SetInsertPoint(mapBadKey);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_BAD_STREAM_KEY, {{"key", keyPtr}});

            b.SetInsertPoint(mapVal);
            llvm::Value *handle = b.CreateLoad(i64Ty, b.CreateGEP(i64Ty, valsArr, {k}, "val.gep"), "handle");
            b.CreateCall(getOrCreateKindGuard(),
                         {handle, llvm::ConstantInt::get(i64Ty, kKindDescriptor),
                          cstr(b, "وصفًا من انبوب أو افتح_وصفا")});
            llvm::Value *fdv = b.CreateCall(getOrCreateFind(), {handle, llvm::ConstantInt::get(i64Ty, 0)}, "fd");
            auto *mapStore = llvm::BasicBlock::Create(ctx, "map.store", fn);
            b.CreateCondBr(b.CreateICmpSLT(fdv, llvm::ConstantInt::get(i64Ty, 0), "fd.miss"),
                           mapMiss, mapStore);

            b.SetInsertPoint(mapMiss);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_BAD_DESCRIPTOR,
                     {{"handle", b.CreateCall(getOrCreateNumToStr(), {handle}, "h.str")}});

            // (AR) نقلُ وصفٍ رقمُه دون ٣ إلى نطاقٍ أعلى قبل حلقةِ dup2: لولاه لدهسَ
            //      توجيهٌ سابقٌ مصدرَ توجيهٍ لاحقٍ ⇒ **توجيهٌ خاطئٌ صامتٌ** لا فشلٌ يُرى.
            b.SetInsertPoint(mapStore);
            auto *fcntlTy = llvm::FunctionType::get(i32Ty, {i32Ty, i32Ty}, true);
            auto fcntlFn = cg_.module_->getOrInsertFunction("fcntl", fcntlTy);
            llvm::Value *fd32 = b.CreateTrunc(fdv, i32Ty, "fd32");
            llvm::Value *low = b.CreateICmpSLT(fd32, llvm::ConstantInt::get(i32Ty, 3), "fd.low");
            auto *shiftBB = llvm::BasicBlock::Create(ctx, "fd.shift", fn);
            auto *keepBB = llvm::BasicBlock::Create(ctx, "fd.keep", fn);
            b.CreateStore(fd32, finalFd);
            b.CreateCondBr(low, shiftBB, keepBB);

            b.SetInsertPoint(shiftBB);
            llvm::Value *moved = b.CreateCall(fcntlFn,
                                              {fd32, llvm::ConstantInt::get(i32Ty, plat().fDupFdCloexec),
                                               llvm::ConstantInt::get(i32Ty, 3)},
                                              "fd.moved");
            // (AR) فشلُ النقلِ لا يُبتلَع: نُبقي الأصلَ فيفشلَ dup2 صاخبًا في الابن
            //      بدل توجيهٍ صامتٍ إلى الوصفِ الخطأ.
            llvm::Value *movedOk = b.CreateICmpSGE(moved, llvm::ConstantInt::get(i32Ty, 0), "moved.ok");
            b.CreateStore(b.CreateSelect(movedOk, moved, fd32, "fd.pick"), finalFd);
            b.CreateStore(b.CreateSelect(movedOk, llvm::ConstantInt::get(i32Ty, 1),
                                         llvm::ConstantInt::get(i32Ty, 0), "owned.pick"),
                          b.CreateGEP(i32Ty, redirOwned, {b.CreateLoad(i64Ty, slotIdx, "idx.o")}, "owned.gep"));
            b.CreateBr(keepBB);

            b.SetInsertPoint(keepBB);
            b.CreateStore(b.CreateLoad(i32Ty, finalFd, "fd.f"),
                          b.CreateGEP(i32Ty, redir, {b.CreateLoad(i64Ty, slotIdx, "idx.v")}, "redir.gep"));
            b.CreateBr(mapNext);

            b.SetInsertPoint(mapNext);
            b.CreateStore(b.CreateAdd(b.CreateLoad(i64Ty, kSlot, "k.c"), llvm::ConstantInt::get(i64Ty, 1)), kSlot);
            b.CreateBr(mapLoop);

            // ── (AR) الأنبوبُ الذاتيّ ثمّ fork ───────────────────────────────
            b.SetInsertPoint(afterMap);
            llvm::Value *errPipe = b.CreateAlloca(i32Ty, llvm::ConstantInt::get(i64Ty, 2), "errpipe");
            llvm::Value *prc = emitCloexecPipe(b, errPipe);
            auto *pipeFail = llvm::BasicBlock::Create(ctx, "pipe.fail", fn);
            auto *doFork = llvm::BasicBlock::Create(ctx, "fork", fn);
            b.CreateCondBr(b.CreateICmpSLT(prc, llvm::ConstantInt::get(i32Ty, 0), "prc.bad"), pipeFail, doFork);

            b.SetInsertPoint(pipeFail);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_PIPE_FAILED,
                     {{"reason", b.CreateCall(strerrorFn, {errnoNow("errno.pipe")}, "reason")}});

            b.SetInsertPoint(doFork);
            auto readEnd = [&]() {
                return b.CreateLoad(i32Ty, b.CreateGEP(i32Ty, errPipe, {llvm::ConstantInt::get(i64Ty, 0)}, "rd.gep"), "rd");
            };
            auto writeEnd = [&]() {
                return b.CreateLoad(i32Ty, b.CreateGEP(i32Ty, errPipe, {llvm::ConstantInt::get(i64Ty, 1)}, "wr.gep"), "wr");
            };
            auto *forkTy = llvm::FunctionType::get(i32Ty, {}, false);
            auto forkFn = cg_.module_->getOrInsertFunction("fork", forkTy);
            llvm::Value *pid = b.CreateCall(forkFn, {}, "pid");
            auto *forkFail = llvm::BasicBlock::Create(ctx, "fork.fail", fn);
            auto *forkOk = llvm::BasicBlock::Create(ctx, "fork.ok", fn);
            b.CreateCondBr(b.CreateICmpSLT(pid, llvm::ConstantInt::get(i32Ty, 0), "pid.bad"), forkFail, forkOk);

            b.SetInsertPoint(forkFail);
            llvm::Value *forkErr = errnoNow("errno.fork");
            b.CreateCall(closeFn, {readEnd()});
            b.CreateCall(closeFn, {writeEnd()});
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_SPAWN_FAILED,
                     {{"path", fn->getArg(0)},
                      {"reason", b.CreateCall(strerrorFn, {forkErr}, "reason")}});

            b.SetInsertPoint(forkOk);
            auto *childBB = llvm::BasicBlock::Create(ctx, "child", fn);
            auto *parentBB = llvm::BasicBlock::Create(ctx, "parent", fn);
            b.CreateCondBr(b.CreateICmpEQ(pid, llvm::ConstantInt::get(i32Ty, 0), "is.child"), childBB, parentBB);

            // ══ (AR) الابن ══════════════════════════════════════════════════
            b.SetInsertPoint(childBB);
            b.CreateCall(closeFn, {readEnd()});
            llvm::Value *report = b.CreateAlloca(i32Ty, llvm::ConstantInt::get(i64Ty, 2), "report");
            auto *writeTy = llvm::FunctionType::get(i64Ty, {i32Ty, ptrTy, i64Ty}, false);
            auto writeFn = cg_.module_->getOrInsertFunction("write", writeTy);
            auto *exitTy = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), {i32Ty}, false);
            auto uexitFn = cg_.module_->getOrInsertFunction("_exit", exitTy);
            auto *dup2Ty = llvm::FunctionType::get(i32Ty, {i32Ty, i32Ty}, false);
            auto dup2Fn = cg_.module_->getOrInsertFunction("dup2", dup2Ty);

            auto childReport = [&](int64_t kind, llvm::Value *errVal) {
                b.CreateStore(llvm::ConstantInt::get(i32Ty, kind),
                              b.CreateGEP(i32Ty, report, {llvm::ConstantInt::get(i64Ty, 0)}, "rep.k"));
                b.CreateStore(errVal,
                              b.CreateGEP(i32Ty, report, {llvm::ConstantInt::get(i64Ty, 1)}, "rep.e"));
                b.CreateCall(writeFn, {writeEnd(), report, llvm::ConstantInt::get(i64Ty, kChildReportBytes)});
                b.CreateCall(uexitFn, {llvm::ConstantInt::get(i32Ty, 127)});
                b.CreateUnreachable();
            };

            // (AR) ثلاثةُ توجيهاتٍ مفرودةٌ لا حلقةً — أوضحُ وأقصرُ في IR.
            for (int s = 0; s < 3; ++s)
            {
                auto *tryBB = llvm::BasicBlock::Create(ctx, "dup.try", fn);
                auto *failBB = llvm::BasicBlock::Create(ctx, "dup.fail", fn);
                auto *nextBB = llvm::BasicBlock::Create(ctx, "dup.next", fn);
                llvm::Value *rfd = b.CreateLoad(
                    i32Ty, b.CreateGEP(i32Ty, redir, {llvm::ConstantInt::get(i64Ty, s)}, "redir.get"), "rfd");
                b.CreateCondBr(b.CreateICmpSGE(rfd, llvm::ConstantInt::get(i32Ty, 0), "rfd.set"), tryBB, nextBB);

                b.SetInsertPoint(tryBB);
                llvm::Value *drc = b.CreateCall(dup2Fn, {rfd, llvm::ConstantInt::get(i32Ty, s)}, "drc");
                b.CreateCondBr(b.CreateICmpSLT(drc, llvm::ConstantInt::get(i32Ty, 0), "drc.bad"), failBB, nextBB);

                b.SetInsertPoint(failBB);
                childReport(kChildFailRedirect, errnoNow("errno.dup"));

                b.SetInsertPoint(nextBB);
            }

            // (AR) تُعادُ الإشاراتُ الثلاثُ إلى الافتراضيّ ويُرفَعُ القناعُ الموروث:
            //      كلاهما ينجو من exec. تجاهلُ SIGPIPE يكسرُ إنهاءَ الأنابيبِ
            //      المعتاد (‏`… | head` لا ينتهي)، والقناعُ الموروثُ يُصِمُّ الابنَ
            //      عن إشاراتٍ لم يختر صممَه عنها.
            auto *signalTy2 = llvm::FunctionType::get(ptrTy, {i32Ty, ptrTy}, false);
            auto signalFn2 = cg_.module_->getOrInsertFunction("signal", signalTy2);
            llvm::Value *dfl = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));
            for (int64_t sig : {plat().sigPipe, plat().sigInt, plat().sigQuit})
                b.CreateCall(signalFn2, {llvm::ConstantInt::get(i32Ty, sig), dfl});

            llvm::Value *emptySet = b.CreateAlloca(i8Ty, llvm::ConstantInt::get(i64Ty, plat().sigsetBytes), "sigset");
            auto *sigemptyTy = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto sigemptyFn = cg_.module_->getOrInsertFunction("sigemptyset", sigemptyTy);
            b.CreateCall(sigemptyFn, {emptySet});
            auto *sigprocTy = llvm::FunctionType::get(i32Ty, {i32Ty, ptrTy, ptrTy}, false);
            auto sigprocFn = cg_.module_->getOrInsertFunction("sigprocmask", sigprocTy);
            b.CreateCall(sigprocFn, {llvm::ConstantInt::get(i32Ty, plat().sigSetMask), emptySet,
                                     llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy))});

            auto *execvpTy = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto execvpFn = cg_.module_->getOrInsertFunction("execvp", execvpTy);
            b.CreateCall(execvpFn, {fn->getArg(0), argv});
            childReport(kChildFailExec, errnoNow("errno.exec"));

            // ══ (AR) الأب ═══════════════════════════════════════════════════
            b.SetInsertPoint(parentBB);
            b.CreateCall(closeFn, {writeEnd()});
            // (AR) تحريرُ ما يملكه الأبُ وحدَه: نسخُ F_DUPFD وناقلةُ argv. الابنُ لا يعنيه
            //      (‏exec يستبدلُ صورتَه، والفشلُ يخرجُ بـ_exit).
            {
                auto *freeTy = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), {ptrTy}, false);
                auto freeFn = cg_.module_->getOrInsertFunction("free", freeTy);
                b.CreateCall(freeFn, {argv});
                for (int s = 0; s < 3; ++s)
                {
                    auto *doClose = llvm::BasicBlock::Create(ctx, "own.close", fn);
                    auto *skip = llvm::BasicBlock::Create(ctx, "own.skip", fn);
                    llvm::Value *ownFlag = b.CreateLoad(
                        i32Ty, b.CreateGEP(i32Ty, redirOwned, {llvm::ConstantInt::get(i64Ty, s)}, "own.gep"), "own");
                    b.CreateCondBr(b.CreateICmpNE(ownFlag, llvm::ConstantInt::get(i32Ty, 0), "own.set"),
                                   doClose, skip);
                    b.SetInsertPoint(doClose);
                    b.CreateCall(closeFn, {b.CreateLoad(
                                              i32Ty, b.CreateGEP(i32Ty, redir, {llvm::ConstantInt::get(i64Ty, s)},
                                                                 "own.fd.gep"),
                                              "own.fd")});
                    b.CreateBr(skip);
                    b.SetInsertPoint(skip);
                }
            }
            llvm::Value *pbuf = b.CreateAlloca(i32Ty, llvm::ConstantInt::get(i64Ty, 2), "pbuf");
            b.CreateStore(llvm::ConstantInt::get(i32Ty, 0),
                          b.CreateGEP(i32Ty, pbuf, {llvm::ConstantInt::get(i64Ty, 0)}, "pb0"));
            b.CreateStore(llvm::ConstantInt::get(i32Ty, 0),
                          b.CreateGEP(i32Ty, pbuf, {llvm::ConstantInt::get(i64Ty, 1)}, "pb1"));
            llvm::Value *gotSlot = b.CreateAlloca(i64Ty, nullptr, "got");

            auto *readLoop = llvm::BasicBlock::Create(ctx, "read.loop", fn);
            auto *readNeg = llvm::BasicBlock::Create(ctx, "read.neg", fn);
            auto *readDone = llvm::BasicBlock::Create(ctx, "read.done", fn);
            auto *readTy = llvm::FunctionType::get(i64Ty, {i32Ty, ptrTy, i64Ty}, false);
            auto readFn = cg_.module_->getOrInsertFunction("read", readTy);
            b.CreateBr(readLoop);

            b.SetInsertPoint(readLoop);
            llvm::Value *got = b.CreateCall(readFn, {readEnd(), pbuf,
                                                     llvm::ConstantInt::get(i64Ty, kChildReportBytes)}, "got");
            b.CreateStore(got, gotSlot);
            b.CreateCondBr(b.CreateICmpSLT(got, llvm::ConstantInt::get(i64Ty, 0), "got.neg"), readNeg, readDone);

            b.SetInsertPoint(readNeg);
            b.CreateCondBr(b.CreateICmpEQ(errnoNow("errno.read"), llvm::ConstantInt::get(i32Ty, plat().eIntr), "rd.eintr"),
                           readLoop, readDone);

            b.SetInsertPoint(readDone);
            b.CreateCall(closeFn, {readEnd()});
            llvm::Value *gotV = b.CreateLoad(i64Ty, gotSlot, "got.v");

            auto *reapKill = llvm::BasicBlock::Create(ctx, "reap.kill", fn);
            auto *checkReport = llvm::BasicBlock::Create(ctx, "check.report", fn);
            b.CreateCondBr(b.CreateICmpSLT(gotV, llvm::ConstantInt::get(i64Ty, 0), "got.err"),
                           reapKill, checkReport);

            // (AR) `got < 0` ليست «لا بلاغَ ⇒ نجاح»: كان السالبُ يسقطُ في فرعِ
            //      النجاحِ فيُعادُ مقبضٌ لعمليّةٍ **لم يُتحقّق من تشغيلها** — عودةُ
            //      الالتباسِ الذي وُضع الأنبوبُ لإزالته. ولأنّ مصيرَ الابنِ مجهول
            //      يُقتَلُ ويُحصَد: حصادٌ حاجبٌ بلا قتلٍ يُعلّقُ البرنامجَ خلفَ ابنٍ طويل،
            //      وتركُه بلا حصادٍ يُخلّفُ يتيمًا لا مقبضَ له.
            b.SetInsertPoint(reapKill);
            llvm::Value *rdErr = errnoNow("errno.rd2");
            auto *killTy = llvm::FunctionType::get(i32Ty, {i32Ty, i32Ty}, false);
            auto killFn = cg_.module_->getOrInsertFunction("kill", killTy);
            b.CreateCall(killFn, {pid, llvm::ConstantInt::get(i32Ty, plat().sigKill)});
            auto *waitpidTy = llvm::FunctionType::get(i32Ty, {i32Ty, ptrTy, i32Ty}, false);
            auto waitpidFn = cg_.module_->getOrInsertFunction("waitpid", waitpidTy);
            llvm::Value *st1 = b.CreateAlloca(i32Ty, nullptr, "st1");
            b.CreateCall(waitpidFn, {pid, st1, llvm::ConstantInt::get(i32Ty, 0)});
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_SPAWN_FAILED,
                     {{"path", fn->getArg(0)},
                      {"reason", b.CreateCall(strerrorFn, {rdErr}, "reason")}});

            b.SetInsertPoint(checkReport);
            auto *failed = llvm::BasicBlock::Create(ctx, "child.failed", fn);
            auto *spawned = llvm::BasicBlock::Create(ctx, "spawned", fn);
            b.CreateCondBr(b.CreateICmpSGT(gotV, llvm::ConstantInt::get(i64Ty, 0), "got.some"),
                           failed, spawned);

            b.SetInsertPoint(failed);
            llvm::Value *st2 = b.CreateAlloca(i32Ty, nullptr, "st2");
            b.CreateCall(waitpidFn, {pid, st2, llvm::ConstantInt::get(i32Ty, 0)});
            llvm::Value *rkind = b.CreateLoad(i32Ty, b.CreateGEP(i32Ty, pbuf, {llvm::ConstantInt::get(i64Ty, 0)}, "rk.gep"), "rkind");
            llvm::Value *rerr = b.CreateLoad(i32Ty, b.CreateGEP(i32Ty, pbuf, {llvm::ConstantInt::get(i64Ty, 1)}, "re.gep"), "rerr");
            llvm::Value *partial = b.CreateICmpSLT(gotV, llvm::ConstantInt::get(i64Ty, kChildReportBytes), "partial");
            llvm::Value *reason = b.CreateSelect(partial, cstr(b, "بلاغُ فشلٍ ناقصٌ من الابن"),
                                                 b.CreateCall(strerrorFn, {rerr}, "reason.raw"), "reason");
            auto *redirFail = llvm::BasicBlock::Create(ctx, "redir.fail", fn);
            auto *execFail = llvm::BasicBlock::Create(ctx, "exec.fail", fn);
            // (AR) يُميَّزُ فشلُ التوجيهِ من فشلِ التنفيذِ برمزٍ مستقلّ: الأوّلُ علّتُه
            //      في الوصفِ لا في المسار، والبرنامجُ لم يُشغَّل أصلًا.
            b.CreateCondBr(b.CreateAnd(b.CreateNot(partial, "full"),
                                       b.CreateICmpEQ(rkind, llvm::ConstantInt::get(i32Ty, kChildFailRedirect), "is.redir"),
                                       "redir.reported"),
                           redirFail, execFail);

            b.SetInsertPoint(redirFail);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_REDIRECT_FAILED,
                     {{"path", fn->getArg(0)}, {"reason", reason}});

            b.SetInsertPoint(execFail);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_SPAWN_FAILED,
                     {{"path", fn->getArg(0)}, {"reason", reason}});

            b.SetInsertPoint(spawned);
            b.CreateRet(b.CreateCall(getOrCreateStore(),
                                     {llvm::ConstantInt::get(i64Ty, kKindProcess),
                                      b.CreateSExt(pid, i64Ty, "pid64")},
                                     "handle"));
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) الموزِّع
        // ════════════════════════════════════════════════════════════════════
        llvm::Value *ProcessesBuiltinsCodeGen::emitProcessesBuiltin(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            switch (inst->opcode)
            {
            case SIROpcode::BUILTIN_PROC_SPAWN:
            case SIROpcode::BUILTIN_PROC_WAIT:
            case SIROpcode::BUILTIN_PROC_PIPE:
            case SIROpcode::BUILTIN_PROC_CLOSE:
            case SIROpcode::BUILTIN_PROC_OPEN_FD:
                break;
            default:
                return nullptr;
            }

            // (AR) هدفٌ خارجَ الثلاثةِ المعروفة: رفضٌ صريحٌ لا توليدُ أعلامٍ خاطئةٍ
            //      صامتة. والرفضُ زمنَ الترجمةِ لا زمنَ التشغيل: ثنائيٌّ يُبنى ثمّ
            //      يفتحُ الملفَّ بعلمٍ آخرَ أسوأُ من ترجمةٍ لا تتمّ.
            if (plat().os == ProcTargetOS::Unsupported)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::RUN_PROC_UNSUPPORTED_PLATFORM,
                                {{"function", "وحدة عمليات"}});
                return nullptr;
            }

            auto &ctx = *cg_.context_;
            auto *i64Ty = cg_.getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);
            llvm::IRBuilder<> &b = *cg_.builder_;

            auto finish = [&](llvm::Value *v) -> llvm::Value * {
                if (v && inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = v;
                return v;
            };
            auto missingArgs = [&]() -> llvm::Value * {
                cg_.reportError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG,
                                {{"detail", std::string(sirOpcodeToString(inst->opcode))}});
                return nullptr;
            };
            auto asPtr = [&](llvm::Value *v, const char *nm) -> llvm::Value * {
                if (!v)
                    return nullptr;
                if (v->getType()->isPointerTy())
                    return v;
                if (isSadDyn(v))
                    return unpackPtr(cg_, v);
                return b.CreateIntToPtr(v, ptrTy, nm);
            };
            auto asI64 = [&](llvm::Value *v, const char *nm) -> llvm::Value * {
                if (!v)
                    return nullptr;
                if (isSadDyn(v))
                    return unpackI64(cg_, v);
                if (v->getType()->isPointerTy())
                    return b.CreatePtrToInt(v, i64Ty, nm);
                if (v->getType()->isIntegerTy() && !v->getType()->isIntegerTy(64))
                    return b.CreateSExtOrTrunc(v, i64Ty, nm);
                return v;
            };

            switch (inst->opcode)
            {
            case SIROpcode::BUILTIN_PROC_OPEN_FD:
            {
                if (inst->operands.size() < 2)
                    return missingArgs();
                llvm::Value *path = asPtr(cg_.resolveOperand(inst->operands[0]), "proc.path");
                llvm::Value *mode = asPtr(cg_.resolveOperand(inst->operands[1]), "proc.mode");
                if (!path || !mode)
                    return nullptr;
                return finish(b.CreateCall(getOrCreateOpenFd(), {path, mode}, "proc.openfd"));
            }

            case SIROpcode::BUILTIN_PROC_CLOSE:
            {
                if (inst->operands.empty())
                    return missingArgs();
                llvm::Value *h = asI64(cg_.resolveOperand(inst->operands[0]), "proc.h");
                if (!h)
                    return nullptr;
                // (AR) `اغلق` تُرجعُ «صحيح» في المفسّر — والنوعُ الساكنُ منطقيّ.
                llvm::Value *r = b.CreateCall(getOrCreateClose(), {h}, "proc.close");
                return finish(b.CreateICmpNE(r, llvm::ConstantInt::get(i64Ty, 0), "proc.close.b"));
            }

            case SIROpcode::BUILTIN_PROC_WAIT:
            {
                if (inst->operands.empty())
                    return missingArgs();
                llvm::Value *h = asI64(cg_.resolveOperand(inst->operands[0]), "proc.h");
                if (!h)
                    return nullptr;
                return finish(b.CreateCall(getOrCreateWait(), {h}, "proc.wait"));
            }

            case SIROpcode::BUILTIN_PROC_PIPE:
            {
                llvm::Value *out = b.CreateAlloca(i64Ty, llvm::ConstantInt::get(i64Ty, 2), "proc.pipe.out");
                b.CreateCall(getOrCreatePipe(), {out});
                llvm::Value *rd = b.CreateLoad(i64Ty, b.CreateGEP(i64Ty, out, {llvm::ConstantInt::get(i64Ty, 0)}, "rd.gep"), "rd");
                llvm::Value *wr = b.CreateLoad(i64Ty, b.CreateGEP(i64Ty, out, {llvm::ConstantInt::get(i64Ty, 1)}, "wr.gep"), "wr");

                // (AR) الخريطةُ تُبنى بنفسِ مسارِ الخريطةِ الحرفيّة (map_ops.cpp)
                //      كي تعملَ عليها كلُّ طرائقِ الخرائطِ بلا استثناءٍ خاصّ.
                // (AR) `emitCallMap` تقرأُ `inst->result` بلا حارس، فتعليمةٌ عدميّةٌ
                //      تُسقِطُ المولِّد. نمرّرُ التعليمةَ الحقيقيّةَ للإنشاء (النتيجةُ
                //      هي الخريطةُ فعلًا)، وتعليمةً صامتةً بلا نتيجةٍ للإدراج.
                std::vector<llvm::Value *> createArgs{llvm::ConstantInt::get(i64Ty, 2)};
                auto created = cg_.emitCallMap("__sad_map_create", createArgs, inst);
                if (!created.has_value() || !created.value())
                    return nullptr;
                llvm::Value *mapPtr = created.value();

                auto silent = std::make_shared<SIRInstruction>(SIROpcode::Nop);
                auto setEntry = [&](const char *key, llvm::Value *val) {
                    std::vector<llvm::Value *> setArgs{
                        mapPtr, b.CreateGlobalStringPtr(key, "proc.pipe.key"), val,
                        llvm::ConstantInt::get(i64Ty, 1)}; // 1 = عدد صحيح
                    cg_.emitCallMap("__sad_map_set_typed", setArgs, silent);
                };
                setEntry("قراءة", rd);
                setEntry("كتابة", wr);
                return finish(mapPtr);
            }

            case SIROpcode::BUILTIN_PROC_SPAWN:
            {
                if (inst->operands.size() < 2)
                    return missingArgs();
                llvm::Value *path = asPtr(cg_.resolveOperand(inst->operands[0]), "proc.path");
                llvm::Value *arrRaw = cg_.resolveOperand(inst->operands[1]);
                if (!path || !arrRaw)
                    return nullptr;

                // (AR) وسيطٌ ثانٍ ليس قائمةً يفشلُ صراحةً — المفسّرُ يرمي
                //      SEM_TYPE_MISMATCH زمنَ التشغيل، وهنا نرفضُه زمنَ الترجمة.
                const SadTypeKind argsKind = inst->operands[1].dataType;
                if (argsKind == SadTypeKind::String || argsKind == SadTypeKind::Integer ||
                    argsKind == SadTypeKind::Float || argsKind == SadTypeKind::Boolean)
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH,
                                    {{"expected", "قائمة وسائط نصّيّة"},
                                     {"found", "قيمة مفردة"}});
                    return nullptr;
                }

                llvm::Value *arrPtr = cg_.normalizeArrayPtr(arrRaw, "proc.args");
                if (!arrPtr)
                    return nullptr;
                llvm::StructType *arrTy = sadArrayStructType(ctx);
                llvm::Value *len = b.CreateLoad(i64Ty, b.CreateStructGEP(arrTy, arrPtr, 0, "args.len.gep"), "args.len");
                llvm::Value *data = b.CreateLoad(ptrTy, b.CreateStructGEP(arrTy, arrPtr, 2, "args.data.gep"), "args.data");

                llvm::Value *mapPtr = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));
                if (inst->operands.size() >= 3)
                {
                    const SadTypeKind mk = inst->operands[2].dataType;
                    if (mk == SadTypeKind::String || mk == SadTypeKind::Integer ||
                        mk == SadTypeKind::Float || mk == SadTypeKind::Boolean)
                    {
                        // (AR) تمريرُ وصفٍ بدل خريطةٍ كان يُهملُ التوجيهَ صامتًا:
                        //      الخرجُ إلى الشاشةِ والملفُّ فارغٌ والرمزُ ٠. «نجحَ» وما كتب.
                        cg_.reportError(::Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH,
                                        {{"expected", "خريطة توجيه {رقم_المجرى: وصف}"},
                                         {"found", "قيمة مفردة"}});
                        return nullptr;
                    }
                    llvm::Value *mv = cg_.resolveOperand(inst->operands[2]);
                    if (mv)
                        mapPtr = asPtr(mv, "proc.redir");
                }

                return finish(b.CreateCall(getOrCreateSpawn(), {path, data, len, mapPtr}, "proc.spawn"));
            }

            default:
                return nullptr;
            }
        }

    } // namespace LLVM
} // namespace Sad
