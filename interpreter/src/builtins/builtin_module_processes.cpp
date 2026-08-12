/**
 * @file builtin_module_processes.cpp
 * @brief (AR) وحدة عمليات — بدائيّات تشغيل العمليّات لتُكتب الصدَفة ببرنامج ص
 * @brief (EN) Processes module — process primitives enabling a shell written in Sad
 *
 * @details
 * (AR) المشكلة التي تحلّها هذه الوحدة: كان سقفُ برنامج ص في تشغيل الأوامر هو
 *      `نفذ_امر` و`نفذ`، وكلتاهما `std::system()` ⇒ `/bin/sh` وسيطًا. فأيّ «صدَفة»
 *      تُكتب فوقهما صدَفةٌ فوق صدَفة: الأنابيبُ والتوجيهُ تنفّذها `sh` لا ص، ولا مقبضَ
 *      عمليّةٍ ولا رمزَ خروجٍ منفصلًا ولا وصفَ ملفّ. هذه الوحدة تكسر ذلك الدَور بخمس
 *      بدائيّات: تشغيلٌ مباشر بـexecvp، وانتظارٌ برمزٍ حقيقيّ، وأنبوب، ووصفُ ملفّ.
 *
 *      ثلاثةُ التزاماتٍ ليست تفاصيلَ تنفيذ — غيابُها يجعل البدائيّات غير صالحة لصدَفة:
 *
 *      ١. **CLOEXEC على كلّ وصفٍ يُنشئه الرنتايم.** وصفٌ متسرّبٌ إلى ابنٍ لم يُوجَّه
 *         إليه صراحةً يمنع نهايةَ الملفّ: طرفُ كتابةٍ باقٍ مفتوحًا في ابنٍ ثالث يجعل
 *         قارئَ الأنبوب يتعلّق إلى الأبد. وهو تعلُّقٌ لا يظهر إلّا تحت الحمل.
 *      ٢. **أنبوبٌ ذاتيّ (self-pipe) يميّز فشل `execvp` من فشل البرنامج.** لولاه لبدا
 *         «مسارٌ غير موجود» رمزَ خروجٍ عاديًّا (127) لا يُفرَّق من برنامجٍ خرج بـ127
 *         عمدًا — تشخيصٌ كاذب. الأنبوبُ نفسه CLOEXEC: نجاحُ exec يُغلقه فيقرأ الأب
 *         نهايةَ ملفّ، وفشلُه يكتب `errno` فيه.
 *      ٣. **معالجة `EINTR`.** إشارةٌ تصل أثناء `waitpid` أو `read` لا يجوز أن تُترجَم
 *         إلى فشل؛ في صدَفةٍ تستقبل `SIGCHLD` هذا هو الوضع الطبيعيّ لا الاستثناء.
 *
 *      وقرارٌ رابع: **المنصّاتُ الثلاث كلُّها**، بمسارين لا بمحاكاة. كان هذا الملفّ
 *      يرمي `RUN_PROC_UNSUPPORTED_PLATFORM` على ويندوز بحجّة أنّ الدلالة مبنيّةٌ على
 *      فصل `fork` عن `exec` — واللحظةُ الوسطى بينهما هي موضعُ التوجيه. والحجّةُ صحيحةٌ
 *      عن `_spawnvp` وحدَها: هي التي لا تعطي تلك اللحظة. لكنّ `CreateProcess` تعطي ما
 *      هو **أقوى** منها: التوجيهُ يُمرَّر بيانًا في `STARTUPINFO` قبل الإنشاء، فلا
 *      حاجةَ إلى لحظةٍ وسطى أصلًا، ولا إلى أنبوبٍ ذاتيّ — لأنّ فشلَ التشغيل يُبلَّغ
 *      **متزامنًا** في قيمة الإرجاع لا بعد شوكةٍ ناجحة. فالمسار الويندوزيّ ليس محاكاةً
 *      أضعف بل تحقيقًا مباشرًا للالتزامات نفسها:
 *
 *        • CLOEXEC ⇐ `_O_NOINHERIT` على كلّ وصفٍ يُنشئه الرنتايم، وتوريثٌ **انتقائيّ**
 *          بنسخةٍ من `DuplicateHandle` تُخلق موروثةً وتُغلق فورَ الإنشاء.
 *        • تمييزُ فشل التوجيه من فشل التشغيل ⇐ فشلُ `DuplicateHandle` مقابل فشل
 *          `CreateProcess`، وكلاهما قبل وجود الابن.
 *        • EINTR ⇐ لا وجودَ له؛ ونظيرُ تحييد SIGINT هو `SetConsoleCtrlHandler`.
 *
 *      وثلاثةُ فروقٍ **معلَنة** لا مسكوتٌ عنها، لأنّها في النظام لا في التنفيذ:
 *        ١. `انتظر_عملية` تُرجع رمزَ الخروج الخامّ. اصطلاحُ `128+الإشارة` لا نظيرَ له:
 *           ويندوز لا يقتل بإشارات، والإنهاءُ القسريّ يظهر رمزًا كبيرًا (‏0xC000013A).
 *        ٢. البحثُ عن البرنامج يتبع قواعد `CreateProcess` لا `execvp`: المجلّدُ الجاري
 *           داخلٌ في البحث، واللواحقُ (‏.exe/.bat) تُكمَّل.
 *        ٣. الشوكةُ الويندوزيّة سطرُ أوامرَ واحدٌ لا ناقلةُ وسائط. تُبنى بقواعد اقتباس
 *           `CommandLineToArgvW` كي يعودَ التقسيمُ في الابن إلى ما أرسله المستعمل.
 *
 *      النوعان `عملية` و`وصف` **لا يمسّان نظام الأنواع**: `language-truth/types.yaml`
 *      ترتيبُه حرجٌ ثنائيًّا. فهما معرّفان عدديّان في جدولٍ داخليّ، على سابقةِ المقابس
 *      (builtin_module_sockets.cpp) و`FILE*` في FFI (builtin_module_assertions.cpp).
 *      والمقترحُ يشترط أن يكونا **مبهمين: لا حسابَ ولا مقارنةَ عليهما**. وعددٌ في
 *      واجهة ص لا يمكن منعُ الحساب عليه نحويًّا، فأقربُ ما بلغناه وسمُ نوعٍ في
 *      البتّات العليا: النوعان لا يتبادلان، والخلطُ يُكشف عند المدخل بـ`RUN073` لا
 *      يُنفَّذ. وقبل الوسم كان أوّلُ وصفٍ وأوّلُ عمليّةٍ كلاهما `1` — قابلَين للتبادل
 *      **بنجاحٍ صامت**. الوفاءُ هنا جزئيٌّ ومقيسٌ حدُّه، لا مُدَّعًى تامًّا.
 *
 * (EN) Five process primitives that break the `system()` ceiling: direct execvp,
 *      real exit codes, pipes, and file descriptors. Three non-negotiable
 *      obligations — CLOEXEC everywhere, a distinction between redirection failure
 *      and spawn failure, and EINTR handling. Implemented on all three platforms:
 *      fork+execvp with a self-pipe on POSIX, CreateProcess with STARTUPINFO on
 *      Windows. Three declared semantic differences on Windows: raw exit codes (no
 *      128+signal convention), CreateProcess search rules, and command-line quoting.
 *
 * @see language-truth/builtins/processes.yaml — مصدر الحقيقة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see tools/repl/src/shell_executor.cpp — النظير المكتوب بـC++ الذي تعِد هذه الوحدة بإغنائه عنه
 */

#include "builtin_common.h"
#include "builtin_registry.h"
namespace Bpr = Sad::Builtins::Names::Processes;

