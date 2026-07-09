// بسم الله الرحمن الرحيم
/**
 * @file shell_lexer.h
 * @brief (AR) مُحلِّل سطر صدَفة ص لأمر ‹:شغّل› — تقسيم إلى مراحل أنبوب، كلّ مرحلة argv مع
 *        إعادة توجيه اختياريّة (‹<›/‹>›/‹>>›/‹2>›/‹2>>›/‹&>›/‹&>>›/‹2>&1›) وتوسيع متغيّرات
 *        (‹$VAR›/‹${VAR}›)، محترمًا الاقتباس والهروب.
 * @brief (EN) Shell-line lexer for the ص ‹:run› command — splits into pipeline stages, each
 *        an argv with optional redirection (‹<›/‹>›/‹>>›/‹2>›/‹2>>›/‹&>›/‹&>>›/‹2>&1›) and
 *        variable expansion (‹$VAR›/‹${VAR}›), honoring quotes and escaping.
 */
#ifndef SAD_REPL_SHELL_LEXER_H
#define SAD_REPL_SHELL_LEXER_H

#include <functional>
#include <string>
#include <vector>

namespace Sad
{
namespace REPL
{

// (AR) مُحلِّل متغيّر بيئة: يُعطى اسم المتغيّر ويُعيد قيمته (سلسلة فارغة إن لم يُعرَّف). يفصل
//      المُحلِّل عن مصدر البيئة (process environ) فيسهل اختباره. / (EN) an environment resolver:
//      given a variable name, returns its value (empty if unset). Decouples the lexer from the
//      process environ, easing testing.
using EnvResolver = std::function<std::string(const std::string&)>;

// (AR) حالة تحليل سطر الصدَفة. Ok = نجح؛ الباقي أخطاء تُترجَم لرموز كتالوج SoT.
// (EN) Shell-line parse status. Ok = success; the rest map to SoT catalog error codes.
enum class ShellParseStatus
{
    Ok,                 ///< تحليل ناجح / parsed successfully
    UnterminatedQuote,  ///< اقتباس " أو ' لم يُغلَق / a " or ' quote was never closed
    EmptyStage,         ///< مرحلة أنبوب بلا أمر (| بلا طرف، أو إعادة توجيه بلا برنامج)
    RedirNoTarget       ///< مُشغِّل إعادة توجيه (< > >>) بلا اسم ملفٍّ بعده / redirection with no target
};

// (AR) مرحلة أنبوب واحدة: البرنامج ومعطياته + إعادة توجيه اختياريّة. inFile فارغة = ترث
//      stdin (المرحلة السابقة أو صدَفة الأصل)؛ outFile فارغة = ترث stdout. appendOut يميّز
//      ‹>>› (إلحاق) عن ‹>› (بتر). errFile هدفُ ‹2›/‹2>>› (خطأ قياسيّ). errToOut يجعل الخطأ
//      يتبع الإخراج القياسيّ في وجهته النهائيّة (‹2>&1› و‹&>›). آخر إعادة توجيه من نوعها تفوز
//      (كصدَفة)؛ وضبط ملفِّ خطأٍ يُلغي errToOut والعكس (حصريّة متبادلة).
// (EN) A single pipeline stage: the program+args plus optional redirection. Empty inFile =
//      inherit stdin (previous stage or the parent shell); empty outFile = inherit stdout.
//      appendOut distinguishes ‹>>› (append) from ‹>› (truncate). errFile is the ‹2>›/‹2>>›
//      (stderr) target. errToOut makes stderr follow stdout's *final* destination (‹2>&1› and
//      ‹&>›). Last redirection of a kind wins (shell-like); setting a stderr file clears
//      errToOut and vice-versa (mutually exclusive).
struct ShellStage
{
    std::vector<std::string> argv;
    std::string inFile;         ///< ‹< ملف› / stdin source file (empty = none)
    std::string outFile;        ///< ‹> ملف› أو ‹>> ملف› / stdout target file (empty = none)
    bool appendOut = false;     ///< true حين ‹>>› / true for ‹>>› append
    std::string errFile;        ///< ‹2> ملف› أو ‹2>> ملف› / stderr target file (empty = none)
    bool appendErr = false;     ///< true حين ‹2>>› / true for ‹2>>› append
    bool errToOut = false;      ///< ‹2>&1›/‹&>›: الخطأ يتبع الإخراج / stderr follows stdout
};

// (AR) نتيجة التحليل: مراحل الأنبوب (كلّ مرحلة argv غير فارغة عند Ok) وحالة النجاح/الخطأ.
//      عند status != Ok تكون stages غير ذات دلالة.
// (EN) Parse result: the pipeline stages (each has a non-empty argv when Ok) and the status.
//      When status != Ok, stages is meaningless.
struct ShellPipeline
{
    std::vector<ShellStage> stages;
    ShellParseStatus status;
};

// (AR) يحلّل السطر الخام (ما بعد ‹:شغّل ›) إلى مراحل أنبوب. القواعد (صدَفة مبسّطة):
//        • المسافة/الجدولة/CR/LF تفصل الوسائط.
//        • "…" اقتباس مزدوج: حرفيّ، و‹\› يهرّب فقط ‹"› و‹\›.
//        • '…' اقتباس مفرد: حرفيّ تماماً، بلا هروب.
//        • خارج الاقتباس: ‹\c› يجعل c حرفيّاً؛ ‹|› غير المقتبَس يفصل المراحل؛ ‹<›/‹>›/‹>>›
//          غير المقتبَسة إعادةُ توجيه (إدخال/إخراج)، والوسيط التالي هدفُها. وعند بداية وسيطٍ
//          تُتعرَّف صيَغ الخطأ: ‹2>›/‹2>>› (خطأ لملفّ)، ‹&>›/‹&>>› (إخراج+خطأ لملفّ)، ‹2>&1›
//          (الخطأ يتبع الإخراج)؛ مِقبضُ fd في موضع هدفٍ يُعامَل اسمَ ملفٍّ حرفيًّا.
//        • المقاطع المتجاورة (مقتبَسة أو لا) تلتحم في وسيطٍ واحد (مثل a"b c" → «ab c»).
//      يعيد ShellPipeline؛ الاستدعاء يعالج الحالة قبل استعمال stages.
// (EN) Parses the raw line (after ‹:run ›) into pipeline stages. Rules (shell-lite):
//      whitespace/CR/LF split args; "…" literal with ‹\› escaping only ‹"›/‹\›; '…' fully
//      literal; unquoted ‹\c› escapes c; unquoted ‹|› splits stages; unquoted ‹<›/‹>›/‹>>›
//      redirect (stdin/stdout), the next token being the target; at a token start the stderr
//      forms are also recognized: ‹2>›/‹2>>› (stderr to file), ‹&>›/‹&>>› (stdout+stderr to
//      file), ‹2>&1› (stderr follows stdout); an fd digit in target position is a literal
//      filename. Adjacent segments concatenate into one arg. Caller checks status before using
//      stages. `env` يُوسِّع ‹$VAR›/‹${VAR}› والوسيطين الخاصّين ‹$?›/‹$$› (يُمرَّران ‹?›/‹$›)
//      خارج الاقتباس المفرد (‹'…'› حرفيّ)؛ متغيّرٌ غير مُعرَّف ⇒ سلسلة فارغة (لا تُعاد قسمتها).
//      / `env` expands ‹$VAR›/‹${VAR}› plus the special params ‹$?›/‹$$› (passed as ‹?›/‹$›)
//      outside single quotes (‹'…'› is literal); an unset var ⇒ empty string (no re-splitting).
ShellPipeline parseShellPipeline(const std::string& raw, const EnvResolver& env);

// (AR) يوسّع مراجع ‹$VAR›/‹${VAR}› (والوسيطين ‹$?›/‹$$›) في نصٍّ حرٍّ (بلا وعيٍ بالاقتباس) —
//      لقيمة أمر ‹:بيئة›. اسم المتغيّر: حرفٌ/‹_›/بايت ≥0x80 (يدعم أسماء عربيّة) ثمّ أرقام/حروف/≥0x80.
// (EN) expands ‹$VAR›/‹${VAR}› (and ‹$?›/‹$$›) references in free text (quote-agnostic) — for
//      the ‹:env› value. A name is: letter/‹_›/byte ≥0x80 (Arabic allowed) then alnum/‹_›/≥0x80.
std::string expandEnvVars(const std::string& text, const EnvResolver& env);

} // namespace REPL
} // namespace Sad

#endif // SAD_REPL_SHELL_LEXER_H
