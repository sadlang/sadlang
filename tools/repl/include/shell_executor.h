// بسم الله الرحمن الرحيم
/**
 * @file shell_executor.h
 * @brief (AR) مُنفِّذ الأوامر الخارجيّة لصدَفة ص — إطلاق برنامج متزامنًا مع وراثة stdio.
 * @brief (EN) External-command executor for the ص shell — synchronous spawn, inherited stdio.
 */
#ifndef SAD_REPL_SHELL_EXECUTOR_H
#define SAD_REPL_SHELL_EXECUTOR_H

#include "shell_lexer.h" // (AR) نوع ShellStage (مرحلة + إعادة توجيه) / (EN) ShellStage type
#include <string>
#include <vector>

namespace Sad
{
namespace REPL
{
// (AR) نتيجة تشغيل برنامج خارجيّ: spawned=false إن تعذّر الإطلاق (غير موجود/صلاحيّة)؛
//      عندها errNo = رقم الخطأ. exitCode صالح حين spawned=true.
// (EN) Result of running an external program: spawned=false if launch failed
//      (not found / permission); exitCode valid when spawned.
struct ShellResult
{
    bool spawned;   ///< هل أُطلق البرنامج فعلًا / did the program actually launch
    int  exitCode;  ///< رمز الخروج إن أُطلق / exit code when spawned
    int  errNo;     ///< errno عند فشل الإطلاق / errno on launch failure
};

// (AR) تفاصيل فشل الإطلاق في سلسلة (يملؤها runPipeline حين spawned=false): إمّا تعذّر
//      تنفيذ برنامج (isRedirect=false، program=اسمه) أو تعذّر فتح ملفّ إعادة توجيه
//      (isRedirect=true، file=مساره). / (EN) launch-failure detail (set by runPipeline when
//      spawned=false): either a program failed to exec (isRedirect=false, program set) or a
//      redirection file failed to open (isRedirect=true, file set).
struct LaunchFailure
{
    std::string program;
    std::string file;
    bool isRedirect = false;
};

// (AR) يُشغّل argv (البرنامج + معطياته) متزامنًا، يرث stdin/stdout/stderr، وينتظر انتهاءه.
//      (أمر مفرد بلا أنابيب). / (EN) Runs argv synchronously, inherits stdio, waits for
//      completion (single command, no pipes).
ShellResult runExternal(const std::vector<std::string>& argv);

// (AR) يُشغّل سلسلة مراحل (بأنابيبها وإعادة توجيهها): stdout كلّ مرحلة يُوصَل بـstdin التالية؛
//      أوّل مرحلة ترث stdin وآخرها stdout ما لم تُعِد المرحلة التوجيه صراحةً (‹<›/‹>›/‹>>› +
//      ‹2>›/‹2>>›/‹&>›/‹2>&1› للخطأ، تفوز على الأنبوب). كلّها تزامنيّة (لا جمود). رمز الخروج
//      = المرحلة الأخيرة. عند تعذّر
//      الإطلاق: spawned=false وتُملأ fail (تنفيذ برنامج أو فتح ملفّ توجيه). متاح على POSIX
//      فقط؛ على Windows يُعيد spawned=false مع errNo=ENOSYS للمراحل المتعدّدة (يحرسه المستدعي).
// (EN) Runs a chain of stages (pipes + redirections): each stage's stdout feeds the next's
//      stdin; the first inherits stdin and the last stdout unless the stage redirects
//      explicitly (‹<›/‹>›/‹>>› win over the pipe). All concurrent (no deadlock). Exit code =
//      the last stage's. On launch failure: spawned=false and `fail` is populated (a program
//      exec or a redirection-file open). POSIX only; on Windows multi-stage returns
//      spawned=false with errNo=ENOSYS (caller guards first).
ShellResult runPipeline(const std::vector<ShellStage>& stages, LaunchFailure& fail);

} // namespace REPL
} // namespace Sad

#endif // SAD_REPL_SHELL_EXECUTOR_H
