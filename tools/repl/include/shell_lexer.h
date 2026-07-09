// بسم الله الرحمن الرحيم
/**
 * @file shell_lexer.h
 * @brief (AR) مُحلِّل سطر صدَفة ص لأمر ‹:شغّل› — تقسيم إلى مراحل أنبوب، كلّ مرحلة argv مع
 *        إعادة توجيه اختياريّة (‹<›/‹>›/‹>>›)، محترمًا الاقتباس والهروب. لا توسيع متغيّرات
 *        (شريحة لاحقة).
 * @brief (EN) Shell-line lexer for the ص ‹:run› command — splits into pipeline stages, each
 *        an argv with optional redirection (‹<›/‹>›/‹>>›), honoring quotes and escaping.
 *        No variable expansion (a later slice).
 */
#ifndef SAD_REPL_SHELL_LEXER_H
#define SAD_REPL_SHELL_LEXER_H

#include <string>
#include <vector>

namespace Sad
{
namespace REPL
{

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
//      ‹>>› (إلحاق) عن ‹>› (بتر). آخر إعادة توجيه من نوعها تفوز (كصدَفة).
// (EN) A single pipeline stage: the program+args plus optional redirection. Empty inFile =
//      inherit stdin (previous stage or the parent shell); empty outFile = inherit stdout.
//      appendOut distinguishes ‹>>› (append) from ‹>› (truncate). Last redirection of a
//      kind wins (shell-like).
struct ShellStage
{
    std::vector<std::string> argv;
    std::string inFile;         ///< ‹< ملف› / stdin source file (empty = none)
    std::string outFile;        ///< ‹> ملف› أو ‹>> ملف› / stdout target file (empty = none)
    bool appendOut = false;     ///< true حين ‹>>› / true for ‹>>› append
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
//          غير المقتبَسة إعادةُ توجيه، والوسيط التالي هدفُها.
//        • المقاطع المتجاورة (مقتبَسة أو لا) تلتحم في وسيطٍ واحد (مثل a"b c" → «ab c»).
//      يعيد ShellPipeline؛ الاستدعاء يعالج الحالة قبل استعمال stages.
// (EN) Parses the raw line (after ‹:run ›) into pipeline stages. Rules (shell-lite):
//      whitespace/CR/LF split args; "…" literal with ‹\› escaping only ‹"›/‹\›; '…' fully
//      literal; unquoted ‹\c› escapes c; unquoted ‹|› splits stages; unquoted ‹<›/‹>›/‹>>›
//      redirect, the next token being the target; adjacent segments concatenate into one
//      arg. Caller checks status before using stages.
ShellPipeline parseShellPipeline(const std::string& raw);

} // namespace REPL
} // namespace Sad

#endif // SAD_REPL_SHELL_LEXER_H