#include <atomic>
#include <cerrno>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
// (AR) يُقلَّص سطحُ windows.h: بلا هذين يجرّ الرأسُ winsock.h ورؤوسَ الواجهة
//      الرسوميّة، فتتصادم ماكرو `min`/`max` مع <algorithm> في وحداتٍ أخرى.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace Sad
{
    namespace Interpreter
    {
        namespace ProcessDetail
        {
            // ════════════════════════════════════════════════════════════
            // (AR) جداول المقابض — معرّفات عدديّة لا أنواع لغويّة جديدة.
            // (EN) Handle tables — numeric ids, not new language types.
            // ════════════════════════════════════════════════════════════

            static std::unordered_map<int64_t, int> g_descriptors; // معرّف ← fd
            static std::unordered_map<int64_t, int64_t> g_processes; // معرّف ← pid
            static std::mutex g_mutex;
            static std::atomic<int64_t> g_next_descriptor{1};
            static std::atomic<int64_t> g_next_process{1};

            // ─── وسمُ النوع في البتّات العليا ───
            // (AR) العدّادان كانا يبدآن من ١ في فضاءٍ واحد، فأوّلُ وصفٍ وأوّلُ
            //      عمليّةٍ كلاهما ١. فـانتظر_عملية(وصف) كانت تحصد عمليّةً أجنبيّة
            //      وتُرجع رمز خروجها، واغلق(عمليّة) تُغلق وصفًا أجنبيًّا — نجاحٌ
            //      كاذبٌ بلا تحذير، وهو بعينه ما وُضعت هذه الوحدة لمحاربته.
            //      الوسمُ يجعل الخلطَ **مكشوفًا عند المدخل** لا منفَّذًا: لا فحصَ
            //      في جدولٍ ولا اعتمادَ على تجاور الأرقام، بل بتّةٌ تُقرأ فورًا.
            //      وهو أقربُ ما يمكن بلوغُه من «مقبضٍ مبهم» (شرطُ المقترح) ما دام
            //      المقبضُ عددًا في واجهة ص: العددُ يبقى عددًا لكنّه لم يعد قابلًا
            //      للتبادل، والحسابُ عليه يُنتج وسمًا غير معروفٍ فيُرفض.
            // (EN) Both counters started at 1 in one space, so descriptor #1 and
            //      process #1 collided and the two kinds were interchangeable —
            //      a silent false success. A high-bit tag makes the mix-up
            //      detectable at entry instead of executable.
            static constexpr int64_t kKindShift = 56;
            static constexpr int64_t kKindMask = static_cast<int64_t>(0xFF) << kKindShift;
            static constexpr int64_t kKindDescriptor = static_cast<int64_t>(0xD0) << kKindShift;
            static constexpr int64_t kKindProcess = static_cast<int64_t>(0xC1) << kKindShift;

            static bool isDescriptorHandle(int64_t id) { return (id & kKindMask) == kKindDescriptor; }
            static bool isProcessHandle(int64_t id) { return (id & kKindMask) == kKindProcess; }

            /// (AR) وصفُ نوعِ مقبضٍ للرسائل — كي تقول الرسالةُ ما هو، لا «مجهول» فقط.
            static const char *handleKindName(int64_t id)
            {
                if (isDescriptorHandle(id))
                    return "وصفُ ملفّ";
                if (isProcessHandle(id))
                    return "مقبضُ عمليّة";
                return "ليس مقبضًا تُنتجه وحدة عمليات";
            }

            static int64_t storeDescriptor(int fd)
            {
                int64_t id = kKindDescriptor | g_next_descriptor.fetch_add(1);
                std::lock_guard<std::mutex> lock(g_mutex);
                g_descriptors[id] = fd;
                return id;
            }

            /// (AR) يُرجع fd المقابل، أو -1 إن كان المعرّف مجهولًا/مُغلَقًا.
            static int lookupDescriptor(int64_t id)
            {
                if (!isDescriptorHandle(id))
                    return -1;
                std::lock_guard<std::mutex> lock(g_mutex);
                auto it = g_descriptors.find(id);
                return it == g_descriptors.end() ? -1 : it->second;
            }

            static bool takeDescriptor(int64_t id, int &outFd)
            {
                if (!isDescriptorHandle(id))
                    return false;
                std::lock_guard<std::mutex> lock(g_mutex);
                auto it = g_descriptors.find(id);
                if (it == g_descriptors.end())
                    return false;
                outFd = it->second;
                g_descriptors.erase(it);
                return true;
            }

            static int64_t storeProcess(int64_t pid)
            {
                int64_t id = kKindProcess | g_next_process.fetch_add(1);
                std::lock_guard<std::mutex> lock(g_mutex);
                g_processes[id] = pid;
                return id;
            }

            /// (AR) يسحب الـpid ويحذف القيد — فانتظارٌ ثانٍ لنفس المعرّف يفشل صراحةً
            ///      بدل أن يُرجع -1 من waitpid متنكّرًا في هيئة رمز خروج.
            static bool takeProcess(int64_t id, int64_t &outPid)
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                auto it = g_processes.find(id);
                if (it == g_processes.end())
                    return false;
                outPid = it->second;
                g_processes.erase(it);
                return true;
            }

            /// (AR) يُعيد قيدَ العمليّة عند فشل الانتظار بغير ECHILD — كي لا يضيع
            ///      المقبض بسبب فشلٍ عابر فيصير الابنُ زومبيًّا لا سبيل لحصاده.
            static void restoreProcess(int64_t id, int64_t pid)
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                g_processes[id] = pid;
            }

            // ═══════════════════════════════════════════════════════════════
            // (AR) ما فوق هذا السطر مشتركٌ بين المنصّات: الجداولُ والوسمُ
            //      وحدَهما يضمنان **تطابقَ دلالةِ المقبض** — معرّفٌ تصاعديٌّ
            //      لا يُعاد استعمالُه، ونوعٌ يُكشف عند المدخل — أيًّا كان ما
            //      يُخزَّن تحته (‏fd على POSIX، وfd من CRT وHANDLE على ويندوز).
            //      ولو تفرّع الجدولُ لتفرّقت الدلالةُ من حيث لا يقيس اختبار.
            // (EN) Everything above is platform-shared: the tables and the kind
            //      tag alone guarantee identical handle semantics across
            //      platforms, whatever the stored payload happens to be.
            // ═══════════════════════════════════════════════════════════════

#ifndef _WIN32
            // (AR) كلّ ما يلي POSIX صرفًا. لُفَّ بالحارس كاملًا — لا مجرّد أجسامِ
            //      الدوالّ — وإلّا بقيت مساعداتٌ `static` بلا مستعمل على ويندوز
            //      (تحذيرات `-Wunused-function`، وفشلٌ تحت `-Werror`)، و`ssize_t`
            //      غير معرَّفةٍ أصلًا تحت MSVC فلا يُبنى الملفّ.
            // (EN) Everything below is POSIX-only. The guard wraps the whole block,
            //      not just function bodies: otherwise unused statics warn on Windows
            //      and ssize_t is undefined under MSVC, so the file would not build.

            // ─── تحييدُ SIGINT/SIGQUIT أثناء الانتظار، بعدّادٍ لا بحفظٍ محلّيّ ───
            // (AR) `sigaction` تُعدّل حالةً **عامّةً للعمليّة** لا للخيط. فانتظاران
            //      متزامنان على خيطين: الثاني يلتقط SIG_IGN بوصفه «القديم» ثمّ
            //      يُعيده عند انتهائه، فيفقد المفسّرُ استجابتَه لـCtrl-C **إلى
            //      الأبد** — عطبٌ دائمٌ من تتابعٍ عابر. والمفسّر يُنشئ خيوطًا فعلًا
            //      (builtin_module_async.cpp)، فليست الحالةُ نظريّة.
            //      العدّاد يجعل الحفظَ والاستعادةَ يقعان مرّةً واحدةً عند الحافّتين،
            //      ولا يُسلسِل الانتظارات (القفلُ يُحمل لحظةَ التبديل لا طوال الانتظار).
            // (EN) sigaction is process-global, not per-thread: two concurrent waits
            //      would have the second capture SIG_IGN as "old" and restore it,
            //      permanently deafening the interpreter to Ctrl-C. A depth counter
            //      saves/restores once at the edges without serialising the waits.
            static std::mutex g_wait_signal_mutex;
            static int g_wait_depth = 0;
            static struct sigaction g_old_int, g_old_quit;

            static void ignoreInterruptsForWait()
            {
                std::lock_guard<std::mutex> lock(g_wait_signal_mutex);
                if (g_wait_depth++ == 0)
                {
                    struct sigaction saIgn{};
                    saIgn.sa_handler = SIG_IGN;
                    // (AR) بلا `::` عمدًا: `sigemptyset` **ماكرو** في رأسِ ماك
                    //      (‏`(*(set) = 0, 0)`) لا دالّة، والتأهيلُ بالنطاق العامّ
                    //      لا يُطبَّق على ماكرو فتفشلُ الترجمةُ هناك وحدَها. وهي
                    //      دالّةٌ على glibc، فالصيغةُ غيرُ المؤهَّلة تعملُ في
                    //      الاثنين. الرمزُ نفسُه مُصدَّرٌ في الحالين، فنداءُ
                    //      المولِّد له في التمثيل الوسيط سليمٌ على المنصّتين.
                    // (EN) No `::` on purpose: sigemptyset is a *macro* in the macOS
                    //      header, and qualifying a macro fails to compile there
                    //      while being a plain function on glibc.
                    sigemptyset(&saIgn.sa_mask);
                    ::sigaction(SIGINT, &saIgn, &g_old_int);
                    ::sigaction(SIGQUIT, &saIgn, &g_old_quit);
                }
            }

            static void restoreInterruptsAfterWait()
            {
                std::lock_guard<std::mutex> lock(g_wait_signal_mutex);
                if (--g_wait_depth == 0)
                {
                    ::sigaction(SIGINT, &g_old_int, nullptr);
                    ::sigaction(SIGQUIT, &g_old_quit, nullptr);
                }
            }

            /// (AR) قراءةٌ كاملة تتجاوز EINTR وتقبل القراءة الجزئيّة.
            /// (EN) Full read across EINTR, tolerating short reads.
            static ssize_t readAll(int fd, void *buf, size_t count)
            {
                size_t got = 0;
                auto *p = static_cast<unsigned char *>(buf);
                while (got < count)
                {
                    ssize_t n = ::read(fd, p + got, count - got);
                    if (n == 0)
                        break; // نهاية ملفّ / EOF
                    if (n < 0)
                    {
                        if (errno == EINTR)
                            continue;
                        return -1;
                    }
                    got += static_cast<size_t>(n);
                }
                return static_cast<ssize_t>(got);
            }

            /// (AR) سببُ فشل الابن — يُكتب في الأنبوب الذاتيّ مع errno كي يُميَّز
            ///      فشلُ التوجيه من فشل التنفيذ. بلا هذا التمييز يُبلَّغ `EBADF` في
            ///      `dup2` بوصفه «تعذّر تشغيل البرنامج»، فيُرسَل المستخدم يفتّش عن
            ///      المسار وعلّتُه في الوصف. النظير `shell_executor.cpp` يميّزهما.
            /// (EN) Child failure kind, written alongside errno so a redirection
            ///      failure is not reported as a spawn failure.
            enum class ChildFail : int
            {
                Redirect = 1,
                Exec = 2
            };

            struct ChildError
            {
                int kind;
                int err;
            };

            /// (AR) ينشئ أنبوبًا بطرفين CLOEXEC. يُفضَّل pipe2 لأنّه ذرّيّ: بين pipe
            ///      وfcntl في النسخة البديلة نافذةٌ يمكن أن يرث فيها fork من خيطٍ آخر
            ///      الطرفَ بلا CLOEXEC — سباقٌ حقيقيّ لا نظريّ في برنامجٍ متعدّد الخيوط.
            static bool makeCloexecPipe(int fds[2])
            {
#ifdef __linux__
                if (::pipe2(fds, O_CLOEXEC) == 0)
                    return true;
                if (errno != ENOSYS)
                    return false;
#endif
                if (::pipe(fds) != 0)
                    return false;
                for (int i = 0; i < 2; ++i)
                {
                    int flags = ::fcntl(fds[i], F_GETFD);
                    if (flags < 0 || ::fcntl(fds[i], F_SETFD, flags | FD_CLOEXEC) < 0)
                    {
                        ::close(fds[0]);
                        ::close(fds[1]);
                        return false;
                    }
                }
                return true;
            }

            /// (AR) ينقل وصفًا رقمُه دون 3 إلى نطاقٍ أعلى قبل حلقة `dup2`.
            ///
            ///      لماذا: `dup2(مصدر, هدف)` يدهس الهدف. فلو كان مصدرُ مجرًى لاحقٍ
            ///      يساوي رقمَ مجرًى هدفٍ سابق (‏0 أو 1 أو 2) لأصبح التوجيه الثاني
            ///      يقرأ وصفًا دُهس — **توجيهٌ خاطئٌ صامت**، لا فشلٌ يُرى. الحالة
            ///      نادرة (وصفاتنا من `pipe`/`open` فهي ≥3 ما دامت 0/1/2 مفتوحة)
            ///      لكنّها ممكنة إن شُغّل المفسّر بمجرًى قياسيّ مُغلَق (`0<&-`).
            ///      النقل قبل الحلقة يُخرج المسألة من حيّز الاحتمال بسطرين.
            /// (EN) Move a sub-3 descriptor out of the way before the dup2 loop:
            ///      otherwise a later source can collide with an earlier target,
            ///      producing a silently wrong redirection rather than a failure.
            static int shiftAboveStd(int fd)
            {
                if (fd < 0 || fd >= 3)
                    return fd;
                int moved = ::fcntl(fd, F_DUPFD_CLOEXEC, 3);
                if (moved < 0)
                {
                    moved = ::fcntl(fd, F_DUPFD, 3);
                    if (moved >= 0)
                    {
                        int flags = ::fcntl(moved, F_GETFD);
                        if (flags >= 0)
                            ::fcntl(moved, F_SETFD, flags | FD_CLOEXEC);
                    }
                }
                return moved < 0 ? fd : moved;
            }

            /// (AR) مجموعةُ التوجيه بملكيّةٍ صريحة.
            ///
            ///      `shiftAboveStd` تُنشئ **وصفًا جديدًا** بـF_DUPFD، وهو ملكُ الأب.
            ///      كان يُترك مفتوحًا على كلّ مسار: بعد fork الناجح، وعند رميِ
            ///      مدخلٍ لاحقٍ في الحلقة، وعند فشل الأنبوب أو fork. فالدالّةُ التي
            ///      كُتبت لسدّ ثغرةٍ نادرة كانت تفتح تسريبًا في نفس الحالة النادرة،
            ///      وفي حلقةِ صدَفةٍ يعني ذلك EMFILE.
            ///      الحلّ RAII لا إغلاقٌ يدويّ: `ctx.error` تَرمي وهي [[noreturn]]،
            ///      فكلُّ إغلاقٍ يدويٍّ يحتاج تكرارَه عند كلّ مخرج — والهادم يكفي مرّة.
            /// (EN) Redirection set with explicit ownership. shiftAboveStd creates a
            ///      new parent-owned descriptor that was leaked on every path.
            ///      RAII rather than manual closes, because ctx.error is [[noreturn]].
            struct RedirectSet
            {
                int fd[3] = {-1, -1, -1};
                bool owned[3] = {false, false, false};

                void set(int slot, int rawFd)
                {
                    int moved = shiftAboveStd(rawFd);
                    fd[slot] = moved;
                    owned[slot] = (moved != rawFd);
                }

                ~RedirectSet()
                {
                    for (int i = 0; i < 3; ++i)
                        if (owned[i] && fd[i] >= 0)
                            ::close(fd[i]);
                }
            };
#else  // _WIN32
            // ═══════════════════════════════════════════════════════════════
            // (AR) المسار الويندوزيّ — Win32 + CRT، لا محاكاةَ POSIX.
            // (EN) The Windows path — Win32 + CRT, not a POSIX emulation.
            // ═══════════════════════════════════════════════════════════════

            /// (AR) UTF-8 ⇐ UTF-16 لنداءات Win32.
            ///
            ///      **لماذا `W` لا `A`**: دوالُّ `*A` تؤوّلُ البايتاتِ بصفحةِ الترميزِ
            ///      المحلّيّة (ACP)، وسلاسلُ ص كلُّها UTF-8. فمسارٌ عربيٌّ يُنشَأُ
            ///      باسمٍ مشوَّهٍ لا تجدُه دوالُّ القراءةِ في ص — وقد **قِيس** ذلك:
            ///      على جهازٍ ACP فيه ١٢٥٥، أنشأ `CreateFileA("توجيه_ويندوز.txt")`
            ///      ملفًّا اسمُه «‏״×ˆ״¬‡_ˆ†״¯ˆ״².txt» فكتب فيه الابنُ بنجاحٍ ثمّ
            ///      عجز `اقرأ_ملف` عن فتحه. أي أنّ الاختبارَ سقط في القراءةِ بينما
            ///      العلّةُ في الفتح، وهو أسوأُ أشكالِ العطب: يشيرُ إلى غيرِ موضعه.
            ///      واللغةُ التي معرّفاتُها ومساراتُها عربيّةٌ لا تحتملُ واجهةً
            ///      مربوطةً بصفحةِ ترميزٍ محلّيّة.
            /// (EN) Win32 *A functions interpret bytes in the local ANSI code page,
            ///      while every Sad string is UTF-8. Measured on an ACP-1255 host:
            ///      CreateFileA created a mojibake-named file that the child wrote
            ///      to successfully and that اقرأ_ملف then could not open — the test
            ///      failed at the read while the defect was at the open.
            static std::wstring widen(const std::string &s)
            {
                if (s.empty())
                    return std::wstring();
                int n = ::MultiByteToWideChar(CP_UTF8, 0, s.data(),
                                              static_cast<int>(s.size()), nullptr, 0);
                if (n <= 0)
                    return std::wstring();
                std::wstring out(static_cast<size_t>(n), L'\0');
                ::MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()),
                                      out.data(), n);
                return out;
            }

            /// (AR) نصُّ خطأٍ من `GetLastError`. لا يُستعمل `strerror` هنا: أخطاءُ
            ///      `CreateProcess` أخطاءُ نظامٍ لا `errno`، و`strerror(errno)` بعدها
            ///      يصف آخرَ فشلٍ في CRT — أي جملةً صحيحةَ الصياغة عن حدثٍ آخر تمامًا.
            /// (EN) Win32 error text. strerror(errno) after CreateProcess would
            ///      describe an unrelated earlier CRT failure — a well-formed lie.
            static std::string winErrorText(DWORD code)
            {
                char *buf = nullptr;
                DWORD n = ::FormatMessageA(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr, code, 0, reinterpret_cast<char *>(&buf), 0, nullptr);
                std::string out;
                if (n && buf)
                {
                    out.assign(buf, n);
                    while (!out.empty() && (out.back() == '\n' || out.back() == '\r' ||
                                            out.back() == '.' || out.back() == ' '))
                        out.pop_back();
                }
                if (buf)
                    ::LocalFree(buf);
                if (out.empty())
                    out = "رمزُ نظامٍ " + std::to_string(static_cast<unsigned long>(code));
                return out;
            }

            /// (AR) اقتباسُ وسيطٍ واحدٍ بقواعد `CommandLineToArgvW`.
            ///
            ///      لماذا لا تكفي إحاطتُه بعلامتَي تنصيص: القاعدةُ الويندوزيّة تُعامل
            ///      الشرطةَ المائلةَ الخلفيّة معاملةً خاصّةً **قبل علامة التنصيص وحدَها**.
            ///      فوسيطٌ ينتهي بـ`\` يجعل التنصيصَ الختاميّ مهروبًا، فيبتلع الوسيطُ
            ///      ما بعده. و`C:\مجلد\` مسارٌ عاديٌّ تمامًا — أي أنّ الحالةَ شائعةٌ لا
            ///      نادرة. والنتيجةُ ليست خطأً بل **وسائطَ مختلفةً في الابن**: نجاحٌ
            ///      كاذبٌ من جنس ما تحاربه هذه الوحدة.
            /// (EN) Backslashes are special only before a quote, so an argument ending
            ///      in `\` would escape the closing quote and swallow the next one —
            ///      and `C:\dir\` is an entirely ordinary path.
            ///
            ///      ولا يُقتبَسُ ما لا يحتاج. جُرِّب اقتباسُ الكلِّ تبسيطًا — وأسقطَ
            ///      الاختباراتِ فورًا: `cmd` تقرأُ ما بعد `/c` **نصًّا خامًّا** لا
            ///      ناقلةَ وسائطَ مقسَّمة، فـ`"cmd" "/c" "exit 0"` لا تُقرأ فيها
            ///      `"/c"` مفتاحًا فينقلبُ الأمرُ كلُّه إلى اسمِ برنامجٍ لا يوجد،
            ///      ويعودُ الرمزُ ١ لكلِّ شيء. أي أنّ الاقتباسَ الزائدَ ليس زينةً
            ///      لا أثرَ لها بل **يغيّرُ ما يُشغَّل**. والقاعدةُ نفسُها منفَّذةٌ
            ///      في المولِّد (‏processes_builtins_win.cpp) كي يتطابقَ النصّان.
            static void appendQuoted(std::string &out, const std::string &arg)
            {
                const bool needsQuotes =
                    arg.empty() || arg.find_first_of(" \t\n\v\"") != std::string::npos;
                if (!needsQuotes)
                {
                    out += arg;
                    return;
                }
                out += '"';
                for (size_t i = 0;; ++i)
                {
                    size_t slashes = 0;
                    while (i < arg.size() && arg[i] == '\\')
                    {
                        ++i;
                        ++slashes;
                    }
                    if (i == arg.size())
                    {
                        // (AR) قبل التنصيص الختاميّ: كلُّ شرطةٍ تُضاعَف.
                        out.append(slashes * 2, '\\');
                        break;
                    }
                    if (arg[i] == '"')
                    {
                        out.append(slashes * 2 + 1, '\\');
                        out += '"';
                    }
                    else
                    {
                        out.append(slashes, '\\');
                        out += arg[i];
                    }
                }
                out += '"';
            }

            static std::string buildCommandLine(const std::vector<std::string> &argv)
            {
                std::string cmd;
                for (size_t i = 0; i < argv.size(); ++i)
                {
                    if (i)
                        cmd += ' ';
                    appendQuoted(cmd, argv[i]);
                }
                return cmd;
            }

            // ─── تحييدُ Ctrl-C أثناء الانتظار، بعدّادٍ — نظيرُ ignoreInterruptsForWait ───
            // (AR) نفسُ العلّة ونفسُ الدواء: `SetConsoleCtrlHandler` حالةٌ **عامّةٌ
            //      للعمليّة**، فانتظاران متزامنان على خيطين كانا سيُزيلان المعالجَ
            //      عند أوّل انتهاء فيُقتل المفسّرُ في الانتظار الثاني. والعدّادُ يجعل
            //      التركيبَ والإزالةَ عند الحافّتين لا عند كلّ انتظار.
            //      والمعالجُ يُرجع TRUE أي «ابتُلع الحدث»: الصدَفةُ تبقى والمقاطَعُ
            //      هو الأمرُ الجاري — وويندوز يرسل CTRL_C_EVENT إلى **مجموعة وحدة
            //      التحكّم كلّها** فيصل الابنَ من تلقائه، فلا يُفوَّت إيصالُه إليه.
            // (EN) SetConsoleCtrlHandler is process-global, exactly like sigaction:
            //      a depth counter installs and removes it once at the edges. The
            //      handler swallows the event for the interpreter; Windows already
            //      delivered it to the whole console group, so the child still sees it.
            static std::mutex g_wait_ctrl_mutex;
            static int g_wait_depth = 0;

            static BOOL WINAPI waitCtrlHandler(DWORD type)
            {
                return (type == CTRL_C_EVENT || type == CTRL_BREAK_EVENT) ? TRUE : FALSE;
            }

            static void ignoreInterruptsForWait()
            {
                std::lock_guard<std::mutex> lock(g_wait_ctrl_mutex);
                if (g_wait_depth++ == 0)
                    ::SetConsoleCtrlHandler(waitCtrlHandler, TRUE);
            }

            static void restoreInterruptsAfterWait()
            {
                std::lock_guard<std::mutex> lock(g_wait_ctrl_mutex);
                if (--g_wait_depth == 0)
                    ::SetConsoleCtrlHandler(waitCtrlHandler, FALSE);
            }

            // ─── قفلُ الشوكة ───
            // (AR) `bInheritHandles=TRUE` تُورّث **كلَّ** مقبضٍ موروثٍ في العمليّة لا
            //      المذكورَ في STARTUPINFO وحدَه. ونحن نُنشئ نسخًا موروثةً لحظةَ الإنشاء
            //      ثمّ نُغلقها، فنافذةٌ ضيّقةٌ يمكن أن يقع فيها إنشاءٌ من خيطٍ آخر
            //      فيرث نسخَنا. القفلُ يُغلق النافذة بين الشوكات — وهو نظيرُ CLOEXEC
            //      لا بديلٌ عنه: `_O_NOINHERIT` يغطّي كلَّ وصفٍ **غيرِ** موجَّه.
            //      (‏PROC_THREAD_ATTRIBUTE_HANDLE_LIST يُغني عن القفل، وتركُه قرارُ
            //      تطابقٍ مع المسار POSIX الذي لا نظيرَ فيه له — دَينٌ معلَن.)
            // (EN) bInheritHandles=TRUE inherits every inheritable handle, not just
            //      the three named ones, so concurrent spawns are serialised.
            //      PROC_THREAD_ATTRIBUTE_HANDLE_LIST would remove the need — a
            //      declared debt, left out to keep both engine paths alike.
            static std::mutex g_spawn_mutex;

            /// (AR) مجموعةُ التوجيه على ويندوز — نظيرُ RedirectSet بالضبط.
            ///
            ///      لا تُبدَّل خصائصُ مقبضِ المستعمل نفسِه (`SetHandleInformation`)،
            ///      بل يُنسخ نسخةً موروثةً نملكها ونُغلقها. الفرق ليس تجميلًا:
            ///      تبديلُ خاصّيّةِ مقبضٍ يملكه المستعمل يتركه موروثًا لو رُميَ خطأٌ
            ///      بين التبديل والاستعادة — تسريبٌ إلى كلّ ابنٍ لاحق. والنسخةُ
            ///      المملوكةُ يهدمها الهادمُ على كلّ مخرج، و`ctx.error` ترمي.
            /// (EN) Duplicate rather than mutate the user's handle: an exception
            ///      between set and restore would leave it inheritable forever.
            struct WinRedirectSet
            {
                HANDLE h[3] = {nullptr, nullptr, nullptr};

                ~WinRedirectSet()
                {
                    for (int i = 0; i < 3; ++i)
                        if (h[i])
                            ::CloseHandle(h[i]);
                }

                /// @return false إن تعذّرت النسخة — أي فشلُ **توجيهٍ** لا فشلُ تشغيل.
                bool set(int slot, int fd)
                {
                    HANDLE src = reinterpret_cast<HANDLE>(::_get_osfhandle(fd));
                    if (src == INVALID_HANDLE_VALUE || src == nullptr)
                        return false;
                    HANDLE dup = nullptr;
                    if (!::DuplicateHandle(::GetCurrentProcess(), src,
                                           ::GetCurrentProcess(), &dup, 0, TRUE,
                                           DUPLICATE_SAME_ACCESS))
                        return false;
                    if (h[slot])
                        ::CloseHandle(h[slot]);
                    h[slot] = dup;
                    return true;
                }
            };
