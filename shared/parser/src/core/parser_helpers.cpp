/**
 * @file parser_core_helpers.cpp
 * @brief (AR) دوال مساعدة للمحلل النحوي - الجزء الثاني من التنفيذ.
 *        (EN) Parser helper functions - second part of implementation.
 *
 * @author Sad Language Development Team
 * @date 2025-11-11
 * @license MIT License
 */

#include <string>
#include "parser_core.h"
#include "class_manager.h"
#include <iostream>
#include <sstream>
#include <cstdlib> // (AR) std::getenv لحجب آثار الاسترداد «🔧» عن المستخدم

namespace Sad
{
    namespace Parser
    {

        using namespace AST;
        using namespace Lexer;
        using TT = TokenType;

        // ======================================================================
        // (AR) تحويل نوع الرمز لنص / (EN) Token type to string conversion
        // ======================================================================

        /**
         * @brief (AR) يحوّل نوع الرمز إلى نصه المتوقع (للمحددات والكلمات المفتاحية).
         *        (EN) Converts token type to its expected text (for delimiters/keywords).
         */
        static std::string tokenTypeToExpectedText(TokenType type)
        {
            switch (type)
            {
            case TT::PAREN_RIGHT:
                return ")";
            case TT::PAREN_LEFT:
                return "(";
            case TT::BRACKET_RIGHT:
                return "]";
            case TT::BRACKET_LEFT:
                return "[";
            case TT::BRACE_RIGHT:
                return "}";
            case TT::BRACE_LEFT:
                return "{";
            case TT::SEMICOLON:
                return ";";
            case TT::COLON:
                return ":";
            case TT::COMMA:
                return ",";
            case TT::KEYWORD_END:
                return "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9"; // نهاية
            case TT::KEYWORD_FUNCTION:
                return "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9"; // دالة
            case TT::KEYWORD_CLASS:
                return "\xD8\xB5\xD9\x86\xD9\x81"; // صنف
            case TT::KEYWORD_IF:
                return "\xD8\xA5\xD8\xB0\xD8\xA7"; // إذا
            case TT::KEYWORD_WHILE:
                return "\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7"; // بينما
            case TT::KEYWORD_FOR:
                return "\xD9\x84\xD9\x83\xD9\x84"; // لكل
            case TT::KEYWORD_RETURN:
                return "\xD8\xA3\xD8\xB1\xD8\xAC\xD8\xB9"; // أرجع
            case TT::KEYWORD_VAR:
                return "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1"; // متغير
            case TT::KEYWORD_CONST:
                return "\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA"; // ثابت
            case TT::IDENTIFIER:
                return "identifier";
            case TT::OP_ASSIGN:
                return "=";
            case TT::ARROW:
                return "->";
            default:
                return "";
            }
        }

        /**
         * @brief (AR) يحسب مسافة Levenshtein بين نصين UTF-8 (على مستوى الحروف).
         *        (EN) Computes Levenshtein distance between two UTF-8 strings (character-level).
         */
        static size_t utf8CharCount(const std::string &s)
        {
            size_t count = 0;
            for (size_t i = 0; i < s.size();)
            {
                unsigned char ch = s[i];
                if (ch < 0x80)
                    i += 1;
                else if (ch < 0xE0)
                    i += 2;
                else if (ch < 0xF0)
                    i += 3;
                else
                    i += 4;
                count++;
            }
            return count;
        }

        static std::vector<uint32_t> utf8ToCodepoints(const std::string &s)
        {
            std::vector<uint32_t> cps;
            for (size_t i = 0; i < s.size();)
            {
                uint32_t cp = 0;
                unsigned char ch = s[i];
                if (ch < 0x80)
                {
                    cp = ch;
                    i += 1;
                }
                else if (ch < 0xE0)
                {
                    cp = (ch & 0x1F) << 6 | (s[i + 1] & 0x3F);
                    i += 2;
                }
                else if (ch < 0xF0)
                {
                    cp = (ch & 0x0F) << 12 | (s[i + 1] & 0x3F) << 6 | (s[i + 2] & 0x3F);
                    i += 3;
                }
                else
                {
                    cp = (ch & 0x07) << 18 | (s[i + 1] & 0x3F) << 12 | (s[i + 2] & 0x3F) << 6 | (s[i + 3] & 0x3F);
                    i += 4;
                }
                cps.push_back(cp);
            }
            return cps;
        }

        static size_t levenshteinDistance(const std::string &a, const std::string &b)
        {
            auto cpa = utf8ToCodepoints(a);
            auto cpb = utf8ToCodepoints(b);
            size_t m = cpa.size(), n = cpb.size();
            std::vector<size_t> prev(n + 1), curr(n + 1);
            for (size_t j = 0; j <= n; j++)
                prev[j] = j;
            for (size_t i = 1; i <= m; i++)
            {
                curr[0] = i;
                for (size_t j = 1; j <= n; j++)
                {
                    size_t cost = (cpa[i - 1] == cpb[j - 1]) ? 0 : 1;
                    curr[j] = std::min({prev[j] + 1, curr[j - 1] + 1, prev[j - 1] + cost});
                }
                std::swap(prev, curr);
            }
            return prev[n];
        }

        /**
         * @brief (AR) يبحث عن أقرب كلمة مفتاحية لنص مُعطى (أقل مسافة تحرير).
         *        (EN) Finds closest keyword to given text (minimum edit distance).
         * @return (AR) الكلمة المفتاحية الأقرب، أو فارغة إذا لم يُوجد تطابق قريب
         */
        static std::string findClosestKeyword(const std::string &text)
        {
            // (AR) الكلمات المفتاحية العربية المحجوزة مع بدائل شائعة
            // (EN) Arabic keywords with common variants
            static const std::vector<std::pair<std::string, std::string>> keywords = {
                {"\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9", "KEYWORD_FUNCTION"},               // دالة
                {"\xD8\xB5\xD9\x86\xD9\x81", "KEYWORD_CLASS"},                          // صنف
                {"\xD8\xA5\xD8\xB0\xD8\xA7", "KEYWORD_IF"},                             // إذا
                {"\xD8\xA7\xD8\xB0\xD8\xA7", "KEYWORD_IF"},                             // اذا (بدون همزة)
                {"\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7", "KEYWORD_ELSE"},                   // وإلا
                {"\xD9\x88\xD8\xA7\xD9\x84\xD8\xA7", "KEYWORD_ELSE"},                   // والا (بدون همزة)
                {"\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7", "KEYWORD_WHILE"},          // بينما
                {"\xD9\x84\xD9\x83\xD9\x84", "KEYWORD_FOR"},                            // لكل
                {"\xD8\xA3\xD8\xB1\xD8\xAC\xD8\xB9", "KEYWORD_RETURN"},                 // أرجع
                {"\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9", "KEYWORD_RETURN"},                 // ارجع (بدون همزة)
                {"\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1", "KEYWORD_VAR"},            // متغير
                {"\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA", "KEYWORD_CONST"},                  // ثابت
                {"\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9", "KEYWORD_END"},            // نهاية
                {"\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x88\xD8\xB1\xD8\xAF", "KEYWORD_IMPORT"}, // استورد
                {"\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84", "KEYWORD_TRY"},                    // حاول
                {"\xD8\xA7\xD9\x85\xD8\xB3\xD9\x83", "KEYWORD_CATCH"},                  // امسك
                {"\xD8\xA7\xD8\xB1\xD9\x85\xD9\x8A", "KEYWORD_THROW"},                  // ارمي
                {"\xD8\xA8\xD9\x86\xD9\x8A\xD8\xA9", "KEYWORD_STRUCT"},                 // بنية
                {"\xD8\xAA\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF", "KEYWORD_ENUM"},           // تعداد
                {"\xD8\xB7\xD8\xA7\xD8\xA8\xD9\x82", "KEYWORD_MATCH"},                  // طابق
                {"\xD8\xB9\xD9\x86\xD8\xAF\xD9\x85\xD8\xA7", "KEYWORD_WHEN"},           // عندما
            };

            size_t bestDist = 999;
            std::string bestKeyword;
            size_t textLen = utf8CharCount(text);

            for (const auto &[kw, _] : keywords)
            {
                size_t kwLen = utf8CharCount(kw);
                // (AR) لا نقارن إذا الفرق في الطول كبير جداً
                if (textLen > kwLen + 2 || kwLen > textLen + 2)
                    continue;

                size_t dist = levenshteinDistance(text, kw);
                if (dist < bestDist && dist <= 2 && dist > 0)
                {
                    bestDist = dist;
                    bestKeyword = kw;
                }
            }
            return bestKeyword;
        }

        // ======================================================================
        // (AR) نظام التعافي الذكي / (EN) Smart Recovery System
        // ======================================================================

