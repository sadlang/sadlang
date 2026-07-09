// بسم الله الرحمن الرحيم
/**
 * @file shell_lexer.h
 * @brief (AR) مُحلِّل سطر صدَفة ص لأمر ‹:شغّل› — تقسيم إلى مراحل أنبوب، كلّ مرحلة argv،
 *        مع احترام الاقتباس المفرد/المزدوج والهروب بـ‹\›. لا توسيع متغيّرات (شريحة لاحقة).
 * @brief (EN) Shell-line lexer for the ص ‹:run› command — splits into pipeline stages,
 *        each an argv, honoring single/double quotes and ‹\› escaping. No variable
 *        expansion (a later slice).
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
    EmptyStage          ///< مرحلة أنبوب فارغة (| بلا أمر أحد طرفيه) / empty pipeline stage
};

// (AR) نتيجة التحليل: مراحل الأنبوب (كلّ مرحلة argv غير فارغة عند Ok) وحالة النجاح/الخطأ.
//      عند status != Ok تكون stages غير ذات دلالة.
// (EN) Parse result: the pipeline stages (each a non-empty argv when Ok) and the status.
//      When status != Ok, stages is meaningless.
struct ShellPipeline
{
    std::vector<std::vector<std::string>> stages;
    ShellParseStatus status;
};

// (AR) يحلّل السطر الخام (ما بعد ‹:شغّل ›) إلى مراحل أنبوب. القواعد (صدَفة مبسّطة):
//        • المسافة/الجدولة تفصل الوسائط.
//        • "…" اقتباس مزدوج: حرفيّ، و‹\› يهرّب فقط ‹"› و‹\›.
//        • '…' اقتباس مفرد: حرفيّ تماماً، بلا هروب.
//        • خارج الاقتباس: ‹\c› يجعل c حرفيّاً؛ ‹|› غير المقتبَس يفصل مراحل الأنبوب.
//        • المقاطع المتجاورة (مقتبَسة أو لا) تلتحم في وسيطٍ واحد (مثل a"b c" → «ab c»).
//      يعيد ShellPipeline؛ الاستدعاء يعالج الحالة قبل استعمال stages.
// (EN) Parses the raw line (after ‹:run ›) into pipeline stages. Rules (shell-lite):
//      whitespace splits args; "…" literal with ‹\› escaping only ‹"›/‹\›; '…' fully
//      literal; unquoted ‹\c› escapes c; unquoted ‹|› splits stages; adjacent segments
//      concatenate into one arg. Caller checks status before using stages.
ShellPipeline parseShellPipeline(const std::string& raw);

} // namespace REPL
} // namespace Sad

#endif // SAD_REPL_SHELL_LEXER_H
