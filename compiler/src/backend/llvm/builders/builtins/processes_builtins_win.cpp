// ============================================================================
// (AR) ملف: processes_builtins_win.cpp
// (EN) File: processes_builtins_win.cpp
// المكون: Sad::LLVM
// ============================================================================
// (AR) نظيرُ ويندوز لمولِّدِ بدائيّاتِ «عمليات» — Win32 + CRT، لا محاكاةَ POSIX.
//
//      **لماذا ملفٌّ منفصلٌ لا فرعٌ في كلِّ دالّة**: ما يفترقُ هنا يفترقُ بنيويًّا
//      لا بالأسماء. لا `fork` فلا لحظةً وسطى، ولا `waitpid` فلا فكَّ حالة، ولا
//      أنبوبَ ذاتيًّا لأنّ `CreateProcess` تُبلّغُ الفشلَ **قبل** وجودِ الابن.
//      أمّا `افتح_وصفا` و`اغلق` و`انبوب` فبنيةٌ واحدةٌ بأعلامٍ مختلفة، فبقيت في
//      الملفِّ الأصلِ مُعامِلةً لا منسوخة.
//
//      **والمساعداتُ المشتركةُ مشتركةٌ عمدًا**: `__sad_proc_store`/`find`/
//      `kind_guard`/`num2str`/`fail` واحدةٌ للمنصّتين، وهي موضعُ دلالةِ المقبضِ
//      كلِّها — المعرّفُ التصاعديُّ ووسمُ النوعِ ونصُّ التشخيص. فتفريقُها كان
//      سيُفرِّقُ **التطابقَ** من حيث لا يقيسُ اختبار: ويندوزُ يمرُّ باختباراتِه
//      وينحرفُ عن لينكس في نقطةٍ لا اختبارَ لها.
//
//      **ثلاثةُ فروقٍ معلَنةٌ في الدلالة** (لا في التنفيذ)، وهي نفسُها المذكورةُ
//      في المفسّرِ حرفًا بحرف:
//        ١. `انتظر_عملية` تُرجعُ رمزَ الخروجِ خامًّا — لا اصطلاحَ `128+الإشارة`.
//        ٢. البحثُ عن البرنامجِ بقواعدِ `CreateProcess` لا `execvp`.
//        ٣. الوسائطُ تُبنى سطرَ أوامرَ واحدًا بقواعدِ اقتباسِ `CommandLineToArgvW`.
//
//      **دَينٌ معلَن**: `bInheritHandles=TRUE` تُورِّثُ كلَّ مقبضٍ موروثٍ لا الثلاثةَ
//      المذكورةَ وحدَها. وكلُّ وصفٍ يُنشئه الرنتايم `_O_NOINHERIT` فلا يتسرّبُ
//      شيءٌ منّا، لكنّ شوكتين متزامنتين من خيطين يمكن أن ترثَ إحداهما نسخةَ
//      الأخرى. المفسّرُ يُغلقُ النافذةَ بقفلٍ؛ وهنا لا قفلَ — الحلُّ التامُّ
//      `PROC_THREAD_ATTRIBUTE_HANDLE_LIST`، وهو دَينٌ مذكورٌ لا مسكوتٌ عنه.
//
// (EN) The Windows counterpart of the processes primitives generator: Win32 +
//      CRT, not a POSIX emulation. Only wait and spawn live here — open/close/
//      pipe differ merely in names and flags and stay parameterised in the main
//      file, and the shared helpers (store/find/kind_guard/num2str/fail) are
//      deliberately shared because that is where all handle semantics live.
// ============================================================================

#include "llvm_codegen.h"
#include "builders/builtins/processes_builtins_codegen.h"

#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/Alignment.h>

#include <string>
#include <vector>

namespace Sad
{
    namespace LLVM
    {
        namespace
        {
            // ════════════════════════════════════════════════════════════════
            // (AR) ثوابتُ Win32 — مُسمّاةٌ لا سحريّة، وكلُّها من winbase.h.
            // ════════════════════════════════════════════════════════════════
            constexpr int64_t kStartfUseStdHandles = 0x00000100;
            constexpr int64_t kStdInputHandle = -10;
            constexpr int64_t kStdOutputHandle = -11;
            constexpr int64_t kStdErrorHandle = -12;
            constexpr int64_t kDuplicateSameAccess = 0x00000002;
            constexpr int64_t kInfinite = 0xFFFFFFFF;
            constexpr int64_t kWaitObject0 = 0;
            constexpr int64_t kFormatFromSystem = 0x00001000;
            constexpr int64_t kFormatIgnoreInserts = 0x00000200;
            constexpr int64_t kWinErrBytes = 512;

            /// (AR) تخطيطُ `STARTUPINFOA` و`PROCESS_INFORMATION` بالبايتات.
            ///
            ///      لماذا إزاحاتٌ صريحةٌ لا `StructType`: حشوُ المُصفِّفِ بين حقلٍ
            ///      ٣٢-بتّيٍّ ومؤشّرٍ ٦٤-بتّيّ يجبُ أن يطابقَ ما يتوقّعه kernel32
            ///      بالضبط، وبناؤه ببنيةٍ في LLVM يتركُ الحشوَ ضمنيًّا فيُقرأُ
            ///      المقبضُ من موضعٍ خاطئ — عطبٌ لا يظهرُ خطأً بل توجيهًا صامتًا
            ///      إلى مقبضٍ عشوائيّ. الإزاحاتُ هنا مكتوبةٌ ومُراجَعةٌ عددًا عددًا.
            /// (EN) Explicit byte offsets rather than a StructType: the padding
            ///      must match kernel32's expectation exactly, and an implicit
            ///      layout would read the handle from the wrong offset — silent
            ///      misredirection, not an error.
            struct WinLayout
            {
                int64_t siSize, siCb, siFlags, siStdIn, siStdOut, siStdErr;
                int64_t piSize, piProcess, piThread;
            };

            constexpr WinLayout kLayout64{104, 0, 60, 80, 88, 96, 24, 0, 8};
            constexpr WinLayout kLayout32{68, 0, 44, 56, 60, 64, 16, 0, 4};
        } // namespace

        // ════════════════════════════════════════════════════════════════════
        // (AR) أدواتٌ محلّيّة
        // ════════════════════════════════════════════════════════════════════
        namespace
        {
            /// (AR) هل الهدفُ ويندوزُ ٣٢-بتّيّ؟ يقرّرُ التخطيطَ **واصطلاحَ النداء**:
            ///      دوالُّ kernel32 هناك `__stdcall`، وندعوها بـC كان سيُفسدُ
            ///      المكدّسَ عند أوّلِ نداء. على x64 اصطلاحٌ واحدٌ فلا مسألة.
            bool isWin32Bit(const std::string &triple)
            {
                return triple.rfind("i386", 0) == 0 || triple.rfind("i486", 0) == 0 ||
                       triple.rfind("i586", 0) == 0 || triple.rfind("i686", 0) == 0;
            }
        } // namespace

        // ════════════════════════════════════════════════════════════════════
        // (AR) __sad_proc_winerr(code) → نصُّ خطأِ النظام
        //
        //      لا يُستعملُ `strerror` هنا: خطأُ `CreateProcess` خطأُ نظامٍ لا
        //      `errno`، و`strerror(errno)` بعده يصفُ آخرَ فشلٍ في CRT — أي جملةً
        //      صحيحةَ الصياغةِ عن حدثٍ آخرَ تمامًا. وهو نفسُ ما يفعله `winErrorText`
        //      في المفسّر، بنفسِ التشذيبِ كي يتطابقَ النصّان.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateWinErrText()
        {
            const char *name = "__sad_proc_winerr";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i8Ty = llvm::Type::getInt8Ty(ctx);
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);
            const bool win32 = isWin32Bit(cg_.module_->getTargetTriple());

            auto *fnTy = llvm::FunctionType::get(ptrTy, {i32Ty}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *trim = llvm::BasicBlock::Create(ctx, "trim", fn);
            auto *trimTest = llvm::BasicBlock::Create(ctx, "trim.test", fn);
            auto *trimStep = llvm::BasicBlock::Create(ctx, "trim.step", fn);
            auto *done = llvm::BasicBlock::Create(ctx, "done", fn);
            auto *fallback = llvm::BasicBlock::Create(ctx, "fallback", fn);
            auto *ret = llvm::BasicBlock::Create(ctx, "ret", fn);

            llvm::IRBuilder<> b(entry);
            auto *mallocTy = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocTy);
            llvm::Value *buf =
                b.CreateCall(mallocFn, {llvm::ConstantInt::get(i64Ty, kWinErrBytes)}, "buf");
            b.CreateStore(llvm::ConstantInt::get(i8Ty, 0), buf);

            llvm::Value *iSlot = b.CreateAlloca(i64Ty, nullptr, "i");