#endif // _WIN32
        } // namespace ProcessDetail

        // ═════════════════════════════════════════════════════════════════
        // (AR) تسجيل دوالّ وحدة عمليات
        // (EN) Register the Processes module builtins
        // ═════════════════════════════════════════════════════════════════
        void registerBuiltinsProcesses(Interpreter &interpreter)
        {
            using ProcessDetail::lookupDescriptor;
            using ProcessDetail::restoreProcess;
            using ProcessDetail::storeDescriptor;
            using ProcessDetail::storeProcess;
            using ProcessDetail::takeDescriptor;
            using ProcessDetail::takeProcess;
#ifndef _WIN32
            using ProcessDetail::readAll;
#endif

            // ─────────────────────────────────────────────────────────────
            // شغل_برنامجا / spawn — fork + execvp (+ dup2 داخليًّا)
            // ─────────────────────────────────────────────────────────────
            auto spawn_func = [&interpreter](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                // (AR) ما بين هذا السطر ولحظةِ الإنشاء **مشتركٌ بين المنصّتين**:
                //      البوّابةُ الأمنيّة، وفحوصُ الأنواع، وبناءُ الوسائط، والتحقّقُ
                //      من خريطة التوجيه. وهو شرطُ التطابق: لو تفرّع الفحصُ لاختلفت
                //      رسائلُ الرفض بين ويندوز وغيره — وهي رسائلُ يقيسها الاختبار.
                // (EN) Everything up to the actual spawn is platform-shared: the
                //      security gate, type checks and redirect-map validation. Were
                //      it forked, rejection messages would diverge across platforms.
                // (AR) نفس بوّابة نفذ_امر للاتّساق (builtin_module_assertions.cpp).
                //      وأنّها ترمي بدل أن تُرجع رمزًا قرارُ مالكٍ مفتوحٌ في المقترح:
                //      صدَفةُ ص تتعطّل كلّيًّا لا جزئيًّا تحت الوضع الآمن.
                // (EN) Same gate as نفذ_امر, for consistency. Open owner question.
                if (interpreter.getOptions().enableSecurity)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                              {{"resource", "عمليات/شغل_برنامجا"},
                               {"reason", "(AR) الوضع الآمن يمنع تشغيل العمليّات — وهو يُفعَّل براية --أمان، فالحلُّ إزالتُها"
                                          " / (EN) secure-mode blocks process spawning"}});
                }
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);

                const std::string path = args[0]->toString();

                // ─── بناء argv: argv[0] هو المسار، ثمّ الوسائط كما هي بلا تأويل ───
                std::vector<std::string> argvOwned;
                argvOwned.push_back(path);
                if (args.size() >= 2 && !args[1]->isNull())
                {
                    // (AR) وسيطٌ ثانٍ ليس قائمةً **يفشل صراحةً**. الابتلاع الصامت
                    //      كان يعني أنّ شغل_برنامجا("/bin/echo", "مرحبا") تُشغّل
                    //      echo **بلا وسائط** وتُرجع 0 — نجاحٌ كاذب، وهو الفشل
                    //      الذي يحاربه هذا الملفّ كلّه.
                    // (EN) A non-array second argument fails loudly: silently
                    //      dropping it ran the program with no arguments and
                    //      returned 0 — a false success.
                    if (!args[1]->isArray())
                        ctx.error(::Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH,
                                  {{"expected", "قائمة وسائط"},
                                   {"found", args[1]->getTypeName()}});
                    for (const auto &el : args[1]->toArrayRef())
                        argvOwned.push_back(el.toString());
                }
