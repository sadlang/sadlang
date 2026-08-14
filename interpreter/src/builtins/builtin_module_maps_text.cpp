/**
 * @file builtin_module_maps_text.cpp
 * @brief (AR) وحدة النصوص المتقدمة — دوال نصوص إضافية وتعبيرات نمطية ويونيكود
 * @brief (EN) Advanced text module — extra string functions, regex, and Unicode
 *
 * @details
 * (AR) الأقسام:
 *   4. التعبيرات النمطية (مطابقة، بحث، استبدال، تقسيم...)
 *   6. دوال نصوص إضافية (تقسيم، ضم، تكرار، عكس...)
 *   14. دوال يونيكود (حرف، رمز، قياس...)
 *
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
#include <algorithm>
#include <regex>
#include <sstream>

// (AR) إلغاء ماكرو VOID الخاص بويندوز إن وُجد
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
#include "bounds_checker.h" // (AR) فحص حدود موحَّد / (EN) unified bounds checking
#include "builtin_error.h"  // (AR) EM-CPP: حامل خطأ الطبقة الأدنى
#include "runtime_throw.h"  // (AR) رميُ RUN061 عند رايةٍ نمطيّةٍ مجهولة
#ifdef VOID
#undef VOID
#endif

// (AR) اختصار لفضاء أسماء ثوابت وحدة الخرائط
namespace Bmp = Sad::Builtins::Names::Maps;

namespace Sad
{
    namespace Interpreter
    {

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) دوال مساعدة / (EN) Helper Functions
        // ═══════════════════════════════════════════════════════════════════════

        static std::shared_ptr<Data::Value> makeVal(int v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVal(double v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVal(const std::string &v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVal(bool v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVoidVal() { return std::make_shared<Data::Value>(); }
        static std::shared_ptr<Data::Value> makeArrayVal(const Data::Value::ArrayType &a) { return std::make_shared<Data::Value>(a); }
        static std::shared_ptr<Data::Value> makeMapVal(const Data::Value::MapType &m) { return std::make_shared<Data::Value>(m); }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) رايات التعابير النمطيّة — وسيطٌ نصّيّ اختياريّ.
        //      كانت الدوالّ تقبل وسيطاً زائداً **وتُهمله صامتاً**، فيظنّ المبرمج أنّه
        //      ضبط رايةً ولم يفعل: نجاحٌ كاذب. الرايةُ المجهولة الآن ترمي RUN061.
        //      المدعوم: i (تجاهل الحالة) وحدَها. ولا رايةَ عامّة (g) — «جد_الكل» تمسح
        //      النصَّ كلَّه أصلاً. ولا m (متعدّد الأسطر): std::regex::multiline من C++17
        //      **غيرُ متوفّرٍ في كلّ نسخ MSVC** (فشل بناءُ CI بـC2039 وهو ينجح محلّيًّا
        //      على 19.50) — وميزةٌ تعمل على جهازٍ وتكسر آخرَ أسوأُ من غيابها.
        // (EN) Regex flags — an optional string argument. The builtins used to accept a
        //      surplus argument and silently ignore it, so a programmer believed a flag
        //      was set when it was not: a false success. Unknown flags now throw RUN061.
        // ═══════════════════════════════════════════════════════════════════════
        static std::regex::flag_type parseRegexFlags(Sad::Interpreter::BuiltinContext &ctx,
                                                     const std::string &flags,
                                                     const char *fnName)
        {
            std::regex::flag_type out = std::regex::ECMAScript;
            for (char c : flags)
            {
                switch (c)
                {
                case 'i': out |= std::regex::icase; break;
                default:
                    ::Sad::Errors::throwRuntime(::Sad::Errors::ErrorCode::RUN_REGEX_UNKNOWN_FLAG,
                                                ctx.position(),
                                                {{"flag", std::string(1, c)}, {"function", std::string(fnName)}});
                }
            }
            return out;
        }

        /** (AR) يبني النمطَ مع الرايات إن مُرِّرت في الموضع flagsIdx. */
        static std::regex makeRegex(Sad::Interpreter::BuiltinContext &ctx,
                                    const std::string &pattern,
                                    size_t flagsIdx,
                                    const char *fnName)
        {
            const auto &args = ctx.args();
            if (args.size() > flagsIdx)
                return std::regex(pattern, parseRegexFlags(ctx, args[flagsIdx]->toString(), fnName));
            return std::regex(pattern);
        }

        void registerBuiltinsMapsText(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // ═══════════════════════════════════════════════════════════════════
            // 4. التعبيرات النمطية / Regex
            // (AR) إصلاح نقطة ضعف رقم 4: عدم وجود دعم للتعبيرات النمطية
            // ═══════════════════════════════════════════════════════════════════

            // regex_match / تعبير_مطابقة — مطابقة كاملة
            auto regex_match_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                try
                {
                    std::string text = args[0]->toString();
                    std::regex pattern = makeRegex(ctx, args[1]->toString(), 2, "تعبير_مطابقة");
                    return makeVal(std::regex_match(text, pattern));
                }
                catch (const std::regex_error &)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
            };
            fm.registerBuiltinFunction(std::string(Bmp::REGEX), regex_match_fn);

            // regex_search / تعبير_بحث — بحث جزئي
            auto regex_search_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                try
                {
                    std::string text = args[0]->toString();
                    std::regex pattern = makeRegex(ctx, args[1]->toString(), 2, "تعبير_بحث");
                    std::smatch match;
                    if (std::regex_search(text, match, pattern))
                    {
                        return makeVal(match[0].str());
                    }
                    return makeVoidVal();
                }
                catch (const std::regex_error &)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
            };
            fm.registerBuiltinFunction(std::string(Bmp::REGEX_SEARCH), regex_search_fn);

            // regex_replace / تعبير_استبدال — استبدال بنمط
            auto regex_replace_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 3)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                try
                {
                    std::string text = args[0]->toString();
                    std::regex pattern = makeRegex(ctx, args[1]->toString(), 3, "تعبير_استبدال");
                    std::string replacement = args[2]->toString();
                    return makeVal(std::regex_replace(text, pattern, replacement));
                }
                catch (const std::regex_error &)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
            };
            fm.registerBuiltinFunction(std::string(Bmp::REGEX_REPLACE), regex_replace_fn);

            // regex_find_all / تعبير_جد_الكل — إيجاد جميع المطابقات
            auto regex_find_all_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                try
                {
                    std::string text = args[0]->toString();
                    std::regex pattern = makeRegex(ctx, args[1]->toString(), 2, "تعبير_جد_الكل");
                    Data::Value::ArrayType results;
                    auto begin = std::sregex_iterator(text.begin(), text.end(), pattern);
                    auto end = std::sregex_iterator();
                    for (auto it = begin; it != end; ++it)
                    {
                        results.push_back(Data::Value((*it)[0].str()));
                    }
                    return makeArrayVal(results);
                }
                catch (const std::regex_error &)
                {
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                }
            };
            fm.registerBuiltinFunction(std::string(Bmp::REGEX_FIND_ALL), regex_find_all_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 6. دوال نصوص إضافية / Extra String Functions
            // (AR) إصلاح نقطة ضعف رقم 6: نقص دوال النصوص
            // ═══════════════════════════════════════════════════════════════════

            // repeat / تكرار_نص — تكرار نص عدة مرات
            auto repeat_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string text = args[0]->toString();
                int count = args[1]->toInt();
                std::string result;
                for (int i = 0; i < count; i++)
                    result += text;
                return makeVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::TEXT_REPEAT), repeat_fn);

            // padStart / حشو_بداية — حشو نص من البداية
            auto padStart_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string text = args[0]->toString();
                int targetLen = args[1]->toInt();
                std::string pad = " ";
                if (args.size() >= 3)
                    pad = args[2]->toString();
                while (::Sad::Security::SafeArithmetic::assertSafeCast<int>(text.size(), "builtin_module_maps_text_size") < targetLen)
                {
                    text = pad + text;
                }
                if (::Sad::Security::SafeArithmetic::assertSafeCast<int>(text.size(), "builtin_module_maps_text_size") > targetLen)
                    text = text.substr(text.size() - targetLen);
                return makeVal(text);
            };
            fm.registerBuiltinFunction(std::string(Bmp::PAD_START), padStart_fn);

            // padEnd / حشو_نهاية — حشو نص من النهاية
            auto padEnd_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string text = args[0]->toString();
                int targetLen = args[1]->toInt();
                std::string pad = " ";
                if (args.size() >= 3)
                    pad = args[2]->toString();
                while (::Sad::Security::SafeArithmetic::assertSafeCast<int>(text.size(), "builtin_module_maps_text_size") < targetLen)
                {
                    text += pad;
                }
                if (::Sad::Security::SafeArithmetic::assertSafeCast<int>(text.size(), "builtin_module_maps_text_size") > targetLen)
                    text = text.substr(0, targetLen);
                return makeVal(text);
            };
            fm.registerBuiltinFunction(std::string(Bmp::PAD_END), padEnd_fn);

            // reverse_string / عكس_نص — عكس نص
            auto reverse_string_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string text = args[0]->toString();
                std::reverse(text.begin(), text.end());
                return makeVal(text);
            };
            fm.registerBuiltinFunction(std::string(Bmp::REVERSE_TEXT), reverse_string_fn);

            // charCodeAt / رمز_حرف — الحصول على رمز UTF-8 لحرف
            auto charCodeAt_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string text = args[0]->toString();
                int idx = args[1]->toInt();
                if (idx < 0 || idx >= ::Sad::Security::SafeArithmetic::assertSafeCast<int>(text.size(), "builtin_module_maps_text_size"))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return makeVal(static_cast<int>(static_cast<unsigned char>(text[idx])));
            };
            fm.registerBuiltinFunction(std::string(Bmp::CHAR_CODE), charCodeAt_fn);

            // fromCharCode / حرف_من_رمز — إنشاء حرف من رمز
            auto fromCharCode_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                // (AR) ترميزُ UTF-8 لنقطةِ الترميز — لا `char` واحدة. كان التنفيذُ
                //      يقتطع النقطةَ إلى بايتٍ فيخالف عقدَه المُعلَنَ في مصدرِ الحقيقة
                //      («تحويل رمز رقمي (Unicode) إلى حرفه المقابل»)، ويجعل الرموزَ
                //      المتطابقةَ في البايتِ الأدنى متساويةً: `حرف_من_رمز(١٥٨٧)`
                //      كانت تساوي `حرف_من_رمز(٥١)` — فلا محرفَ عربيًّا ولا كشفَ خطأ.
                //      وهو الحاجزُ اللغويُّ أمام فكِّ `\uXXXX` في مكتبةِ جيسون.
                // (EN) UTF-8-encode the code point instead of truncating it to one char.
                //      The old body contradicted its own SoT contract ("Convert a Unicode
                //      code point to its character") and made code points sharing a low
                //      byte equal: حرف_من_رمز(1587) == حرف_من_رمز(51). It is the language
                //      barrier blocking \uXXXX decoding in the JSON library.
                long long codePoint = args[0]->toInt();
                if (codePoint < 0 || codePoint > 0x10FFFF ||
                    (codePoint >= 0xD800 && codePoint <= 0xDFFF))
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string encoded;
                auto pushByte = [&encoded](unsigned int byte)
                { encoded.push_back(static_cast<char>(static_cast<unsigned char>(byte))); };
                auto point = static_cast<unsigned int>(codePoint);
                if (point < 0x80)
                {
                    pushByte(point);
                }
                else if (point < 0x800)
                {
                    pushByte(0xC0u | (point >> 6));
                    pushByte(0x80u | (point & 0x3Fu));
                }
                else if (point < 0x10000)
                {
                    pushByte(0xE0u | (point >> 12));
                    pushByte(0x80u | ((point >> 6) & 0x3Fu));
                    pushByte(0x80u | (point & 0x3Fu));
                }
                else
                {
                    pushByte(0xF0u | (point >> 18));
                    pushByte(0x80u | ((point >> 12) & 0x3Fu));
                    pushByte(0x80u | ((point >> 6) & 0x3Fu));
                    pushByte(0x80u | (point & 0x3Fu));
                }
                return makeVal(encoded);
            };
            fm.registerBuiltinFunction(std::string(Bmp::FROM_CHAR_CODE), fromCharCode_fn);

            // count / عدّ — عدد ظهور نص فرعي في نص
            auto count_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string text = args[0]->toString();
                std::string sub = args[1]->toString();
                if (sub.empty())
                    return makeVal(0);
                int count = 0;
                size_t pos = 0;
                while ((pos = text.find(sub, pos)) != std::string::npos)
                {
                    count++;
                    pos += sub.length();
                }
                return makeVal(count);
            };
            fm.registerBuiltinFunction(std::string(Bmp::COUNT), count_fn);

            // format / تنسيق — تنسيق نص بسيط (استبدال {} بالقيم)
            auto format_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string tmpl = args[0]->toString();
                std::string result;
                size_t argIdx = 1;
                size_t i = 0;
                while (i < tmpl.size())
                {
                    if (i + 1 < tmpl.size() && tmpl[i] == '{' && tmpl[i + 1] == '}')
                    {
                        if (Sad::Security::BoundsChecker::checkArrayIndex(argIdx, args.size()))
                        {
                            result += args[argIdx]->toString();
                            argIdx++;
                        }
                        else
                        {
                            result += "{}";
                        }
                        i += 2;
                    }
                    else
                    {
                        result += tmpl[i];
                        i++;
                    }
                }
                return makeVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::FORMAT), format_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 14. دوال يونيكود / Unicode Functions
            // ═══════════════════════════════════════════════════════════════════

            // ازل_تشكيل / strip_diacritics — إزالة التشكيل العربي من النص
            // Arabic diacritics are U+064B to U+065F (encoded as 2-byte UTF-8: 0xD9 0x8B-0x9F, 0xDA 0x80-0x9F)
            // (AR) EM-CPP: يبقى بالتوقيع القديم — يُستدعى داخلياً بـvector (دالة عليا).
            auto strip_diacritics_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isString())
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string input = args[0]->toString();
                std::string result;
                result.reserve(input.size());
                size_t i = 0;
                while (i < input.size())
                {
                    unsigned char c = static_cast<unsigned char>(input[i]);
                    if (c < 0x80)
                    {
                        // ASCII byte
                        result += input[i];
                        ++i;
                    }
                    else if ((c & 0xE0) == 0xC0 && i + 1 < input.size())
                    {
                        // 2-byte UTF-8 sequence
                        unsigned char c2 = static_cast<unsigned char>(input[i + 1]);
                        // Decode codepoint
                        uint32_t cp = ((c & 0x1F) << 6) | (c2 & 0x3F);
                        // Arabic diacritics: U+064B to U+065F (Fathatan to Hamza below)
                        // Also U+0610-U+061A (Quranic signs) and U+06D6-U+06ED
                        if (cp >= 0x064B && cp <= 0x065F)
                        {
                            i += 2; // skip diacritic
                        }
                        else if (cp >= 0x0610 && cp <= 0x061A)
                        {
                            i += 2; // skip Quranic annotation signs
                        }
                        else
                        {
                            result += input[i];
                            result += input[i + 1];
                            i += 2;
                        }
                    }
                    else if ((c & 0xF0) == 0xE0 && i + 2 < input.size())
                    {
                        // 3-byte UTF-8 sequence
                        unsigned char c2 = static_cast<unsigned char>(input[i + 1]);
                        unsigned char c3 = static_cast<unsigned char>(input[i + 2]);
                        uint32_t cp = ((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                        (void)cp;
                        // U+06D6-U+06ED (Quranic marks in 3-byte range if any, but these are 2-byte)
                        // U+FE70-U+FEFF (Arabic Presentation Forms-B) — keep these
                        result += input[i];
                        result += input[i + 1];
                        result += input[i + 2];
                        i += 3;
                    }
                    else if ((c & 0xF8) == 0xF0 && i + 3 < input.size())
                    {
                        // 4-byte UTF-8 — keep as-is
                        result += input[i];
                        result += input[i + 1];
                        result += input[i + 2];
                        result += input[i + 3];
                        i += 4;
                    }
                    else
                    {
                        result += input[i];
                        ++i;
                    }
                }
                return makeVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::STRIP_DIACRITICS), strip_diacritics_fn);

            // مقارنة_نص / compare_text — مقارنة نصوص مع خيار تجاهل التشكيل
            // Compares two strings optionally ignoring Arabic diacritics
            auto compare_text_fn = [&strip_diacritics_fn](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isString() || !args[1]->isString())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);

                bool ignoreDiacritics = false;
                if (args.size() >= 3 && args[2]->isBoolean())
                {
                    ignoreDiacritics = args[2]->toBool();
                }

                std::string a = args[0]->toString();
                std::string b = args[1]->toString();

                if (ignoreDiacritics)
                {
                    // Strip diacritics from both before comparison
                    std::vector<std::shared_ptr<Data::Value>> wrapA = {std::make_shared<Data::Value>(a)};
                    std::vector<std::shared_ptr<Data::Value>> wrapB = {std::make_shared<Data::Value>(b)};
                    // (AR) EM-CPP: سياق للاستدعاء الداخلي (التوقيع الجديد BuiltinContext).
                    Sad::Interpreter::BuiltinContext _ctxA(wrapA, Sad::Lexer::Position{}, "ازل_تشكيل");
                    Sad::Interpreter::BuiltinContext _ctxB(wrapB, Sad::Lexer::Position{}, "ازل_تشكيل");
                    a = strip_diacritics_fn(_ctxA)->toString();
                    b = strip_diacritics_fn(_ctxB)->toString();
                }

                return makeVal(a == b);
            };
            fm.registerBuiltinFunction(std::string(Bmp::COMPARE_TEXT), compare_text_fn);

            // نص_يونيكود / unicode_codepoints — تحويل نص إلى مصفوفة نقاط يونيكود
            auto unicode_codepoints_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isString())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string input = args[0]->toString();
                Data::Value::ArrayType codepoints;
                size_t i = 0;
                while (i < input.size())
                {
                    unsigned char c = static_cast<unsigned char>(input[i]);
                    uint32_t cp = 0;
                    int bytes = 1;
                    if (c < 0x80)
                    {
                        cp = c;
                        bytes = 1;
                    }
                    else if ((c & 0xE0) == 0xC0 && i + 1 < input.size())
                    {
                        cp = ((c & 0x1F) << 6) | (static_cast<unsigned char>(input[i + 1]) & 0x3F);
                        bytes = 2;
                    }
                    else if ((c & 0xF0) == 0xE0 && i + 2 < input.size())
                    {
                        cp = ((c & 0x0F) << 12) | ((static_cast<unsigned char>(input[i + 1]) & 0x3F) << 6) | (static_cast<unsigned char>(input[i + 2]) & 0x3F);
                        bytes = 3;
                    }
                    else if ((c & 0xF8) == 0xF0 && i + 3 < input.size())
                    {
                        cp = ((c & 0x07) << 18) | ((static_cast<unsigned char>(input[i + 1]) & 0x3F) << 12) | ((static_cast<unsigned char>(input[i + 2]) & 0x3F) << 6) | (static_cast<unsigned char>(input[i + 3]) & 0x3F);
                        bytes = 4;
                    }
                    codepoints.push_back(Data::Value(static_cast<int>(cp)));
                    i += bytes;
                }
                return makeArrayVal(codepoints);
            };
            fm.registerBuiltinFunction(std::string(Bmp::UNICODE_CODEPOINTS), unicode_codepoints_fn);

        } // registerBuiltinsMapsText

    } // namespace Interpreter
} // namespace Sad