        /**
         * @brief (AR) يحاول التعافي الذكي من خطأ باستخدام ErrorRecoverySystem.
         *        (EN) Attempts smart error recovery using ErrorRecoverySystem.
         */
        bool ParserCore::tryRecoverFromError(TokenType expectedType, const std::string &context)
        {
            // (AR) استراتيجية 0: تخطي رمز زائد — إذا كان الرمز التالي هو المطلوب
            // (EN) Strategy 0: Skip extra token — if next token is what we need
            if (nextToken_.getType() == expectedType)
            {
                std::cerr << "\n\xF0\x9F\x94\xA7 "
                          << "\xD8\xAA\xD8\xAE\xD8\xB7\xD9\x8A \xD8\xB1\xD9\x85\xD8\xB2 \xD8\xB2\xD8\xA7\xD8\xA6\xD8\xAF '"
                          << current_.getValue() << "'"
                          << " | Skipped extra token '" << current_.getValue() << "'\n";
                advance();    // (AR) تقدم لتخطي الرمز الزائد — الآن current_ = المطلوب
                return false; // (AR) نُرجع false ليتابع consume() بالطريقة العادية (check + advance)
            }

            // (AR) استراتيجية 0.5: تصحيح كلمة مفتاحية مكتوبة خطأ
            // (EN) Strategy 0.5: Correct misspelled keyword
            if (current_.getType() == TT::IDENTIFIER && expectedType != TT::IDENTIFIER)
            {
                std::string closest = findClosestKeyword(current_.getValue());
                if (!closest.empty())
                {
                    std::cerr << "\n\xF0\x9F\x94\xA7 "
                              << "\xD9\x87\xD9\x84 \xD8\xAA\xD9\x82\xD8\xB5\xD8\xAF '"                      // هل تقصد
                              << closest << "' \xD8\xA8\xD8\xAF\xD9\x84\xD8\xA7\xD9\x8B \xD9\x85\xD9\x86 '" // بدلاً من
                              << current_.getValue() << "'?"
                              << " | Did you mean '" << closest << "' instead of '" << current_.getValue() << "'?\n";
                    // (AR) لا نعدّل الرمز — نسجّل الاقتراح فقط، والمحلل يكمل بالتعافي العادي
                }
            }

            std::string expected = tokenTypeToExpectedText(expectedType);
            if (expected.empty())
            {
                expected = std::to_string(static_cast<int>(expectedType));
            }

            std::string found = current_.getValue();
            if (found.empty())
            {
                found = std::to_string(static_cast<int>(current_.getType()));
            }

            Errors::RecoveryResult result = recoverySystem_.tryRecover(
                expected,
                found,
                context,
                current_.getPosition().line,
                current_.getPosition().column);

            if (!result.recovered)
            {
                return false;
            }

            // (AR) تطبيق استراتيجية التعافي
            // (EN) Apply recovery strategy
            switch (result.action.strategy)
            {
            case Errors::RecoveryStrategyType::INSERT_TOKEN:
                // (AR) الرمز "أُدرج" افتراضياً — لا يتقدم المحلل، ويتابع consume() التحليل.
                //      لكنّ هذا خطأ نحويّ حقيقيّ (رمز مفقود): يجب تسجيله في ErrorManager
                //      كي يعكسه hasErrors()، وإلّا يُبتلع الخطأ صامتاً ويُقبل كودٌ مُشوَّه
                //      مثل "(1 + 2;" بلا أيّ تشخيص (BF-04: عالج السبب لا العرَض).
                // (EN) The token is "virtually inserted" so parsing continues, but this is
                //      a genuine syntax error (a missing token). We MUST record it in the
                //      ErrorManager so hasErrors() reflects it; otherwise malformed input
                //      like "(1 + 2;" is silently accepted with no diagnostic.
                {
                    Errors::SourceLocation insLoc(
                        filename_.empty() ? "<source>" : filename_,
                        current_.getPosition().line,
                        current_.getPosition().column,
                        current_.getPosition().offset,
                        current_.getPosition().length);
                    // (AR) الرسالة تصف المشكلة للمستخدم (رمزٌ متوقَّع مفقود)، لا فعل الاسترداد
                    //      الداخليّ («إدراج ')'») الذي يوحي خطأً بأنّ الأداة أصلحت الكود ثمّ
                    //      تُبلّغ عنه خطأً في آنٍ (تناقض «تمّ الإدراج» ⇄ «❌ خطأ»). [تشخيص أنظف]
                    // (EN) The message describes the problem to the user (an expected token is
                    //      missing), not the internal recovery ACTION ("Inserted ')'") which
                    //      misleadingly implies the tool fixed the code yet still reports an
                    //      error (the "inserted" ⇄ "❌ error" contradiction).
                    Errors::ErrorManager::getInstance().reportError(
                        Errors::ErrorCode::SYN_UNCLOSED_BRACKET,
                        insLoc,
                        "\xD8\xAA\xD9\x88\xD9\x82\xD9\x91\xD8\xB9\xD8\xAA '" + expected + "' \xD9\x87\xD9\x86\xD8\xA7", // توقّعت '<expected>' هنا
                        "Expected '" + expected + "' here");
                }
                // (AR) طبع فعل الاسترداد «🔧» أثرٌ داخليّ للمطوّر: أظهره فقط عند التشخيص.
                // (EN) The "🔧" recovery-action print is an internal dev trace: show it only when debugging.
                if (std::getenv(Errors::kDiagStatsEnvVar) != nullptr)
                {
                    std::cerr << "\n\xF0\x9F\x94\xA7 " << result.action.description
                              << " | " << result.action.descriptionEn << "\n";
                }
                return true;

            case Errors::RecoveryStrategyType::SYNC_TO_DELIMITER:
            case Errors::RecoveryStrategyType::SYNC_TO_KEYWORD:
            {
                // (AR) تقدم حتى نقطة المزامنة
                // (EN) Advance to sync point
                const auto &syncPoints = recoverySystem_.getSyncPoints();
                int skipped = 0;
                while (!isAtEnd() && skipped < 50)
                {
                    std::string val = current_.getValue();
                    if (syncPoints.count(val) > 0)
                    {
                        // (AR) أثر مزامنة داخليّ للمطوّر: أظهره فقط عند التشخيص. [تشخيص أنظف]
                        // (EN) Internal sync trace for devs: show only when debugging.
                        if (std::getenv(Errors::kDiagStatsEnvVar) != nullptr)
                        {
                            std::cerr << "\n\xF0\x9F\x94\xA7 " << result.action.description
                                      << " (" << skipped << " tokens skipped)"
                                      << " | " << result.action.descriptionEn << "\n";
                        }
                        panicMode_ = false;
                        return true;
                    }
                    // (AR) توقف عند كلمات مفتاحية تبدأ جملاً جديدة
                    // (EN) Stop at statement-starting keywords
                    switch (current_.getType())
                    {
                    case TT::KEYWORD_CLASS:
                    case TT::KEYWORD_FUNCTION:
                    case TT::KEYWORD_CONST:
                    case TT::KEYWORD_FOR:
                    case TT::KEYWORD_IF:
                    case TT::KEYWORD_WHILE:
                    case TT::KEYWORD_RETURN:
                    case TT::KEYWORD_VAR:
                    case TT::KEYWORD_END:
                    case TT::KEYWORD_IMPORT:
                    case TT::KEYWORD_TRY:
                    case TT::KEYWORD_STRUCT:
                    case TT::KEYWORD_ENUM:
                        panicMode_ = false;
                        return true;
                    default:
                        break;
                    }
                    advance();
                    skipped++;
                }
                return false;
            }

            case Errors::RecoveryStrategyType::SKIP_TOKEN:
                // (AR) تخطي الرمز الحالي
                // (EN) Skip current token
                advance();
                return true;

            case Errors::RecoveryStrategyType::REPLACE_TOKEN:
            case Errors::RecoveryStrategyType::SKIP_TO_END:
            default:
                return false;
            }
        }

        /**
         * @brief (AR) يُرجع إحصائيات التعافي من الأخطاء.
         *        (EN) Returns error recovery statistics.
         */
        std::string ParserCore::getRecoveryStats(bool useArabic) const
        {
            return recoverySystem_.formatStats(useArabic);
        }

        /**
         * @brief (AR) يُرجع سجل التعافي من الأخطاء (للاستخدام في LSP).
         *        (EN) Returns error recovery log (for LSP diagnostics).
         */
        const std::vector<Errors::RecoveryResult> &ParserCore::getRecoveryLog() const
        {
            return recoverySystem_.getRecoveryLog();
        }

        // ======================================================================
        // (AR) ملاحظة: دالّتا parseListComprehension/parseDictComprehension القديمتان
        //      (بالترتيب البايثونيّ «تعبير لكل …») حُذفتا في RFC 25 م1ب. الاستيعابات الآن
        //      تُحلَّل حصرًا في parseArrayLiteral/parseMapLiteral بالترتيب العربيّ «لكل … أنتج …».
        // (EN) The old python-order parseListComprehension/parseDictComprehension were removed
        //      in RFC 25 م1ب; comprehensions parse only in parseArrayLiteral/parseMapLiteral now.
        // ======================================================================

        // ======================================================================
        // (AR) الدوال المساعدة / (EN) Helper Functions
        // ======================================================================

        /**
         * @brief (AR) يتقدم إلى الرمز التالي.
         *        (EN) Advances to next token.
         */
        void ParserCore::advance()
        {
            previous_ = current_;
            current_ = nextToken_; // Move nextToken_ to current_

            // Fetch new nextToken_ for lookahead
            nextToken_ = lexer_.nextToken();

            // ─────────────────────────────────────────────────────────────────
            // (AR) إصلاح BF-04 (السبب الجذري): ترحيل تعليقات التوثيق التي
            //      ظهرت في دورة advance() السابقة أثناء ملء nextToken_.
            //      تلك التعليقات تظهر فعلياً قبل current_ الجديد (لأن
            //      current_ هو ما كان nextToken_)، فهي تنتمي إلى التصريح
            //      الذي يبدأ عند current_ — وليس إلى التصريح السابق.
            //
            // (EN) BF-04 root-cause fix: promote doc comments that were
            //      buffered in the previous advance() while refilling
            //      nextToken_. Those doc comments physically precede the
            //      new current_ (which IS the previous nextToken_), so they
            //      belong to the declaration starting at current_, not to
            //      the declaration that came before current_.
            // ─────────────────────────────────────────────────────────────────
            if (!nextDocComment_.empty())
            {
                if (!pendingDocComment_.empty())
                    pendingDocComment_ += '\n';
                pendingDocComment_ += nextDocComment_;
                nextDocComment_.clear();
            }

            // Skip whitespace, comments, and doc-comments in current_
            // (AR) تجاوز المسافات والتعليقات — التقاط التعليقات التوثيقية بدل تخطيها
            // (AR) هذه التعليقات تسبق current_ النهائي فعلياً → pendingDocComment_
            while (current_.getType() == TT::WHITESPACE ||
                   current_.getType() == TT::COMMENT ||
                   current_.getType() == TT::DOC_COMMENT ||
                   current_.getType() == TT::NEWLINE)
            {
                // (AR) إصلاح BF-04: تجميع أسطر ## المتتالية (كل سطر = رمز منفصل)
                // (EN) BF-04 fix: accumulate consecutive ## lines (each line = separate token)
                if (current_.getType() == TT::DOC_COMMENT)
                {
                    if (!pendingDocComment_.empty())
                        pendingDocComment_ += '\n';
                    pendingDocComment_ += current_.getValue();
                }
                current_ = nextToken_;
                nextToken_ = lexer_.nextToken();
            }

            // Also skip whitespace in nextToken_ for correct lookahead (peekNext)
            // (AR) أيضاً تجاوز المسافات في nextToken_ للنظر المسبق الصحيح
            // (AR) إصلاح BF-04: التعليقات هنا تظهر بعد current_ فعلياً —
            //      نخزّنها مؤقتاً في nextDocComment_ ولا نضمها إلى pending الآن.
            //      ستُرحَّل في advance() القادم عندما يصبح هذا الموقع هو current_.
            // (EN) BF-04 fix: doc comments encountered while refilling
            //      nextToken_ appear AFTER current_; buffer them in
            //      nextDocComment_ instead of attaching to pending now. They
            //      will be promoted on the next advance() once we cross them.
            while (nextToken_.getType() == TT::WHITESPACE ||
                   nextToken_.getType() == TT::COMMENT ||
                   nextToken_.getType() == TT::DOC_COMMENT ||
                   nextToken_.getType() == TT::NEWLINE)
            {
                if (nextToken_.getType() == TT::DOC_COMMENT)
                {
                    if (!nextDocComment_.empty())
                        nextDocComment_ += '\n';
                    nextDocComment_ += nextToken_.getValue();
                }
                nextToken_ = lexer_.nextToken();
            }
        }

