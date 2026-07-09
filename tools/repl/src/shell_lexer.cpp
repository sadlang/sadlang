// بسم الله الرحمن الرحيم
/**
 * @file shell_lexer.cpp
 * @brief (AR) تطبيق مُحلِّل سطر صدَفة ص (اقتباسات + هروب + تقسيم أنابيب).
 * @brief (EN) ص shell-line lexer implementation (quotes + escaping + pipe splitting).
 */
#include "shell_lexer.h"

namespace Sad
{
namespace REPL
{

namespace
{
// (AR) فاصل المراحل: ‹|› غير المقتبَس / (EN) stage separator: unquoted ‹|›
constexpr char kPipe = '|';
// (AR) محرف الهروب / (EN) escape character
constexpr char kEscape = '\\';
constexpr char kDoubleQuote = '"';
constexpr char kSingleQuote = '\'';

// (AR) فراغٌ يفصل الوسائط: مسافة/جدولة + CR/LF (كي لا يخلق سطرٌ منتهٍ بـCRLF وسيطًا
//      زائفًا «\r» — مهمّ حين يُسحب الإدخال على Windows). / (EN) arg-separating whitespace:
//      space/tab + CR/LF (so a CRLF-terminated line never yields a bogus "\r" arg).
inline bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
} // namespace

ShellPipeline parseShellPipeline(const std::string& raw)
{
    ShellPipeline result;
    result.status = ShellParseStatus::Ok;

    std::vector<std::string> curStage;  // (AR) وسائط المرحلة الجارية / current stage's args
    std::string curTok;                 // (AR) الوسيط الجاري بناؤه / token being built
    bool hasTok = false;                // (AR) بدأ وسيطٌ (يسمح بوسيط مقتبَس فارغ "") / a token has begun

    enum class State
    {
        Normal,
        InSingle,
        InDouble
    } state = State::Normal;

    // (AR) ينهي الوسيط الجاري بإضافته للمرحلة (إن كان قد بدأ) / finalize the current token
    auto endToken = [&]()
    {
        if (hasTok)
        {
            curStage.push_back(curTok);
            curTok.clear();
            hasTok = false;
        }
    };

    const std::size_t n = raw.size();
    for (std::size_t i = 0; i < n; ++i)
    {
        char c = raw[i];

        if (state == State::InSingle)
        {
            // (AR) اقتباس مفرد: كلّ شيء حرفيّ حتّى ' التالية / single quote: all literal until next '
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
            // (AR) اقتباس مزدوج: ‹\› يهرّب فقط ‹"›/‹\›؛ غيرهما يبقى ‹\› حرفيّاً معه.
            // (EN) double quote: ‹\› escapes only ‹"›/‹\›; otherwise ‹\› is literal.
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

        // (AR) الحالة العاديّة / Normal state
        if (c == kEscape)
        {
            // (AR) ‹\c› خارج الاقتباس: c حرفيّ؛ ‹\› أخيرة تبقى حرفيّاً / ‹\c› escapes c; trailing ‹\› is literal
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
            hasTok = true; // (AR) '' وسيطٌ فارغ صالح / '' is a valid empty arg
        }
        else if (c == kDoubleQuote)
        {
            state = State::InDouble;
            hasTok = true;
        }
        else if (c == kPipe)
        {
            // (AR) نهاية مرحلة: أنهِ الوسيط ثمّ أغلِق المرحلة. مرحلةٌ فارغة = خطأ.
            // (EN) stage boundary: finish token, close stage. An empty stage is an error.
            endToken();
            if (curStage.empty())
            {
                result.status = ShellParseStatus::EmptyStage;
                return result;
            }
            result.stages.push_back(std::move(curStage));
            curStage.clear();
        }
        else if (isSpace(c))
        {
            endToken();
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

    endToken();

    if (!curStage.empty())
    {
        result.stages.push_back(std::move(curStage));
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