#ifndef _WIN32
                std::vector<char *> argv;
                argv.reserve(argvOwned.size() + 1);
                for (auto &s : argvOwned)
                    argv.push_back(const_cast<char *>(s.c_str()));
                argv.push_back(nullptr);
#endif

                // ─── خريطة التوجيه {رقم_المجرى: وصف} ───
                // (AR) تُمرَّر خريطةً لا عبر كشف dup2: فيستحيل على المستخدم أن ينسى
                //      إغلاق الوصف في الابن، ولا يُسرَّب رقمُ وصفٍ خامّ إلى واجهة ص.
#ifdef _WIN32
                ProcessDetail::WinRedirectSet redirectSet;
#else
                ProcessDetail::RedirectSet redirectSet;
                int *redirect = redirectSet.fd;
#endif
                if (args.size() >= 3 && !args[2]->isNull())
                {
                    // (AR) وسيطٌ ثالثٌ ليس خريطةً يفشل صراحةً كذلك — وهو الأخطر:
                    //      تمريرُ وصفٍ بدل خريطة كان يُهمل التوجيه صامتًا، فيذهب
                    //      خرجُ البرنامج إلى شاشة الأب ويبقى الملفّ الهدف فارغًا،
                    //      والرمز 0. «نجحَ» وما كتب شيئًا.
                    // (EN) A non-map third argument also fails loudly: passing a
                    //      descriptor instead of a map silently dropped the
                    //      redirection — output to the parent, target file empty,
                    //      exit code 0.
                    if (!args[2]->isMap())
                        ctx.error(::Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH,
                                  {{"expected", "خريطة توجيه {رقم_المجرى: وصف}"},
                                   {"found", args[2]->getTypeName()}});
                    for (const auto &kv : args[2]->toMapRef())
                    {
                        int slot = -1;
                        if (kv.first == "0") slot = 0;
                        else if (kv.first == "1") slot = 1;
                        else if (kv.first == "2") slot = 2;
                        // (AR) رمزٌ خاصّ بالمفتاح: العلّة فيه لا في الوصف. كان
                        //      يُبلَّغ بـ«الوصف 'مفتاح مجرى: 3' غير معروف» —
                        //      كذبٌ مرّتين: الوصفُ معروفٌ ومفتوح، والمذكورُ ليس وصفًا.
                        if (slot < 0)
                            ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_BAD_STREAM_KEY,
                                      {{"key", kv.first}});
                        int64_t id = kv.second.toInt64();
                        if (!ProcessDetail::isDescriptorHandle(id))
                            ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_HANDLE_KIND,
                                      {{"handle", std::to_string(id)},
                                       {"expected", "وصفًا من انبوب أو افتح_وصفا"},
                                       {"found", ProcessDetail::handleKindName(id)}});
                        int fd = lookupDescriptor(id);
                        if (fd < 0)
                            ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_BAD_DESCRIPTOR,
                                      {{"handle", std::to_string(id)}});