        /**
         * @brief (AR) يتحقق من نوع الرمز الحالي دون التقدم.
         *        (EN) Checks current token type without advancing.
         */
        bool ParserCore::check(TokenType type) const
        {
            if (isAtEnd())
                return false;
            return current_.getType() == type;
        }

        /**
         * @brief (AR) يتطابق ويتقدم إذا كان الرمز من النوع المحدد.
         *        (EN) Matches and advances if token is of specified type.
         */
        bool ParserCore::match(TokenType type)
        {
            if (check(type))
            {
                advance();
                return true;
            }
            return false;
        }

        /**
         * @brief (AR) يتطابق مع أي نوع من القائمة ويتقدم.
         *        (EN) Matches any type from list and advances.
         */
        bool ParserCore::matchAny(const std::vector<TokenType> &types)
        {
            for (const auto &type : types)
            {
                if (check(type))
                {
                    advance();
                    return true;
                }
            }
            return false;
        }

        // =========================================================================
        // (AR) دوال مساعدة للفواصل والمنقوطات العربية/الإنجليزية
        // (EN) Unified helpers for Arabic/English comma and semicolon handling
        // =========================================================================

        bool ParserCore::checkComma() const
        {
            return current_.getType() == TT::COMMA || current_.getType() == TT::ARABIC_COMMA;
        }

        bool ParserCore::matchComma()
        {
            if (checkComma())
            {
                advance();
                return true;
            }
            return false;
        }

        bool ParserCore::checkSemicolon() const
        {
            return current_.getType() == TT::SEMICOLON || current_.getType() == TT::ARABIC_SEMICOLON;
        }

        bool ParserCore::matchSemicolon()
        {
            if (checkSemicolon())
            {
                advance();
                return true;
            }
            return false;
        }

        void ParserCore::skipSemicolons()
        {
            while (checkSemicolon())
            {
                advance();
            }
        }

        // =========================================================================
        // (AR) دوال الكلمات السياقية (v4.1)
        //      تعتمد على KeywordTable::getEntry() المبنية من YAML — لا توجد
        //      أي مقارنة سلسلة عربية يدوية في هذه الدوال أو في مستدعيها.
        // (EN) Contextual keyword helpers (v4.1)
        //      Backed by KeywordTable::getEntry() built from YAML — no hand-
        //      written Arabic string comparison lives here or in callers.
        // =========================================================================

        bool ParserCore::checkContextual(TokenType type) const
        {
            if (isAtEnd())
                return false;

            // (AR) (1) Lexer أصدرها كرمز خاص (للكلمات المحجوزة)
            if (current_.getType() == type)
                return true;

            // (AR) (2) IDENTIFIER يطابق الكلمة الرئيسية أو أحد الأسماء البديلة
            //         للنوع المطلوب — كل ذلك يأتي من keywords.yaml
            if (current_.getType() != TT::IDENTIFIER)
                return false;

            const auto* entry = Lexer::KeywordTable::getEntry(type);
            if (!entry)
                return false;

            const std::string& word = current_.getValue();
            if (word == entry->primaryWord)
                return true;
            for (const auto& alias : entry->aliases)
            {
                if (word == alias)
                    return true;
            }
            return false;
        }

        bool ParserCore::matchContextual(TokenType type)
        {
            if (checkContextual(type))
            {
                advance();
                return true;
            }
            return false;
        }

        Token ParserCore::consumeContextual(TokenType type, const std::string &message)
        {
            if (checkContextual(type))
            {
                advance();
                return previous();
            }
            // (AR) إعادة الاستخدام: السلوك الموحَّد للأخطاء عبر consume العادي
            return consume(type, message);
        }

        /**
         * @brief (AR) يستهلك رمزاً من النوع المحدد أو يرفع خطأ.
         *        (EN) Consumes token of specified type or raises error.
         */
        Token ParserCore::consume(TokenType type, const std::string &message)
        {
            if (check(type))
            {
                advance();
                return previous();
            }

            // (AR) محاولة التعافي الذكي قبل رفع الخطأ
            // (EN) Try smart recovery before raising error
            if (tryRecoverFromError(type, message))
            {
                // (AR) التعافي نجح — إذا كانت الاستراتيجية INSERT_TOKEN، نُرجع رمزاً وهمياً
                //      وكأن الرمز المفقود كان موجوداً
                // (EN) Recovery succeeded — if strategy was INSERT_TOKEN, return dummy token
                //      as if the missing token was present
                return Token(type, tokenTypeToExpectedText(type), current_.getPosition());
            }

            // (AR) إنشاء رسالة خطأ ثنائية اللغة تلقائية
            // (EN) Create automatic bilingual error message
            std::string expected_ar;
            std::string expected_en;
            std::string fixText;
            std::string fixDesc_ar;
            std::string fixDesc_en;
            bool addFixIt = false;

            switch (type)
            {
            case TT::PAREN_RIGHT:
                expected_ar = "قوس إغلاق ')'";
                expected_en = "closing parenthesis ')'";
                fixText = ")";
                fixDesc_ar = "أضف ')' هنا";
                fixDesc_en = "Add ')' here";
                addFixIt = true;
                break;
            case TT::PAREN_LEFT:
                expected_ar = "قوس فتح '('";
                expected_en = "opening parenthesis '('";
                fixText = "(";
                fixDesc_ar = "أضف '(' هنا";
                fixDesc_en = "Add '(' here";
                addFixIt = true;
                break;
            case TT::BRACKET_RIGHT:
                expected_ar = "قوس مربع إغلاق ']'";
                expected_en = "closing bracket ']'";
                fixText = "]";
                fixDesc_ar = "أضف ']' هنا";
                fixDesc_en = "Add ']' here";
                addFixIt = true;
                break;
            case TT::BRACKET_LEFT:
                expected_ar = "قوس مربع فتح '['";
                expected_en = "opening bracket '['";
                fixText = "[";
                fixDesc_ar = "أضف '[' هنا";
                fixDesc_en = "Add '[' here";
                addFixIt = true;
                break;
            case TT::BRACE_RIGHT:
                expected_ar = "قوس معقوف إغلاق '}'";
                expected_en = "closing brace '}'";
                fixText = "}";
                fixDesc_ar = "أضف '}' هنا";
                fixDesc_en = "Add '}' here";
                addFixIt = true;
                break;
            case TT::BRACE_LEFT:
                expected_ar = "قوس معقوف فتح '{'";
                expected_en = "opening brace '{'";
                fixText = "{";
                fixDesc_ar = "أضف '{' هنا";
                fixDesc_en = "Add '{' here";
                addFixIt = true;
                break;
            case TT::SEMICOLON:
            case TT::ARABIC_SEMICOLON:
                expected_ar = "فاصلة منقوطة ';'";
                expected_en = "semicolon ';'";
                fixText = ";";
                fixDesc_ar = "أضف ';' في نهاية الجملة";
                fixDesc_en = "Add ';' at end of statement";
                addFixIt = true;
                break;
            case TT::COMMA:
                expected_ar = "فاصلة ','";
                expected_en = "comma ','";
                fixText = ",";
                fixDesc_ar = "أضف ',' للفصل بين العناصر";
                fixDesc_en = "Add ',' to separate items";
                addFixIt = true;
                break;
            case TT::COLON:
                expected_ar = "نقطتان ':'";
                expected_en = "colon ':'";
                fixText = ":";
                fixDesc_ar = "أضف ':' هنا";
                fixDesc_en = "Add ':' here";
                addFixIt = true;
                break;
            case TT::KEYWORD_END:
                // (AR) التهجئة من معجم SoT — لا سلاسل خام لكلمات المعجم
                // (EN) Spelling from the SoT lexicon — no raw keyword literals
                expected_ar = "كلمة '" + kw(TT::KEYWORD_END) + "'";
                expected_en = "keyword '" + kw(TT::KEYWORD_END) + "' (end)";
                fixText = kw(TT::KEYWORD_END);
                fixDesc_ar = "أضف '" + kw(TT::KEYWORD_END) + "' لإغلاق الكتلة";
                fixDesc_en = "Add '" + kw(TT::KEYWORD_END) + "' to close block";
                addFixIt = true;
                break;
            case TT::IDENTIFIER:
                expected_ar = "معرّف (اسم متغير أو دالة)";
                expected_en = "identifier (variable or function name)";
                break;
            case TT::OP_ASSIGN:
                expected_ar = "علامة إسناد '='";
                expected_en = "assignment '='";
                break;
            case TT::ARROW:
                expected_ar = "سهم '->'";
                expected_en = "arrow '->'";
                break;
            default:
                expected_ar = "رمز من نوع " + std::to_string(static_cast<int>(type));
                expected_en = "token of type " + std::to_string(static_cast<int>(type));
                break;
            }

            // (AR) إذا كانت رسالة مخصصة موجودة، استخدمها
            // (EN) If custom message exists, use it
            if (!message.empty() && message.find("Expected") == std::string::npos)
            {
                error(message);
            }
            else
            {
                // (AR) إنشاء رسالة تلقائية
                // (EN) Create automatic message
                std::string msg_ar = "خطأ نحوي: توقعت " + expected_ar +
                                     "، لكن وجدت '" + current_.getValue() + "'";

                if (addFixIt)
                {
                    errorWithFixIt(msg_ar, fixText, fixDesc_ar, fixDesc_en);
                }
                else
                {
                    // (AR) عبر الكتالوج المركزي: SYN001 مع {expected}/{found}
                    // (EN) Via the central catalog: SYN001 with {expected}/{found}
                    errorCatalog(Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
                                 {{"found", current_.getValue()},
                                  {"expected", expected_ar + " / " + expected_en}});
                }
            }

            // Return dummy token to allow continuation
            // (AR) إرجاع رمز وهمي للسماح بالاستمرار
            return current_;
        }

        /**
         * @brief (AR) يتحقق من الوصول لنهاية الملف.
         *        (EN) Checks if reached end of file.
         */
        bool ParserCore::isAtEnd() const
        {
            bool result = current_.getType() == TT::END_OF_FILE;
            if (result)
            {
                // DEBUG: Disabled
                // std::cout << "[parser_core_helpers.cpp] isAtEnd() = true - current token type: "
                //           << static_cast<int>(current_.getType()) << "\n";
            }
            return result;
        }

        /**
         * @brief (AR) يرجع الرمز الحالي.
         *        (EN) Returns current token.
         */
        const Token &ParserCore::peek() const
        {
            return current_;
        }

        /**
         * @brief (AR) يرجع الرمز التالي (lookahead 2) بشكل صحيح.
         *        (EN) Returns next token (lookahead 2) correctly.
         *
         * @note (AR) الآن تستخدم nextToken_ المخزن مسبقًا لتوفير نظر مسبق حقيقي.
         *       (EN) Now uses pre-cached nextToken_ to provide true lookahead.
         */
        const Token &ParserCore::peekNext() const
        {
            return nextToken_;
        }

