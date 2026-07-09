// بسم الله الرحمن الرحيم
// repl_sot_generated.cpp — كتالوج «مصدر حقيقة الأدوات» المولَّد آلياً / auto-generated Tool-SoT catalog
// (AR) ⚠️ لا تُعدِّل يدوياً — عدِّل language-truth/tools/repl/*.yaml ثمّ أعد التوليد.
// (EN) ⚠️ DO NOT EDIT — modify language-truth/tools/repl/*.yaml then rebuild.

#include "repl_sot_generated.h"

namespace Sad {
namespace REPL {
namespace SoT {

static const ErrorEntry kErrors[] = {
    { Error::SYNTAX, "REPL001", Severity::Error,
      "خطأ نحويّ", "Syntax Error",
      "خطأ نحويّ", "Syntax Error",
      nullptr, nullptr,
      true },
    { Error::EVAL, "REPL002", Severity::Error,
      "خطأ تنفيذ", "Error",
      "خطأ", "Error",
      nullptr, nullptr,
      true },
    { Error::FILE_OPEN, "REPL003", Severity::Error,
      "تعذّر فتح الملفّ", "Failed to open file",
      "فشل فتح الملفّ", "Failed to open file",
      "تأكّد من صحّة المسار ووجود الملفّ وصلاحيّة القراءة", "Check the path exists and is readable",
      true },
    { Error::INTERNAL, "REPL004", Severity::Error,
      "خطأ داخليّ في الصدَفة", "Internal shell error",
      "خطأ داخليّ", "Internal error",
      "ليست علّة في برنامجك — يُرجى الإبلاغ عن سلوك الأداة", "Not a bug in your program — please report the tool behavior",
      true },
    { Error::UNKNOWN_COMMAND, "REPL005", Severity::Error,
      "أمر غير معروف", "Unknown command",
      "أمر غير معروف", "Unknown command",
      "اكتب ':help' لعرض الأوامر المتاحة", "Type ':help' to list available commands",
      true },
    { Error::NOT_AN_EXPRESSION, "REPL006", Severity::Error,
      "ليس تعبيراً يُقيَّم", "Not an evaluable expression",
      "ليس تعبيراً يُقيَّم", "Not an evaluable expression",
      nullptr, nullptr,
      false },
    { Error::EMPTY_EXPRESSION, "REPL007", Severity::Error,
      "تعبير فارغ", "Empty expression",
      "تعبير فارغ", "Empty expression",
      nullptr, nullptr,
      false },
    { Error::UNKNOWN_OPTION, "REPL008", Severity::Error,
      "خيار غير معروف", "Unknown option",
      "خيار غير معروف", "Unknown option",
      "استخدم --help للمساعدة", "Use --help for help",
      true },
    { Error::LOAD_FAILED, "REPL009", Severity::Error,
      "تعذّر تحميل الملفّ", "Failed to load file",
      "فشل تحميل الملفّ", "Failed to load file",
      nullptr, nullptr,
      true },
    { Error::FATAL, "REPL010", Severity::Error,
      "خطأ فادح", "Fatal error",
      "خطأ فادح", "Fatal error",
      nullptr, nullptr,
      true },
};
static const std::size_t kErrorsCount = sizeof(kErrors) / sizeof(kErrors[0]);

const ErrorEntry *findError(Error code) {
    for (std::size_t i = 0; i < kErrorsCount; ++i) {
        if (kErrors[i].code == code) return &kErrors[i];
    }
    return nullptr;
}

std::string errorMessage(Error code, std::string_view detail) {
    const ErrorEntry *e = findError(code);
    if (!e) return std::string(detail);
    std::string out = std::string(e->briefAr) + kBilingualSeparator + e->briefEn;
    if (e->hasDetail && !detail.empty()) {
        out += std::string(kDetailSeparator) + std::string(detail);
    }
    return out;
}

static const MessageEntry kMessages[] = {
    { Message::VERSION_LABEL, "الإصدار", "Version" },
    { Message::HELP_HINT, "اكتب ':help' للمساعدة أو ':exit' للخروج", "Type ':help' for help or ':exit' to quit" },
    { Message::GOODBYE, "وداعاً!", "Goodbye!" },
    { Message::RESET_DONE, "تم إعادة تعيين حالة REPL", "REPL state reset." },
    { Message::COMMANDS_HEADER, "الأوامر المتاحة", "Available Commands" },
    { Message::VARS_HEADER, "المتغيّرات", "Variables" },
    { Message::FUNCS_HEADER, "الدوال", "Functions" },
    { Message::HISTORY_HEADER, "تاريخ الأوامر", "Command History" },
    { Message::USAGE_LABEL, "الاستخدام", "Usage" },
    { Message::NO_VARS, "لا متغيّرات معرّفة", "No variables defined." },
    { Message::NO_FUNCS, "لا دوال معرّفة من المستخدم", "No user-defined functions." },
    { Message::NO_HISTORY, "لا تاريخ بعد", "No history yet." },
    { Message::NO_INTERPRETER, "لا مفسّر", "No interpreter." },
    { Message::CLI_OPTIONS_HEADER, "الخيارات", "Options" },
    { Message::CLI_SPECIAL_COMMANDS_HEADER, "الأوامر الخاصة", "Special Commands" },
    { Message::CLI_EXAMPLES_HEADER, "أمثلة", "Examples" },
    { Message::CLI_OPT_HELP, "عرض هذه المساعدة", "Show this help" },
    { Message::CLI_OPT_VERSION, "عرض رقم الإصدار", "Show version" },
    { Message::CLI_OPT_NO_COLOR, "تعطيل الألوان", "Disable colors" },
    { Message::CLI_OPT_NO_HISTORY, "تعطيل التاريخ", "Disable history" },
    { Message::CLI_OPT_HISTORY_FILE, "ملف التاريخ", "History file" },
    { Message::CLI_OPT_LOAD, "تحميل ملف عند البدء", "Load file on start" },
    { Message::CLI_OPT_RICH, "إدخال تفاعليّ غنيّ: أسهم/Tab/تحرير (طرفيّة فقط)", "Rich interactive input: arrows/Tab/editing (TTY only)" },
};
static const std::size_t kMessagesCount = sizeof(kMessages) / sizeof(kMessages[0]);

const MessageEntry *findMessage(Message key) {
    for (std::size_t i = 0; i < kMessagesCount; ++i) {
        if (kMessages[i].key == key) return &kMessages[i];
    }
    return nullptr;
}

std::string_view messageAr(Message key) {
    const MessageEntry *m = findMessage(key);
    return m ? std::string_view(m->ar) : std::string_view();
}

std::string_view messageEn(Message key) {
    const MessageEntry *m = findMessage(key);
    return m ? std::string_view(m->en) : std::string_view();
}

std::string messageBoth(Message key) {
    const MessageEntry *m = findMessage(key);
    if (!m) return std::string();
    return std::string(m->ar) + kBilingualSeparator + m->en;
}

const CommandEntry kCommands[] = {
    { CommandHandler::HELP, "help", "مساعدة",
      "عرض المساعدة", "Show help", ":help" },
    { CommandHandler::EXIT, "exit", "خروج",
      "الخروج من REPL", "Exit REPL", ":exit" },
    { CommandHandler::EXIT, "quit", "إنهاء",
      "الخروج من REPL", "Exit REPL", ":quit" },
    { CommandHandler::CLEAR, "clear", "مسح",
      "مسح الشاشة", "Clear screen", ":clear" },
    { CommandHandler::TYPE, "type", "نوع",
      "عرض نوع التعبير", "Show expression type", ":type <expression>" },
    { CommandHandler::LOAD, "load", "حمل",
      "تحميل وتنفيذ ملفّ", "Load and execute file", ":load <filename>" },
    { CommandHandler::HISTORY, "history", "تاريخ",
      "عرض تاريخ الأوامر", "Show command history", ":history" },
    { CommandHandler::RESET, "reset", "إعادة",
      "إعادة تعيين حالة REPL", "Reset REPL state", ":reset" },
    { CommandHandler::VARS, "vars", "متغيرات",
      "عرض جميع المتغيّرات", "Show all variables", ":vars" },
    { CommandHandler::FUNCS, "funcs", "دوال",
      "عرض جميع الدوال", "Show all functions", ":funcs" },
};
const std::size_t kCommandsCount = sizeof(kCommands) / sizeof(kCommands[0]);

} // namespace SoT
} // namespace REPL
} // namespace Sad
