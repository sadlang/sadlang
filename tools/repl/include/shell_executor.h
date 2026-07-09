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
//      لا أنابيب في هذه الشريحة (أمر مفرد). / (EN) Runs argv synchronously, inherits stdio,
//      waits for completion. No pipes in this slice (single command).
ShellResult runExternal(const std::vector<std::string>& argv);

} // namespace REPL
} // namespace Sad

#endif // SAD_REPL_SHELL_EXECUTOR_H