        /**
         * @brief (AR) يرجع الرمز السابق.
         *        (EN) Returns previous token.
         */
        const Token &ParserCore::previous() const
        {
            return previous_;
        }

        /**
         * @brief (AR) يسجل خطأ تحليل مع معلومات المكان.
         *        (EN) Records parsing error with location information.
         */
        void ParserCore::errorWithFixIt(const std::string &message,
                                        const std::string &fixText,
                                        const std::string &fixDesc_ar,
                                        const std::string &fixDesc_en)
        {
            if (panicMode_)
                return; // Avoid error cascades

            panicMode_ = true;

            // (AR) بناء SourceLocation من الرمز الحالي
            // (EN) Build SourceLocation from current token
            Errors::SourceLocation loc(
                filename_.empty() ? "<source>" : filename_,
                current_.getPosition().line,
                current_.getPosition().column,
                current_.getPosition().offset,
                current_.getPosition().length);

            // (AR) إنشاء FixItHint
            // (EN) Create FixItHint
            Errors::FixItHint fixIt(
                Errors::FixItHint::Type::INSERT,
                loc,
                fixText,
                fixDesc_ar,
                fixDesc_en);

            // (AR) إنشاء Diagnostic كامل مع Fix-it hint
            // (EN) Create complete Diagnostic with Fix-it hint
            Errors::Diagnostic diag(
                Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
                Errors::DiagnosticSeverity::ERROR,
                loc,
                message, // Arabic message
                message  // English message (same for now)
            );
            diag.addFixIt(fixIt);

            // (AR) استخدام ErrorManager لتسجيل الخطأ
            // (EN) Use ErrorManager to report error
            Errors::ErrorManager::getInstance().report(diag);
        }

        void ParserCore::error(const std::string &message)
        {
            // (AR) لا نتجاهل الأخطاء حتى في panic mode - نعرضها دائماً
            // (EN) Don't ignore errors even in panic mode - always display them
            // if (panicMode_) return;  // REMOVED: We want to see ALL errors

            panicMode_ = true;

            // (AR) بناء SourceLocation من الرمز الحالي
            // (EN) Build SourceLocation from current token
            Errors::SourceLocation loc(
                filename_.empty() ? "<source>" : filename_,
                current_.getPosition().line,
                current_.getPosition().column,
                current_.getPosition().offset,
                current_.getPosition().length);

            // (AR) إضافة معلومات السياق لرسالة الخطأ
            // (EN) Add context information to error message
            std::string enhanced_ar = "⛔ " + message;
            std::string enhanced_en = "⛔ " + message;

            // إضافة معلومات عن الرمز الحالي
            enhanced_ar += "\n   📍 الموقع: السطر " + std::to_string(loc.line) +
                           "، العمود " + std::to_string(loc.column);
            enhanced_en += "\n   📍 Location: line " + std::to_string(loc.line) +
                           ", column " + std::to_string(loc.column);

            // (AR) إضافة سطر الكود المصدري لتسهيل التصحيح
            // (EN) Include source line content for easier debugging
            {
                const std::string &src = lexer_.getSource();
                size_t targetLine = loc.line;
                size_t currentLine = 1;
                size_t lineStart = 0;
                for (size_t i = 0; i < src.size() && currentLine < targetLine; ++i)
                {
                    if (src[i] == '\n')
                    {
                        ++currentLine;
                        lineStart = i + 1;
                    }
                }
                size_t lineEnd = src.find('\n', lineStart);
                if (lineEnd == std::string::npos)
                    lineEnd = src.size();
                std::string srcLine = src.substr(lineStart, lineEnd - lineStart);

                std::string lineNum = std::to_string(targetLine);
                enhanced_ar += "\n   " + lineNum + " | " + srcLine;
                enhanced_en += "\n   " + lineNum + " | " + srcLine;

                // (AR) سهم يشير للعمود الخاطئ
                // (EN) Arrow pointing to error column
                if (loc.column > 0 && loc.column <= srcLine.size() + 1)
                {
                    std::string arrow(lineNum.size() + 3, ' '); // "   " + lineNum + " | "
                    // حساب المسافة مع دعم UTF-8
                    size_t col = 0;
                    for (size_t i = 0; i < srcLine.size() && col < loc.column - 1; ++col)
                    {
                        unsigned char ch = srcLine[i];
                        if (ch < 0x80)
                            ++i;
                        else if (ch < 0xE0)
                            i += 2;
                        else if (ch < 0xF0)
                            i += 3;
                        else
                            i += 4;
                        arrow += ' ';
                    }
                    arrow += '^';
                    enhanced_ar += "\n   " + arrow;
                    enhanced_en += "\n   " + arrow;
                }
            }

            if (current_.getType() != TokenType::END_OF_FILE)
            {
                enhanced_ar += "\n   🔎 الرمز الحالي: '" + current_.getValue() + "'";
                enhanced_en += "\n   🔎 Current token: '" + current_.getValue() + "'";
            }
            else
            {
                enhanced_ar += "\n   🔎 الرمز الحالي: <نهاية الملف>";
                enhanced_en += "\n   🔎 Current token: <end of file>";
            }

            // (AR) استخدام ErrorManager لتسجيل الخطأ
            // (EN) Use ErrorManager to report error
            Errors::ErrorManager::getInstance().reportError(
                Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
                loc,
                enhanced_ar,
                enhanced_en);

            // (AR) طباعة الخطأ فوراً للمستخدم
            // (EN) Print error immediately for user
            std::cerr << "\n"
                      << enhanced_ar << "\n"
                      << enhanced_en << "\n"
                      << std::endl;
        }

        /**
         * @brief (AR) التهجئة الرئيسية لكلمة مفتاحية من معجم SoT (keywords.yaml).
         *        (EN) Primary spelling of a keyword from the SoT lexicon (keywords.yaml).
         */
        std::string ParserCore::kw(TokenType type)
        {
            const auto *entry = Lexer::KeywordTable::getEntry(type);
            return entry ? entry->primaryWord : std::string("?");
        }

        /**
         * @brief (AR) التهجئة البديلة الأولى من معجم SoT (صيغة التأنيث للصفات، مثل
         *        «خارجية» لـKEYWORD_EXTERN وفق RFC 0034) أو الرئيسية إن غابت البدائل.
         *        (EN) First alias spelling from the SoT lexicon (feminine adjective
         *        form, e.g. 'خارجية' for KEYWORD_EXTERN per RFC 0034), falling back
         *        to the primary spelling when no aliases exist.
         */
        std::string ParserCore::kwAlias(TokenType type)
        {
            const auto *entry = Lexer::KeywordTable::getEntry(type);
            if (!entry)
                return std::string("?");
            return entry->aliases.empty() ? entry->primaryWord : entry->aliases.front();
        }

        /**
         * @brief (AR) يسجل خطأً نحوياً عبر كتالوج الأخطاء المركزي: يبني ar/en من قالب
         *        الكتالوج (مع 💡 تلميح الإصلاح إن وُجد) ويطبع فوراً برمز خطأ ظاهر،
         *        بنفس سلوك errorBilingual (يرفع panicMode ويطبع دائماً).
         *        (EN) Reports a syntax error via the central catalog: renders ar/en from
         *        the template (with 💡 fix hint when present) and prints immediately with
         *        a visible error code, mirroring errorBilingual behavior.
         */
        void ParserCore::errorCatalog(Errors::ErrorCode code, CatalogArgs placeholders)
        {
            panicMode_ = true;

            Errors::SourceLocation loc(
                filename_.empty() ? "<source>" : filename_,
                current_.getPosition().line,
                current_.getPosition().column,
                current_.getPosition().offset,
                current_.getPosition().length);

            Errors::RenderContext rctx(loc);
            rctx.placeholders = std::move(placeholders);

            auto &em = Errors::ErrorManager::getInstance();
            const auto rendered = Errors::ErrorCatalog::instance().render(
                code, em.getExplanationLevel(), Errors::Language::BOTH, rctx);

            // (AR) الرسالة المسجَّلة مركزياً بلا بادئة [SYNxxx] — المقرِّر الذكيّ يطبع
            //      الرمز في ترويسته أصلاً، فالبادئة هنا تعني طباعته مرتين.
            // (EN) Centrally recorded message carries no [SYNxxx] prefix — the smart
            //      reporter already prints the code in its heading (avoid doubling).
            std::string message_ar = rendered.messageAr;
            std::string message_en = rendered.messageEn;
            if (rendered.fixHintAr && !rendered.fixHintAr->empty())
                message_ar += "\n   💡 " + *rendered.fixHintAr;
            if (rendered.fixHintEn && !rendered.fixHintEn->empty())
                message_en += "\n   💡 " + *rendered.fixHintEn;

            // (AR) نفس إخراج errorBilingual: موقع + الرمز الحالي ثم طباعة فورية وتسجيل
            //      (الطباعة الفورية وحدها تحمل بادئة [SYNxxx] الظاهرة)
            // (EN) Same output shape as errorBilingual: location + current token, then
            //      immediate print and central registration (only the immediate print
            //      carries the visible [SYNxxx] prefix)
            const std::string codeStr = Errors::getErrorCodeString(code);
            std::string enhanced_ar = "⛔ [" + codeStr + "] " + message_ar;
            std::string enhanced_en = "⛔ [" + codeStr + "] " + message_en;

            enhanced_ar += "\n   📍 الموقع: السطر " + std::to_string(loc.line) +
                           "، العمود " + std::to_string(loc.column);
            enhanced_en += "\n   📍 Location: line " + std::to_string(loc.line) +
                           ", column " + std::to_string(loc.column);

            if (current_.getType() != TokenType::END_OF_FILE)
            {
                enhanced_ar += "\n   🔎 الرمز الحالي: '" + current_.getValue() + "'";
                enhanced_en += "\n   🔎 Current token: '" + current_.getValue() + "'";
            }
            else
            {
                enhanced_ar += "\n   🔎 الرمز الحالي: <نهاية الملف>";
                enhanced_en += "\n   🔎 Current token: <end of file>";
            }

            em.reportError(code, loc, message_ar, message_en);

            std::cerr << "\n"
                      << enhanced_ar << "\n"
                      << enhanced_en << "\n"
                      << std::endl;
        }

