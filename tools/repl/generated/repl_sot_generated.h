// بسم الله الرحمن الرحيم
// repl_sot_generated.h — كتالوج «مصدر حقيقة الأدوات» المولَّد آلياً / auto-generated Tool-SoT catalog
// (AR) ⚠️ لا تُعدِّل يدوياً — عدِّل language-truth/tools/repl/*.yaml ثمّ أعد التوليد (x.py gen).
// (EN) ⚠️ DO NOT EDIT — modify language-truth/tools/repl/*.yaml then rebuild (x.py gen).
// (AR) المصدر: _meta.yaml + errors.yaml + messages.yaml + commands.yaml + applets.yaml + calendar.yaml
// (EN) Source: _meta.yaml + errors.yaml + messages.yaml + commands.yaml + applets.yaml + calendar.yaml

#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace Sad {
namespace REPL {
namespace SoT {

// ── الهوية / Meta ──
inline constexpr const char *kToolId = "repl";
inline constexpr const char *kVersion = "1.0.0";
inline constexpr const char *kDisplayNameAr = "لغة ص - REPL";
inline constexpr const char *kDisplayNameEn = "Sad Language REPL";
inline constexpr const char *kBilingualSeparator = " / ";
inline constexpr const char *kDetailSeparator = ": ";

// ── الأخطاء / Errors ──
enum class Error {
    SYNTAX,
    EVAL,
    FILE_OPEN,
    INTERNAL,
    UNKNOWN_COMMAND,
    NOT_AN_EXPRESSION,
    EMPTY_EXPRESSION,
    UNKNOWN_OPTION,
    LOAD_FAILED,
    FATAL,
    RUN_FAILED,
    SHELL_UNTERMINATED_QUOTE,
    SHELL_EMPTY_STAGE,
    PIPE_UNSUPPORTED,
    REDIRECT_NO_TARGET,
    REDIRECT_FAILED,
    REDIRECT_UNSUPPORTED,
    CD_FAILED,
    ENV_INVALID_NAME
};

enum class Severity { Error, Warning, Info };

/// (AR) إدخال خطأٍ مولَّد — نصوص ثنائيّة اللغة (nullptr للنصّ الغائب).
struct ErrorEntry {
    Error code;
    const char *id;
    Severity severity;
    const char *titleAr;
    const char *titleEn;
    const char *briefAr;
    const char *briefEn;
    const char *hintAr;
    const char *hintEn;
    bool hasDetail;
};

/// (AR) بحث عن إدخال خطأ — يُرجع nullptr إن لم يوجد.
const ErrorEntry *findError(Error code);

/// (AR) رسالة ثنائيّة اللغة: «briefAr / briefEn» + «: detail» إن وُجد تفصيل.
std::string errorMessage(Error code, std::string_view detail = {});

// ── الرسائل / Messages ──
enum class Message {
    VERSION_LABEL,
    HELP_HINT,
    GOODBYE,
    RESET_DONE,
    COMMANDS_HEADER,
    VARS_HEADER,
    FUNCS_HEADER,
    HISTORY_HEADER,
    USAGE_LABEL,
    HELP_DELEGATE_LABEL,
    HELP_NO_DESC,
    NO_VARS,
    NO_FUNCS,
    NO_HISTORY,
    NO_INTERPRETER,
    CLI_OPTIONS_HEADER,
    CLI_SPECIAL_COMMANDS_HEADER,
    CLI_EXAMPLES_HEADER,
    CLI_OPT_HELP,
    CLI_OPT_VERSION,
    CLI_OPT_NO_COLOR,
    CLI_OPT_NO_HISTORY,
    CLI_OPT_HISTORY_FILE,
    CLI_OPT_LOAD,
    CLI_OPT_RICH
};

struct MessageEntry {
    Message key;
    const char *ar;
    const char *en;
};

const MessageEntry *findMessage(Message key);
std::string_view messageAr(Message key);
std::string_view messageEn(Message key);
/// (AR) «ar / en» على سطرٍ واحد.
std::string messageBoth(Message key);

// ── الأوامر / Commands ──
enum class CommandHandler {
    HELP,
    EXIT,
    CLEAR,
    TYPE,
    LOAD,
    HISTORY,
    RESET,
    VARS,
    FUNCS,
    RUN,
    ENV,
    CD
};

struct CommandEntry {
    CommandHandler handler;
    const char *name;
    const char *arabicName;
    const char *descAr;
    const char *descEn;
    const char *usage;
};

extern const CommandEntry kCommands[];
extern const std::size_t kCommandsCount;

// ── معجم الأوامر العربيّة / Arabic applet lexicon ──
// (AR) اسمٌ عربيّ صريح → برنامج التنفيذ الحقيقيّ؛ يترجمه الموزِّع قبل execvp.
//      descAr (تعريب ١٠ — شريحة الإغلاق): وصف عربيّ فصيح بسطر واحد يعرضه
//      «:مساعدة اسم» — غير فارغ دائمًا: التغطية اكتملت 272/272 والمخطّط يفرض
//      «وصف» لكلّ آبلت (بقي النوع مؤشّرًا والمستهلكون يدافعون عن nullptr عمقًا).
// (EN) an explicit Arabic name → the real exec program; the dispatcher translates before execvp.
//      descAr: one-line Arabic help description shown by «:مساعدة name» — always
//      non-null now: full coverage reached and the schema requires it (consumers keep null-guards as defense).
struct AppletEntry {
    const char *arabic;
    const char *exec;
    const char *descAr;
};

extern const AppletEntry kApplets[];
extern const std::size_t kAppletsCount;

/// (AR) يترجم اسمًا عربيًّا صريحًا إلى برنامج التنفيذ؛ nullptr إن لم يُعرَّف (فيبقى الاسم كما هو).
/// (EN) translates an explicit Arabic name to its exec program; nullptr if undefined (kept as-is).
const char *appletExec(std::string_view arabic);

/// (AR) يعيد مدخل المعجم كاملًا لاسمٍ عربيّ صريح (لقراءة descAr)؛ nullptr إن لم يُعرَّف.
/// (EN) full lexicon entry for an explicit Arabic name (to read descAr); nullptr if undefined.
const AppletEntry *findApplet(std::string_view arabic);

// ── التقويم / Calendar ──
// (AR) الشريحة الأولى من «تعريب ٨»: أسماء الأيّام السبعة فقط (calendar.yaml).
//      التوسّع (أشهر/هجريّ/أرقام مشرقيّة/منطقة زمنيّة) قرار مالك معلَّق — لا يُوسَّع هنا.
// (EN) First slice: the seven Arabic weekday names only; months/Hijri are pending owner decisions.

/// (AR) الاسم العربيّ القانونيّ لآبلت التاريخ — يلتقطه الموزِّع بلا وسائط كأمر داخليّ.
/// (EN) canonical Arabic date-applet name — intercepted argument-less as a builtin.
inline constexpr const char *kDateAppletArabic = "التاريخ";

/// (AR) صيغة printf لسطر التاريخ العربيّ: %s اسم اليوم ثمّ سنة/شهر/يوم/ساعة/دقيقة/ثانية.
/// (EN) printf format of the Arabic date line: %s weekday then y/m/d h:m:s integers.
inline constexpr const char *kDateLineFormat = "%s %04d-%02d-%02d، الساعة %02d:%02d:%02d";

/// (AR) أسماء الأيّام مرتّبة بترتيب tm_wday في POSIX: 0=الأحد … 6=السبت.
/// (EN) weekday names ordered by POSIX tm_wday: 0=Sunday … 6=Saturday.
extern const char *const kWeekdays[];
extern const std::size_t kWeekdaysCount;

/// (AR) اسم اليوم لفهرس tm_wday؛ nullptr خارج المدى [0،6].
/// (EN) weekday name for a tm_wday index; nullptr outside [0,6].
const char *weekdayName(int tmWday);

} // namespace SoT
} // namespace REPL
} // namespace Sad