#ifdef _WIN32
                        // (AR) فشلُ النسخة فشلُ **توجيه** لا فشلُ تشغيل: الابنُ لم
                        //      يُخلق بعد، والعلّةُ في الوصف لا في المسار. نفسُ
                        //      التمييز الذي يصنعه الأنبوبُ الذاتيّ على POSIX، إلّا
                        //      أنّه هنا متزامنٌ بلا أنبوبٍ أصلًا.
                        if (!redirectSet.set(slot, fd))
                            ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_REDIRECT_FAILED,
                                      {{"path", path},
                                       {"reason", ProcessDetail::winErrorText(::GetLastError())}});
#else
                        redirectSet.set(slot, fd);
#endif
                    }
                }

#ifdef _WIN32
                // ═══ الإنشاء على ويندوز ═══
                // (AR) لا شوكةَ ولا أنبوبٌ ذاتيّ: `CreateProcess` تُبلّغ فشلَ التشغيل
                //      في قيمة الإرجاع **قبل** أن يوجد ابن. فالالتباسُ الذي وُضع
                //      الأنبوبُ لإزالته (‏127 من execvp فاشلة يُشبه 127 مقصودًا) لا
                //      يقع هنا أصلًا — لا لأنّنا عالجناه بل لأنّ الواجهة لا تُنتجه.
                {
                    // (AR) الأمرُ يُبنى UTF-8 ثمّ يُوسَّع: اسمُ برنامجٍ أو وسيطٌ
                    //      عربيٌّ عبر `CreateProcessA` كان سيُؤوَّلُ بصفحةٍ محلّيّةٍ
                    //      فيُطلَبَ برنامجٌ آخر — أو لا شيء.
                    std::wstring cmdline = ProcessDetail::widen(
                        ProcessDetail::buildCommandLine(argvOwned));
                    std::vector<wchar_t> cmdbuf(cmdline.begin(), cmdline.end());
                    cmdbuf.push_back(L'\0'); // CreateProcessW تكتب في هذا العازل

                    STARTUPINFOW si{};
                    si.cb = sizeof(si);
                    si.dwFlags = STARTF_USESTDHANDLES;
                    // (AR) ما لم يُوجَّه يرث مجرى الأب — نظيرُ ترك fd على حاله بعد fork.
                    si.hStdInput = redirectSet.h[0] ? redirectSet.h[0]
                                                    : ::GetStdHandle(STD_INPUT_HANDLE);
                    si.hStdOutput = redirectSet.h[1] ? redirectSet.h[1]
                                                     : ::GetStdHandle(STD_OUTPUT_HANDLE);
                    si.hStdError = redirectSet.h[2] ? redirectSet.h[2]
                                                    : ::GetStdHandle(STD_ERROR_HANDLE);

                    PROCESS_INFORMATION pi{};
                    BOOL ok;
                    DWORD lastError;
                    {
                        std::lock_guard<std::mutex> lock(ProcessDetail::g_spawn_mutex);
                        ok = ::CreateProcessW(nullptr, cmdbuf.data(), nullptr, nullptr,
                                              TRUE, 0, nullptr, nullptr, &si, &pi);
                        lastError = ::GetLastError();
                    }
                    if (!ok)
                        ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_SPAWN_FAILED,
                                  {{"path", path},
                                   {"reason", ProcessDetail::winErrorText(lastError)}});

                    // (AR) مقبضُ الخيط يُغلق فورًا: لا حاجةَ لنا به، وإبقاؤه يمنع
                    //      تحريرَ كائن الخيط بعد انتهائه — تسريبٌ لا يظهر إلّا بالعدّ.
                    ::CloseHandle(pi.hThread);
                    return std::make_shared<Data::Value>(static_cast<int64_t>(
                        storeProcess(reinterpret_cast<int64_t>(pi.hProcess))));
                }
