// بسم الله الرحمن الرحيم
/**
 * @file shell_lexer.cpp
 * @brief (AR) تطبيق مُحلِّل سطر صدَفة ص (اقتباسات + هروب + أنابيب + إعادة توجيه < > >>).
 * @brief (EN) ص shell-line lexer (quotes + escaping + pipes + redirection < > >>).
 */
#include "shell_lexer.h"

namespace Sad
{
namespace REPL
{

namespace
{
// (AR) محدّدات نحويّة غير مقتبَسة / (EN) unquoted syntactic delimiters
constexpr char kPipe = '|';
constexpr char kEscape = '\\';
constexpr char kDoubleQuote = '"';
constexpr char kSingleQuote = '\'';
constexpr char kRedirOut = '>';
constexpr char kRedirIn = '<';

// (AR) فراغٌ يفصل الوسائط: مسافة/جدولة + CR/LF (كي لا يخلق سطرٌ منتهٍ بـCRLF وسيطًا
//      زائفًا «\r» — مهمّ حين يُسحب الإدخال على Windows). / (EN) arg-separating whitespace:
//      space/tab + CR/LF (so a CRLF-terminated line never yields a bogus "\r" arg).
inline bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }

// (AR) وجهةُ الوسيط التالي: وسيطٌ عاديّ أم هدفُ إعادة توجيه (وأيّ نوع). / (EN) where the next
//      completed token goes: a normal arg, or a redirection target (and which kind).
enum class Pending
{
    Arg,
    RedirIn,
    RedirOutTrunc,
    RedirOutAppend
};
} // namespace

ShellPipeline parseShellPipeline(const std::string& raw)
{
    ShellPipeline result;
    result.status = ShellParseStatus::Ok;

    ShellStage curStage;   // (AR) المرحلة الجارية / current stage
    std::string curTok;    // (AR) الوسيط الجاري بناؤه / token being built
    bool hasTok = false;   // (AR) بدأ وسيطٌ (يسمح بوسيطٍ مقتبَسٍ فارغ "") / a token has begun
    Pending pending = Pending::Arg;

    enum class State
    {
        Normal,
        InSingle,
        InDouble
    } state = State::Normal;

    // (AR) ينهي الوسيط الجاري: يوجّهه إمّا لـargv أو لهدف إعادة توجيه (حسب pending). يعيد
    //      false ويضبط الحالة إن كان الهدف اقتباسًا فارغًا (‹> ""›) — وسيطٌ فارغٌ صالح لكن
    //      ملفَّ توجيهٍ فارغٌ لا. / (EN) finalize the current token: route to argv or a
    //      redirection target. Returns false (and sets status) if a redirection target is an
    //      empty quote (‹> ""›) — an empty arg is valid, an empty redirection filename is not.
    auto endToken = [&]() -> bool
    {
        if (!hasTok)
        {
            return true;
        }
        if (pending != Pending::Arg && curTok.empty())
        {
            result.status = ShellParseStatus::RedirNoTarget;
            return false;
        }
        switch (pending)
        {
            case Pending::Arg:
                curStage.argv.push_back(curTok);
                break;
            case Pending::RedirIn:
                curStage.inFile = curTok;
                break;
            case Pending::RedirOutTrunc:
                curStage.outFile = curTok;
                curStage.appendOut = false;
                break;
            case Pending::RedirOutAppend:
                curStage.outFile = curTok;
                curStage.appendOut = true;
                break;
        }
        pending = Pending::Arg;
        curTok.clear();
        hasTok = false;
        return true;
    };

    const std::size_t n = raw.size();
    for (std::size_t i = 0; i < n; ++i)
    {
        char c = raw[i];

        if (state == State::InSingle)
        {
            if (c == kSingleQuote)
            {
                state = State::Normal;
            }
            else
            {
                curTok.push_back(c);
            }
            continue;
        }

        if (state == State::InDouble)
        {
            if (c == kEscape && i + 1 < n && (raw[i + 1] == kDoubleQuote || raw[i + 1] == kEscape))
            {
                curTok.push_back(raw[++i]);
            }
            else if (c == kDoubleQuote)
            {
                state = State::Normal;
            }
            else
            {
                curTok.push_back(c);
            }
            continue;
        }

        // ── الحالة العاديّة / Normal state ──
        if (c == kEscape)
        {
            if (i + 1 < n)
            {
                curTok.push_back(raw[++i]);
            }
            else
            {
                curTok.push_back(kEscape);
            }
            hasTok = true;
        }
        else if (c == kSingleQuote)
        {
            state = State::InSingle;
            hasTok = true;
        }
        else if (c == kDoubleQuote)
        {
            state = State::InDouble;
            hasTok = true;
        }
        else if (c == kRedirIn || c == kRedirOut)
        {
            // (AR) مُشغِّل إعادة توجيه: أنهِ الوسيط الجاري أوّلًا. لكن إن كنّا ننتظر هدفًا
            //      لإعادة توجيهٍ سابقة (pending != Arg وبلا وسيطٍ بعد) فهذا مُشغِّلٌ مكان الهدف = خطأ.
            // (EN) a redirection operator: finish the current token first. If we were already
            //      awaiting a target (pending != Arg with no token yet), an operator here = error.
            if (pending != Pending::Arg && !hasTok)
            {
                result.status = ShellParseStatus::RedirNoTarget;
                return result;
            }
            if (!endToken())
            {
                return result;
            }
            if (c == kRedirIn)
            {
                pending = Pending::RedirIn;
            }
            else if (i + 1 < n && raw[i + 1] == kRedirOut)
            {
                pending = Pending::RedirOutAppend; // ‹>>›
                ++i;
            }
            else
            {
                pending = Pending::RedirOutTrunc;  // ‹>›
            }
        }
        else if (c == kPipe)
        {
            // (AR) نهاية مرحلة. مُشغِّل إعادة توجيه معلّق بلا هدف = خطأ. مرحلةٌ بلا برنامج = خطأ.
            // (EN) stage boundary. A pending redirection with no target = error. A stage with
            //      no program = error.
            if (pending != Pending::Arg && !hasTok)
            {
                result.status = ShellParseStatus::RedirNoTarget;
                return result;
            }
            if (!endToken())
            {
                return result;
            }
            if (curStage.argv.empty())
            {
                result.status = ShellParseStatus::EmptyStage;
                return result;
            }
            result.stages.push_back(std::move(curStage));
            curStage = ShellStage{};
        }
        else if (isSpace(c))
        {
            if (!endToken())
            {
                return result;
            }
        }
        else
        {
            curTok.push_back(c);
            hasTok = true;
        }
    }

    // (AR) نهاية السطر: اقتباس مفتوح = خطأ / end of line: an open quote is an error
    if (state != State::Normal)
    {
        result.status = ShellParseStatus::UnterminatedQuote;
        return result;
    }
    // (AR) مُشغِّل إعادة توجيه معلّق بلا هدف عند النهاية = خطأ / dangling redirection at EOL
    if (pending != Pending::Arg && !hasTok)
    {
        result.status = ShellParseStatus::RedirNoTarget;
        return result;
    }

    if (!endToken())
    {
        return result;
    }

    if (!curStage.argv.empty())
    {
        result.stages.push_back(std::move(curStage));
    }
    else if (!curStage.inFile.empty() || !curStage.outFile.empty())
    {
        // (AR) إعادة توجيهٍ بلا برنامج (مثل «> ملف») / a redirection with no program
        result.status = ShellParseStatus::EmptyStage;
        return result;
    }
    else if (!result.stages.empty())
    {
        // (AR) أنبوب زائد بلا مرحلة أخيرة (مثل «ls |») / trailing pipe with no final stage
        result.status = ShellParseStatus::EmptyStage;
        return result;
    }

    return result;
}

} // namespace REPL
} // namespace Sad