        /**
         * @brief (AR) نظير errorExpectedToken عبر الكتالوج: محروس بوضع الهلع ويسجل
         *        مركزياً بلا طباعة فورية (نفس السلوك القديم).
         *        (EN) Catalog counterpart of errorExpectedToken: panic-guarded and
         *        records centrally without immediate printing (legacy behavior).
         */
        void ParserCore::errorCatalogExpected(Errors::ErrorCode code, CatalogArgs placeholders)
        {
            if (panicMode_)
                return;
            panicMode_ = true;

            Errors::SourceLocation loc(
                filename_.empty() ? "<source>" : filename_,
                current_.getPosition().line,
                current_.getPosition().column,
                current_.getPosition().offset,
                current_.getPosition().length);

            Errors::RenderContext rctx(loc);
            rctx.placeholders = std::move(placeholders);

            auto &em = Errors::ErrorManager::getInstance();
            const auto rendered = Errors::ErrorCatalog::instance().render(
                code, em.getExplanationLevel(), Errors::Language::BOTH, rctx);

            // (AR) بلا بادئة [SYNxxx]: هذه الرسالة تظهر حصراً في التقرير الذكيّ الذي
            //      يطبع الرمز في ترويسته (تفادي الطباعة المزدوجة).
            // (EN) No [SYNxxx] prefix: this message surfaces only in the smart report,
            //      whose heading already prints the code (avoid doubling).
            std::string message_ar = rendered.messageAr +
                                     "، لكن وجدت '" + current_.getValue() + "'";
            std::string message_en = rendered.messageEn +
                                     ", but found '" + current_.getValue() + "'";

            em.reportError(code, loc, message_ar, message_en);
        }

        /**
         * @brief (AR) يحاول التعافي من خطأ بالتقدم للجملة التالية.
         *        (EN) Attempts error recovery by advancing to next statement.
         */
        void ParserCore::synchronize()
        {
            // (AR) عرض رسالة التعافي
            // (EN) Display recovery message
            std::cerr << "\n\xF0\x9F\x94\xA7 (AR) \xD9\x85\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84\xD8\xA9 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB9\xD8\xA7\xD9\x81\xD9\x8A \xD9\x85\xD9\x86 \xD8\xA7\xD9\x84\xD8\xAE\xD8\xB7\xD8\xA3...\n";
            std::cerr << "\xF0\x9F\x94\xA7 (EN) Attempting error recovery...\n"
                      << std::endl;

            panicMode_ = false;

            advance();

            int tokens_skipped = 0;

            // (AR) استخدام نقاط المزامنة من ErrorRecoverySystem + الكلمات المفتاحية
            // (EN) Use sync points from ErrorRecoverySystem + keywords
            const auto &syncPoints = recoverySystem_.getSyncPoints();

            // (AR) موازنة الأقواس — تخطي الكتل المتوازنة
            // (EN) Bracket balancing — skip balanced blocks
            int parenDepth = 0;
            int bracketDepth = 0;
            int braceDepth = 0;

            while (!isAtEnd())
            {
                // (AR) تتبع الأقواس المتداخلة
                // (EN) Track nested brackets
                switch (current_.getType())
                {
                case TT::PAREN_LEFT:
                    parenDepth++;
                    break;
                case TT::PAREN_RIGHT:
                    parenDepth--;
                    break;
                case TT::BRACKET_LEFT:
                    bracketDepth++;
                    break;
                case TT::BRACKET_RIGHT:
                    bracketDepth--;
                    break;
                case TT::BRACE_LEFT:
                    braceDepth++;
                    break;
                case TT::BRACE_RIGHT:
                    braceDepth--;
                    break;
                default:
                    break;
                }

                // (AR) لا نتوقف داخل أقواس متداخلة
                // (EN) Don't stop inside nested brackets
                if (parenDepth > 0 || bracketDepth > 0 || braceDepth > 0)
                {
                    tokens_skipped++;
                    advance();
                    continue;
                }

                // (AR) التوقف عند فاصلة منقوطة سابقة
                // (EN) Stop at previous semicolon
                if (previous().getType() == TT::SEMICOLON ||
                    previous().getType() == TT::ARABIC_SEMICOLON)
                {
                    std::cerr << "\xE2\x9C\x93 (AR) \xD8\xAA\xD9\x85 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB9\xD8\xA7\xD9\x81\xD9\x8A \xD8\xB9\xD9\x86\xD8\xAF \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9 \xD8\xA7\xD9\x84\xD8\xAC\xD9\x85\xD9\x84\xD8\xA9 (" << tokens_skipped << " \xD8\xB1\xD9\x85\xD8\xB2)\n";
                    std::cerr << "\xE2\x9C\x93 (EN) Recovered at statement end (skipped " << tokens_skipped << " tokens)\n\n";
                    return;
                }

                // (AR) التوقف عند كلمات مفتاحية تبدأ جملاً جديدة
                // (EN) Stop at statement-starting keywords
                switch (current_.getType())
                {
                case TT::KEYWORD_CLASS:
                case TT::KEYWORD_FUNCTION:
                case TT::KEYWORD_CONST:
                case TT::KEYWORD_FOR:
                case TT::KEYWORD_IF:
                case TT::KEYWORD_WHILE:
                case TT::KEYWORD_RETURN:
                case TT::KEYWORD_VAR:
                case TT::KEYWORD_END:
                case TT::KEYWORD_IMPORT:
                case TT::KEYWORD_TRY:
                case TT::KEYWORD_STRUCT:
                case TT::KEYWORD_ENUM:
                case TT::KEYWORD_MATCH:
                    std::cerr << "\xE2\x9C\x93 (AR) \xD8\xAA\xD9\x85 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB9\xD8\xA7\xD9\x81\xD9\x8A \xD8\xB9\xD9\x86\xD8\xAF \xD8\xA8\xD8\xAF\xD8\xA7\xD9\x8A\xD8\xA9 \xD8\xAA\xD8\xB5\xD8\xB1\xD9\x8A\xD8\xAD/\xD8\xAC\xD9\x85\xD9\x84\xD8\xA9 \xD8\xAC\xD8\xAF\xD9\x8A\xD8\xAF\xD8\xA9 (" << tokens_skipped << " \xD8\xB1\xD9\x85\xD8\xB2)\n";
                    std::cerr << "\xE2\x9C\x93 (EN) Recovered at new declaration/statement (skipped " << tokens_skipped << " tokens)\n\n";
                    return;
                default:
                    break;
                }

                // (AR) التوقف عند نقاط مزامنة إضافية (حسب قيمة الرمز)
                // (EN) Stop at additional sync points (by token value)
                if (current_.getType() == TT::IDENTIFIER && syncPoints.count(current_.getValue()) > 0)
                {
                    std::cerr << "\xE2\x9C\x93 (AR) \xD8\xAA\xD9\x85 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB9\xD8\xA7\xD9\x81\xD9\x8A \xD8\xB9\xD9\x86\xD8\xAF \xD9\x86\xD9\x82\xD8\xB7\xD8\xA9 \xD9\x85\xD8\xB2\xD8\xA7\xD9\x85\xD9\x86\xD8\xA9 '" << current_.getValue() << "' (" << tokens_skipped << " \xD8\xB1\xD9\x85\xD8\xB2)\n";
                    std::cerr << "\xE2\x9C\x93 (EN) Recovered at sync point '" << current_.getValue() << "' (skipped " << tokens_skipped << " tokens)\n\n";
                    return;
                }

                tokens_skipped++;
                advance();
            }

            std::cerr << "\xE2\x9A\xA0 (AR) \xD9\x88\xD8\xB5\xD9\x84\xD9\x86\xD8\xA7 \xD9\x84\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9 \xD8\xA7\xD9\x84\xD9\x85\xD9\x84\xD9\x81 \xD8\xA3\xD8\xAB\xD9\x86\xD8\xA7\xD8\xA1 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB9\xD8\xA7\xD9\x81\xD9\x8A\n";
            std::cerr << "\xE2\x9A\xA0 (EN) Reached end of file during recovery\n\n";
        }

        /**
         * @brief (AR) يحلل قائمة معاملات الدالة: (x, y, z) أو (x، y، z).
         *        (EN) Parses function parameter list: (x, y, z) or (x، y، z).
         */
        std::vector<std::string> ParserCore::parseParameterList()
        {
            std::vector<std::string> parameters;

            // Parse parameters
            // (AR) تحليل المعاملات
            // (AR) داخل الأقواس () يمكن استخدام الكلمات المحجوزة كأسماء معاملات
            //      مثال: دالة من_إلى(بداية, نهاية) — نهاية هنا اسم معامل وليس منهي كتلة
            // (EN) Inside () context, reserved keywords can be used as parameter names
            //      Example: function fromTo(start, end) — end is a param name, not block terminator
            if (!check(TT::PAREN_RIGHT))
            {
                do
                {
                    Token param(TT::IDENTIFIER, "", Lexer::Position());
                    if (check(TT::IDENTIFIER))
                    {
                        param = current_;
                        advance();
                    }
                    else if (isTokenUsableAsName(current_.getType()))
                    {
                        param = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                        advance();
                    }
                    else
                    {
                        param = consume(TT::IDENTIFIER,
                                        "(AR) توقع اسم معامل. (EN) Expected parameter name.");
                    }
                    parameters.push_back(param.getValue());
                } while (matchComma()); // (AR) دعم الفاصلة العربية (،)
            }

            return parameters;
        }