#else
                // ─── الأنبوب الذاتيّ: يميّز فشل execvp من فشل البرنامج ───
                int errPipe[2];
                if (!ProcessDetail::makeCloexecPipe(errPipe))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_PIPE_FAILED,
                              {{"reason", std::strerror(errno)}});

                pid_t pid = ::fork();
                if (pid < 0)
                {
                    int saved = errno;
                    ::close(errPipe[0]);
                    ::close(errPipe[1]);
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_SPAWN_FAILED,
                              {{"path", path}, {"reason", std::strerror(saved)}});
                }

                if (pid == 0)
                {
                    // ═══ الابن ═══
                    // (AR) ما بين fork وexec يجب أن يقتصر على ما هو آمنٌ إشاريًّا
                    //      (async-signal-safe): المفسّر يُنشئ خيوطًا
                    //      (builtin_module_async.cpp)، وأخذُ قفلٍ حجزَه خيطٌ آخر
                    //      لحظةَ fork جمودٌ لا مخرج منه.
                    //      **دَينٌ معلَن**: `execvp` نفسها ليست آمنةً إشاريًّا قطعًا —
                    //      تبحث في PATH وقد تلجأ glibc إلى malloc حين يطول. إغلاقُه
                    //      التامّ يستلزم حلّ PATH **قبل** fork ثمّ `execv`، أو
                    //      `posix_spawn`. الاحتمال ضئيل والدَّين مذكورٌ لا مسكوتٌ عنه.
                    // (EN) Only async-signal-safe work belongs here. Declared debt:
                    //      execvp itself is not strictly safe (PATH search may
                    //      malloc); closing that gap means resolving PATH before
                    //      fork then execv, or using posix_spawn.
                    ::close(errPipe[0]);

                    ProcessDetail::ChildError cerr{};

                    for (int slot = 0; slot < 3; ++slot)
                    {
                        if (redirect[slot] >= 0 && ::dup2(redirect[slot], slot) < 0)
                        {
                            cerr.kind = static_cast<int>(ProcessDetail::ChildFail::Redirect);
                            cerr.err = errno;
                            ssize_t ignored = ::write(errPipe[1], &cerr, sizeof(cerr));
                            (void)ignored;
                            ::_exit(127);
                        }
                    }

                    // (AR) تُعاد الإشارات الثلاث إلى الافتراضيّ — لا SIGPIPE وحدها:
                    //      التجاهلُ يُورَّث عبر exec. تجاهلُ SIGPIPE يكسر إنهاءَ
                    //      الأنابيب المعتاد (‏`… | head` لا ينتهي)، وتجاهلُ SIGINT
                    //      يجعل الابنَ لا يستجيب لـCtrl-C. نفس ثلاثيّة
                    //      shell_executor.cpp:54-62 لا واحدةً منها.
                    //      ويُرفع قناعُ الإشارات الموروث كذلك: قناعٌ موروثٌ يبقى بعد
                    //      exec فيصمّ الابنَ عن إشاراتٍ لم يختر صممه عنها.
                    // (EN) Restore all three (not just SIGPIPE) and clear the
                    //      inherited signal mask — both survive exec.
                    ::signal(SIGPIPE, SIG_DFL);
                    ::signal(SIGINT, SIG_DFL);
                    ::signal(SIGQUIT, SIG_DFL);
                    sigset_t empty;
                    sigemptyset(&empty); // ماكرو على ماك — انظر التعليق أعلاه
                    ::pthread_sigmask(SIG_SETMASK, &empty, nullptr);

                    ::execvp(path.c_str(), argv.data());

                    cerr.kind = static_cast<int>(ProcessDetail::ChildFail::Exec);
                    cerr.err = errno;
                    ssize_t ignored = ::write(errPipe[1], &cerr, sizeof(cerr));
                    (void)ignored;
                    ::_exit(127);
                }

                // ═══ الأب ═══
                ::close(errPipe[1]);
                ProcessDetail::ChildError cerr{};
                ssize_t got = readAll(errPipe[0], &cerr, sizeof(cerr));
                ::close(errPipe[0]);

                // (AR) أيّ بايتٍ وصل يعني أنّ الابن أبلغ فشلًا. لا يُشترط الحجمُ
                //      كاملًا: كتابةُ 8 بايت دون PIPE_BUF ذرّيّة فالجزئيّة شبه
                //      مستحيلة، لكن **اشتراطَ** الحجم كان سيحوّل الجزئيّة النادرة
                //      إلى «نجاحٍ» يُرجع مقبضًا ثمّ يُرجع 127 لاحقًا — أي بالضبط
                //      الالتباسَ الذي وُضع الأنبوبُ لمنعه.
                // (EN) Any byte received means the child reported failure; requiring
                //      the exact size would turn a rare short read back into the
                //      very ambiguity the self-pipe exists to remove.
                // (AR) `got < 0` ليست «لا بلاغَ ⇐ نجاح». كان الشرط `got > 0` فيسقط
                //      السالبُ في فرع النجاح، فيُعاد مقبضٌ لعمليّةٍ **لم يُتحقّق من
                //      تشغيلها** — أي بالضبط الالتباسُ الذي وُضع الأنبوبُ لإزالته،
                //      عائدًا من الباب الذي لم يُفكَّر فيه. الحالةُ شبه مستحيلة
                //      (الوصفُ من صنعنا للتوّ) لكنّ «شبه مستحيل» ليس «مستحيل».
                //      ولأنّ مصيرَ الابن مجهول: يُقتل ويُحصد. حصادٌ حاجبٌ بلا قتلٍ كان
                //      سيُعلّق المفسّرَ خلف برنامجٍ طويل، وتركُه بلا حصادٍ يُخلّف يتيمًا
                //      لا مقبضَ له. القتلُ هو الخيار الوحيد الحتميّ.
                // (EN) got < 0 is not "no report ⇒ success": the old `got > 0` let a
                //      read error fall into the success branch and returned a handle
                //      for a process whose exec was never verified. Since the child's
                //      fate is unknown it is killed and reaped — a blocking reap would
                //      hang behind a long-running program, and leaving it orphans it.
                if (got < 0)
                {
                    const std::string reason = std::strerror(errno);
                    ::kill(pid, SIGKILL);
                    int status = 0;
                    while (::waitpid(pid, &status, 0) < 0 && errno == EINTR)
                    {
                    }
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_SPAWN_FAILED,
                              {{"path", path},
                               {"reason", "تعذّرت قراءةُ بلاغِ الابن فلم يُتحقّق من التشغيل: " + reason}});
                }

                if (got > 0)
                {
                    // (AR) نحصد الابن كي لا يبقى زومبيًّا، ثمّ نرمي بالسبب الصحيح.
                    int status = 0;
                    while (::waitpid(pid, &status, 0) < 0 && errno == EINTR)
                    {
                    }
                    const bool partial = got < static_cast<ssize_t>(sizeof(cerr));
                    const std::string reason =
                        partial ? std::string("بلاغُ فشلٍ ناقصٌ من الابن")
                                : std::string(std::strerror(cerr.err));
                    // (AR) يُميَّز فشلُ التوجيه من فشل التنفيذ برمزٍ مستقلّ: الأوّل
                    //      علّتُه في الوصف لا في المسار، والبرنامجُ لم يُشغَّل أصلًا.
                    //      كان يُبلَّغ بـRUN064 ونصُّ strerror مدسوسٌ في موضع {handle}،
                    //      فتُصاغ جملةٌ لا تصف شيئًا: «الوصف 'توجيه مجرًى: Bad file
                    //      descriptor' غير معروف». الرمزُ المستقلّ يُطابق الرسالةَ بالواقع.
                    if (!partial &&
                        cerr.kind == static_cast<int>(ProcessDetail::ChildFail::Redirect))
                        ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_REDIRECT_FAILED,
                                  {{"path", path}, {"reason", reason}});
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_SPAWN_FAILED,
                              {{"path", path}, {"reason", reason}});
                }

                return std::make_shared<Data::Value>(
                    static_cast<int64_t>(storeProcess(static_cast<int64_t>(pid))));
