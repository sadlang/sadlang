// بسم الله الرحمن الرحيم
// repl_sot_generated.h — كتالوج «مصدر حقيقة الأدوات» المولَّد آلياً / auto-generated Tool-SoT catalog
// (AR) ⚠️ لا تُعدِّل يدوياً — عدِّل language-truth/tools/repl/*.yaml ثمّ أعد التوليد (x.py gen).
// (EN) ⚠️ DO NOT EDIT — modify language-truth/tools/repl/*.yaml then rebuild (x.py gen).
// (AR) المصدر: _meta.yaml + errors.yaml + messages.yaml + commands.yaml
// (EN) Source: _meta.yaml + errors.yaml + messages.yaml + commands.yaml

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
    ENV
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
// (EN) an explicit Arabic name → the real exec program; the dispatcher translates before execvp.
struct AppletEntry {
    const char *arabic;
    const char *exec;
};

extern const AppletEntry kApplets[];
extern const std::size_t kAppletsCount;

/// (AR) يترجم اسمًا عربيًّا صريحًا إلى برنامج التنفيذ؛ nullptr إن لم يُعرَّف (فيبقى الاسم كما هو).
/// (EN) translates an explicit Arabic name to its exec program; nullptr if undefined (kept as-is).
const char *appletExec(std::string_view arabic);

} // namespace SoT
} // namespace REPL
} // namespace Sad