        /**
         * @brief (AR) يحلل قائمة معاملات مكتوبة: (x: int, y: float = 10) أو (int x, float y = 10).
         *        (EN) Parses typed parameter list: (x: int, y: float = 10) or (int x, float y = 10).
         */
        std::vector<Parameter> ParserCore::parseTypedParameterList()
        {
            std::vector<Parameter> parameters;

            // Parse parameters
            // (AR) تحليل المعاملات
            if (!check(TT::PAREN_RIGHT))
            {
                do
                {
                    // (AR) دعم صيغتين: "نوع اسم" أو "اسم: نوع"
                    // (EN) Support two syntaxes: "type name" or "name: type"
                    Types::SadTypeKind paramType = Types::SadTypeKind::Unknown;

                    // Check if next token is a type keyword (type-first syntax: "رقم س")
                    // (AR) تحقق إذا كان الرمز التالي كلمة مفتاحية لنوع (صيغة النوع أولاً: "رقم س")
                    // (AR) لكن فقط إذا كان بعده معرّف — وإلا يُعامل كاسم معامل
                    // (EN) BUT only if followed by an identifier — otherwise treat as param name
                    if (isTypeToken(current_.getType()) &&
                        peekNext().getType() == TT::IDENTIFIER)
                    {
                        // Type-first syntax: "int x"
                        // (AR) صيغة النوع أولاً: "رقم س"
                        paramType = parseType();
                        Types::SadTypeKind paramInnerTF = lastOptionalInner_;
                        // (AR) دعم استخدام الكلمات المفتاحية الناعمة كأسماء معاملات بعد النوع
                        // (EN) Support soft keywords as parameter names after type annotation
                        Token paramName(TT::IDENTIFIER, "", Lexer::Position());
                        if (check(TT::IDENTIFIER))
                        {
                            paramName = current_;
                            advance();
                        }
                        else if (isTokenUsableAsName(current_.getType()))
                        {
                            paramName = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                            advance();
                        }
                        else
                        {
                            paramName = consume(TT::IDENTIFIER,
                                                "(AR) توقع اسم معامل بعد النوع. (EN) Expected parameter name after type.");
                        }

                        // Optional default value: type name = value
                        // (AR) القيمة الافتراضية الاختيارية: نوع اسم = قيمة
                        ExprPtr defaultValue = nullptr;
                        if (match(TT::OP_ASSIGN))
                        {
                            defaultValue = parseExpression();
                        }

                        // (AR) سباكة النوع الداخليّ للمعامل الاختياريّ T؟ (NS-06 موجة 3)
                        // (EN) Plumb inner type for optional parameter T? (NS-06 wave 3)
                        Types::SadTypePtr paramSadTypeTF = nullptr;
                        if (paramType == Types::SadTypeKind::Optional &&
                            paramInnerTF != Types::SadTypeKind::Unknown)
                        {
                            paramSadTypeTF = Types::SadTypeRegistry::instance().makeOptional(
                                Types::SadType::fromValueType(paramInnerTF));
                        }

                        parameters.emplace_back(
                            paramName.getValue(),
                            paramType,
                            std::move(defaultValue),
                            "",
                            std::move(paramSadTypeTF));
                    }
                    else if (check(TT::IDENTIFIER) && peekNext().getType() == TT::IDENTIFIER)
                    {
                        // ═══════════════════════════════════════════════════════════════
                        // (AR) صيغة نوع الصنف: "اسم_صنف اسم_معامل"
                        //      مثال: "شخص ش" أو "سيارة س"
                        //      يُعامل المعرّف الأول كاسم صنف والمعرّف الثاني كاسم المعامل
                        //
                        // (EN) Class-type syntax: "ClassName paramName"
                        //      Example: "Person p" or "Car c"
                        //      Treats first identifier as class type, second as parameter name
                        // ═══════════════════════════════════════════════════════════════
                        std::string className = current_.getValue();
                        advance(); // (AR) تخطي اسم الصنف / (EN) skip class name
                        // (AR) دعم الكلمات المفتاحية الناعمة كأسماء معاملات بعد اسم الصنف
                        // (EN) Support soft keywords as parameter names after class type
                        Token paramName(TT::IDENTIFIER, "", Lexer::Position());
                        if (check(TT::IDENTIFIER))
                        {
                            paramName = current_;
                            advance();
                        }
                        else if (isTokenUsableAsName(current_.getType()))
                        {
                            paramName = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                            advance();
                        }
                        else
                        {
                            paramName = consume(TT::IDENTIFIER,
                                                "(AR) توقع اسم معامل بعد اسم الصنف. (EN) Expected parameter name after class type.");
                        }

                        // (AR) القيمة الافتراضية الاختيارية
                        // (EN) Optional default value
                        ExprPtr defaultValue = nullptr;
                        if (match(TT::OP_ASSIGN))
                        {
                            defaultValue = parseExpression();
                        }

                        parameters.emplace_back(
                            paramName.getValue(),
                            Types::SadTypeKind::Class,
                            std::move(defaultValue),
                            className);
                    }
                    else
                    {
                        // Name-first syntax: "x: int"
                        // (AR) صيغة الاسم أولاً: "س: رقم"
                        // (AR) دعم الكلمات المفتاحية الناعمة كأسماء معاملات (مثل: نوع، حجم، احصل)
                        // (EN) Support soft keywords as parameter names (e.g., نوع, حجم, احصل)
                        Token paramName(TT::IDENTIFIER, "", Lexer::Position());
                        if (check(TT::IDENTIFIER))
                        {
                            paramName = current_;
                            advance();
                        }
                        else if (isTokenUsableAsName(current_.getType()) || isTypeToken(current_.getType()))
                        {
                            paramName = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                            advance();
                        }
                        else
                        {
                            paramName = consume(TT::IDENTIFIER,
                                                "(AR) توقع اسم معامل. (EN) Expected parameter name.");
                        }

                        // Optional type annotation: name : type
                        // (AR) تصريح النوع الاختياري: اسم : نوع
                        Types::SadTypeKind paramInnerNF = Types::SadTypeKind::Unknown;
                        if (match(TT::COLON))
                        {
                            paramType = parseType();
                            paramInnerNF = lastOptionalInner_;
                        }

                        // Optional default value: name : type = value
                        // (AR) القيمة الافتراضية الاختيارية: اسم : نوع = قيمة
                        ExprPtr defaultValue = nullptr;
                        if (match(TT::OP_ASSIGN))
                        {
                            defaultValue = parseExpression();
                        }

                        // (AR) سباكة النوع الداخليّ للمعامل الاختياريّ T؟ (NS-06 موجة 3)
                        // (EN) Plumb inner type for optional parameter T? (NS-06 wave 3)
                        Types::SadTypePtr paramSadTypeNF = nullptr;
                        if (paramType == Types::SadTypeKind::Optional &&
                            paramInnerNF != Types::SadTypeKind::Unknown)
                        {
                            paramSadTypeNF = Types::SadTypeRegistry::instance().makeOptional(
                                Types::SadType::fromValueType(paramInnerNF));
                        }

                        parameters.emplace_back(
                            paramName.getValue(),
                            paramType,
                            std::move(defaultValue),
                            "",
                            std::move(paramSadTypeNF));
                    }

                } while (matchComma()); // (AR) دعم الفاصلة العربية (،)
            }

            return parameters;
        }

        /**
         * @brief (AR) يحلل قائمة وسائط استدعاء: f(1, 2, 3) أو f(1، 2، 3).
         *        يدعم الوسائط المسمّاة بصيغة Flutter: عمود(تباعد: 10، خلفية: "ابيض")
         *        (EN) Parses function call argument list: f(1, 2, 3) or f(1، 2، 3).
         *        Supports Flutter-like named arguments: column(spacing: 10, bg: "white")
         */
        ExprList ParserCore::parseArgumentList()
        {
            ExprList arguments;

            // Parse arguments
            // (AR) تحليل الوسائط
            if (!check(TT::PAREN_RIGHT))
            {
                do
                {
                    // ─── كشف الوسائط المسمّاة: مُعرّف : تعبير ───
                    // (EN) Detect named arguments: IDENTIFIER : expression
                    // (AR) إذا كان الرمز الحالي مُعرّف (أو كلمة تصلح كاسم) والتالي نقطتان
                    //      فهذا وسيط مسمّى (مثل Flutter)
                    // (AR) يدعم أيضاً: خطأ: قيمة (LITERAL_FALSE)، صحيح: قيمة (LITERAL_TRUE)، لاشيء: قيمة
                    bool isNamedArgCandidate = (check(TT::IDENTIFIER) || isTokenUsableAsName(current_.getType()) ||
                                                check(TT::LITERAL_FALSE) || check(TT::LITERAL_TRUE) || check(TT::LITERAL_NULL)) &&
                                               nextToken_.getType() == TT::COLON;
                    // (AR) تحقق إضافي: IDENTIFIER = قيمة (صيغة بديلة للوسائط المسمّاة)
                    bool isNamedArgEquals = (check(TT::IDENTIFIER) || isTokenUsableAsName(current_.getType()) ||
                                             check(TT::LITERAL_FALSE) || check(TT::LITERAL_TRUE) || check(TT::LITERAL_NULL)) &&
                                            nextToken_.getType() == TT::OP_ASSIGN;
                    if (isNamedArgCandidate || isNamedArgEquals)
                    {
                        // (AR) وسيط مسمّى: اسم: قيمة
                        std::string argName = current_.getValue();
                        auto pos = current_.getPosition();
                        advance(); // (AR) تجاوز اسم الوسيط
                        advance(); // (AR) تجاوز ':'
                        auto value = parseExpression();
                        arguments.push_back(
                            std::make_unique<NamedArgExpr>(argName, std::move(value), pos));
                    }
                    else
                    {
                        arguments.push_back(parseExpression());
                    }
                } while (matchComma()); // (AR) دعم الفاصلة العربية (،)
            }

            return arguments;
        }

        // ======================================================================
        // (AR) دوال نظام الأنواع / (EN) Type System Functions
        // ======================================================================

        /**
         * @brief (AR) يحلل نوع بيانات أساسي أو مركب.
         *        (EN) Parses basic or composite data type.
         *
         * @details (AR) تدعم الأنواع الأساسية بالعربية والإنجليزية:
         *               - رقم / int → INTEGER
         *               - عشري / float → FLOAT
         *               - نص / string → STRING
         *               - منطقي / bool → BOOLEAN
         *               - فراغ / void → NONE
         *               - مصفوفة / array → ARRAY (with optional generic params)
         *               - قاموس / dict/map → MAP (with optional generic params)
         *
         *          (EN) Supports basic types in Arabic and English:
         *               - رقم / int → INTEGER
         *               - عشري / float → FLOAT
         *               - نص / string → STRING
         *               - منطقي / bool → BOOLEAN
         *               - فراغ / void → NONE
         *               - مصفوفة / array → ARRAY (with optional generic params)
         *               - قاموس / dict/map → MAP (with optional generic params)
         */
        // (AR) [S-TS-P4] غلاف parseType: يحلّل النوع الأساس ثم يستهلك لاحقة `؟` الاختيارية.
        //      `رقم؟` → Optional. parseType يُستدعى في مواضع الأنواع فقط (لا وسط تعبير)،
        //      فلا غموض مع الثلاثي `أ ؟ ب : ج`. (النوع الداخلي الغنيّ لـOptional<T> عبر
        //      sadType في العقد — تمثيل أغنى مخطّط لاحقًا؛ هنا على مستوى الـkind.)
        // (EN) [S-TS-P4] parseType wrapper: parse base type then consume optional `?` suffix.
        //      parseType is only called in type positions, so no ternary ambiguity.
        Types::SadTypeKind ParserCore::parseType()
        {
            lastOptionalInner_ = Types::SadTypeKind::Unknown;
            Types::SadTypeKind base = parseTypeCore();
            if (check(TT::QUESTION))
            {
                advance(); // consume '?'
                // (AR) [NS-06 موجة 2] احفظ النوع الأساس T لبناء Optional<T> غنيّ لاحقًا.
                // (EN) [NS-06 wave 2] remember base T to build a rich Optional<T> later.
                lastOptionalInner_ = base;
                return Types::SadTypeKind::Optional;
            }
            return base;
        }