#endif
            };
            interpreter.getFunctionManager().registerBuiltinFunction(
                std::string(Bpr::PROC_SPAWN), spawn_func);

            // ─────────────────────────────────────────────────────────────
            // انتظر_عملية / wait — waitpid
            // ─────────────────────────────────────────────────────────────
            auto wait_func = [](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);

                int64_t id = args[0]->toInt64();
                // (AR) فحصُ النوع قبل الجدول: تمريرُ وصفٍ هنا كان يحصد عمليّةً
                //      أجنبيّة ويُرجع رمزَ خروجها بلا تحذير. الرسالةُ تقول ما هو
                //      المقبض لا «مجهول» فحسب — الفرق بين «أخطأتَ الدالّة» و«ضاع
                //      المقبض» فرقٌ في اتّجاه البحث كلّه.
                if (!ProcessDetail::isProcessHandle(id))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_HANDLE_KIND,
                              {{"handle", std::to_string(id)},
                               {"expected", "مقبضَ عمليّة من شغل_برنامجا"},
                               {"found", ProcessDetail::handleKindName(id)}});
                int64_t pid = 0;
                if (!takeProcess(id, pid))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_NOT_CHILD,
                              {{"handle", std::to_string(id)}});

                // (AR) يُحيَّد SIGINT/SIGQUIT طوال الانتظار ثمّ يُعادان.
                //      بلا هذا يقتل Ctrl-C **مفسّر ص نفسه** مع الابن — وهو سلوكٌ
                //      خاطئ لصدَفة: الصدَفة تبقى، والمقاطَعُ هو الأمر الجاري.
                //      نفس ما يفعله النظير shell_executor.cpp:120-125.
                // (EN) Ignore SIGINT/SIGQUIT while waiting, then restore: otherwise
                //      Ctrl-C kills the interpreter along with the child — wrong for
                //      a shell, where the shell survives and the command dies.
                ProcessDetail::ignoreInterruptsForWait();

#ifdef _WIN32
                // ═══ الانتظار على ويندوز ═══
                HANDLE hProc = reinterpret_cast<HANDLE>(pid);
                DWORD waited = ::WaitForSingleObject(hProc, INFINITE);
                DWORD exitCode = 0;
                bool gotCode = (waited == WAIT_OBJECT_0) &&
                               ::GetExitCodeProcess(hProc, &exitCode) != 0;
                DWORD winErr = gotCode ? 0 : ::GetLastError();

                ProcessDetail::restoreInterruptsAfterWait();

                if (!gotCode)
                {
                    // (AR) فشلٌ عابر: المقبضُ ما يزال صالحًا فيُعاد إلى الجدول —
                    //      نفسُ قرار POSIX بحرفه. ولا نظيرَ لـECHILD هنا: مقبضُ
                    //      العمليّة على ويندوز يبقى صالحًا بعد موتها، فلا تختفي
                    //      «الابنيّة» من تحت اليد كما تختفي على POSIX.
                    restoreProcess(id, pid);
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_WAIT_FAILED,
                              {{"handle", std::to_string(id)},
                               {"reason", ProcessDetail::winErrorText(winErr)}});
                }

                ::CloseHandle(hProc);
                // (AR) الرمزُ يُرجع خامًّا. اصطلاحُ `128+الإشارة` لا نظيرَ له: ويندوز
                //      لا يقتل بإشارات، والإنهاءُ القسريّ يظهر رمزًا كبيرًا مثل
                //      0xC000013A لا رقمًا صغيرًا يُجمع. وتحويلُه إلى الاصطلاح
                //      POSIX كان سيخترع إشارةً لم تقع — حدٌّ معلَنٌ في مصدر الحقيقة.
                // (EN) Raw exit code: Windows has no signal deaths, so the
                //      128+signal convention would invent a signal that never fired.
                return std::make_shared<Data::Value>(
                    static_cast<int64_t>(static_cast<uint32_t>(exitCode)));
#else
                int status = 0;
                int waitErrno = 0;
                bool waitFailed = false;
                while (::waitpid(static_cast<pid_t>(pid), &status, 0) < 0)
                {
                    if (errno == EINTR)
                        continue; // إشارةٌ أثناء الانتظار ليست فشلًا
                    waitErrno = errno;
                    waitFailed = true;
                    break;
                }

                ProcessDetail::restoreInterruptsAfterWait();

                if (waitFailed)
                {
                    if (waitErrno != ECHILD)
                    {
                        // (AR) فشلٌ عابر: المقبض ما يزال صالحًا فيُعاد إلى الجدول،
                        //      ويُبلَّغ برمزٍ يقول ذلك. كان يُبلَّغ سابقًا بـ«مجهول أو
                        //      سبق انتظاره» — وصفٌ كاذب لحالةٍ نُعيد فيها القيد فعلًا.
                        // (EN) Transient failure: the handle is restored and reported
                        //      as such, not as "unknown or already awaited".
                        restoreProcess(id, pid);
                        ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_WAIT_FAILED,
                                  {{"handle", std::to_string(id)},
                                   {"reason", std::strerror(waitErrno)}});
                    }
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_NOT_CHILD,
                              {{"handle", std::to_string(id)}});
                }

                // (AR) 128+الإشارة اصطلاحُ الصدَفات — كي يُميَّز القتلُ من الخروج العاديّ.
                int code = WIFEXITED(status)     ? WEXITSTATUS(status)
                           : WIFSIGNALED(status) ? 128 + WTERMSIG(status)
                                                 : -1;
                return std::make_shared<Data::Value>(static_cast<int64_t>(code));
#endif
            };
            interpreter.getFunctionManager().registerBuiltinFunction(
                std::string(Bpr::PROC_WAIT), wait_func);

            // ─────────────────────────────────────────────────────────────
            // انبوب / pipe
            // ─────────────────────────────────────────────────────────────
            auto pipe_func = [](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                int fds[2];
#ifdef _WIN32
                // (AR) `_O_NOINHERIT` نظيرُ CLOEXEC حرفًا بحرف: الطرفُ لا يُورَّث إلّا
                //      إن نُسخ نسخةً موروثةً صراحةً في خريطة التوجيه. ولولاه لبقي
                //      طرفُ الكتابة مفتوحًا في ابنٍ ثالثٍ فلا يرى القارئُ نهايةَ
                //      ملفّ — نفسُ التعلّق الذي يحرسه اختبار الأنبوب.
                //      و`_O_BINARY` شرطُ تطابق: الوضعُ النصّيّ يترجم «\n» إلى «\r\n»
                //      فيختلف ما يعبر الأنبوبَ عن نظيره POSIX بايتًا بايتًا.
                // (EN) _O_NOINHERIT is CLOEXEC exactly; _O_BINARY keeps the bytes
                //      crossing the pipe identical to the POSIX path.
                if (::_pipe(fds, 65536, _O_BINARY | _O_NOINHERIT) != 0)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_PIPE_FAILED,
                              {{"reason", std::strerror(errno)}});
#else
                if (!ProcessDetail::makeCloexecPipe(fds))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_PIPE_FAILED,
                              {{"reason", std::strerror(errno)}});
