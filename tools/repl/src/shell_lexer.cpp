// بسم الله الرحمن الرحيم
/**
 * @file shell_lexer.cpp
 * @brief (AR) تطبيق مُحلِّل سطر صدَفة ص (اقتباس + هروب + أنابيب + توجيه < > >> 2> 2>> &> 2>&1 + $VAR).
 * @brief (EN) ص shell-line lexer (quotes + escaping + pipes + redirect < > >> 2> 2>> &> 2>&1 + $VAR).
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
constexpr char kDollar = '$';

// (AR) فراغٌ يفصل الوسائط: مسافة/جدولة + CR/LF (كي لا يخلق سطرٌ منتهٍ بـCRLF وسيطًا
//      زائفًا «\r» — مهمّ حين يُسحب الإدخال على Windows). / (EN) arg-separating whitespace:
//      space/tab + CR/LF (so a CRLF-terminated line never yields a bogus "\r" arg).
inline bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }

inline bool isAsciiAlpha(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
inline bool isAsciiDigit(char c) { return c >= '0' && c <= '9'; }

// (AR) بايت صالحٌ في اسم متغيّر بيئة: حرف/رقم/‹_›، أو بايت UTF-8 مُتابع/عربيّ (≥0x80) —
//      يتيح أسماء عربيّة مثل ‹$اسم›. الرقم لا يصلح أوّلَ حرفٍ (يُحرَسُ عند نقطة النداء).
// (EN) a byte valid in an env-var name: letter/digit/‹_›, or a UTF-8/Arabic byte (≥0x80) —
//      enabling Arabic names like ‹$اسم›. A digit is invalid as the first char (guarded at call).
inline bool isNameByte(char c)
{
    return isAsciiAlpha(c) || isAsciiDigit(c) || c == '_' ||
           (static_cast<unsigned char>(c) >= 0x80);
}
inline bool isNameStart(char c)
{
    return isAsciiAlpha(c) || c == '_' || (static_cast<unsigned char>(c) >= 0x80);
}

// (AR) عند s[i]=='$': يوسّع المرجع ويُقدّم i إلى ما بعده. الصيَغ: ‹${اسم}› (حتّى ‹}›)، و‹$اسم›،
//      والوسيطان الخاصّان ‹$?› (رمز خروج آخر أمر) و‹$$› (مِقبض الصدَفة) — يُحلَّان عبر env
//      باسمٍ خاصّ ‹?›/‹$›. ‹$› متبوعًا بغير ذلك ⇒ ‹$› حرفيّ. متغيّرٌ غير مُعرَّف ⇒ سلسلة فارغة.
// (EN) at s[i]=='$': expand the reference and advance i past it. Forms: ‹${name}› (until ‹}›),
//      ‹$name›, and the special params ‹$?› (last exit code) and ‹$$› (shell pid) — resolved
//      via env under the special name ‹?›/‹$›. A ‹$› not followed by any of these ⇒ literal
//      ‹$›. Unset var ⇒ empty string.
std::string expandDollarRef(const std::string& s, std::size_t& i, const EnvResolver& env)
{
    const std::size_t n = s.size();
    // precondition: s[i] == '$'
    if (i + 1 < n && s[i + 1] == '{')
    {
        std::size_t close = s.find('}', i + 2);
        if (close == std::string::npos)
        {
            // (AR) ‹${› بلا ‹}› — عامله حرفيًّا كي لا نبتلع بقيّة السطر / no ‹}›: treat literally
            i += 1;
            return std::string(1, kDollar);
        }
        // (AR) بما أنّ ‹?›/‹$› يُمرَّران اسمًا للمُحلِّل، فإنّ ‹${?}›/‹${$}› كنيةٌ للوسيطين الخاصّين
        //      (متّسقة وغير ضارّة). / since ‹?›/‹$› pass through as the name, ‹${?}›/‹${$}› alias
        //      the special params (consistent and harmless).
        std::string name = s.substr(i + 2, close - (i + 2));
        i = close + 1;
        return env(name);
    }
    // (AR) الوسائط الخاصّة ‹$?›/‹$$›: يُمرَّر الرمز اسمًا للمُحلِّل (‹?›/‹$› ليسا اسمَي بيئةٍ
    //      صالحين فلا يتصادمان). / (EN) special params ‹$?›/‹$$›: the symbol is passed as the
    //      resolver name (‹?›/‹$› are not valid env names, so no clash).
    if (i + 1 < n && (s[i + 1] == '?' || s[i + 1] == kDollar))
    {
        std::string special(1, s[i + 1]);
        i += 2;
        return env(special);
    }
    if (i + 1 < n && isNameStart(s[i + 1]))
    {
        std::size_t j = i + 1;
        while (j < n && isNameByte(s[j]))
        {
            ++j;
        }
        std::string name = s.substr(i + 1, j - (i + 1));
        i = j;
        return env(name);
    }
    // (AR) ‹$› وحده أو متبوعًا برقم/رمز ⇒ حرفيّ / lone ‹$› or followed by digit/symbol ⇒ literal
    i += 1;
    return std::string(1, kDollar);
}

// (AR) وجهةُ الوسيط التالي: وسيطٌ عاديّ أم هدفُ إعادة توجيه (وأيّ نوع). / (EN) where the next
//      completed token goes: a normal arg, or a redirection target (and which kind).
enum class Pending
{
    Arg,
    RedirIn,
    RedirOutTrunc,
    RedirOutAppend,
    RedirErrTrunc,   ///< هدف ‹2>›  / ‹2>› target
    RedirErrAppend   ///< هدف ‹2>>› / ‹2>>› target
};

// (AR) نوع مُشغِّل إعادة توجيهٍ مُطابَق عند موضعٍ ما. None = لا مُشغِّل هنا. / (EN) a matched
//      redirection operator kind at a position. None = no operator here.
enum class RedirKind
{
    None,
    In,          ///< ‹<›
    OutTrunc,    ///< ‹>›  أو ‹1>›
    OutAppend,   ///< ‹>>› أو ‹1>>›
    ErrTrunc,    ///< ‹2>›
    ErrAppend,   ///< ‹2>>›
    BothTrunc,   ///< ‹&>›  (الإخراج+الخطأ، بتر)
    BothAppend,  ///< ‹&>>› (الإخراج+الخطأ، إلحاق)
    ErrToOut     ///< ‹2>&1› (الخطأ يتبع الإخراج، بلا ملفّ هدف)
};

struct RedirMatch
{
    RedirKind kind;
    int len;   ///< عدد البايتات المُستهلَكة / bytes consumed
};

// (AR) يتعرّف مُشغِّل إعادة توجيهٍ عند s[i]. الصيَغ المسبوقة برقم مِقبض (‹2>›/‹2>>›/‹2>&1›/‹1>›)
//      أو بـ‹&› (‹&>›/‹&>>›) تُقبَل فقط عند بداية وسيط (atTokenStart) كي يبقى ‹echo 2 >f›
//      و‹ls2>f› نصًّا لا توجيهَ خطأ (قاعدة الصدَفة: المِقبض يلاصق ‹>› بلا فراغ عند بداية كلمة).
//      أمّا ‹<›/‹>›/‹>>› فتُطابَق في أيّ موضع (تُنهي الوسيط الجاري). / (EN) recognizes a
//      redirection operator at s[i]. fd-prefixed forms (‹2>›/‹2>>›/‹2>&1›/‹1>›) and ‹&›-forms
//      (‹&>›/‹&>>›) are accepted only at a token boundary (atTokenStart) so ‹echo 2 >f› and
//      ‹ls2>f› stay text, not stderr redirection. Plain ‹<›/‹>›/‹>>› match anywhere (they end
//      the current token).
inline RedirMatch matchRedirOp(const std::string& s, std::size_t i, bool atTokenStart)
{
    const std::size_t n = s.size();
    auto at = [&](std::size_t k) -> char { return k < n ? s[k] : '\0'; };
    const char c = s[i];

    if (atTokenStart)
    {
        // ‹&>›/‹&>>›
        if (c == '&' && at(i + 1) == kRedirOut)
        {
            return at(i + 2) == kRedirOut ? RedirMatch{RedirKind::BothAppend, 3}
                                          : RedirMatch{RedirKind::BothTrunc, 2};
        }
        // ‹2>&1›/‹2>>›/‹2>›
        if (c == '2' && at(i + 1) == kRedirOut)
        {
            if (at(i + 2) == '&' && at(i + 3) == '1')
            {
                return RedirMatch{RedirKind::ErrToOut, 4};
            }
            return at(i + 2) == kRedirOut ? RedirMatch{RedirKind::ErrAppend, 3}
                                          : RedirMatch{RedirKind::ErrTrunc, 2};
        }
        // ‹1>>›/‹1>› (مِقبض stdout الصريح) / explicit stdout fd
        if (c == '1' && at(i + 1) == kRedirOut)
        {
            return at(i + 2) == kRedirOut ? RedirMatch{RedirKind::OutAppend, 3}
                                          : RedirMatch{RedirKind::OutTrunc, 2};
        }
    }
    if (c == kRedirIn)
    {
        return RedirMatch{RedirKind::In, 1};
    }
    if (c == kRedirOut)
    {
        return at(i + 1) == kRedirOut ? RedirMatch{RedirKind::OutAppend, 2}
                                      : RedirMatch{RedirKind::OutTrunc, 1};
    }
    return RedirMatch{RedirKind::None, 0};
}
} // namespace

std::string expandEnvVars(const std::string& text, const EnvResolver& env)
{
    std::string out;
    out.reserve(text.size());
    const std::size_t n = text.size();
    for (std::size_t i = 0; i < n;)
    {
        if (text[i] == kDollar)
        {
            out += expandDollarRef(text, i, env);
        }
        else
        {
            out.push_back(text[i]);
            ++i;
        }
    }
    return out;
}

ShellPipeline parseShellPipeline(const std::string& raw, const EnvResolver& env)
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
            case Pending::RedirErrTrunc:
                // (AR) ملفُّ خطأٍ صريح يُلغي «الخطأ يتبع الإخراج» (حصريّة، آخرٌ يفوز).
                // (EN) an explicit stderr file overrides errToOut (mutual exclusion, last wins).
                curStage.errFile = curTok;
                curStage.appendErr = false;
                curStage.errToOut = false;
                break;
            case Pending::RedirErrAppend:
                curStage.errFile = curTok;
                curStage.appendErr = true;
                curStage.errToOut = false;
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
            else if (c == kDollar)
            {
                // (AR) توسيع ‹$VAR› داخل الاقتباس المزدوج (كصدَفة). j يتقدّم بعد المرجع؛ نطرح
                //      1 لأنّ حلقة for ستزيد i. / expand ‹$VAR› inside double quotes; j advances
                //      past the ref, we subtract 1 because the for-loop will ++i.
                std::size_t j = i;
                curTok += expandDollarRef(raw, j, env);
                i = j - 1;
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
        // (AR) جرّب مُشغِّل إعادة توجيه أوّلًا (يشمل الصيَغ المسبوقة بمِقبض/‹&›، فتُلتقَط قبل
        //      أن تُعامَل ‹2›/‹&›/‹1› كحرفٍ عاديّ). التوسيع لا يُعاد فحصه هنا (أمانٌ بالبناء).
        //      «بداية وسيط» تشترط أيضًا ألّا ننتظر هدفَ توجيهٍ سابق (pending==Arg): في موضع
        //      الهدف يكون المِقبض اسمَ ملفٍّ حرفيًّا (‹>2>f› ⇒ الإخراج للملفّ «f»، لا خطأ).
        // (EN) try a redirection operator first (covers fd/‹&›-prefixed forms before ‹2›/‹&›/‹1›
        //      would be treated as a normal char). Expansions are never re-scanned here. A
        //      "token start" also requires we are not awaiting a prior redirection target
        //      (pending==Arg): in target position an fd digit is a literal filename byte
        //      (‹>2>f› ⇒ stdout to file "f", not an error).
        const bool atTokenStart = (!hasTok && pending == Pending::Arg);
        RedirMatch rm = matchRedirOp(raw, i, atTokenStart);
        if (rm.kind != RedirKind::None)
        {
            // (AR) مُشغِّلٌ بينما ننتظر هدفَ توجيهٍ سابق (بلا وسيطٍ بعد) = هدفٌ مفقود.
            // (EN) an operator while awaiting a prior redirection target (no token yet) = missing.
            if (pending != Pending::Arg && !hasTok)
            {
                result.status = ShellParseStatus::RedirNoTarget;
                return result;
            }
            if (!endToken()) // (AR) أنهِ الوسيط الجاري ووجّهه لهدفه / finish & route current token
            {
                return result;
            }
            switch (rm.kind)
            {
                case RedirKind::In:        pending = Pending::RedirIn; break;
                case RedirKind::OutTrunc:  pending = Pending::RedirOutTrunc; break;
                case RedirKind::OutAppend: pending = Pending::RedirOutAppend; break;
                case RedirKind::ErrTrunc:  pending = Pending::RedirErrTrunc; break;
                case RedirKind::ErrAppend: pending = Pending::RedirErrAppend; break;
                case RedirKind::BothTrunc:
                    // (AR) ‹&>›: الخطأ يتبع الإخراج، والاسم التالي هدفُ stdout / stderr follows stdout
                    curStage.errToOut = true;
                    curStage.errFile.clear();
                    curStage.appendErr = false;
                    pending = Pending::RedirOutTrunc;
                    break;
                case RedirKind::BothAppend:
                    curStage.errToOut = true;
                    curStage.errFile.clear();
                    curStage.appendErr = false;
                    pending = Pending::RedirOutAppend;
                    break;
                case RedirKind::ErrToOut:
                    // (AR) ‹2>&1›: بلا اسم هدف؛ الخطأ يتبع الإخراج، ويُلغي أيّ ملفّ خطأٍ سابق.
                    // (EN) ‹2>&1›: no target name; stderr follows stdout, clears any prior stderr file.
                    curStage.errToOut = true;
                    curStage.errFile.clear();
                    curStage.appendErr = false;
                    pending = Pending::Arg;
                    break;
                case RedirKind::None: break; // (AR) لا يقع / unreachable
            }
            i += rm.len - 1; // (AR) حلقة for تزيد i / the for-loop's ++i consumes the last op char
        }
        else if (c == kEscape)
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
        else if (c == kDollar)
        {
            // (AR) توسيع ‹$VAR›/‹${VAR}› خارج الاقتباس. j يتقدّم بعد المرجع؛ نطرح 1 لأنّ حلقة
            //      for ستزيد i. القيمة تُلحَق كما هي (لا تُعاد قسمتها بالمسافات — تبسيطٌ موثّق).
            //      نضبط hasTok فقط إن كانت القيمة غير فارغة، فـ‹$غير_معرّف› مجرّدًا يختفي (كصدَفة)
            //      بينما ‹"$غير_معرّف"› المقتبَس يُنتِج وسيطًا فارغًا (عبر علامة الاقتباس الفاتحة).
            // (EN) expand ‹$VAR›/‹${VAR}› outside quotes; j advances past the ref, subtract 1
            //      for the for-loop's ++i. The value is appended as-is (no word-splitting — a
            //      documented simplification). hasTok is set only when the value is non-empty,
            //      so a bare ‹$unset› vanishes (shell-like) while quoted ‹"$unset"› still yields
            //      an empty arg (via the opening quote).
            std::size_t j = i;
            std::string val = expandDollarRef(raw, j, env);
            curTok += val;
            i = j - 1;
            if (!val.empty())
            {
                hasTok = true;
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
    else if (!curStage.inFile.empty() || !curStage.outFile.empty() ||
             !curStage.errFile.empty() || curStage.errToOut)
    {
        // (AR) إعادة توجيهٍ بلا برنامج (مثل «> ملف» أو «2> ملف») / a redirection with no program
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

ShellCommandLine parseCommandLine(const std::string& raw, const EnvResolver& env)
{
    ShellCommandLine result;
    result.status = ShellParseStatus::Ok;

    // (AR) الخطوة 1: قسمةٌ واعيةٌ بالاقتباس على ‹&&›/‹||› في المستوى الأعلى. لا نوسّع هنا؛
    //      نتتبّع حالة الاقتباس/الهروب فقط كي نميّز الرابط الحقيقيّ من ‹&&›/‹||› داخل اقتباسٍ أو
    //      قادمٍ عبر ‹$VAR› (الذي لا يُقسِّم — يُحلّه parseShellPipeline لاحقًا كنصّ). ‹&›/‹|› مفردًا
    //      لا يُقسِّم (‹&›/‹&>› حرفيّ أو توجيه، و‹|› أنبوبُ مراحلَ داخل المقطع). / (EN) step 1:
    //      quote-aware split on top-level ‹&&›/‹||›. No expansion here; we track only quote/escape
    //      state to tell a real connector from ‹&&›/‹||› inside a quote or arriving via ‹$VAR›
    //      (which does not split — parseShellPipeline later treats it as text). A lone ‹&›/‹|›
    //      does not split (‹&›/‹&>› is literal/redirect, ‹|› pipes stages within the segment).
    struct RawSeg
    {
        ChainOp op;
        std::string text;
    };
    std::vector<RawSeg> rawSegs;
    ChainOp curOp = ChainOp::First;
    std::size_t segStart = 0;

    enum class St
    {
        Normal,
        InSingle,
        InDouble
    } st = St::Normal;

    const std::size_t n = raw.size();
    for (std::size_t i = 0; i < n; ++i)
    {
        const char c = raw[i];

        if (st == St::InSingle)
        {
            if (c == kSingleQuote)
            {
                st = St::Normal;
            }
            continue;
        }
        if (st == St::InDouble)
        {
            // (AR) نتخطّى المحرف المهروب (يكفي للقسمة تخطّي التالي) / skip the escaped char
            if (c == kEscape && i + 1 < n)
            {
                ++i;
            }
            else if (c == kDoubleQuote)
            {
                st = St::Normal;
            }
            continue;
        }

        // ── Normal ──
        if (c == kEscape)
        {
            if (i + 1 < n)
            {
                ++i; // (AR) ‹\c› — تخطَّ c فلا يُفسَّر رابطًا / skip c so it is not read as a connector
            }
        }
        else if (c == kSingleQuote)
        {
            st = St::InSingle;
        }
        else if (c == kDoubleQuote)
        {
            st = St::InDouble;
        }
        else if (c == '&' && i + 1 < n && raw[i + 1] == '&')
        {
            rawSegs.push_back({curOp, raw.substr(segStart, i - segStart)});
            curOp = ChainOp::And;
            ++i; // (AR) استهلك ‹&› الثانية / consume the second ‹&›
            segStart = i + 1;
        }
        else if (c == kPipe && i + 1 < n && raw[i + 1] == kPipe)
        {
            rawSegs.push_back({curOp, raw.substr(segStart, i - segStart)});
            curOp = ChainOp::Or;
            ++i; // (AR) استهلك ‹|› الثانية / consume the second ‹|›
            segStart = i + 1;
        }
    }
    rawSegs.push_back({curOp, raw.substr(segStart)}); // (AR) المقطع الأخير / trailing segment

    // (AR) الخطوة 2: حلّل كلّ مقطعٍ بمُحلِّل الأنبوب. مقطعٌ فارغ في سلسلةٍ (رابطٌ بلا أمر) = خطأ.
    // (EN) step 2: parse each segment with the pipeline lexer. An empty segment in a chain
    //      (a connector with no command) is an error.
    for (const RawSeg& rs : rawSegs)
    {
        ShellPipeline p = parseShellPipeline(rs.text, env);
        if (p.status != ShellParseStatus::Ok)
        {
            result.status = p.status;
            result.segments.clear();
            return result;
        }
        if (p.stages.empty())
        {
            // (AR) مقطعٌ فارغ: في سلسلةٍ (أكثر من مقطع) خطأ ‹&&›/‹||› بلا طرف؛ إن كان المقطع
            //      الوحيد فهو «لا أمر» (يعرض المستدعي الاستعمال عبر segments الفارغة). / an empty
            //      segment: in a chain (more than one) it is a dangling ‹&&›/‹||›; a lone empty
            //      means "no command" (caller shows usage via the empty segments).
            if (rawSegs.size() > 1)
            {
                result.status = ShellParseStatus::EmptyStage;
                result.segments.clear();
                return result;
            }
            return result; // (AR) segments فارغة / segments left empty
        }
        result.segments.push_back({rs.op, std::move(p.stages)});
    }
    return result;
}

} // namespace REPL
} // namespace Sad