        Types::SadTypeKind ParserCore::parseTypeCore()
        {
            // ========== الأنواع الأساسية - من رموز TYPE_* ==========
            // Basic Types - from TYPE_* tokens (legacy support)

            if (match(TT::TYPE_INTEGER))
                return Types::SadTypeKind::Integer;
            if (match(TT::TYPE_DOUBLE))
                return Types::SadTypeKind::Float;
            if (match(TT::TYPE_STRING))
                return Types::SadTypeKind::String;
            if (match(TT::TYPE_BOOLEAN))
                return Types::SadTypeKind::Boolean;
            if (match(TT::TYPE_VOID))
                return Types::SadTypeKind::Void;
            if (match(TT::TYPE_NULL))
                return Types::SadTypeKind::Void;

            if (match(TT::TYPE_ARRAY))
            {
                if (check(TT::OP_LESS))
                    return parseGenericType(Types::SadTypeKind::Array);
                return Types::SadTypeKind::Array;
            }
            if (match(TT::TYPE_MAP))
            {
                if (check(TT::OP_LESS))
                    return parseGenericType(Types::SadTypeKind::Map);
                return Types::SadTypeKind::Map;
            }

            // ========== الأنواع كمُعرّفات مدمجة ==========
            // Built-in type identifiers (no longer reserved keywords)
            // (AR) أنواع البيانات أصبحت مُعرّفات عادية يتعرف عليها المحلل سياقياً
            if (check(TT::IDENTIFIER))
            {
                const std::string &name = current_.getValue();
                Types::SadTypeKind resolved = Types::SadTypeKind::Unknown;
                if (name == "رقم")
                    resolved = Types::SadTypeKind::Integer;
                else if (name == "عشري")
                    resolved = Types::SadTypeKind::Float;
                else if (name == "مضاعف")
                {
                    // (AR) ❌ كلمة `مضاعف` أُزيلت — استخدم `عشري`
                    error("(AR) ❌ `مضاعف` لم تعد مدعومة. استخدم `عشري` بدلاً منها.\n"
                          "(EN) `مضاعف` is no longer supported. Use `عشري` instead.");
                    resolved = Types::SadTypeKind::Float; // recover
                }
                else if (name == "نص")
                    resolved = Types::SadTypeKind::String;
                else if (name == "منطقي")
                    resolved = Types::SadTypeKind::Boolean;
                else if (name == "فراغ")
                    resolved = Types::SadTypeKind::Void;
                else if (name == "عدم")
                    resolved = Types::SadTypeKind::Void;
                else if (name == "مصفوفة")
                    resolved = Types::SadTypeKind::Array;
                else if (name == "خريطة")
                    resolved = Types::SadTypeKind::Map;
                else if (name == "أي")
                    resolved = Types::SadTypeKind::Class;

                if (resolved != Types::SadTypeKind::Unknown)
                {
                    advance(); // consume the identifier
                    if ((resolved == Types::SadTypeKind::Array || resolved == Types::SadTypeKind::Map) && check(TT::OP_LESS))
                    {
                        return parseGenericType(resolved);
                    }
                    return resolved;
                }
            }

            // ========== Type not found ==========
            error("(AR) توقع نوع بيانات صحيح (رقم، نص، منطقي، إلخ). "
                  "(EN) Expected valid data type (int, string, bool, etc).");
            return Types::SadTypeKind::Unknown;
        }

        /**
         * @brief (AR) يحلل نوع عام مع معاملات: Array<T>, Map<K, V>.
         *        (EN) Parses generic type with parameters: Array<T>, Map<K, V>.
         *
         * @param baseType (AR) النوع الأساسي (ARRAY أو MAP).
         *                 (EN) Base type (ARRAY or MAP).
         *
         * @details (AR) مثال: Array<int> → يحلل int كمعامل عام
         *               مثال: Map<string, float> → يحلل string و float
         *
         *          (EN) Example: Array<int> → parses int as generic parameter
         *               Example: Map<string, float> → parses string and float
         *
         * @note (AR) التنفيذ الحالي يقرأ المعاملات العامة لكن لا يخزنها.
         *            سيتم تحسينه في المرحلة التالية لدعم Type Checking الكامل.
         *
         *       (EN) Current implementation reads generic parameters but doesn't store them.
         *            Will be enhanced in next phase to support full Type Checking.
         */
        Types::SadTypeKind ParserCore::parseGenericType(Types::SadTypeKind baseType)
        {
            // Consume '<'
            consume(TT::OP_LESS,
                    "(AR) توقع '<' بعد اسم النوع العام. "
                    "(EN) Expected '<' after generic type name.");

            // Parse first type parameter
            // (AR) تحليل معامل النوع الأول
            Types::SadTypeKind param1 = parseType();
            (void)param1; // Suppress unused variable warning

            // For Map type, parse second parameter
            // (AR) للنوع Map، تحليل المعامل الثاني
            if (baseType == Types::SadTypeKind::Map)
            {
                consume(TT::COMMA,
                        "(AR) توقع ',' بين معاملات Map. "
                        "(EN) Expected ',' between Map parameters.");

                Types::SadTypeKind param2 = parseType();
                (void)param2; // Suppress unused variable warning
            }

            // Consume '>'
            consume(TT::OP_GREATER,
                    "(AR) توقع '>' بعد معاملات النوع العام. "
                    "(EN) Expected '>' after generic type parameters.");

            // TODO: Store generic parameters in AST for type checking
            // (AR) مستقبلاً: حفظ المعاملات العامة في AST للتحقق من الأنواع

            return baseType;
        }

        // ======================================================================
        // (AR) تحليل النوع الموحد / (EN) Unified Type Parsing
        // ======================================================================

        /**
         * @brief (AR) يحلل نوع بيانات ويُرجع SadTypePtr من النظام الموحد
         *        (EN) Parses data type and returns SadTypePtr from unified system
         *
         * (AR) هذه الدالة تحلل نوع البيانات وتُرجع SadTypePtr مباشرة.
         *      تدعم الأنواع البدائية والمركبة والعامة.
         *      للأنواع المركبة (مصفوفة<رقم>، خريطة<نص، رقم>)
         *      تُخزِّن المعاملات العامة فعلياً في SadType.
         *
         * @return SadTypePtr — nullptr إذا فشل التحليل
         */
        Types::SadTypePtr ParserCore::parseSadType()
        {
            auto &reg = Types::SadTypeRegistry::instance();

            // ========== الأنواع الأساسية من رموز TYPE_* ==========
            if (match(TT::TYPE_INTEGER))
                return reg.getInteger();
            if (match(TT::TYPE_DOUBLE))
                return reg.getFloat();
            if (match(TT::TYPE_STRING))
                return reg.getString();
            if (match(TT::TYPE_BOOLEAN))
                return reg.getBoolean();
            if (match(TT::TYPE_VOID))
                return reg.getVoid();
            if (match(TT::TYPE_NULL))
                return reg.getVoid();

            if (match(TT::TYPE_ARRAY))
            {
                if (check(TT::OP_LESS))
                {
                    // (AR) تحليل المعامل العام: مصفوفة<T>
                    advance(); // consume '<'
                    auto elemType = parseSadType();
                    consume(TT::OP_GREATER,
                            "(AR) توقع '>' بعد نوع عنصر المصفوفة. "
                            "(EN) Expected '>' after array element type.");
                    return reg.makeArray(elemType);
                }
                return reg.makeArray();
            }
            if (match(TT::TYPE_MAP))
            {
                if (check(TT::OP_LESS))
                {
                    // (AR) تحليل المعاملين: خريطة<K, V>
                    advance(); // consume '<'
                    auto keyType = parseSadType();
                    consume(TT::COMMA,
                            "(AR) توقع ',' بين معاملات الخريطة. "
                            "(EN) Expected ',' between Map parameters.");
                    auto valType = parseSadType();
                    consume(TT::OP_GREATER,
                            "(AR) توقع '>' بعد معاملات الخريطة. "
                            "(EN) Expected '>' after Map parameters.");
                    return reg.makeMap(keyType, valType);
                }
                return reg.makeMap();
            }

            // ========== الأنواع كمُعرّفات مدمجة ==========
            if (check(TT::IDENTIFIER))
            {
                const std::string &name = current_.getValue();
                Types::SadTypePtr resolved = nullptr;

                if (name == "رقم")
                    resolved = reg.getInteger();
                else if (name == "عشري")
                    resolved = reg.getFloat();
                else if (name == "مضاعف")
                {
                    error("(AR) ❌ `مضاعف` لم تعد مدعومة. استخدم `عشري` بدلاً منها.\n"
                          "(EN) `مضاعف` is no longer supported. Use `عشري` instead.");
                    resolved = reg.getFloat();
                }
                else if (name == "نص")
                    resolved = reg.getString();
                else if (name == "منطقي")
                    resolved = reg.getBoolean();
                else if (name == "فراغ")
                    resolved = reg.getVoid();
                else if (name == "عدم")
                    resolved = reg.getVoid();
                else if (name == "أي")
                    resolved = reg.getAny();
                else if (name == "بايت")
                    resolved = reg.getByte();
                else if (name == "مصفوفة")
                {
                    advance(); // consume the identifier
                    if (check(TT::OP_LESS))
                    {
                        advance(); // consume '<'
                        auto elemType = parseSadType();
                        consume(TT::OP_GREATER,
                                "(AR) توقع '>' بعد نوع عنصر المصفوفة. "
                                "(EN) Expected '>' after array element type.");
                        return reg.makeArray(elemType);
                    }
                    return reg.makeArray();
                }
                else if (name == "خريطة")
                {
                    advance(); // consume the identifier
                    if (check(TT::OP_LESS))
                    {
                        advance(); // consume '<'
                        auto keyType = parseSadType();
                        consume(TT::COMMA,
                                "(AR) توقع ',' بين معاملات الخريطة. "
                                "(EN) Expected ',' between Map parameters.");
                        auto valType = parseSadType();
                        consume(TT::OP_GREATER,
                                "(AR) توقع '>' بعد معاملات الخريطة. "
                                "(EN) Expected '>' after Map parameters.");
                        return reg.makeMap(keyType, valType);
                    }
                    return reg.makeMap();
                }

                if (resolved)
                {
                    advance(); // consume the identifier
                    return resolved;
                }

                // (AR) اسم صنف مُعرَّف من المستخدم
                std::string className = name;
                advance();
                return reg.getOrCreateClass(className);
            }

            // (AR) فشل التحليل
            error("(AR) توقع نوع بيانات صحيح (رقم، نص، منطقي، إلخ). "
                  "(EN) Expected valid data type (int, string, bool, etc).");
            return reg.getUnknown();
        }

        // ======================================================================
        // (AR) دوال فحص النوع / (EN) Type Checking Functions
        // ======================================================================