#endif

                Data::Value::MapType out;
                out["قراءة"] = Data::Value(static_cast<int64_t>(storeDescriptor(fds[0])));
                out["كتابة"] = Data::Value(static_cast<int64_t>(storeDescriptor(fds[1])));
                return std::make_shared<Data::Value>(std::move(out));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(
                std::string(Bpr::PROC_PIPE), pipe_func);

            // ─────────────────────────────────────────────────────────────
            // اغلق / close
            // ─────────────────────────────────────────────────────────────
            auto close_func = [](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);

                int64_t id = args[0]->toInt64();
                // (AR) فحصُ النوع قبل الجدول: تمريرُ مقبضِ عمليّةٍ هنا كان يُغلق
                //      وصفًا أجنبيًّا ويُرجع «صحيح».
                if (!ProcessDetail::isDescriptorHandle(id))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_HANDLE_KIND,
                              {{"handle", std::to_string(id)},
                               {"expected", "وصفًا من انبوب أو افتح_وصفا"},
                               {"found", ProcessDetail::handleKindName(id)}});
                int fd = -1;
                // (AR) السحب قبل الإغلاق: القيد يُحذف ذرّيًّا، فإغلاقان متزامنان لا
                //      ينجحان معًا — وإغلاقٌ مزدوجٌ قد يقتل وصفًا أعادت النواة استعماله.
                if (!takeDescriptor(id, fd))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_BAD_DESCRIPTOR,
                              {{"handle", std::to_string(id)}});

#ifdef _WIN32
                // (AR) لا EINTR على ويندوز — الحلقةُ هناك لأجله وحدَه.
                if (::_close(fd) != 0)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_CLOSE_FAILED,
                              {{"handle", std::to_string(id)},
                               {"reason", std::strerror(errno)}});
#else
                while (::close(fd) < 0)
                {
                    // (AR) EINTR على close: لا يُعاد النداء على لينكس — الوصف مُغلَقٌ
                    //      أصلًا وإعادةُ النداء قد تُغلق وصفًا آخر. يُعامَل نجاحًا.
                    if (errno == EINTR)
                        break;
                    // (AR) فشلٌ حقيقيّ من النواة (EIO على NFS مثلًا). كان يُبلَّغ
                    //      بـ«الوصف غير معروف أو مُغلَق سلفًا» — كذبٌ صريح: الوصف
                    //      كان معروفًا وقد سُحب للتوّ من الجدول.
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_CLOSE_FAILED,
                              {{"handle", std::to_string(id)},
                               {"reason", std::strerror(errno)}});
                }
#endif
                return std::make_shared<Data::Value>(true);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(
                std::string(Bpr::PROC_CLOSE), close_func);

            // ─────────────────────────────────────────────────────────────
            // افتح_وصفا / open
            // ─────────────────────────────────────────────────────────────
            auto open_func = [&interpreter](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args();
                if (interpreter.getOptions().enableSecurity)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                              {{"resource", "عمليات/افتح_وصفا"},
                               {"reason", "(AR) الوضع الآمن يمنع فتح الوصفات — وهو يُفعَّل براية --أمان، فالحلُّ إزالتُها"
                                          " / (EN) secure-mode blocks descriptor opening"}});
                }
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);

                const std::string path = args[0]->toString();
                const std::string mode = args[1]->toString();

                // (AR) الأعلامُ تُبنى بأسماءِ المنصّة لا بأرقامٍ منقولة، **والوضعُ
                //      مجموعةٌ مغلقةٌ واحدةٌ في المنصّتين**.
                // (EN) Flags are built from each platform's own names; the mode set
                //      itself is one closed set on both.
#ifdef _WIN32
                // (AR) ولماذا `CreateFileA` لا `_open` على ويندوز — وقد كانت `_open`
                //      أقصرَ وأقربَ إلى نظيرها POSIX:
                //
                //      لأنّ «إلحاق» في CRT ليست خاصّيّةَ **المقبض** بل مسلكًا تسلكه
                //      CRT عند كلّ كتابة (تقفزُ إلى النهايةِ ثمّ تكتب). ووصفاتُنا
                //      وُضعت لتُمرَّرَ إلى ابنٍ يكتبُ بالمقبضِ الخامِّ لا بـCRT، فيبدأ
                //      من الموضع صفر و**يدهسُ** ما في الملفّ. أي أنّ «إلحاق» كانت
                //      تعملُ عملَ «كتابة» بلا خطأٍ ولا تحذير — وهو بعينه الفرقُ بين
                //      `>>` و`>` الذي وُضع اختبارُ ١٨١ ليحرسه، ولولاه لمرّ الوسمُ
                //      «مدعوم» على عيبٍ يفقدُ بياناتِ المستعمل.
                //      و`FILE_APPEND_DATA` **بلا** `FILE_WRITE_DATA` يجعلُ الإلحاقَ
                //      خاصّيّةً في المقبضِ نفسِه: كلُّ كتابةٍ تقعُ في النهايةِ ذرّيًّا،
                //      أيًّا كان الكاتبُ وأيًّا كانت مكتبتُه — وهي دلالةُ `O_APPEND`
                //      حرفًا لا مقاربتُها.
                // (EN) Why CreateFileA and not _open: CRT append is a CRT behaviour
                //      (seek-to-end before each write), not a property of the handle.
                //      Our descriptors exist to be handed to a child that writes
                //      through the raw handle, which starts at offset 0 and
                //      overwrites — "append" silently behaving as "truncate".
                //      FILE_APPEND_DATA without FILE_WRITE_DATA puts the semantics
                //      in the handle, which is exactly what O_APPEND means.
                constexpr DWORD kGenericRead = 0x80000000u;
                constexpr DWORD kGenericWrite = 0x40000000u;
                constexpr DWORD kFileAppendData = 0x0004u;
                constexpr DWORD kSynchronize = 0x00100000u;
                // (AR) المشاركةُ تشمل الحذف: POSIX يسمح بفكِّ ارتباطِ ملفٍّ مفتوح،
                //      ومنعُه هنا كان سيجعل سلوكَ المنصّتين يفترقُ في الحذف لا الكتابة.
                constexpr DWORD kShareAll = 0x00000007u; // READ|WRITE|DELETE
                constexpr DWORD kCreateAlways = 2u;
                constexpr DWORD kOpenExisting = 3u;
                constexpr DWORD kOpenAlways = 4u;
                constexpr DWORD kAttrNormal = 0x00000080u;

                DWORD access = 0, disposition = 0;
                int osFlags = 0;
                if (mode == "قراءة")
                {
                    access = kGenericRead;
                    disposition = kOpenExisting;
                    osFlags = _O_RDONLY;
                }
                else if (mode == "كتابة")
                {
                    access = kGenericWrite;
                    disposition = kCreateAlways;
                }
                else if (mode == "إلحاق")
                {
                    access = kFileAppendData | kSynchronize;
                    disposition = kOpenAlways;
                    osFlags = _O_APPEND;
                }
                else
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_BAD_MODE, {{"mode", mode}});
                    return std::make_shared<Data::Value>(0.0);
                }

                // (AR) `lpSecurityAttributes = nullptr` ⇒ مقبضٌ غيرُ موروث — نظيرُ
                //      CLOEXEC. والتوريثُ انتقائيٌّ بنسخةٍ تُصنَعُ في خريطة التوجيه.
                HANDLE h = ::CreateFileW(ProcessDetail::widen(path).c_str(), access, kShareAll,
                                         nullptr, disposition, kAttrNormal, nullptr);
                if (h == INVALID_HANDLE_VALUE)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_OPEN_FAILED,
                              {{"path", path},
                               {"reason", ProcessDetail::winErrorText(::GetLastError())}});

                // (AR) لا `_O_BINARY` هنا: `_open_osfhandle` ثنائيٌّ ما لم يُطلَب
                //      `_O_TEXT` صراحةً، وتمريرُ علمٍ لا تقبله يُفسدُ التأويل.
                int fd = ::_open_osfhandle(reinterpret_cast<intptr_t>(h), osFlags);
                if (fd < 0)
                {
                    ::CloseHandle(h);
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_OPEN_FAILED,
                              {{"path", path}, {"reason", std::strerror(errno)}});
                }
#else
                int flags;
                if (mode == "قراءة")
                    flags = O_RDONLY | O_CLOEXEC;
                else if (mode == "كتابة")
                    flags = O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC;
                else if (mode == "إلحاق")
                    flags = O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC;
                else
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_BAD_MODE, {{"mode", mode}});
                    return std::make_shared<Data::Value>(0.0);
                }

                int fd;
                while ((fd = ::open(path.c_str(), flags, 0644)) < 0)
                {
                    if (errno == EINTR)
                        continue;
                    ctx.error(::Sad::Errors::ErrorCode::RUN_PROC_OPEN_FAILED,
                              {{"path", path}, {"reason", std::strerror(errno)}});
                }
#endif
                return std::make_shared<Data::Value>(
                    static_cast<int64_t>(storeDescriptor(fd)));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(
                std::string(Bpr::PROC_OPEN_FD), open_func);
        }
    } // namespace Interpreter
} // namespace Sad