            auto *fmtTy = llvm::FunctionType::get(
                i32Ty, {i32Ty, ptrTy, i32Ty, i32Ty, ptrTy, i32Ty, ptrTy}, false);
            auto fmtCallee = cg_.module_->getOrInsertFunction("FormatMessageA", fmtTy);
            if (win32)
                if (auto *f = llvm::dyn_cast<llvm::Function>(fmtCallee.getCallee()))
                    f->setCallingConv(llvm::CallingConv::X86_StdCall);
            auto *nullPtr = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));
            auto *fmtCall = b.CreateCall(
                fmtCallee,
                {llvm::ConstantInt::get(i32Ty, kFormatFromSystem | kFormatIgnoreInserts), nullPtr,
                 fn->getArg(0), llvm::ConstantInt::get(i32Ty, 0), buf,
                 llvm::ConstantInt::get(i32Ty, kWinErrBytes), nullPtr},
                "n");
            if (win32)
                fmtCall->setCallingConv(llvm::CallingConv::X86_StdCall);
            b.CreateStore(b.CreateZExt(fmtCall, i64Ty, "n64"), iSlot);
            b.CreateBr(trim);

            // (AR) تشذيبُ «\r\n» والنقطةِ والفراغِ من الآخر — نفسُ تشذيبِ المفسّر،
            //      وإلّا انتهت رسالةُ الخطأ بسطرٍ فارغٍ في محرّكٍ دون آخر.
            b.SetInsertPoint(trim);
            llvm::Value *i = b.CreateLoad(i64Ty, iSlot, "i.v");
            b.CreateCondBr(b.CreateICmpEQ(i, llvm::ConstantInt::get(i64Ty, 0), "i.zero"), done,
                           trimTest);

            b.SetInsertPoint(trimTest);
            llvm::Value *iCur = b.CreateLoad(i64Ty, iSlot, "i.c");
            llvm::Value *last = b.CreateLoad(
                i8Ty,
                b.CreateGEP(i8Ty, buf, {b.CreateSub(iCur, llvm::ConstantInt::get(i64Ty, 1))},
                            "last.gep"),
                "last");
            llvm::Value *isTrim = b.CreateOr(
                b.CreateOr(b.CreateICmpEQ(last, llvm::ConstantInt::get(i8Ty, '\r')),
                           b.CreateICmpEQ(last, llvm::ConstantInt::get(i8Ty, '\n'))),
                b.CreateOr(b.CreateICmpEQ(last, llvm::ConstantInt::get(i8Ty, '.')),
                           b.CreateICmpEQ(last, llvm::ConstantInt::get(i8Ty, ' '))),
                "is.trim");
            b.CreateCondBr(isTrim, trimStep, done);

            b.SetInsertPoint(trimStep);
            b.CreateStore(b.CreateSub(b.CreateLoad(i64Ty, iSlot, "i.d"),
                                      llvm::ConstantInt::get(i64Ty, 1)),
                          iSlot);
            b.CreateBr(trim);

            b.SetInsertPoint(done);
            llvm::Value *iEnd = b.CreateLoad(i64Ty, iSlot, "i.e");
            b.CreateStore(llvm::ConstantInt::get(i8Ty, 0),
                          b.CreateGEP(i8Ty, buf, {iEnd}, "end.gep"));
            b.CreateCondBr(b.CreateICmpEQ(iEnd, llvm::ConstantInt::get(i64Ty, 0), "empty"),
                           fallback, ret);

            // (AR) رسالةٌ فارغةٌ من النظام (رمزٌ لا نصَّ له): يُطبَعُ الرمزُ نفسُه
            //      كي يقودَ البحث — لا صمتَ ولا نصٌّ مخترَع.
            b.SetInsertPoint(fallback);
            b.CreateRet(b.CreateCall(getOrCreateNumToStr(),
                                     {b.CreateZExt(fn->getArg(0), i64Ty, "code64")}, "code.str"));

            b.SetInsertPoint(ret);
            b.CreateRet(buf);
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) __sad_proc_widen(utf8) → UTF-16 محجوزةٌ في الكومة
        //
        //      **لماذا `W` لا `A`**: دوالُّ `*A` تؤوّلُ البايتاتِ بصفحةِ الترميزِ
        //      المحلّيّة (ACP)، وسلاسلُ ص كلُّها UTF-8. وقد قِيس الأثر: على جهازٍ
        //      ACP فيه ١٢٥٥ أنشأ `CreateFileA("توجيه_ويندوز.txt")` ملفًّا باسمٍ
        //      مشوَّه، فكتب فيه الابنُ بنجاحٍ ثمّ عجزت القراءةُ عن فتحه — عطبٌ
        //      يشيرُ إلى غيرِ موضعه. ولغةٌ مساراتُها عربيّةٌ لا تحتملُ واجهةً
        //      مربوطةً بصفحةِ ترميزٍ محلّيّة.
        //
        //      ولا تُحرَّرُ النتيجة: عمرُها عمرُ النداءِ الواحد، والتحريرُ يستلزمُ
        //      تتبّعًا على كلِّ مخرجٍ بما فيه مخارجُ `emitFail` غيرُ العائدة.
        //      تسريبٌ محدودٌ **معلَن** بحجم المسار لكلِّ فتحٍ أو تشغيل.
        // (EN) Win32 *A functions are bound to the local ANSI code page while every
        //      Sad string is UTF-8 — measured to create mojibake-named files. The
        //      result is deliberately not freed: its lifetime is one call, and
        //      freeing it would require tracking every exit including emitFail's
        //      non-returning ones. A declared, bounded leak.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateWinWiden()
        {
            const char *name = "__sad_proc_widen";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);
            const bool win32 = isWin32Bit(cg_.module_->getTargetTriple());

            auto stdcall = [&](llvm::FunctionCallee callee, llvm::CallInst *call) {
                if (!win32)
                    return;
                if (auto *f = llvm::dyn_cast<llvm::Function>(callee.getCallee()))
                    f->setCallingConv(llvm::CallingConv::X86_StdCall);
                call->setCallingConv(llvm::CallingConv::X86_StdCall);
            };

            constexpr int64_t kCpUtf8 = 65001;

            auto *fnTy = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            llvm::IRBuilder<> b(llvm::BasicBlock::Create(ctx, "entry", fn));

            auto *nullPtr = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));
            auto *mbTy = llvm::FunctionType::get(
                i32Ty, {i32Ty, i32Ty, ptrTy, i32Ty, ptrTy, i32Ty}, false);
            auto mbFn = cg_.module_->getOrInsertFunction("MultiByteToWideChar", mbTy);

            // (AR) ‎-1 للطول ⇒ يشملُ العدُّ المُرجَعُ محرفَ الإنهاء، فلا زيادةَ يدويّة.
            auto *need = b.CreateCall(mbFn,
                                      {llvm::ConstantInt::get(i32Ty, kCpUtf8),
                                       llvm::ConstantInt::get(i32Ty, 0), fn->getArg(0),
                                       llvm::ConstantInt::get(i32Ty, -1), nullPtr,
                                       llvm::ConstantInt::get(i32Ty, 0)},
                                      "need");
            stdcall(mbFn, need);
            llvm::Value *need64 = b.CreateSExt(need, i64Ty, "need64");
            // (AR) ‎+1 حارسٌ ضدّ إرجاعِ صفرٍ عند الفشل: `malloc(0)` ثمّ كتابةٌ فيه.
            llvm::Value *bytes =
                b.CreateMul(b.CreateAdd(need64, llvm::ConstantInt::get(i64Ty, 1)),
                            llvm::ConstantInt::get(i64Ty, 2), "wbytes");
            auto *mallocTy = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocTy);
            llvm::Value *buf = b.CreateCall(mallocFn, {bytes}, "wbuf");
            b.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt16Ty(ctx), 0), buf);

            auto *conv = b.CreateCall(mbFn,
                                      {llvm::ConstantInt::get(i32Ty, kCpUtf8),
                                       llvm::ConstantInt::get(i32Ty, 0), fn->getArg(0),
                                       llvm::ConstantInt::get(i32Ty, -1), buf, need},
                                      "conv");
            stdcall(mbFn, conv);
            b.CreateRet(buf);
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) __sad_proc_open_fd على ويندوز — CreateFileW لا _open
        //
        //      كانت `_open` أقصرَ وأقربَ إلى نظيرها POSIX، ثمّ قاس اختبارُ ١٨١
        //      أنّها **خاطئة**: «إلحاق» في CRT ليست خاصّيّةَ المقبضِ بل مسلكًا
        //      تسلكه CRT عند كلّ كتابة (تقفزُ إلى النهايةِ ثمّ تكتب). ووصفاتُنا
        //      وُضعت لتُمرَّرَ إلى ابنٍ يكتبُ بالمقبضِ الخامِّ لا بـCRT، فيبدأ من
        //      الموضعِ صفر و**يدهسُ** ما في الملفّ: «إلحاق» تعملُ عملَ «كتابة»
        //      بلا خطأٍ ولا تحذير — وهو بعينه الفرقُ بين `>>` و`>`.
        //      و`FILE_APPEND_DATA` **بلا** `FILE_WRITE_DATA` يضعُ الدلالةَ في
        //      المقبضِ نفسِه: كلُّ كتابةٍ في النهايةِ ذرّيًّا أيًّا كان الكاتب.
        // (EN) _open was shorter and closer to the POSIX shape — and measurably
        //      wrong: CRT append is a CRT behaviour, not a handle property, so a
        //      child writing through the raw handle overwrites from offset zero.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateWinOpenFd()
        {
            const char *name = "__sad_proc_open_fd";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);
            const bool win32 = isWin32Bit(cg_.module_->getTargetTriple());
            auto *intptrTy = win32 ? i32Ty : i64Ty;

            auto stdcall = [&](llvm::FunctionCallee callee, llvm::CallInst *call) {
                if (!win32)
                    return;
                if (auto *f = llvm::dyn_cast<llvm::Function>(callee.getCallee()))
                    f->setCallingConv(llvm::CallingConv::X86_StdCall);
                call->setCallingConv(llvm::CallingConv::X86_StdCall);
            };

            constexpr int64_t kGenericRead = 0x80000000LL;
            constexpr int64_t kGenericWrite = 0x40000000LL;
            constexpr int64_t kFileAppendData = 0x0004;
            constexpr int64_t kSynchronize = 0x00100000;
            // (AR) المشاركةُ تشمل الحذف: POSIX يسمح بفكِّ ارتباطِ ملفٍّ مفتوح.
            constexpr int64_t kShareAll = 0x00000007;
            constexpr int64_t kCreateAlways = 2;
            constexpr int64_t kOpenExisting = 3;
            constexpr int64_t kOpenAlways = 4;
            constexpr int64_t kAttrNormal = 0x00000080;
            constexpr int64_t kOpenAppendFlag = 0x0008; // _O_APPEND

            auto *fnTy = llvm::FunctionType::get(i64Ty, {ptrTy, ptrTy}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *badMode = llvm::BasicBlock::Create(ctx, "bad.mode", fn);
            auto *doOpen = llvm::BasicBlock::Create(ctx, "open", fn);
            auto *openFail = llvm::BasicBlock::Create(ctx, "open.fail", fn);
            auto *mkFd = llvm::BasicBlock::Create(ctx, "mkfd", fn);
            auto *fdFail = llvm::BasicBlock::Create(ctx, "fd.fail", fn);
            auto *ok = llvm::BasicBlock::Create(ctx, "ok", fn);

            llvm::IRBuilder<> b(entry);
            auto *strcmpTy = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto strcmpFn = cg_.module_->getOrInsertFunction("strcmp", strcmpTy);
            auto isMode = [&](const char *text, const char *nm) {
                return b.CreateICmpEQ(
                    b.CreateCall(strcmpFn,
                                 {fn->getArg(1), b.CreateGlobalStringPtr(text, "proc.mode")}, nm),
                    llvm::ConstantInt::get(i32Ty, 0), std::string(nm) + ".eq");
            };
            llvm::Value *isRead = isMode("قراءة", "mode.read");
            llvm::Value *isWrite = isMode("كتابة", "mode.write");
            llvm::Value *isAppend = isMode("إلحاق", "mode.append");

            auto pick = [&](int64_t r, int64_t w, int64_t a, const char *nm) -> llvm::Value * {
                return b.CreateSelect(
                    isRead, llvm::ConstantInt::get(i32Ty, r),
                    b.CreateSelect(isWrite, llvm::ConstantInt::get(i32Ty, w),
                                   llvm::ConstantInt::get(i32Ty, a), std::string(nm) + ".wa"),
                    nm);
            };
            llvm::Value *access =
                pick(kGenericRead, kGenericWrite, kFileAppendData | kSynchronize, "access");
            llvm::Value *disposition =
                pick(kOpenExisting, kCreateAlways, kOpenAlways, "disposition");
            llvm::Value *osFlags = pick(0, 0, kOpenAppendFlag, "osflags");

            b.CreateCondBr(b.CreateOr(b.CreateOr(isRead, isWrite), isAppend, "mode.known"),
                           doOpen, badMode);

            b.SetInsertPoint(badMode);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_BAD_MODE, {{"mode", fn->getArg(1)}});

            b.SetInsertPoint(doOpen);
            auto *nullPtr = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));
            auto *cfTy = llvm::FunctionType::get(
                ptrTy, {ptrTy, i32Ty, i32Ty, ptrTy, i32Ty, i32Ty, ptrTy}, false);
            auto cfFn = cg_.module_->getOrInsertFunction("CreateFileW", cfTy);
            llvm::Value *wpath =
                b.CreateCall(getOrCreateWinWiden(), {fn->getArg(0)}, "wpath");
            // (AR) `lpSecurityAttributes = null` ⇒ مقبضٌ غيرُ موروث — نظيرُ CLOEXEC.
            auto *h = b.CreateCall(cfFn,
                                   {wpath, access,
                                    llvm::ConstantInt::get(i32Ty, kShareAll), nullPtr, disposition,
                                    llvm::ConstantInt::get(i32Ty, kAttrNormal), nullPtr},
                                   "h");
            stdcall(cfFn, h);
            llvm::Value *hInt = b.CreatePtrToInt(h, intptrTy, "h.int");
            b.CreateCondBr(b.CreateICmpEQ(hInt, llvm::ConstantInt::get(intptrTy, -1), "h.bad"),
                           openFail, mkFd);

            b.SetInsertPoint(openFail);
            {
                auto *leTy = llvm::FunctionType::get(i32Ty, {}, false);
                auto leFn = cg_.module_->getOrInsertFunction("GetLastError", leTy);
                auto *le = b.CreateCall(leFn, {}, "le");
                stdcall(leFn, le);
                emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_OPEN_FAILED,
                         {{"path", fn->getArg(0)},
                          {"reason", b.CreateCall(getOrCreateWinErrText(), {le}, "reason")}});
            }

            // (AR) لا `_O_BINARY` هنا: `_open_osfhandle` ثنائيٌّ ما لم يُطلَب
            //      `_O_TEXT` صراحةً، وتمريرُ علمٍ لا تقبله يُفسدُ التأويل.
            b.SetInsertPoint(mkFd);
            auto *oofTy = llvm::FunctionType::get(i32Ty, {intptrTy, i32Ty}, false);
            auto oofFn = cg_.module_->getOrInsertFunction("_open_osfhandle", oofTy);
            llvm::Value *fd = b.CreateCall(oofFn, {hInt, osFlags}, "fd");
            b.CreateCondBr(b.CreateICmpSLT(fd, llvm::ConstantInt::get(i32Ty, 0), "fd.bad"), fdFail,
                           ok);

            // (AR) فشلُ الالتفافِ يتركُ مقبضًا حيًّا لا مالكَ له — يُغلَقُ قبل الرمي.
            b.SetInsertPoint(fdFail);
            {
                auto *chTy = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
                auto chFn = cg_.module_->getOrInsertFunction("CloseHandle", chTy);
                stdcall(chFn, b.CreateCall(chFn, {h}));
                auto *strerrorTy = llvm::FunctionType::get(ptrTy, {i32Ty}, false);
                auto strerrorFn = cg_.module_->getOrInsertFunction("strerror", strerrorTy);
                emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_OPEN_FAILED,
                         {{"path", fn->getArg(0)},
                          {"reason", b.CreateCall(strerrorFn, {errnoValue(b, "errno.v")},
                                                  "reason")}});
            }

            b.SetInsertPoint(ok);
            b.CreateRet(b.CreateCall(getOrCreateStore(),
                                     {llvm::ConstantInt::get(i64Ty, kKindDescriptor),
                                      b.CreateSExt(fd, i64Ty, "fd64")},
                                     "handle"));
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) __sad_proc_winquote(dst, arg) → dst بعد الكتابة
        //
        //      قواعدُ `CommandLineToArgvW`: الشرطةُ المائلةُ الخلفيّةُ خاصّةٌ **قبل
        //      علامةِ التنصيصِ وحدَها**. فوسيطٌ ينتهي بـ`\` يجعلُ التنصيصَ الختاميَّ
        //      مهروبًا فيبتلعُ الوسيطُ ما بعده — و`C:\مجلد\` مسارٌ عاديٌّ تمامًا،
        //      أي أنّ الحالةَ شائعةٌ لا نادرة. والنتيجةُ ليست خطأً بل **وسائطَ
        //      مختلفةً في الابن**: نجاحٌ كاذبٌ من جنسِ ما تحاربُه هذه الوحدة.
        //
        //      ولا يُقتبَسُ ما لا يحتاج. جُرِّب اقتباسُ الكلِّ تبسيطًا — وأسقطَ
        //      الاختباراتِ فورًا: `cmd` تقرأُ ما بعد `/c` **نصًّا خامًّا** لا ناقلةَ
        //      وسائطَ مقسَّمة، فـ`"cmd" "/c" "exit 0"` لا تُقرأ فيها `"/c"` مفتاحًا
        //      فينقلبُ الأمرُ كلُّه إلى اسمِ برنامجٍ لا يوجد، ويعودُ الرمزُ ١ لكلِّ
        //      شيء. أي أنّ الاقتباسَ الزائدَ ليس زينةً بل **يغيّرُ ما يُشغَّل**.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateWinQuoteArg()
        {
            const char *name = "__sad_proc_winquote";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i8Ty = llvm::Type::getInt8Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            auto *fnTy = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *scan = llvm::BasicBlock::Create(ctx, "scan", fn);
            auto *scanTest = llvm::BasicBlock::Create(ctx, "scan.test", fn);
            auto *plainLoop = llvm::BasicBlock::Create(ctx, "plain.loop", fn);
            auto *plainStep = llvm::BasicBlock::Create(ctx, "plain.step", fn);
            auto *quoted = llvm::BasicBlock::Create(ctx, "quoted", fn);
            auto *loop = llvm::BasicBlock::Create(ctx, "loop", fn);
            auto *bsLoop = llvm::BasicBlock::Create(ctx, "bs.loop", fn);
            auto *bsStep = llvm::BasicBlock::Create(ctx, "bs.step", fn);
            auto *afterBs = llvm::BasicBlock::Create(ctx, "after.bs", fn);
            auto *atEnd = llvm::BasicBlock::Create(ctx, "at.end", fn);
            auto *notEnd = llvm::BasicBlock::Create(ctx, "not.end", fn);
            auto *isQuote = llvm::BasicBlock::Create(ctx, "is.quote", fn);
            auto *isChar = llvm::BasicBlock::Create(ctx, "is.char", fn);
            auto *emitLoop = llvm::BasicBlock::Create(ctx, "emit.loop", fn);
            auto *emitStep = llvm::BasicBlock::Create(ctx, "emit.step", fn);
            auto *emitDone = llvm::BasicBlock::Create(ctx, "emit.done", fn);
            auto *tailQuote = llvm::BasicBlock::Create(ctx, "tail.quote", fn);
            auto *tailChar = llvm::BasicBlock::Create(ctx, "tail.char", fn);
            auto *next = llvm::BasicBlock::Create(ctx, "next", fn);
            auto *finish = llvm::BasicBlock::Create(ctx, "finish", fn);

            llvm::IRBuilder<> b(entry);
            // (AR) كلُّ الحجوزِ في كتلةِ المدخلِ لا داخلَ الحلقة: حجزٌ في حلقةٍ
            //      ينمو المكدَّسَ بكلِّ دورةٍ حتّى يفيض.
            llvm::Value *pSlot = b.CreateAlloca(ptrTy, nullptr, "p");
            llvm::Value *iSlot = b.CreateAlloca(i64Ty, nullptr, "i");
            llvm::Value *slSlot = b.CreateAlloca(i64Ty, nullptr, "sl");
            llvm::Value *nSlot = b.CreateAlloca(i64Ty, nullptr, "n");
            llvm::Value *modeSlot = b.CreateAlloca(i64Ty, nullptr, "mode"); // 0=نهاية 1=تنصيص 2=محرف

            // ── (AR) هل يحتاجُ اقتباسًا أصلًا؟ ──────────────────────────────
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iSlot);
            b.CreateStore(fn->getArg(0), pSlot);
            b.CreateBr(scan);

            b.SetInsertPoint(scan);
            llvm::Value *sc = b.CreateLoad(
                i8Ty,
                b.CreateGEP(i8Ty, fn->getArg(1), {b.CreateLoad(i64Ty, iSlot, "i.sc")}, "sc.gep"),
                "sc");
            // (AR) نهايةُ النصّ: يحتاجُ اقتباسًا إن كان **فارغًا** — وسيطٌ فارغٌ بلا
            //      تنصيصين يختفي من ناقلةِ الابنِ اختفاءً تامًّا فتنزاحُ البقيّة.
            auto *scanEnd = llvm::BasicBlock::Create(ctx, "scan.end", fn);
            b.CreateCondBr(b.CreateICmpEQ(sc, llvm::ConstantInt::get(i8Ty, 0), "sc.nul"), scanEnd,
                           scanTest);

            b.SetInsertPoint(scanEnd);
            b.CreateCondBr(b.CreateICmpEQ(b.CreateLoad(i64Ty, iSlot, "i.e0"),
                                          llvm::ConstantInt::get(i64Ty, 0), "arg.empty"),
                           quoted, plainLoop);

            b.SetInsertPoint(scanTest);
            llvm::Value *needs = b.CreateOr(
                b.CreateOr(b.CreateICmpEQ(sc, llvm::ConstantInt::get(i8Ty, ' ')),
                           b.CreateICmpEQ(sc, llvm::ConstantInt::get(i8Ty, '\t'))),
                b.CreateOr(b.CreateOr(b.CreateICmpEQ(sc, llvm::ConstantInt::get(i8Ty, '\n')),
                                      b.CreateICmpEQ(sc, llvm::ConstantInt::get(i8Ty, '\v'))),
                           b.CreateICmpEQ(sc, llvm::ConstantInt::get(i8Ty, '"'))),
                "needs.q");
            b.CreateStore(
                b.CreateAdd(b.CreateLoad(i64Ty, iSlot, "i.s1"), llvm::ConstantInt::get(i64Ty, 1)),
                iSlot);
            b.CreateCondBr(needs, quoted, scan);

            // ── (AR) لا يحتاج: نسخٌ حرفيّ ───────────────────────────────────
            b.SetInsertPoint(plainLoop);
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iSlot);
            b.CreateBr(plainStep);

            b.SetInsertPoint(plainStep);
            llvm::Value *pIdx = b.CreateLoad(i64Ty, iSlot, "i.p");
            llvm::Value *pc = b.CreateLoad(
                i8Ty, b.CreateGEP(i8Ty, fn->getArg(1), {pIdx}, "pc.gep"), "pc");
            llvm::Value *pDst = b.CreateLoad(ptrTy, pSlot, "p.p");
            auto *plainDone = llvm::BasicBlock::Create(ctx, "plain.done", fn);
            auto *plainWrite = llvm::BasicBlock::Create(ctx, "plain.write", fn);
            b.CreateCondBr(b.CreateICmpEQ(pc, llvm::ConstantInt::get(i8Ty, 0), "pc.nul"),
                           plainDone, plainWrite);

            b.SetInsertPoint(plainWrite);
            b.CreateStore(pc, pDst);
            b.CreateStore(b.CreateConstInBoundsGEP1_64(i8Ty, pDst, 1), pSlot);
            b.CreateStore(b.CreateAdd(pIdx, llvm::ConstantInt::get(i64Ty, 1)), iSlot);
            b.CreateBr(plainStep);

            b.SetInsertPoint(plainDone);
            b.CreateRet(b.CreateLoad(ptrTy, pSlot, "p.plain"));

            // ── (AR) يحتاج: القاعدةُ الكاملة ────────────────────────────────
            b.SetInsertPoint(quoted);
            b.CreateStore(llvm::ConstantInt::get(i8Ty, '"'), fn->getArg(0));
            b.CreateStore(b.CreateConstInBoundsGEP1_64(i8Ty, fn->getArg(0), 1), pSlot);
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iSlot);
            b.CreateBr(loop);

            // ── عدُّ الشرطاتِ المائلةِ المتتالية ──
            b.SetInsertPoint(loop);
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), slSlot);
            b.CreateBr(bsLoop);

            b.SetInsertPoint(bsLoop);
            llvm::Value *ci = b.CreateLoad(
                i8Ty,
                b.CreateGEP(i8Ty, fn->getArg(1), {b.CreateLoad(i64Ty, iSlot, "i.v")}, "c.gep"),
                "c");
            b.CreateCondBr(b.CreateICmpEQ(ci, llvm::ConstantInt::get(i8Ty, '\\'), "is.bs"), bsStep,
                           afterBs);

            b.SetInsertPoint(bsStep);
            b.CreateStore(
                b.CreateAdd(b.CreateLoad(i64Ty, iSlot, "i.b"), llvm::ConstantInt::get(i64Ty, 1)),
                iSlot);
            b.CreateStore(
                b.CreateAdd(b.CreateLoad(i64Ty, slSlot, "sl.b"), llvm::ConstantInt::get(i64Ty, 1)),
                slSlot);
            b.CreateBr(bsLoop);

            b.SetInsertPoint(afterBs);
            llvm::Value *cc = b.CreateLoad(
                i8Ty,
                b.CreateGEP(i8Ty, fn->getArg(1), {b.CreateLoad(i64Ty, iSlot, "i.a")}, "cc.gep"),
                "cc");
            b.CreateCondBr(b.CreateICmpEQ(cc, llvm::ConstantInt::get(i8Ty, 0), "is.nul"), atEnd,
                           notEnd);

            // (AR) نهاية: كلُّ شرطةٍ تُضاعَفُ لأنّ التنصيصَ الختاميَّ يليها.
            b.SetInsertPoint(atEnd);
            b.CreateStore(b.CreateMul(b.CreateLoad(i64Ty, slSlot, "sl.e"),
                                      llvm::ConstantInt::get(i64Ty, 2)),
                          nSlot);
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), modeSlot);
            b.CreateBr(emitLoop);

            b.SetInsertPoint(notEnd);
            b.CreateCondBr(b.CreateICmpEQ(cc, llvm::ConstantInt::get(i8Ty, '"'), "is.q"), isQuote,
                           isChar);

            // (AR) تنصيصٌ داخليّ: تُضاعَفُ الشرطاتُ وتُضافُ واحدةٌ تهرُبُ التنصيص.
            b.SetInsertPoint(isQuote);
            b.CreateStore(b.CreateAdd(b.CreateMul(b.CreateLoad(i64Ty, slSlot, "sl.q"),
                                                  llvm::ConstantInt::get(i64Ty, 2)),
                                      llvm::ConstantInt::get(i64Ty, 1)),
                          nSlot);
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 1), modeSlot);
            b.CreateBr(emitLoop);

            // (AR) محرفٌ عاديّ: الشرطاتُ تبقى كما هي — ليست قبلَ تنصيص.
            b.SetInsertPoint(isChar);
            b.CreateStore(b.CreateLoad(i64Ty, slSlot, "sl.c"), nSlot);
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 2), modeSlot);
            b.CreateBr(emitLoop);

            // ── حلقةُ كتابةِ n شرطةً ──
            b.SetInsertPoint(emitLoop);
            b.CreateCondBr(b.CreateICmpSGT(b.CreateLoad(i64Ty, nSlot, "n.v"),
                                           llvm::ConstantInt::get(i64Ty, 0), "n.pos"),
                           emitStep, emitDone);

            b.SetInsertPoint(emitStep);
            llvm::Value *p1 = b.CreateLoad(ptrTy, pSlot, "p.1");
            b.CreateStore(llvm::ConstantInt::get(i8Ty, '\\'), p1);
            b.CreateStore(b.CreateConstInBoundsGEP1_64(i8Ty, p1, 1), pSlot);
            b.CreateStore(
                b.CreateSub(b.CreateLoad(i64Ty, nSlot, "n.d"), llvm::ConstantInt::get(i64Ty, 1)),
                nSlot);
            b.CreateBr(emitLoop);

            b.SetInsertPoint(emitDone);
            llvm::Value *mode = b.CreateLoad(i64Ty, modeSlot, "mode.v");
            auto *sw = b.CreateSwitch(mode, finish, 2);
            sw->addCase(llvm::ConstantInt::get(llvm::cast<llvm::IntegerType>(i64Ty), 1), tailQuote);
            sw->addCase(llvm::ConstantInt::get(llvm::cast<llvm::IntegerType>(i64Ty), 2), tailChar);

            b.SetInsertPoint(tailQuote);
            llvm::Value *p2 = b.CreateLoad(ptrTy, pSlot, "p.2");
            b.CreateStore(llvm::ConstantInt::get(i8Ty, '"'), p2);
            b.CreateStore(b.CreateConstInBoundsGEP1_64(i8Ty, p2, 1), pSlot);
            b.CreateBr(next);

            b.SetInsertPoint(tailChar);
            llvm::Value *p3 = b.CreateLoad(ptrTy, pSlot, "p.3");
            b.CreateStore(cc, p3);
            b.CreateStore(b.CreateConstInBoundsGEP1_64(i8Ty, p3, 1), pSlot);
            b.CreateBr(next);

            b.SetInsertPoint(next);
            b.CreateStore(
                b.CreateAdd(b.CreateLoad(i64Ty, iSlot, "i.n"), llvm::ConstantInt::get(i64Ty, 1)),
                iSlot);
            b.CreateBr(loop);

            b.SetInsertPoint(finish);
            llvm::Value *p4 = b.CreateLoad(ptrTy, pSlot, "p.4");
            b.CreateStore(llvm::ConstantInt::get(i8Ty, '"'), p4);
            b.CreateRet(b.CreateConstInBoundsGEP1_64(i8Ty, p4, 1));
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) __sad_proc_winctrl — معالجُ Ctrl-C أثناء الانتظار
        //
        //      يُرجعُ ١ («ابتُلع الحدث») لـCTRL_C_EVENT وCTRL_BREAK_EVENT وحدَهما.
        //      وهو نظيرُ تحييدِ SIGINT/SIGQUIT في المسار POSIX: الصدَفةُ تبقى
        //      والمقاطَعُ هو الأمرُ الجاري. وويندوز يُرسلُ الحدثَ إلى **مجموعةِ
        //      وحدةِ التحكّمِ كلِّها**، فالابنُ يراه من تلقائه ولا يُفوَّتُ عليه.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateWinCtrlHandler()
        {
            const char *name = "__sad_proc_winctrl";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *fnTy = llvm::FunctionType::get(i32Ty, {i32Ty}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            if (isWin32Bit(cg_.module_->getTargetTriple()))
                fn->setCallingConv(llvm::CallingConv::X86_StdCall);

            llvm::IRBuilder<> b(llvm::BasicBlock::Create(ctx, "entry", fn));
            llvm::Value *isCtrl = b.CreateICmpULT(
                fn->getArg(0), llvm::ConstantInt::get(i32Ty, 2), "is.ctrl"); // 0=C، 1=BREAK
            b.CreateRet(b.CreateSelect(isCtrl, llvm::ConstantInt::get(i32Ty, 1),
                                       llvm::ConstantInt::get(i32Ty, 0), "ret"));
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) __sad_proc_wait على ويندوز
        //
        //      البحثُ بلا سحبٍ (‏take=0) ثمّ السحبُ عند النجاحِ وحدَه — نفسُ قرارِ
        //      المسارِ POSIX وللسببِ نفسِه: العدّادُ تصاعديٌّ، فمقبضٌ سُحب ثمّ
        //      أُعيدَ تخزينُه يُولَدُ بمعرّفٍ جديدٍ ويموتُ الذي في يدِ المستعمل —
        //      فشلٌ عابرٌ يصيرُ خسارةً دائمةً للابن.
        //
        //      ولا نظيرَ لـECHILD هنا: مقبضُ العمليّةِ على ويندوز يبقى صالحًا بعد
        //      موتها، فلا تختفي «الابنيّةُ» من تحتِ اليدِ كما تختفي على POSIX.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateWinWait()
        {
            const char *name = "__sad_proc_wait";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);
            const bool win32 = isWin32Bit(cg_.module_->getTargetTriple());

            auto stdcall = [&](llvm::FunctionCallee callee, llvm::CallInst *call) {
                if (!win32)
                    return;
                if (auto *f = llvm::dyn_cast<llvm::Function>(callee.getCallee()))
                    f->setCallingConv(llvm::CallingConv::X86_StdCall);
                call->setCallingConv(llvm::CallingConv::X86_StdCall);
            };

            auto *fnTy = llvm::FunctionType::get(i64Ty, {i64Ty}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());
            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *unknown = llvm::BasicBlock::Create(ctx, "unknown", fn);
            auto *doWait = llvm::BasicBlock::Create(ctx, "wait", fn);
            auto *waitFail = llvm::BasicBlock::Create(ctx, "wait.fail", fn);
            auto *ok = llvm::BasicBlock::Create(ctx, "ok", fn);

            llvm::IRBuilder<> b(entry);
            llvm::Value *codeSlot = b.CreateAlloca(i32Ty, nullptr, "code");
            b.CreateStore(llvm::ConstantInt::get(i32Ty, 0), codeSlot);

            b.CreateCall(getOrCreateKindGuard(),
                         {fn->getArg(0), llvm::ConstantInt::get(i64Ty, kKindProcess),
                          cstr(b, "مقبضَ عمليّة من شغل_برنامجا")});
            llvm::Value *raw = b.CreateCall(getOrCreateFind(),
                                            {fn->getArg(0), llvm::ConstantInt::get(i64Ty, 0)},
                                            "hraw");
            b.CreateCondBr(b.CreateICmpSLT(raw, llvm::ConstantInt::get(i64Ty, 0), "h.miss"),
                           unknown, doWait);

            b.SetInsertPoint(unknown);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_NOT_CHILD,
                     {{"handle", b.CreateCall(getOrCreateNumToStr(), {fn->getArg(0)}, "h.str")}});

            b.SetInsertPoint(doWait);
            llvm::Value *hProc = b.CreateIntToPtr(raw, ptrTy, "hproc");

            auto *setCtrlTy = llvm::FunctionType::get(i32Ty, {ptrTy, i32Ty}, false);
            auto setCtrl = cg_.module_->getOrInsertFunction("SetConsoleCtrlHandler", setCtrlTy);
            llvm::Function *handler = getOrCreateWinCtrlHandler();
            stdcall(setCtrl, b.CreateCall(setCtrl, {handler, llvm::ConstantInt::get(i32Ty, 1)}));

            auto *waitTy = llvm::FunctionType::get(i32Ty, {ptrTy, i32Ty}, false);
            auto waitFn = cg_.module_->getOrInsertFunction("WaitForSingleObject", waitTy);
            auto *waitCall =
                b.CreateCall(waitFn, {hProc, llvm::ConstantInt::get(i32Ty, kInfinite)}, "waited");
            stdcall(waitFn, waitCall);

            auto *exitTy = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto exitFn = cg_.module_->getOrInsertFunction("GetExitCodeProcess", exitTy);
            auto *exitCall = b.CreateCall(exitFn, {hProc, codeSlot}, "got.code");
            stdcall(exitFn, exitCall);

            auto *lastErrTy = llvm::FunctionType::get(i32Ty, {}, false);
            auto lastErrFn = cg_.module_->getOrInsertFunction("GetLastError", lastErrTy);
            auto *lastErrCall = b.CreateCall(lastErrFn, {}, "lasterr");
            stdcall(lastErrFn, lastErrCall);

            // (AR) يُرفعُ المعالجُ **قبل** أيِّ فرعٍ يخرج: تركُه مركَّبًا يُصمُّ
            //      البرنامجَ عن Ctrl-C إلى الأبد — عطبٌ دائمٌ من فشلٍ عابر.
            stdcall(setCtrl, b.CreateCall(setCtrl, {handler, llvm::ConstantInt::get(i32Ty, 0)}));

            llvm::Value *waitOk = b.CreateICmpEQ(
                waitCall, llvm::ConstantInt::get(i32Ty, kWaitObject0), "wait.ok");
            llvm::Value *codeOk =
                b.CreateICmpNE(exitCall, llvm::ConstantInt::get(i32Ty, 0), "code.ok");
            b.CreateCondBr(b.CreateAnd(waitOk, codeOk, "both.ok"), ok, waitFail);

            // (AR) فشلٌ عابر: المقبضُ لم يُسحَبْ أصلًا فيبقى صالحًا في يدِ المستعمل.
            b.SetInsertPoint(waitFail);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_WAIT_FAILED,
                     {{"handle", b.CreateCall(getOrCreateNumToStr(), {fn->getArg(0)}, "h.str2")},
                      {"reason", b.CreateCall(getOrCreateWinErrText(), {lastErrCall}, "reason")}});

            b.SetInsertPoint(ok);
            // (AR) السحبُ بعد النجاحِ وحدَه، ثمّ يُغلَقُ مقبضُ النظام.
            b.CreateCall(getOrCreateFind(), {fn->getArg(0), llvm::ConstantInt::get(i64Ty, 1)},
                         "drop");
            auto *closeHTy = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto closeH = cg_.module_->getOrInsertFunction("CloseHandle", closeHTy);
            stdcall(closeH, b.CreateCall(closeH, {hProc}));

            // (AR) الرمزُ خامٌّ: اصطلاحُ `128+الإشارة` لا نظيرَ له — ويندوز لا يقتلُ
            //      بإشارات، وتحويلُ رمزٍ كبيرٍ إليه كان سيخترعُ إشارةً لم تقع.
            b.CreateRet(b.CreateZExt(b.CreateLoad(i32Ty, codeSlot, "code.v"), i64Ty, "code64"));
            return fn;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) __sad_proc_spawn على ويندوز
        //
        //      لا شوكةَ ولا أنبوبَ ذاتيّ: `CreateProcess` تُبلّغُ فشلَ التشغيلِ في
        //      قيمةِ الإرجاعِ **قبل** أن يوجدَ ابن. فالالتباسُ الذي وُضع الأنبوبُ
        //      لإزالته (‏١٢٧ من execvp فاشلةٍ تُشبهُ ١٢٧ مقصودًا) لا يقعُ هنا أصلًا
        //      — لا لأنّنا عالجناه بل لأنّ الواجهةَ لا تُنتجه.
        //
        //      والتوجيهُ يُنسَخُ نسخةً موروثةً بـ`DuplicateHandle` بدل تبديلِ
        //      خاصّيّةِ مقبضِ المستعمل: تبديلُ الخاصّيّةِ يتركُه موروثًا لو خرجنا
        //      بخطأٍ بين التبديلِ والاستعادة — تسريبٌ إلى كلِّ ابنٍ لاحق.
        // ════════════════════════════════════════════════════════════════════
        llvm::Function *ProcessesBuiltinsCodeGen::getOrCreateWinSpawn()
        {
            const char *name = "__sad_proc_spawn";
            if (llvm::Function *existing = cg_.module_->getFunction(name))
                return existing;

            auto &ctx = *cg_.context_;
            auto *i8Ty = llvm::Type::getInt8Ty(ctx);
            auto *i32Ty = llvm::Type::getInt32Ty(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);
            const std::string triple = cg_.module_->getTargetTriple();
            const bool win32 = isWin32Bit(triple);
            const WinLayout &L = win32 ? kLayout32 : kLayout64;

            auto stdcall = [&](llvm::FunctionCallee callee, llvm::CallInst *call) {
                if (!win32)
                    return;
                if (auto *f = llvm::dyn_cast<llvm::Function>(callee.getCallee()))
                    f->setCallingConv(llvm::CallingConv::X86_StdCall);
                call->setCallingConv(llvm::CallingConv::X86_StdCall);
            };

            // (AR) نفسُ توقيعِ نظيرِه POSIX: (مسار، بيانات الوسائط، طولها، خريطة).
            auto *fnTy = llvm::FunctionType::get(i64Ty, {ptrTy, ptrTy, i64Ty, ptrTy}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage, name,
                                              cg_.module_.get());

            auto *entry = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto *sizeLoop = llvm::BasicBlock::Create(ctx, "size.loop", fn);
            auto *sizeBody = llvm::BasicBlock::Create(ctx, "size.body", fn);
            auto *sizeDone = llvm::BasicBlock::Create(ctx, "size.done", fn);
            auto *qLoop = llvm::BasicBlock::Create(ctx, "q.loop", fn);
            auto *qBody = llvm::BasicBlock::Create(ctx, "q.body", fn);
            auto *qDone = llvm::BasicBlock::Create(ctx, "q.done", fn);
            auto *hasMap = llvm::BasicBlock::Create(ctx, "map", fn);
            auto *mapLoop = llvm::BasicBlock::Create(ctx, "map.loop", fn);
            auto *mapBody = llvm::BasicBlock::Create(ctx, "map.body", fn);
            auto *mapSkip = llvm::BasicBlock::Create(ctx, "map.skip", fn);
            auto *mapKey = llvm::BasicBlock::Create(ctx, "map.key", fn);
            auto *mapBadKey = llvm::BasicBlock::Create(ctx, "map.badkey", fn);
            auto *mapVal = llvm::BasicBlock::Create(ctx, "map.val", fn);
            auto *mapMiss = llvm::BasicBlock::Create(ctx, "map.miss", fn);
            auto *mapDup = llvm::BasicBlock::Create(ctx, "map.dup", fn);
            auto *mapDupFail = llvm::BasicBlock::Create(ctx, "map.dupfail", fn);
            auto *mapKeep = llvm::BasicBlock::Create(ctx, "map.keep", fn);
            // (AR) `mapNext` **زيادةُ العدّادِ وحدَها**. فصلُها عن `mapKeep` ليس
            //      ترتيبًا: خانةٌ فارغةٌ في الخريطةِ تقفزُ إلى هنا، ولو مرّت بخزنِ
            //      النسخةِ لكتبت `dupOut` **البائتةَ** في خانةٍ يدلُّ عليها `slotIdx`
            //      بائتٌ كذلك — توجيهٌ صامتٌ إلى مقبضٍ من دورةٍ سابقة.
            auto *mapNext = llvm::BasicBlock::Create(ctx, "map.next", fn);
            auto *afterMap = llvm::BasicBlock::Create(ctx, "after.map", fn);
            auto *spawnFail = llvm::BasicBlock::Create(ctx, "spawn.fail", fn);
            auto *spawnOk = llvm::BasicBlock::Create(ctx, "spawn.ok", fn);

            llvm::IRBuilder<> b(entry);

            // ── (AR) حجوزٌ في كتلةِ المدخلِ وحدَها ────────────────────────────
            llvm::Value *iSlot = b.CreateAlloca(i64Ty, nullptr, "i");
            llvm::Value *kSlot = b.CreateAlloca(i64Ty, nullptr, "k");
            llvm::Value *slotIdx = b.CreateAlloca(i64Ty, nullptr, "slotidx");
            llvm::Value *totalSlot = b.CreateAlloca(i64Ty, nullptr, "total");
            llvm::Value *pSlot = b.CreateAlloca(ptrTy, nullptr, "p");
            llvm::Value *dupSlot = b.CreateAlloca(ptrTy, llvm::ConstantInt::get(i64Ty, 3), "dup");
            // (AR) البنيتان تُحجَزان مصفوفتَي بايتاتٍ لأنّ الحشوَ يجبُ أن يطابقَ ما
            //      يتوقّعه kernel32 عددًا لا ضمنًا. ومصفوفةُ بايتاتٍ محاذاتُها ١،
            //      فيجبُ **رفعُها صراحةً** إلى ٨: مقبضٌ يُقرأُ من موضعٍ غيرِ محاذٍ
            //      سلوكٌ غيرُ معرَّفٍ يمرُّ على x86 ويسقطُ على معماريّةٍ أخرى — أي
            //      عطبٌ يظهرُ بعد النقلِ لا عند الكتابة.
            auto *siA = b.CreateAlloca(i8Ty, llvm::ConstantInt::get(i64Ty, L.siSize), "si");
            auto *piA = b.CreateAlloca(i8Ty, llvm::ConstantInt::get(i64Ty, L.piSize), "pi");
            siA->setAlignment(llvm::Align(8));
            piA->setAlignment(llvm::Align(8));
            llvm::Value *si = siA;
            llvm::Value *pi = piA;
            llvm::Value *dupOut = b.CreateAlloca(ptrTy, nullptr, "dup.out");

            auto *nullPtr = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));
            for (int s = 0; s < 3; ++s)
                b.CreateStore(nullPtr, b.CreateConstInBoundsGEP1_64(ptrTy, dupSlot, s));

            auto *strlenTy = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            auto strlenFn = cg_.module_->getOrInsertFunction("strlen", strlenTy);

            llvm::Value *n = fn->getArg(2);

            // ── (AR) قياسُ سطرِ الأوامرِ قبلَ حجزه ────────────────────────────
            // (AR) الحدُّ الأعلى: كلُّ محرفٍ قد يصيرُ محرفين (شرطةٌ تُضاعَف)، وكلُّ
            //      وسيطٍ يزيدُ تنصيصين وفراغًا. الحسابُ أعلى من الحاجةِ عمدًا —
            //      فيضُ عازلٍ هنا يكتبُ في كومةِ البرنامجِ لا يُخطئُ خطأً مقروءًا.
            b.CreateStore(b.CreateCall(strlenFn, {fn->getArg(0)}, "path.len"), totalSlot);
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iSlot);
            b.CreateBr(sizeLoop);

            b.SetInsertPoint(sizeLoop);
            b.CreateCondBr(b.CreateICmpSLT(b.CreateLoad(i64Ty, iSlot, "i.s"), n, "i.lt"), sizeBody,
                           sizeDone);

            b.SetInsertPoint(sizeBody);
            llvm::Value *argPtr = b.CreateIntToPtr(
                b.CreateLoad(i64Ty,
                             b.CreateGEP(i64Ty, fn->getArg(1),
                                         {b.CreateLoad(i64Ty, iSlot, "i.g")}, "arg.gep"),
                             "arg.i64"),
                ptrTy, "arg.ptr");
            b.CreateStore(b.CreateAdd(b.CreateLoad(i64Ty, totalSlot, "t.v"),
                                      b.CreateCall(strlenFn, {argPtr}, "arg.len")),
                          totalSlot);
            b.CreateStore(
                b.CreateAdd(b.CreateLoad(i64Ty, iSlot, "i.i"), llvm::ConstantInt::get(i64Ty, 1)),
                iSlot);
            b.CreateBr(sizeLoop);

            b.SetInsertPoint(sizeDone);
            llvm::Value *bufSize = b.CreateAdd(
                b.CreateMul(b.CreateLoad(i64Ty, totalSlot, "t.f"),
                            llvm::ConstantInt::get(i64Ty, 2)),
                b.CreateAdd(b.CreateMul(b.CreateAdd(n, llvm::ConstantInt::get(i64Ty, 1)),
                                        llvm::ConstantInt::get(i64Ty, 4)),
                            llvm::ConstantInt::get(i64Ty, 16)),
                "buf.size");
            auto *mallocTy = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocTy);
            llvm::Value *cmdBuf = b.CreateCall(mallocFn, {bufSize}, "cmd");

            llvm::Function *quote = getOrCreateWinQuoteArg();
            b.CreateStore(b.CreateCall(quote, {cmdBuf, fn->getArg(0)}, "p.path"), pSlot);
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iSlot);
            b.CreateBr(qLoop);

            b.SetInsertPoint(qLoop);
            b.CreateCondBr(b.CreateICmpSLT(b.CreateLoad(i64Ty, iSlot, "i.q"), n, "i.qlt"), qBody,
                           qDone);

            b.SetInsertPoint(qBody);
            llvm::Value *pCur = b.CreateLoad(ptrTy, pSlot, "p.c");
            b.CreateStore(llvm::ConstantInt::get(i8Ty, ' '), pCur);
            llvm::Value *argQ = b.CreateIntToPtr(
                b.CreateLoad(i64Ty,
                             b.CreateGEP(i64Ty, fn->getArg(1),
                                         {b.CreateLoad(i64Ty, iSlot, "i.qg")}, "argq.gep"),
                             "argq.i64"),
                ptrTy, "argq.ptr");
            b.CreateStore(
                b.CreateCall(quote, {b.CreateConstInBoundsGEP1_64(i8Ty, pCur, 1), argQ}, "p.next"),
                pSlot);
            b.CreateStore(
                b.CreateAdd(b.CreateLoad(i64Ty, iSlot, "i.qi"), llvm::ConstantInt::get(i64Ty, 1)),
                iSlot);
            b.CreateBr(qLoop);

            b.SetInsertPoint(qDone);
            b.CreateStore(llvm::ConstantInt::get(i8Ty, 0), b.CreateLoad(ptrTy, pSlot, "p.end"));

            // ── (AR) خريطةُ التوجيه ⇒ ثلاثةُ مقابضَ موروثةٍ نملكها ───────────
            llvm::Value *mapPtr = fn->getArg(3);
            b.CreateCondBr(b.CreateICmpNE(mapPtr, nullPtr, "has.map"), hasMap, afterMap);

            b.SetInsertPoint(hasMap);
            llvm::Value *cap = b.CreateLoad(
                i64Ty, b.CreateGEP(i64Ty, mapPtr, {llvm::ConstantInt::get(i64Ty, 1)}, "cap.gep"),
                "cap");
            llvm::Value *keys = b.CreateIntToPtr(
                b.CreateLoad(i64Ty,
                             b.CreateGEP(i64Ty, mapPtr, {llvm::ConstantInt::get(i64Ty, 2)},
                                         "keys.gep"),
                             "keys.i64"),
                ptrTy, "keys");
            llvm::Value *valsArr = b.CreateIntToPtr(
                b.CreateLoad(i64Ty,
                             b.CreateGEP(i64Ty, mapPtr, {llvm::ConstantInt::get(i64Ty, 3)},
                                         "vals.gep"),
                             "vals.i64"),
                ptrTy, "vals");
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), kSlot);
            b.CreateBr(mapLoop);

            b.SetInsertPoint(mapLoop);
            llvm::Value *k = b.CreateLoad(i64Ty, kSlot, "k.v");
            b.CreateCondBr(b.CreateICmpSLT(k, cap, "k.lt"), mapBody, afterMap);

            b.SetInsertPoint(mapBody);
            llvm::Value *keyPtr =
                b.CreateLoad(ptrTy, b.CreateGEP(ptrTy, keys, {k}, "key.gep"), "key");
            b.CreateCondBr(b.CreateICmpEQ(keyPtr, nullPtr, "key.empty"), mapSkip, mapKey);

            b.SetInsertPoint(mapSkip);
            b.CreateBr(mapNext);

            b.SetInsertPoint(mapKey);
            auto *strcmpTy = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto strcmpFn = cg_.module_->getOrInsertFunction("strcmp", strcmpTy);
            auto keyIs = [&](const char *text, const char *nm) {
                return b.CreateICmpEQ(
                    b.CreateCall(strcmpFn,
                                 {keyPtr, b.CreateGlobalStringPtr(text, "proc.key")}, nm),
                    llvm::ConstantInt::get(i32Ty, 0), std::string(nm) + ".eq");
            };
            llvm::Value *is0 = keyIs("0", "key0");
            llvm::Value *is1 = keyIs("1", "key1");
            llvm::Value *is2 = keyIs("2", "key2");
            b.CreateStore(b.CreateSelect(is0, llvm::ConstantInt::get(i64Ty, 0),
                                         b.CreateSelect(is1, llvm::ConstantInt::get(i64Ty, 1),
                                                        llvm::ConstantInt::get(i64Ty, 2),
                                                        "idx.12"),
                                         "idx"),
                          slotIdx);
            b.CreateCondBr(b.CreateOr(b.CreateOr(is0, is1), is2, "key.known"), mapVal, mapBadKey);

            b.SetInsertPoint(mapBadKey);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_BAD_STREAM_KEY, {{"key", keyPtr}});

            b.SetInsertPoint(mapVal);
            llvm::Value *handle =
                b.CreateLoad(i64Ty, b.CreateGEP(i64Ty, valsArr, {k}, "val.gep"), "handle");
            b.CreateCall(getOrCreateKindGuard(),
                         {handle, llvm::ConstantInt::get(i64Ty, kKindDescriptor),
                          cstr(b, "وصفًا من انبوب أو افتح_وصفا")});
            llvm::Value *fdv = b.CreateCall(getOrCreateFind(),
                                            {handle, llvm::ConstantInt::get(i64Ty, 0)}, "fd");
            b.CreateCondBr(b.CreateICmpSLT(fdv, llvm::ConstantInt::get(i64Ty, 0), "fd.miss"),
                           mapMiss, mapDup);

            b.SetInsertPoint(mapMiss);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_BAD_DESCRIPTOR,
                     {{"handle", b.CreateCall(getOrCreateNumToStr(), {handle}, "h.str")}});

            b.SetInsertPoint(mapDup);
            // (AR) `_get_osfhandle` تُرجعُ `intptr_t`: ٦٤ بتّةً على x64 و٣٢ على x86.
            //      تثبيتُها على ٦٤ كان سيقرأُ نصفَ المقبضِ زائدًا نصفَ سجلٍّ آخرَ
            //      على المنصّةِ الضيّقة — مقبضٌ صالحُ الشكلِ يشيرُ إلى لا شيء.
            auto *intptrTy = win32 ? i32Ty : i64Ty;
            auto *osfTy = llvm::FunctionType::get(intptrTy, {i32Ty}, false);
            auto osfFn = cg_.module_->getOrInsertFunction("_get_osfhandle", osfTy);
            llvm::Value *osf = b.CreateCall(osfFn, {b.CreateTrunc(fdv, i32Ty, "fd32")}, "osf");
            llvm::Value *srcH = b.CreateIntToPtr(osf, ptrTy, "src.h");

            auto *curProcTy = llvm::FunctionType::get(ptrTy, {}, false);
            auto curProcFn = cg_.module_->getOrInsertFunction("GetCurrentProcess", curProcTy);
            auto *curProc = b.CreateCall(curProcFn, {}, "cur.proc");
            stdcall(curProcFn, curProc);

            auto *dupTy = llvm::FunctionType::get(
                i32Ty, {ptrTy, ptrTy, ptrTy, ptrTy, i32Ty, i32Ty, i32Ty}, false);
            auto dupFn = cg_.module_->getOrInsertFunction("DuplicateHandle", dupTy);
            auto *dupCall = b.CreateCall(
                dupFn,
                {curProc, srcH, curProc, dupOut, llvm::ConstantInt::get(i32Ty, 0),
                 llvm::ConstantInt::get(i32Ty, 1),
                 llvm::ConstantInt::get(i32Ty, kDuplicateSameAccess)},
                "dup.ok");
            stdcall(dupFn, dupCall);
            // (AR) وصفٌ سيّئٌ (‏-1 من _get_osfhandle) يُفشلُ النسخةَ فيُلتقطُ هنا.
            b.CreateCondBr(b.CreateICmpNE(dupCall, llvm::ConstantInt::get(i32Ty, 0), "dup.good"),
                           mapKeep, mapDupFail);

            // (AR) فشلُ النسخةِ فشلُ **توجيه** لا فشلُ تشغيل: الابنُ لم يُخلَق بعدُ،
            //      والعلّةُ في الوصفِ لا في المسار. نفسُ التمييزِ الذي يصنعه الأنبوبُ
            //      الذاتيُّ على POSIX، إلّا أنّه هنا متزامنٌ بلا أنبوبٍ أصلًا.
            b.SetInsertPoint(mapDupFail);
            {
                auto *leTy = llvm::FunctionType::get(i32Ty, {}, false);
                auto leFn = cg_.module_->getOrInsertFunction("GetLastError", leTy);
                auto *le = b.CreateCall(leFn, {}, "le");
                stdcall(leFn, le);
                emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_REDIRECT_FAILED,
                         {{"path", fn->getArg(0)},
                          {"reason", b.CreateCall(getOrCreateWinErrText(), {le}, "reason")}});
            }

            // (AR) خانةٌ تُكتبُ مرّتين: تُغلَقُ الأولى وإلّا تسرّبت. يقعُ فعلًا حين
            //      يمرَّرُ وصفان مختلفان لنفسِ المفتاح — وهو خطأُ مستعملٍ لا يُعاقَبُ
            //      عليه بتسريبِ مقبضٍ صامت.
            b.SetInsertPoint(mapKeep);
            {
                llvm::Value *slotPtr = b.CreateGEP(ptrTy, dupSlot,
                                                   {b.CreateLoad(i64Ty, slotIdx, "idx.n")},
                                                   "dup.slot");
                llvm::Value *prev = b.CreateLoad(ptrTy, slotPtr, "prev");
                auto *prevBB = llvm::BasicBlock::Create(ctx, "dup.prev", fn);
                auto *setBB = llvm::BasicBlock::Create(ctx, "dup.set", fn);
                b.CreateCondBr(b.CreateICmpNE(prev, nullPtr, "prev.live"), prevBB, setBB);

                b.SetInsertPoint(prevBB);
                auto *chTy = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
                auto chFn = cg_.module_->getOrInsertFunction("CloseHandle", chTy);
                stdcall(chFn, b.CreateCall(chFn, {prev}));
                b.CreateBr(setBB);

                b.SetInsertPoint(setBB);
                b.CreateStore(b.CreateLoad(ptrTy, dupOut, "dup.v"), slotPtr);
                b.CreateBr(mapNext);
            }

            b.SetInsertPoint(mapNext);
            b.CreateStore(
                b.CreateAdd(b.CreateLoad(i64Ty, kSlot, "k.c"), llvm::ConstantInt::get(i64Ty, 1)),
                kSlot);
            b.CreateBr(mapLoop);

            // ── (AR) STARTUPINFOA ثمّ CreateProcessA ─────────────────────────
            b.SetInsertPoint(afterMap);
            auto *memsetTy = llvm::FunctionType::get(ptrTy, {ptrTy, i32Ty, i64Ty}, false);
            auto memsetFn = cg_.module_->getOrInsertFunction("memset", memsetTy);
            b.CreateCall(memsetFn, {si, llvm::ConstantInt::get(i32Ty, 0),
                                    llvm::ConstantInt::get(i64Ty, L.siSize)});
            b.CreateCall(memsetFn, {pi, llvm::ConstantInt::get(i32Ty, 0),
                                    llvm::ConstantInt::get(i64Ty, L.piSize)});

            auto at = [&](llvm::Value *base, int64_t off, const char *nm) {
                return b.CreateConstInBoundsGEP1_64(i8Ty, base, off, nm);
            };
            b.CreateStore(llvm::ConstantInt::get(i32Ty, L.siSize), at(si, L.siCb, "si.cb"));
            b.CreateStore(llvm::ConstantInt::get(i32Ty, kStartfUseStdHandles),
                          at(si, L.siFlags, "si.flags"));

            auto *stdhTy = llvm::FunctionType::get(ptrTy, {i32Ty}, false);
            auto stdhFn = cg_.module_->getOrInsertFunction("GetStdHandle", stdhTy);
            const int64_t stdIds[3] = {kStdInputHandle, kStdOutputHandle, kStdErrorHandle};
            const int64_t siOffs[3] = {L.siStdIn, L.siStdOut, L.siStdErr};
            for (int s = 0; s < 3; ++s)
            {
                llvm::Value *mine =
                    b.CreateLoad(ptrTy, b.CreateConstInBoundsGEP1_64(ptrTy, dupSlot, s), "mine");
                auto *inherited = b.CreateCall(
                    stdhFn, {llvm::ConstantInt::get(i32Ty, stdIds[s])}, "std.h");
                stdcall(stdhFn, inherited);
                // (AR) ما لم يُوجَّه يرثُ مجرى الأب — نظيرُ تركِ fd على حالِه بعد fork.
                b.CreateStore(b.CreateSelect(b.CreateICmpNE(mine, nullPtr, "mine.live"), mine,
                                             inherited, "pick"),
                              at(si, siOffs[s], "si.std"));
            }

            auto *cpTy = llvm::FunctionType::get(
                i32Ty, {ptrTy, ptrTy, ptrTy, ptrTy, i32Ty, i32Ty, ptrTy, ptrTy, ptrTy, ptrTy},
                false);
            auto cpFn = cg_.module_->getOrInsertFunction("CreateProcessW", cpTy);
            // (AR) `CreateProcessW` تكتبُ في عازلِ الأمر، فيُمرَّرُ التوسيعُ المحجوزُ
            //      في الكومةِ لا ثابتًا. واسمُ برنامجٍ عربيٌّ عبر `A` كان يُؤوَّلُ
            //      بصفحةٍ محلّيّةٍ فيُطلَبُ برنامجٌ آخر — أو لا شيء.
            llvm::Value *wcmd = b.CreateCall(getOrCreateWinWiden(), {cmdBuf}, "wcmd");
            auto *cpCall = b.CreateCall(
                cpFn,
                {nullPtr, wcmd, nullPtr, nullPtr, llvm::ConstantInt::get(i32Ty, 1),
                 llvm::ConstantInt::get(i32Ty, 0), nullPtr, nullPtr, si, pi},
                "cp.ok");
            stdcall(cpFn, cpCall);

            auto *leTy = llvm::FunctionType::get(i32Ty, {}, false);
            auto leFn = cg_.module_->getOrInsertFunction("GetLastError", leTy);
            auto *lastErr = b.CreateCall(leFn, {}, "cp.err");
            stdcall(leFn, lastErr);

            // (AR) النسخُ الموروثةُ مِلكُنا لا مِلكُ المستعمل: تُغلَقُ فورَ الإنشاءِ
            //      على كلا المسارين. إبقاؤها يُبقيها موروثةً لكلِّ ابنٍ لاحق، وفي
            //      حلقةِ صدَفةٍ استنزافٌ مؤكَّد — نظيرُ إغلاقِ نسخِ F_DUPFD في الأب.
            auto *chTy2 = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto chFn2 = cg_.module_->getOrInsertFunction("CloseHandle", chTy2);
            for (int s = 0; s < 3; ++s)
            {
                llvm::Value *h =
                    b.CreateLoad(ptrTy, b.CreateConstInBoundsGEP1_64(ptrTy, dupSlot, s), "own");
                auto *liveBB = llvm::BasicBlock::Create(ctx, "own.live", fn);
                auto *contBB = llvm::BasicBlock::Create(ctx, "own.cont", fn);
                b.CreateCondBr(b.CreateICmpNE(h, nullPtr, "own.ne"), liveBB, contBB);
                b.SetInsertPoint(liveBB);
                stdcall(chFn2, b.CreateCall(chFn2, {h}));
                b.CreateBr(contBB);
                b.SetInsertPoint(contBB);
            }

            auto *freeTy = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), {ptrTy}, false);
            auto freeFn = cg_.module_->getOrInsertFunction("free", freeTy);
            b.CreateCall(freeFn, {cmdBuf});

            b.CreateCondBr(b.CreateICmpNE(cpCall, llvm::ConstantInt::get(i32Ty, 0), "cp.good"),
                           spawnOk, spawnFail);

            b.SetInsertPoint(spawnFail);
            emitFail(b, ::Sad::Errors::ErrorCode::RUN_PROC_SPAWN_FAILED,
                     {{"path", fn->getArg(0)},
                      {"reason", b.CreateCall(getOrCreateWinErrText(), {lastErr}, "reason")}});

            b.SetInsertPoint(spawnOk);
            llvm::Value *hProcess =
                b.CreateLoad(ptrTy, at(pi, L.piProcess, "pi.proc"), "hproc");
            // (AR) مقبضُ الخيطِ يُغلَقُ فورًا: لا حاجةَ لنا به، وإبقاؤه يمنعُ تحريرَ
            //      كائنِ الخيطِ بعد انتهائه — تسريبٌ لا يظهرُ إلّا بالعدّ.
            llvm::Value *hThread = b.CreateLoad(ptrTy, at(pi, L.piThread, "pi.thr"), "hthread");
            stdcall(chFn2, b.CreateCall(chFn2, {hThread}));

            b.CreateRet(b.CreateCall(getOrCreateStore(),
                                     {llvm::ConstantInt::get(i64Ty, kKindProcess),
                                      b.CreatePtrToInt(hProcess, i64Ty, "hproc.i64")},
                                     "handle"));
            return fn;
        }

    } // namespace LLVM
} // namespace Sad