        /**
         * @brief (AR) يفحص ما إذا كان الرمز الحالي رمز نوع بيانات.
         *        (EN) Checks if current token is a data type token.
         *
         * (AR) يتحقق من أن الرمز الحالي يمثل نوع بيانات أساسي (رقم، عشري، نص، إلخ)
         * (EN) Checks if current token represents a basic data type (int, float, string, etc)
         */
        bool ParserCore::isTypeToken(TokenType tokenType)
        {
            using TT = TokenType;

            // (AR) أنواع البيانات من رموز TYPE_* (دعم قديم)
            // (EN) TYPE_* tokens (legacy support)
            if (tokenType == TT::TYPE_INTEGER || tokenType == TT::TYPE_DOUBLE ||
                tokenType == TT::TYPE_STRING || tokenType == TT::TYPE_BOOLEAN ||
                tokenType == TT::TYPE_VOID || tokenType == TT::TYPE_NULL ||
                tokenType == TT::TYPE_ARRAY || tokenType == TT::TYPE_MAP)
            {
                return true;
            }

            // (AR) أنواع البيانات كمُعرّفات مدمجة — فقط عندما يليها معرّف آخر (اسم متغير)
            //      هذا يمنع الالتباس بين "رقم" كاسم نوع و"رقم" كاسم متغير
            // (EN) Built-in type identifiers — only when followed by another identifier (var name)
            //      This prevents ambiguity between "رقم" as type and "رقم" as variable name
            if (tokenType == TT::IDENTIFIER)
            {
                const std::string &name = current_.getValue();
                bool isTypeName = (name == "رقم" || name == "عشري" || name == "مضاعف" || name == "نص" ||
                                   name == "منطقي" || name == "فراغ" || name == "عدم" ||
                                   name == "مصفوفة" || name == "خريطة" || name == "أي");
                if (isTypeName)
                {
                    // (AR) تحقق من أن الرمز التالي هو معرّف (اسم متغير/حقل) أو لاحقة
                    //      الاختياريّ '؟' (نوع اختياريّ يليه الاسم: دالة نص؟ جد(...)) — NS-06.
                    // (EN) Next token must be an identifier (var/field name) or the optional
                    //      suffix '?' (optional type before the name: `function نص؟ f(...)`) — NS-06.
                    TokenType nextType = peekNext().getType();
                    return nextType == TT::IDENTIFIER || nextType == TT::QUESTION;
                }
            }

            return false;
        }

        // ======================================================================
        // (AR) هل الكلمة المفتاحية يمكن استخدامها كمعرّف (اسم دالة أو معامل)؟
        // (EN) Can this keyword be used as an identifier (function/parameter name)?
        // ======================================================================
        bool ParserCore::isKeywordUsableAsName(TokenType tokenType)
        {
            using TT = TokenType;
            // (AR) بعد تقليص الكلمات المفتاحية إلى 40، نرفض فقط الكلمات البنيوية
            // (EN) After reducing to 40 keywords, only reject structural keywords
            switch (tokenType)
            {
            // (AR) كلمات بنية الكتل / (EN) Block structure keywords
            case TT::KEYWORD_FUNCTION:
            case TT::KEYWORD_CLASS:
            case TT::KEYWORD_END:
            case TT::KEYWORD_CONSTRUCTOR:
            case TT::KEYWORD_STRUCT:
            case TT::KEYWORD_ENUM:
            // (AR) التحكم في التدفق / (EN) Control flow
            case TT::KEYWORD_IF:
            case TT::KEYWORD_ELSE:
            case TT::KEYWORD_WHILE:
            case TT::KEYWORD_FOR:
            case TT::KEYWORD_RETURN:
            case TT::KEYWORD_MATCH:
            case TT::KEYWORD_WHEN:
            case TT::KEYWORD_DEFAULT:
            // (AR) المتغيرات / (EN) Variables
            case TT::KEYWORD_VAR:
            case TT::KEYWORD_CONST:
            case TT::KEYWORD_STATIC:
            // (AR) الأخطاء / (EN) Error handling
            case TT::KEYWORD_TRY:
            case TT::KEYWORD_CATCH:
            case TT::KEYWORD_THROW:
            case TT::KEYWORD_FINALLY:
            // (AR) الاستيراد / (EN) Modules
            case TT::KEYWORD_IMPORT:
            case TT::KEYWORD_FROM:
            case TT::KEYWORD_AS:
            case TT::KEYWORD_EXPORT:
            // (AR) الكائنات / (EN) Objects
            case TT::KEYWORD_THIS:
            case TT::KEYWORD_INHERITS:
            case TT::KEYWORD_ABSTRACT:
            // (AR) أخرى / (EN) Others
            case TT::KEYWORD_EXTERN:
            case TT::KEYWORD_BREAK:
            case TT::KEYWORD_CONTINUE:
            case TT::KEYWORD_IN:
                return false;
            // (AR) جديد وأساس — يمكن استخدامها كأسماء معاملات/متغيرات
            // (EN) NEW and SUPER — allowed as parameter/variable names
            case TT::KEYWORD_NEW:
            case TT::KEYWORD_SUPER:
                return true;
            default:
                // (AR) الكلمات المفتاحية غير المسجلة في الـ 40 (لن يُنتجها المعجم بعد الآن)
                //      لكن نحافظ على التوافق مع الأنواع القديمة في token.h
                // (EN) Keywords not in the 40 (lexer won't produce them anymore)
                //      but maintain compatibility with legacy types in token.h
                return tokenType >= TT::KEYWORD_FUNCTION && tokenType <= TT::KEYWORD_COMPTIME;
            }
        }

        // ======================================================================
        // (AR) هل الرمز يمكن استخدامه كاسم في سياقات محددة (بعد نقطة، اسم دالة، معامل، إلخ)؟
        // (EN) Can this token be used as a name in specific contexts (after dot, method name, param, etc.)?
        // ======================================================================
        bool ParserCore::isTokenUsableAsName(TokenType tokenType)
        {
            // (AR) أولاً: تحقق من isKeywordUsableAsName
            if (isKeywordUsableAsName(tokenType))
                return true;
            // (AR) ثانياً: تحقق من أنواع البيانات
            if (isTypeToken(tokenType))
                return true;

            using TT = TokenType;
            switch (tokenType)
            {
            // (AR) الحروف المحجوزة — تُستخدم كأسماء خواص/دوال (مثل: ألوان.خطأ، دالة صحيح())
            // (EN) Literals — used as property/method names (e.g., colors.error, function true())
            case TT::LITERAL_TRUE:  // صحيح
            case TT::LITERAL_FALSE: // خطأ
            case TT::LITERAL_NULL:  // لاشيء
            // (AR) كلمات مفتاحية تُستخدم كأسماء في stdlib (ارجع كاسم دالة، افتراضي كاسم حقل، إلخ)
            // (EN) Keywords used as names in stdlib (return as method name, default as field name, etc.)
            case TT::KEYWORD_RETURN:      // ارجع
            case TT::KEYWORD_DEFAULT:     // افتراضي
            case TT::KEYWORD_FROM:        // من
            case TT::KEYWORD_IN:          // في
            case TT::KEYWORD_EXPORT:      // صدّر
            case TT::KEYWORD_IMPORT:      // استورد
            case TT::KEYWORD_CONSTRUCTOR: // باني
            case TT::KEYWORD_THROW:       // ارمي
            case TT::KEYWORD_CATCH:       // امسك
            case TT::KEYWORD_CLASS:       // صنف (for functions like صنّف)
            case TT::KEYWORD_ABSTRACT:    // مجرد
            case TT::KEYWORD_EXTERN:      // خارجي (تعداد.خارجي)
            case TT::KEYWORD_PUBLIC:      // عام (تعداد.عام)
            case TT::KEYWORD_PRIVATE:     // خاص (تعداد.خاص)
            case TT::KEYWORD_PROTECTED:   // محمي (تعداد.محمي)
            case TT::KEYWORD_STATIC:      // ساكن (تعداد.ساكن)
            case TT::KEYWORD_BREAK:       // توقف (تعداد.توقف)
            case TT::KEYWORD_CONTINUE:    // استمر (تعداد.استمر)
            case TT::KEYWORD_NEW:         // جديد (متغير جديد = ...)
            case TT::KEYWORD_SUPER:       // الأساس (معامل اسمه الأساس)
            // (AR) كلمات بنيوية يمكن استخدامها كأسماء داخل الأقواس/بعد متغير/ثابت
            // (EN) Structural keywords allowed as names in param/var contexts
            case TT::KEYWORD_END:      // نهاية (دالة من_إلى(بداية, نهاية))
            case TT::KEYWORD_VAR:      // متغير (معامل اسمه متغير)
            case TT::KEYWORD_CONST:    // ثابت (معامل اسمه ثابت)
            case TT::KEYWORD_THIS:     // هذا (استخدام كاسم)
            case TT::KEYWORD_TRY:      // حاول
            case TT::KEYWORD_FINALLY:  // أخيراً
            case TT::KEYWORD_INHERITS: // يرث
            case TT::KEYWORD_FUNCTION: // دالة (دالة بعد(تأخير، دالة))
            case TT::KEYWORD_IF:       // إذا (استخدام كاسم)
            case TT::KEYWORD_WHILE:    // بينما
            case TT::KEYWORD_FOR:      // لكل
            case TT::KEYWORD_MATCH:    // طابق
            case TT::KEYWORD_WHEN:     // عندما
            case TT::KEYWORD_ELSE:     // وإلا
            case TT::KEYWORD_ELSE_IF:  // وإلا_إذا
            case TT::KEYWORD_ENUM:     // تعداد
            case TT::KEYWORD_STRUCT:   // بنية
            case TT::OP_AND:           // و (used as method name like .و())
            case TT::OP_OR:            // أو (used as method name like .أو())
            case TT::OP_NOT:           // ليس (used as method name like .ليس())
                return true;
            default:
                return false;
            }
        }

        Types::SadTypeKind ParserCore::mapTokenTypeToKind(TokenType tokenType)
        {
            using TT = TokenType;

            switch (tokenType)
            {
            case TT::TYPE_INTEGER:
                return Types::SadTypeKind::Integer;
            case TT::TYPE_DOUBLE:
                return Types::SadTypeKind::Float;
            case TT::TYPE_STRING:
                return Types::SadTypeKind::String;
            case TT::TYPE_BOOLEAN:
                return Types::SadTypeKind::Boolean;
            case TT::TYPE_VOID:
                return Types::SadTypeKind::Void;
            case TT::TYPE_NULL:
                return Types::SadTypeKind::Void;
            case TT::TYPE_ARRAY:
                return Types::SadTypeKind::Array;
            case TT::TYPE_MAP:
                return Types::SadTypeKind::Map;
            default:
                return Types::SadTypeKind::Unknown;
            }
        }

        bool ParserCore::isClassName(const std::string &name)
        {
            // (AR) التحقق من أن المعرّف هو اسم صنف مسجّل في ClassManager
            // (EN) Check if identifier is registered class name in ClassManager

            auto *classManager = Data::ClassManager::getInstance();
            return classManager->hasClass(name);
        }

    } // namespace Parser
} // namespace Sad
