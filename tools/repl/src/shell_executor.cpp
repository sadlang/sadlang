// بسم الله الرحمن الرحيم
/**
 * @file shell_executor.cpp
 * @brief (AR) تنفيذ مُنفِّذ الأوامر الخارجيّة (POSIX fork/execvp + Windows _spawnvp).
 * @brief (EN) External-command executor (POSIX fork/execvp + Windows _spawnvp).
 */
#include "shell_executor.h"

#include <cerrno>
#include <csignal>
#include <vector>

#ifdef _WIN32
#include <process.h>
#else
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace Sad
{
namespace REPL
{

#ifndef _WIN32
namespace
{
// (AR) يضبط علَم CLOEXEC على وصفٍ (يُغلَق تلقائيًّا عند exec). نستعمل F_SETFD لا
//      pipe2(O_CLOEXEC) لأنّ pipe2 غير متوفّرة على macOS (ضمن مصفوفة CI). النافذة بين
//      pipe() والضبط غير ذرّيّة، لكنّ معالجة أوامر REPL أحاديّة الخيط فلا fork مُنافِس فيها.
// (EN) Sets FD_CLOEXEC on a descriptor (auto-closed on exec). We use F_SETFD rather than
//      pipe2(O_CLOEXEC) because pipe2 is absent on macOS (in the CI matrix). The window
//      between pipe() and the set is non-atomic, but REPL command handling is single-
//      threaded so no competing fork occurs.
inline void setCloexec(int fd)
{
    int fl = fcntl(fd, F_GETFD);
    if (fl >= 0)
    {
        (void)fcntl(fd, F_SETFD, fl | FD_CLOEXEC);
    }
}

// (AR) في الابن قبل exec: أعِد الإشارات المُتحكّم فيها إلى الافتراضيّ صراحةً (لا استعادة
//      وضعيّة REPL) — كصدَفةٍ تُطلق أمرًا أماميًّا. الأهمّ SIGPIPE: لو ضبطته مكتبة (شبكة/
//      FFI) على SIG_IGN، ورثه الابن عبر exec، فمرحلةُ أنبوبٍ تكتب لطرفٍ مغلق (مثل
//      «مُنتِج | head») تحصل على EPIPE بدل الموت فقد تدور بلا نهاية ⇒ يتعلّق waitpid الأب.
// (EN) In the child before exec: explicitly reset the controlled signals to default (not
//      restore the REPL's disposition) — like a shell launching a foreground command. Most
//      importantly SIGPIPE: if a library (network/FFI) set it to SIG_IGN, the child inherits
//      that across exec, so a pipeline stage writing to a closed end (e.g. «producer | head»)
//      gets EPIPE instead of dying and may spin forever ⇒ the parent hangs in waitpid.
inline void resetChildSignalsToDefault()
{
    struct sigaction dfl;
    dfl.sa_handler = SIG_DFL;
    sigemptyset(&dfl.sa_mask);
    dfl.sa_flags = 0;
    sigaction(SIGINT, &dfl, nullptr);
    sigaction(SIGQUIT, &dfl, nullptr);
    sigaction(SIGPIPE, &dfl, nullptr);
}
} // namespace
#endif

ShellResult runExternal(const std::vector<std::string>& argv)
{
    ShellResult res{false, -1, 0};
    if (argv.empty())
    {
        res.errNo = EINVAL;
        return res;
    }

    // (AR) بناء مصفوفة argv بنمط C (منتهية بـnullptr) / (EN) build C-style argv (null-terminated)
    std::vector<char*> cargv;
    cargv.reserve(argv.size() + 1);
    for (const std::string& a : argv)
    {
        cargv.push_back(const_cast<char*>(a.c_str()));
    }
    cargv.push_back(nullptr);

#ifdef _WIN32
    // (AR) Windows: إطلاق متزامن. -1 يعني فشل الإطلاق فقط إذا ضُبِط errno؛ وإلّا فهو رمز
    //      خروج فعليّ (برنامج قد يخرج بـ-1). / (EN) synchronous spawn; -1 is a launch
    //      failure only if errno is set, otherwise it is a genuine exit code.
    errno = 0;
    intptr_t rc = _spawnvp(_P_WAIT, cargv[0], cargv.data());
    if (rc == -1 && errno != 0)
    {
        res.spawned = false;
        res.errNo   = errno;
        return res;
    }
    res.spawned  = true;
    res.exitCode = static_cast<int>(rc);
    return res;
#else
    // (AR) POSIX: أنبوب self-pipe بعلَم CLOEXEC للتمييز القاطع بين فشل execvp
    //      (يكتب errno ثمّ يُغلَق) والنجاح (يُغلَق تلقائيًّا عند exec فيُقرأ EOF).
    // (EN) POSIX: CLOEXEC self-pipe to distinguish exec failure (child writes errno)
    //      from success (pipe auto-closes on exec -> parent reads EOF).
    int pfd[2];
    if (pipe(pfd) != 0)
    {
        res.errNo = errno;
        return res;
    }
    setCloexec(pfd[1]);

    // (AR) أثناء انتظار الابن في المقدّمة تتجاهل الأمّ SIGINT/SIGQUIT كي تذهب Ctrl-C إلى
    //      الابن وحده وتنجو الصدَفة (سلوك صدَفة قياسيّ، كـsystem(3)). الابن يستعيد الوضع
    //      السابق قبل execvp، والأمّ تستعيده بعد الانتظار.
    // (EN) While waiting on the foreground child the parent ignores SIGINT/SIGQUIT so
    //      Ctrl-C reaches only the child and the shell survives (standard shell behavior,
    //      like system(3)). The child restores the prior disposition before execvp; the
    //      parent restores after waiting.
    struct sigaction ignoreAction, oldInt, oldQuit;
    ignoreAction.sa_handler = SIG_IGN;
    sigemptyset(&ignoreAction.sa_mask);
    ignoreAction.sa_flags = 0;
    sigaction(SIGINT,  &ignoreAction, &oldInt);
    sigaction(SIGQUIT, &ignoreAction, &oldQuit);

    pid_t pid = fork();
    if (pid < 0)
    {
        res.errNo = errno;
        sigaction(SIGINT,  &oldInt,  nullptr);
        sigaction(SIGQUIT, &oldQuit, nullptr);
        close(pfd[0]);
        close(pfd[1]);
        return res;
    }
    if (pid == 0)
    {
        // (AR) الابن: يعيد الإشارات (INT/QUIT/PIPE) إلى الافتراضيّ كي تصله Ctrl-C طبيعيًّا
        //      ولا يرث SIGPIPE مُتجاهَلة، يرث stdio، يحاول exec؛ إن فشل يكتب errno ثمّ يخرج.
        // (EN) child: resets signals (INT/QUIT/PIPE) to default so Ctrl-C reaches it and no
        //      ignored SIGPIPE is inherited; inherits stdio, tries exec; on failure writes errno.
        resetChildSignalsToDefault();
        close(pfd[0]);
        execvp(cargv[0], cargv.data());
        int e = errno;
        ssize_t wr = write(pfd[1], &e, sizeof(e));
        (void)wr;
        _exit(127);
    }

    // (AR) الأب: يقرأ الأنبوب (errno عند فشل exec، أو EOF عند النجاح) ثمّ ينتظر. يُعاد
    //      read وwaitpid عند EINTR (تماثلًا، كي لا يُساء تصنيف فشل exec عند مقاطعة إشارة).
    //      يُحصَد الابن المباشر هنا؛ حصاد يتامى الأحفاد (حين sad-repl=PID1) دَين متابعة
    //      (حلقة SIGCHLD على مستوى init، خارج نطاق الأمر المفرد).
    // (EN) parent: read the pipe (errno on exec failure, EOF on success) then wait. Both
    //      read and waitpid retry on EINTR (symmetry — never misclassify exec failure).
    //      The direct child is reaped here; reaping grandchild orphans (when sad-repl is
    //      PID1) is a follow-up (an init-level SIGCHLD loop, out of scope for a single command).
    close(pfd[1]);
    int childErrno = 0;
    ssize_t n;
    do
    {
        n = read(pfd[0], &childErrno, sizeof(childErrno));
    } while (n < 0 && errno == EINTR);
    close(pfd[0]);

    int status = 0;
    while (waitpid(pid, &status, 0) < 0 && errno == EINTR)
    {
        // (AR) أعِد المحاولة عند مقاطعة إشارة / (EN) retry on signal interruption
    }

    // (AR) استعادة إشارات المقدّمة في الأمّ / (EN) restore foreground signals in the parent
    sigaction(SIGINT,  &oldInt,  nullptr);
    sigaction(SIGQUIT, &oldQuit, nullptr);

    if (n > 0)
    {
        // (AR) execvp فشل / (EN) execvp failed
        res.spawned = false;
        res.errNo   = childErrno;
        return res;
    }

    res.spawned = true;
    if (WIFEXITED(status))
    {
        res.exitCode = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
        res.exitCode = 128 + WTERMSIG(status);
    }
    return res;
#endif
}

#ifndef _WIN32
namespace
{
// (AR) سجلّ فشل exec يُكتب عبر أنبوب self-pipe: أيّ مرحلة أخفقت ورقم خطئها.
// (EN) exec-failure record written through the self-pipe: which stage failed and its errno.
struct ExecFailure
{
    int stage;
    int err;
};
} // namespace
#endif

ShellResult runPipeline(const std::vector<std::vector<std::string>>& stages,
                        std::string& failedProgram)
{
    ShellResult res{false, -1, 0};
    failedProgram.clear();

    if (stages.empty())
    {
        res.errNo = EINVAL;
        return res;
    }
    // (AR) مرحلة واحدة = أمر مفرد؛ فوّضه لمسار runExternal المُختبَر (صفر انحدار).
    // (EN) a single stage is a plain command; delegate to the tested runExternal path.
    if (stages.size() == 1)
    {
        return runExternal(stages[0]);
    }

#ifdef _WIN32
    // (AR) أنابيب متعدّدة المراحل غير مدعومة على Windows في هذا الإصدار (يحرسه المستدعي).
    // (EN) multi-stage pipes are unsupported on Windows in this release (caller guards).
    res.errNo = ENOSYS;
    return res;
#else
    const std::size_t n = stages.size();

    // (AR) أنبوب self-pipe للإبلاغ عن فشل exec لأيّ مرحلة (write-end بعلَم CLOEXEC كي
    //      يُغلَق تلقائيًّا عند نجاح exec فيرى الأب EOF، ويبقى مفتوحًا عند الفشل ليُكتَب فيه).
    // (EN) self-pipe reporting any stage's exec failure (write end CLOEXEC: auto-closes on
    //      successful exec -> parent sees EOF; stays open on failure so the child can write).
    int efd[2];
    if (pipe(efd) != 0)
    {
        res.errNo = errno;
        return res;
    }
    setCloexec(efd[0]); // (AR) طرف قراءة الأب (لا يُنفَّذ فيه exec، لكن للاتّساق) / parent read end
    setCloexec(efd[1]); // (AR) طرف الكتابة: ينغلق تلقائيًّا عند نجاح exec ⇒ EOF للأب / auto-close on exec

    // (AR) تحييد SIGINT/SIGQUIT في الأب طوال عمل السلسلة (كـsystem(3)/runExternal).
    // (EN) neutralize SIGINT/SIGQUIT in the parent for the pipeline's lifetime.
    struct sigaction ignoreAction, oldInt, oldQuit;
    ignoreAction.sa_handler = SIG_IGN;
    sigemptyset(&ignoreAction.sa_mask);
    ignoreAction.sa_flags = 0;
    sigaction(SIGINT, &ignoreAction, &oldInt);
    sigaction(SIGQUIT, &ignoreAction, &oldQuit);

    std::vector<pid_t> pids;
    pids.reserve(n);
    int prevRead = -1;      // (AR) طرف القراءة من أنبوب المرحلة السابقة / read end from previous stage
    int forkErrno = 0;      // (AR) errno عند فشل fork/pipe في المنتصف / errno if fork/pipe fails mid-way
    bool spawnLoopOk = true;

    for (std::size_t k = 0; k < n && spawnLoopOk; ++k)
    {
        const bool notLast = (k + 1 < n);
        int pfd[2] = {-1, -1};
        if (notLast)
        {
            if (pipe(pfd) != 0)
            {
                forkErrno = errno;
                spawnLoopOk = false;
                break;
            }
            // (AR) CLOEXEC على طرفي أنبوب البيانات: dup2 في الابن يمسحه عن STDIN/STDOUT
            //      فيبقيان موروثَين، ويمنع تسرّبهما لأيّ exec غير مرتبط. / CLOEXEC on both
            //      data-pipe ends; the child's dup2 clears it on STDIN/STDOUT so they survive.
            setCloexec(pfd[0]);
            setCloexec(pfd[1]);
        }

        pid_t pid = fork();
        if (pid < 0)
        {
            forkErrno = errno;
            if (notLast)
            {
                close(pfd[0]);
                close(pfd[1]);
            }
            spawnLoopOk = false;
            break;
        }

        if (pid == 0)
        {
            // (AR) الابن: يعيد الإشارات (INT/QUIT/PIPE) للافتراضيّ، يوصّل stdin من السابق
            //      وstdout للتالي، يُغلق الأوصاف الزائدة ثمّ exec؛ عند الفشل يكتب السجلّ ويخرج.
            // (EN) child: reset signals (INT/QUIT/PIPE) to default, wire stdin/stdout, close
            //      extra fds, exec; on failure write the failure record and exit 127.
            resetChildSignalsToDefault();

            if (prevRead != -1)
            {
                dup2(prevRead, STDIN_FILENO);
            }
            if (notLast)
            {
                dup2(pfd[1], STDOUT_FILENO);
            }

            // (AR) أغلِق كلّ الأوصاف الأصليّة بعد التكرار (لا نسرّبها للبرنامج) / close originals
            if (prevRead != -1)
            {
                close(prevRead);
            }
            if (notLast)
            {
                close(pfd[0]);
                close(pfd[1]);
            }
            close(efd[0]);

            std::vector<char*> cargv;
            cargv.reserve(stages[k].size() + 1);
            for (const std::string& a : stages[k])
            {
                cargv.push_back(const_cast<char*>(a.c_str()));
            }
            cargv.push_back(nullptr);

            execvp(cargv[0], cargv.data());

            ExecFailure fail{static_cast<int>(k), errno};
            ssize_t wr = write(efd[1], &fail, sizeof(fail));
            (void)wr;
            _exit(127);
        }

        // (AR) الأب: احتفظ بالـpid، أغلِق أوصاف الأنبوب التي لم تعد تلزمه، ومرّر القراءة للتالي.
        // (EN) parent: keep pid, close pipe ends it no longer needs, pass read end forward.
        pids.push_back(pid);
        if (prevRead != -1)
        {
            close(prevRead);
        }
        if (notLast)
        {
            close(pfd[1]);
            prevRead = pfd[0];
        }
    }

    // (AR) إن انقطع الإطلاق في المنتصف، أغلِق أيّ طرف قراءة معلّق (لكيلا يتعلّق أبناء لاحقون).
    // (EN) if launching broke mid-way, close any dangling read end.
    if (!spawnLoopOk && prevRead != -1)
    {
        close(prevRead);
        prevRead = -1;
    }

    // (AR) أغلِق طرف الكتابة في الأب كي يصل EOF بعد إغلاق كلّ الأبناء له / close parent's write end
    close(efd[1]);

    // (AR) اقرأ سجلّات فشل exec (إن وُجدت). نحتفظ بأوّل مرحلةٍ أخفقت. read يُعاد عند EINTR.
    // (EN) read exec-failure records (if any); remember the first failing stage.
    ExecFailure firstFail{-1, 0};
    for (;;)
    {
        ExecFailure rec;
        ssize_t rd = read(efd[0], &rec, sizeof(rec));
        if (rd == static_cast<ssize_t>(sizeof(rec)))
        {
            if (firstFail.stage < 0 || rec.stage < firstFail.stage)
            {
                firstFail = rec;
            }
        }
        else if (rd < 0 && errno == EINTR)
        {
            continue;
        }
        else
        {
            break; // EOF (0) أو خطأ آخر / EOF or other error
        }
    }
    close(efd[0]);

    // (AR) احصد كلّ الأبناء المُطلَقين؛ احفظ حالة المرحلة الأخيرة (رمز خروج السلسلة).
    // (EN) reap all launched children; keep the last stage's status (pipeline exit code).
    int lastStatus = 0;
    for (std::size_t i = 0; i < pids.size(); ++i)
    {
        int st = 0;
        while (waitpid(pids[i], &st, 0) < 0 && errno == EINTR)
        {
        }
        if (i + 1 == pids.size())
        {
            lastStatus = st;
        }
    }

    sigaction(SIGINT, &oldInt, nullptr);
    sigaction(SIGQUIT, &oldQuit, nullptr);

    // (AR) فشل fork/pipe في المنتصف = فشل إطلاق كامل / a mid-way fork/pipe failure = launch failure
    if (!spawnLoopOk)
    {
        res.spawned = false;
        res.errNo = forkErrno;
        return res;
    }

    if (firstFail.stage >= 0)
    {
        res.spawned = false;
        res.errNo = firstFail.err;
        const std::size_t si = static_cast<std::size_t>(firstFail.stage);
        if (si < stages.size() && !stages[si].empty())
        {
            failedProgram = stages[si][0];
        }
        return res;
    }

    res.spawned = true;
    if (WIFEXITED(lastStatus))
    {
        res.exitCode = WEXITSTATUS(lastStatus);
    }
    else if (WIFSIGNALED(lastStatus))
    {
        res.exitCode = 128 + WTERMSIG(lastStatus);
    }
    return res;
#endif
}

} // namespace REPL
} // namespace Sad
