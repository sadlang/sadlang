// بسم الله الرحمن الرحيم
/**
 * @file shell_executor.h
 * @brief (AR) مُنفِّذ الأوامر الخارجيّة لصدَفة ص — إطلاق برنامج متزامنًا مع وراثة stdio.
 * @brief (EN) External-command executor for the ص shell — synchronous spawn, inherited stdio.
 */
#ifndef SAD_REPL_SHELL_EXECUTOR_H
#define SAD_REPL_SHELL_EXECUTOR_H

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

// (AR) يُشغّل argv (البرنامج + معطياته) متزامنًا، يرث stdin/stdout/stderr، وينتظر انتهاءه.
//      (أمر مفرد بلا أنابيب). / (EN) Runs argv synchronously, inherits stdio, waits for
//      completion (single command, no pipes).
ShellResult runExternal(const std::vector<std::string>& argv);

// (AR) يُشغّل سلسلة أنبوب: stdout كلّ مرحلة يُوصَل بـstdin التالية؛ أوّل مرحلة ترث stdin
//      وآخرها ترث stdout (كصدَفة). كلّ المراحل تعمل تزامنيّاً (لا جمود). رمز الخروج =
//      رمز المرحلة الأخيرة (كصدَفة). عند تعذّر إطلاق مرحلةٍ ما: spawned=false و
//      failedProgram = اسم برنامج تلك المرحلة. متاح على POSIX فقط؛ على Windows يُعيد
//      spawned=false مع errNo=ENOSYS للمراحل المتعدّدة (يحرسه المستدعي مسبقاً).
// (EN) Runs a pipeline: each stage's stdout feeds the next's stdin; the first inherits
//      stdin and the last inherits stdout (shell-like). All stages run concurrently (no
//      deadlock). Exit code = the last stage's (shell-like). On a stage that fails to
//      launch: spawned=false and failedProgram = that stage's program. POSIX only; on
//      Windows multi-stage returns spawned=false with errNo=ENOSYS (caller guards first).
ShellResult runPipeline(const std::vector<std::vector<std::string>>& stages,
                        std::string& failedProgram);

} // namespace REPL
} // namespace Sad

#endif // SAD_REPL_SHELL_EXECUTOR_H
