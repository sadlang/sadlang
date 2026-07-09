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
    int flags = fcntl(pfd[1], F_GETFD);
    if (flags >= 0)
    {
        (void)fcntl(pfd[1], F_SETFD, flags | FD_CLOEXEC);
    }

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
        // (AR) الابن: يستعيد إشارات المقدّمة للوضع السابق كي تصله Ctrl-C طبيعيًّا،
        //      يرث stdio، يحاول exec؛ إن فشل يكتب errno ثمّ يخرج.
        // (EN) child: restores the prior foreground-signal disposition so Ctrl-C reaches
        //      it, inherits stdio, tries exec; on failure writes errno and exits.
        sigaction(SIGINT,  &oldInt,  nullptr);
        sigaction(SIGQUIT, &oldQuit, nullptr);
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

} // namespace REPL
} // namespace Sad
