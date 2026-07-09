// بسم الله الرحمن الرحيم
/**
 * @file repl_engine.cpp
 * @brief تنفيذ محرك REPL للغة ص / REPL Engine Implementation
 *
 * @author Sad Language Team
 * @date January 2026
 */

#include "repl_engine.h"
#include "repl_colors.h"    // (AR) ثوابت ألوان ANSI مشتركة (م-2) / (EN) shared ANSI color constants (م-2)
#include "repl_sot_generated.h" // (AR) كتالوج «مصدر حقيقة الأدوات» — أخطاء/رسائل REPL / (EN) Tool-SoT catalog
#include "statements.h"     // (AR) AST::ExprStmt لاستخراج تعبير :type / (EN) AST::ExprStmt for :type expression extraction
#include "lexer_keywords.h" // (AR) استعلام كلمات فتح/إغلاق الكتل من SoT / (EN) query block opener/closer keywords from SoT
#include <iostream>
#include <fstream>
#include <sstream>

// (AR) DEF-002: جسر تطبيق سياسة الذاكرة الموحَّد. الـ REPL لا يحلّل أعلام CLI
//      لسياسة الذاكرة بعد، لذا الاستدعاء أدناه `policySet=false` ⇒ no-op.
//      يبقى حاضراً كنقطة استعداد: عند إضافة أعلام للـ REPL لاحقاً يكفي تمرير
//      الإعدادات الفعلية + true بدون إعادة هندسة.
// (EN) DEF-002: unified policy bridge. REPL does not parse CLI flags yet, so
//      we invoke with policySet=false (no-op) — placeholder for future wiring.
#include "memory/gc/policy_bridge.h"
#include "memory/policy/gc_mode.h"
#ifndef SAD_ENABLE_GRAPHICS
#define SAD_ENABLE_GRAPHICS 1 // (AR) افتراضيًّا مفعّلة؛ يُعطّل ببناء headless
#endif
#if SAD_ENABLE_GRAPHICS
#include "ui/sad_graphics_bridge.h" // (AR) م2-أ: تثبيت جسر الواجهات (REPL مضيف مفسّر تفاعليّ)
#endif

// Windows VOID macro conflicts with ::Sad::Types::SadTypeKind::Void
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <io.h>    // (AR) _isatty / (EN) _isatty
#include <cstdio>  // (AR) _fileno / (EN) _fileno
#else
#include <termios.h>
#include <unistd.h>
#endif

#ifdef VOID
#undef VOID
#endif

// (AR) يُضمَّن عمدًا بعد <unistd.h> العالميّ (الكتلة أعلاه): utf8_utils.h يُضمّن
//      <unistd.h> داخل فضاء sad::utf8، فلو سبق لَسمّم حارسُه التضمينَ العالميّ فاختفى
//      isatty من النطاق العامّ (يفشل بناء Linux/GCC، بينما MSVC يستعمل _isatty فلا يظهر).
//      هنا حارس <unistd.h> مضبوطٌ سلفًا، فيبقى isatty في النطاق العامّ.
// (EN) Included AFTER the global <unistd.h> (block above) on purpose: utf8_utils.h
//      includes <unistd.h> INSIDE namespace sad::utf8, so if it came first its include
//      guard would keep isatty out of global scope (Linux/GCC build failure; MSVC uses
//      _isatty so it never surfaced). Here <unistd.h>'s guard is already set, so isatty
//      stays at global scope.
#include "utf8_utils.h" // (AR) sad::utf8::read_file لفتح المسارات العربيّة / (EN) sad::utf8::read_file for Arabic paths

namespace Sad
{
    namespace REPL
    {

        // ============================================================================
        // Helper Functions / دوال مساعدة
        // ============================================================================

        // C++17 compatible starts_with
        inline bool starts_with(const std::string &str, const std::string &prefix)
        {
            if (prefix.length() > str.length())
                return false;
            return str.compare(0, prefix.length(), prefix) == 0;
        }

        // (AR) أدوار SoT الدلاليّة (مُعرَّفة في language-truth/keywords.yaml) — نصّها
        //      مفتاحُ استعلامٍ في KeywordTable::getKeywordsByRole. نُركّزها هنا في
        //      ثابتين مسمّيين بدل تكرارها كسلاسل مباشرة.
        // (EN) SoT semantic roles (defined in language-truth/keywords.yaml) — these
        //      are query keys for KeywordTable::getKeywordsByRole. Centralized here
        //      as named constants instead of repeated raw strings.
        static const char *const kRoleBlockOpener = "block_opener";
        static const char *const kRoleBlockCloser = "block_closer";
        static const char *const kRoleInterBlock = "inter_block";

        // ============================================================================
        // Color Codes / أكواد الألوان
        // ============================================================================

        // (AR) الثوابت مُعرَّفة مرّة واحدة في repl_colors.h (م-2: إزالة التكرار).
        // (EN) constants defined once in repl_colors.h (م-2: de-duplicated).
        using namespace Colors;

        // ============================================================================
        // REPLEngine Implementation / تنفيذ REPLEngine
        // ============================================================================

        REPLEngine::REPLEngine(const REPLConfig &config)
            : config_(config), state_(REPLState::Ready)
        {
            // Initialize color support on Windows / تفعيل دعم الألوان على ويندوز
#ifdef _WIN32
            if (config_.enableColor)
            {
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                DWORD dwMode = 0;
                GetConsoleMode(hOut, &dwMode);
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
#endif

            // Initialize components / تهيئة المكونات
            if (config_.enableHistory)
            {
                history_ = std::make_unique<HistoryManager>(
                    config_.maxHistorySize,
                    config_.historyFile);
                history_->load();
            }

            commands_ = std::make_unique<REPLCommands>(this);

            // (AR) استيراد كلمات فتح/إغلاق الكتل من SoT المعجم (لا قائمة مضمّنة).
            //      تُستعمل لكشف اكتمال الكتل متعدّدة الأسطر (… نهاية) في isBufferIncomplete.
            // (EN) Import block opener/closer keywords from the lexicon SoT (no inline
            //      list). Used by isBufferIncomplete to detect multiline block completeness.
            Lexer::KeywordTable::initialize();

            // (AR) مُساعِد: يُدرج التهجئة الرئيسيّة + كلَّ بدائلها (aliases) من SoT.
            //      حاسم لأنّ المعجم يجرّد التشكيل (الشدّة) من قيمة الرمز: «عيّن» يُقطَّع
            //      «عين» و«نفّذ» يُقطَّع «نفذ». الفحص في isBufferIncomplete يطابق قيمة
            //      الرمز (المجرَّدة) بمجموعاتنا، فلو أدرجنا التهجئة المُشكَّلة وحدها لما
            //      طابقت ⇒ يُفوَّت فاتح/مُغلِّق الكتلة. البدائل في SoT هي الأشكال المجرَّدة.
            // (EN) Helper: inserts a keyword's primary spelling PLUS all its SoT aliases.
            //      Critical because the lexer strips diacritics (shadda) from a token's
            //      value: «عيّن» tokenizes as «عين», «نفّذ» as «نفذ». isBufferIncomplete
            //      matches the (stripped) token value against our sets, so inserting only
            //      the diacritized spelling would never match ⇒ a missed block opener/
            //      closer. The SoT aliases are exactly those stripped forms.
            auto addWordAndAliases =
                [](std::unordered_set<std::string> &set, const std::string &word)
            {
                set.insert(word);
                if (const auto *e = Lexer::KeywordTable::getEntry(word))
                {
                    for (const auto &alias : e->aliases)
                    {
                        set.insert(alias);
                    }
                }
            };

            for (const auto &w : Lexer::KeywordTable::getKeywordsByRole(kRoleBlockOpener))
            {
                addWordAndAliases(blockOpeners_, w);
            }
            for (const auto &w : Lexer::KeywordTable::getKeywordsByRole(kRoleBlockCloser))
            {
                addWordAndAliases(blockClosers_, w);
            }

            // (AR) تعويض فجوة أدوار SoT: «احصل/عيّن» (getter/setter داخل خاصيّة) يفتحان
            //      جسمًا يُغلَق بـ«نهاية» (احصل() ... نهاية)، لكنّ جدول الأدوار لا يَسِمهما
            //      block_opener، فيبقى «نهاية» جسمِهما بلا فاتح مقابل ⇒ يهبط عمق الكتلة
            //      باكرًا فتُنفَّذ كتلة الصنف المتعدّدة الأسطر قبل «نهاية» الخاصيّة/الصنف
            //      (خطأ نحويّ: توقّع «نهاية»). نعدّهما فاتحين محلّيًّا في REPL بتهجئة SoT
            //      (getEntry لا getKeyword: كلمتان سياقيّتان غير مُصدَّرتين فيرمي getKeyword؛
            //      addWordAndAliases يضيف «عين» المجرَّدة لأنّ «عيّن» تُقطَّع كذلك). الإصلاح
            //      الجذريّ (وسمهما block_opener في language-truth) خارج نطاق tools/repl.
            // (EN) Compensate a SoT-role gap: «احصل/عيّن» (property getter/setter) open a
            //      «نهاية»-terminated body (احصل() ... نهاية) yet the role table does NOT
            //      tag them block_opener, so their closing «نهاية» has no matching opener ⇒
            //      block depth drops early and a multiline class body executes before the
            //      property/class «نهاية» (syntax error: expected «نهاية»). We count them as
            //      openers locally in the REPL using their SoT spelling (getEntry, not
            //      getKeyword: both are non-emitted contextual keywords so getKeyword would
            //      throw; addWordAndAliases also adds the stripped «عين» since «عيّن»
            //      tokenizes that way). The root fix (tagging them block_opener in
            //      language-truth) is out of scope for tools/repl here.
            for (const Lexer::TokenType propAccessor :
                 {Lexer::TokenType::KEYWORD_GET, Lexer::TokenType::KEYWORD_SET})
            {
                if (const auto *entry = Lexer::KeywordTable::getEntry(propAccessor))
                {
                    addWordAndAliases(blockOpeners_, entry->primaryWord);
                }
            }

            // (AR) «لامدا» ليست فاتحًا كتليًّا عامًّا: رغم وسمها block_opener في SoT، لها
            //      ثلاث صيغ (راجع parser_expressions::parseLambda) تختلف في حاجتها لـ«نهاية»:
            //        (١) لامدا(م) => تعبير            ⇐ بلا «نهاية» (تعبيريّة، تكتمل بالسطر).
            //        (٢) لامدا(م) => جملة… نهاية       ⇐ بـ«نهاية» (جسم كتليّ يبدأ بكلمة جملة).
            //        (٣) لامدا(م) جسم… نهاية           ⇐ بـ«نهاية» (بلا سهم — كتلة).
            //      عدُّها فاتحًا دائمًا يكسر (١) [multiline خطأً + ابتلاع «:خروج»]، ونزعُها
            //      كليًّا يكسر (٢)/(٣) [تُنفَّذ الكتلة متعدّدة الأسطر باكرًا]. الحلّ: ننزعها من
            //      المجموعة العامّة، ونحتفظ بتهجئاتها في lambdaWords_ ليعالجها isBufferIncomplete
            //      بنظرةٍ أماميّة تُطابق قرار المحلّل تمامًا (سهمٌ يتبعه تعبير ⇒ لا «نهاية»).
            // (EN) «لامدا» is NOT a blanket block opener: though tagged block_opener in SoT, it
            //      has three forms (see parser_expressions::parseLambda) that differ on «نهاية»:
            //        (1) لامدا(m) => expression   ⇒ NO «نهاية» (expression form, completes on line).
            //        (2) لامدا(m) => stmt… نهاية   ⇒ needs «نهاية» (block body starting with a stmt kw).
            //        (3) لامدا(m) body… نهاية      ⇒ needs «نهاية» (no arrow — block).
            //      Counting it always breaks (1) [wrong multiline + swallows «:exit»]; removing it
            //      entirely breaks (2)/(3) [a multiline block lambda executes prematurely]. Fix:
            //      drop it from the generic set and keep its spellings in lambdaWords_ so
            //      isBufferIncomplete decides per-occurrence with a lookahead mirroring the
            //      parser's exact rule (an arrow followed by an expression ⇒ no «نهاية»).
            if (const auto *lambdaEntry =
                    Lexer::KeywordTable::getEntry(Lexer::TokenType::KEYWORD_LAMBDA))
            {
                blockOpeners_.erase(lambdaEntry->primaryWord);
                lambdaWords_.insert(lambdaEntry->primaryWord);
                for (const auto &alias : lambdaEntry->aliases)
                {
                    blockOpeners_.erase(alias);
                    lambdaWords_.insert(alias);
                }
            }

            // (AR) الكلمات البينيّة (وإلا/امسك/عندما/…) — تُميّز «وإلا إذا» (else-if)
            //      عن كتلة جديدة، فلا يُعَدّ فاتحُها فتحًا مستقلًّا. من SoT كذلك.
            // (EN) inter-block keywords (else/catch/when/…) — used to tell «وإلا إذا»
            //      (else-if) from a new block so its opener isn't counted. From SoT too.
            for (const auto &w : Lexer::KeywordTable::getKeywordsByRole(kRoleInterBlock))
            {
                addWordAndAliases(blockContinuations_, w);
            }

            // (AR) الإدخال الغنيّ الاختياريّ: يُفعَّل فقط عند طلبه صراحةً (--rich)
            //      ووجود طرفيّة تفاعليّة. خلاف ذلك يبقى getline (المسار المُتحقَّق).
            //      عند التوجيه بأنبوب (اختبارات/سكربتات) لا طرفيّة ⇒ getline حتمًا.
            // (EN) Optional rich input: enabled only when explicitly requested (--rich)
            //      AND stdin is an interactive TTY. Otherwise plain getline (verified
            //      path). Under a pipe (tests/scripts) there is no TTY ⇒ always getline.
#ifdef _WIN32
            const bool stdinIsTty = _isatty(_fileno(stdin)) != 0;
#else
            const bool stdinIsTty = isatty(STDIN_FILENO) != 0;
#endif
            useLineEditor_ = config_.enableLineEditor && stdinIsTty;
            if (useLineEditor_)
            {
                completer_ = std::make_unique<AutoCompleter>();
                lineEditor_ = std::make_unique<LineEditor>(history_.get(), completer_.get());
            }

            // Initialize interpreter / تهيئة المفسر
            // (AR) م2-أ: تثبيت جسر الواجهات قبل المفسّر (عديم التكرار، يشمل مفسّر
            //      إعادة التعيين لأنّ التثبيت حالة عالميّة) كي يدعم REPL الرسومات.
            // (EN) Phase 2-A: install the UI bridge before the interpreter (idempotent,
            //      global — also covers the reset interpreter) so the REPL supports UI.
#if SAD_ENABLE_GRAPHICS
            Sad::Interpreter::installSadGraphicsBridge();
#endif

            Interpreter::InterpreterOptions interpOpts;
            interpOpts.enableDebugMode = false;
            interpOpts.printResults = false;
            interpreter_ = std::make_unique<Interpreter::Interpreter>(interpOpts);

            // (AR) DEF-002: نقطة استعداد لجسر سياسة الذاكرة (الآن no-op لعدم
            //      وجود أعلام CLI في REPL). راجع التعليق أعلى الملف.
            // (EN) DEF-002: ready-point for the policy bridge (no-op today).
            ::Sad::Memory::MemoryModeSettings replPolicy{};
            ::Sad::Memory::GC::applyMemoryPolicyGlobal(replPolicy,
                                                       /*policySet=*/false,
                                                       /*debugMode=*/false);

            // ErrorManager is a singleton, no need to initialize
            // errorManager_ = std::make_unique<Errors::ErrorManager>();
        }

        REPLEngine::~REPLEngine()
        {
            // Save history / حفظ التاريخ
            if (history_ && config_.enableHistory)
            {
                history_->save();
            }
        }

        int REPLEngine::run()
        {
            printHeader();

            while (state_ != REPLState::Exiting)
            {
                try
                {
                    // (AR) محرّر السطر يطبع موجّهه بنفسه؛ في وضع getline نطبعه هنا.
                    // (EN) The line editor prints its own prompt; in getline mode we do it here.
                    if (!useLineEditor_)
                    {
                        printPrompt();
                    }
                    std::string line = readLine();

                    // (AR) بلغنا EOF (نهاية الإدخال/الأنبوب، أو Ctrl+D): إن بقي في مخزن
                    //      الأسطر المتعدّدة كتلةٌ لم تُنفَّذ، نفّذها قبل الخروج بدل إسقاطها
                    //      صامتًا — يُظهر نتيجتها أو خطأها النحويّ (لا نُخفي فشلًا).
                    // (EN) EOF reached (end of input/pipe, or Ctrl+D): if a block remains
                    //      in the multiline buffer, execute it before exiting instead of
                    //      silently dropping it — surfaces its result or syntax error.
                    if (state_ == REPLState::Exiting)
                    {
                        if (!multilineBuffer_.empty())
                        {
                            runMultilineBuffer();
                        }
                        break;
                    }

                    if (!processLine(line))
                    {
                        break;
                    }
                }
                catch (const std::exception &e)
                {
                    printError(SoT::errorMessage(SoT::Error::INTERNAL, e.what()));
                    state_ = REPLState::Ready;
                }
            }

            const std::string goodbye = SoT::messageBoth(SoT::Message::GOODBYE);
            if (config_.enableColor)
            {
                std::cout << GREEN << goodbye << RESET << std::endl;
            }
            else
            {
                std::cout << goodbye << std::endl;
            }

            return 0;
        }

        // (AR) تصفير تشخيصات المفرد قبل كلّ تحليلٍ جديد. تفصيل السبب في إعلان الدالّة
        //      (repl_engine.h). يُستدعى من evaluate و loadFile — نقطتا التحليل التفاعليّتان.
        // (EN) Clear singleton diagnostics before each fresh parse. Rationale on the
        //      declaration (repl_engine.h). Called by evaluate and loadFile — the two
        //      interactive parse entry points.
        void REPLEngine::resetDiagnostics()
        {
            Sad::Errors::ErrorManager::getInstance().clear();
        }

        std::string REPLEngine::evaluate(const std::string &line)
        {
            try
            {
                // (AR) استرداد الصدَفة: صفّر تشخيصات السطر السابق قبل تحليل هذا السطر.
                // (EN) Shell recovery: clear the prior line's diagnostics before parsing this one.
                resetDiagnostics();

                // Tokenize and Parse / التحليل المعجمي والنحوي
                Lexer::LexerCore lexer(line);
                Parser::ParserCore parser(lexer);
                auto ast = parser.parseProgram();

                // (AR) ع-1: افحص أخطاء التحليل **قبل** ast.empty() — فخطأٌ نحويّ قد يُنتج
                //      شجرةً فارغة، فلو رجعنا "" عند الفراغ أوّلًا لابتُلع الخطأ صامتًا
                //      (المستخدم يظنّ إدخاله الخاطئ صحيحًا فارغ النتيجة). loadFile يعتمد
                //      الترتيب الصحيح أصلًا؛ هذا يوحّد المسارين.
                // (EN) ع-1: check parse errors **before** ast.empty() — a syntax error may
                //      yield an empty tree, so returning "" on empty first would swallow the
                //      error silently (user thinks bad input was valid-and-empty). loadFile
                //      already uses the correct order; this unifies both paths.
                if (parser.hasErrors())
                {
                    std::string errors;
                    for (const auto &err : parser.getErrors())
                    {
                        if (!errors.empty())
                            errors += "\n";
                        errors += err;
                    }
                    lastResultIsError_ = true;
                    return SoT::errorMessage(SoT::Error::SYNTAX, errors);
                }

                if (ast.empty())
                {
                    // (AR) لا خطأ لكن لا قيمة (مثل سطر تعليقٍ فقط). / (EN) No error but no value (e.g. comment-only line).
                    lastResultIsError_ = false;
                    return "";
                }

                // (AR) تنفيذ تدريجيّ عبر المفسّر الدائم: جملةً جملةً بـexecuteStatement،
                //      لا execute(program) الكامل لكلّ سطر. فـexecute() يُعيد كامل إعداد
                //      البرنامج (فحص قاعدة الدالة الرئيسيّة + مرحلة تسجيل التصريحات) في
                //      كلّ استدعاء، ما يُفسد الحالة عند الاستدعاء المتكرّر في REPL (مخرج
                //      بائت/انهيار تعريف الدالّة). أمّا executeStatement فينفّذ الجملة على
                //      المُنفِّذ الدائم statementExecutor_ مباشرةً، فتثبُت المتغيّرات والدوال
                //      عبر الأسطر بلا إعادة إعداد ولا تلف. (المفسّر نفسه سليم — تُثبته
                //      حزمة الاختبارات؛ العلّة كانت في إساءة REPL استعمال execute.)
                // (EN) Incremental execution over the persistent interpreter: one statement
                //      at a time via executeStatement — NOT the whole-program execute() per
                //      line. execute() re-runs full-program setup (main-function rule check +
                //      declaration-registration phase) on every call, corrupting state when
                //      called repeatedly in a REPL (stale output / function-def crash).
                //      executeStatement runs the statement directly on the persistent
                //      statementExecutor_, so variables and functions persist across lines
                //      with no re-setup and no corruption. (The interpreter itself is sound —
                //      proven by the test suite; the bug was the REPL misusing execute.)
                // (AR) انقل شجرة السطر بأكملها إلى مرسى الأشجار قبل تنفيذها كي تبقى حيّةً
                //      طوال الجلسة (لا تُعاد عناوين عقدها ولا تُحرَّر رموزها/أجسام دوالها) —
                //      يمنع تلف كاش الحرفيّات البائت والمؤشّرات المعلّقة في المفسّر الدائم.
                // (EN) Move the whole line's tree into the AST arena before executing so it
                //      stays alive for the session (node addresses aren't reused, tokens/function
                //      bodies aren't freed) — prevents stale literal-cache and dangling pointers.
                astArena_.push_back(std::move(ast));
                AST::StmtList &live = astArena_.back();

                Interpreter::ExecutionResult result(true);
                for (auto &stmt : live)
                {
                    if (!stmt)
                    {
                        continue;
                    }
                    result = interpreter_->executeStatement(*stmt);
                    if (!result.success)
                    {
                        lastResultIsError_ = true;
                        return SoT::errorMessage(SoT::Error::EVAL, result.errorMessage);
                    }
                }

                lastResultIsError_ = false;
                // Return result value if not void / إرجاع القيمة إن لم تكن فارغة
                if (result.result.getKind() != Types::SadTypeKind::Void)
                {
                    return result.result.toString();
                }

                return "";
            }
            catch (const std::exception &e)
            {
                lastResultIsError_ = true;
                return SoT::errorMessage(SoT::Error::EVAL, e.what());
            }
        }

        bool REPLEngine::exprTypeName(const std::string &expr,
                                      std::string &typeNameOut,
                                      std::string &errorOut)
        {
            try
            {
                // (AR) استرداد الصدَفة: صفّر تشخيصات ما سبق قبل تحليل هذا التعبير.
                // (EN) Shell recovery: clear prior diagnostics before parsing this expression.
                resetDiagnostics();

                Lexer::LexerCore lexer(expr);
                Parser::ParserCore parser(lexer);
                auto ast = parser.parseProgram();

                if (parser.hasErrors())
                {
                    std::string errors;
                    for (const auto &err : parser.getErrors())
                    {
                        if (!errors.empty())
                            errors += "\n";
                        errors += err;
                    }
                    errorOut = SoT::errorMessage(SoT::Error::SYNTAX, errors);
                    return false;
                }

                if (ast.empty())
                {
                    errorOut = SoT::errorMessage(SoT::Error::EMPTY_EXPRESSION);
                    return false;
                }

                // (AR) نتوقّع جملة تعبير مفردة. نُبقي شجرتها في المرسى (كسائر الأشجار) ثمّ
                //      نقيّم تعبيرها **مباشرةً** عبر evaluateExpression — لا كجملة، إذ جملة
                //      التعبير المجرَّدة تُرجع Void فتُفقَد قيمتها (ومعها نوعها).
                // (EN) Expect a single expression statement. Keep its tree in the arena (like
                //      all trees), then evaluate its expression DIRECTLY via evaluateExpression
                //      — not as a statement, since a bare expression statement returns Void,
                //      losing the value (and its type).
                astArena_.push_back(std::move(ast));
                AST::StmtList &live = astArena_.back();
                auto *exprStmt = dynamic_cast<AST::ExprStmt *>(live.front().get());
                if (!exprStmt || !exprStmt->expression)
                {
                    errorOut = SoT::errorMessage(SoT::Error::NOT_AN_EXPRESSION);
                    return false;
                }

                Data::Value value = interpreter_->evaluateExpression(*exprStmt->expression);
                typeNameOut = std::string(::Sad::Types::sadTypeKindArabicName(value.getKind())) + " / " + value.getTypeName();
                return true;
            }
            catch (const std::exception &e)
            {
                errorOut = SoT::errorMessage(SoT::Error::EVAL, e.what());
                return false;
            }
        }

        std::string REPLEngine::evaluateMultiline(const std::vector<std::string> &lines)
        {
            // Combine lines / دمج الأسطر
            std::string combined;
            for (const auto &line : lines)
            {
                combined += line + "\n";
            }

            return evaluate(combined);
        }

        bool REPLEngine::loadFile(const std::string &filename)
        {
            // (AR) نستعمل مساعد UTF-8 المشترك (sad::utf8::read_file) الذي يفتح المسارات
            //      العربيّة/اليونيكود على Windows عبر wstring — كما يفعل المفسّر sad-run.
            //      std::ifstream(std::string) يستخدم صفحة ANSI فيفشل مع الأسماء العربيّة.
            // (EN) Use the shared UTF-8 helper (sad::utf8::read_file) which opens Arabic/
            //      Unicode paths on Windows via wstring — exactly like the sad-run interpreter.
            //      std::ifstream(std::string) uses the ANSI codepage and fails on Arabic names.
            auto content = sad::utf8::read_file(filename);
            if (!content)
            {
                printError(SoT::errorMessage(SoT::Error::FILE_OPEN, filename));
                return false;
            }

            // (AR) نحلّل الملفّ كبرنامجٍ كامل وننفّذه عبر execute() (لا حلقة executeStatement
            //      السطريّة) كي تُطبَّق قاعدة الدالة الرئيسيّة (SEM018) وتُستدعى «رئيسية» إن
            //      وُجدت — وهو سلوك تحميل السكربت الصحيح. نُبقي شجرته في المرسى فيأمن تحميلُ
            //      ملفٍّ فيه دوال/أصناف من use-after-free (المفسّر يحمل مؤشّرات لأجسامها).
            // (EN) Parse the file as a whole program and run it via execute() (not the
            //      per-line executeStatement loop) so the main-function rule (SEM018) applies
            //      and «رئيسية» (main) is invoked if present — correct script-loading behavior.
            //      Keep its tree in the arena so loading a file with functions/classes is safe
            //      from use-after-free (the interpreter holds pointers into their bodies).
            try
            {
                // (AR) نفس بوّابة المفرد: صفّر كي لا يسمّم سطرٌ/تحميلٌ سابق هذا التحميل
                //      (الأمر :load). عائلة العلّة نفسها التي في evaluate.
                // (EN) Same singleton gate: clear so a prior line/load can't poison this
                //      load (the :load command). Same bug family as evaluate.
                resetDiagnostics();

                Lexer::LexerCore lexer(*content);
                Parser::ParserCore parser(lexer);
                auto ast = parser.parseProgram();

                if (parser.hasErrors())
                {
                    std::string errors;
                    for (const auto &err : parser.getErrors())
                    {
                        if (!errors.empty())
                            errors += "\n";
                        errors += err;
                    }
                    printError(SoT::errorMessage(SoT::Error::SYNTAX, errors));
                    return false;
                }

                if (ast.empty())
                {
                    return true;
                }

                astArena_.push_back(std::move(ast));
                auto result = interpreter_->execute(astArena_.back());

                if (!result.success)
                {
                    printError(SoT::errorMessage(SoT::Error::EVAL, result.errorMessage));
                    return false;
                }

                if (result.result.getKind() != Types::SadTypeKind::Void)
                {
                    std::cout << result.result.toString() << std::endl;
                }

                return true;
            }
            catch (const std::exception &e)
            {
                printError(SoT::errorMessage(SoT::Error::EVAL, e.what()));
                return false;
            }
        }

        void REPLEngine::reset()
        {
            // Reset interpreter state / إعادة تعيين حالة المفسر
            Interpreter::InterpreterOptions interpOpts;
            interpOpts.enableDebugMode = false;
            interpOpts.printResults = false;
            interpreter_ = std::make_unique<Interpreter::Interpreter>(interpOpts);

            // (AR) DEF-002: نفس نقطة الاستعداد عند إعادة التهيئة.
            // (EN) DEF-002: same ready-point on reset.
            ::Sad::Memory::MemoryModeSettings replPolicy{};
            ::Sad::Memory::GC::applyMemoryPolicyGlobal(replPolicy,
                                                       /*policySet=*/false,
                                                       /*debugMode=*/false);

            // Clear multiline buffer / مسح المخزن متعدد الأسطر
            multilineBuffer_.clear();
            // (AR) المفسّر أُعيد إنشاؤه (كاش نظيف)، فيمكن تحرير مرسى الأشجار القديم بأمان.
            // (EN) The interpreter was recreated (clean cache), so the old AST arena is safe to free.
            astArena_.clear();
            if (completer_)
            {
                completer_->clearUserIdentifiers();
            }
            state_ = REPLState::Ready;

            std::cout << SoT::messageBoth(SoT::Message::RESET_DONE) << std::endl;
        }

        bool REPLEngine::processLine(const std::string &line)
        {
            // (AR) أضِف للتاريخ الأسطرَ غير الفارغة على المستوى الأعلى فقط
            // (EN) Add only non-empty top-level lines to history
            if (history_ && !line.empty() && state_ == REPLState::Ready)
            {
                history_->add(line);
            }

            // (AR) الأوامر الخاصّة (:help/:exit/...) تُعالَج على المستوى الأعلى فقط،
            //      لا في منتصف كتلة متعدّدة الأسطر (كي لا تُختطَف أسطرٌ تبدأ بـ':').
            // (EN) Special commands are handled only at top level, not mid-block.
            if (state_ != REPLState::MultiLine && commands_->isCommand(line))
            {
                return commands_->process(line);
            }

            // (AR) داخل كتلة متعدّدة الأسطر / (EN) Inside a multiline block
            if (state_ == REPLState::MultiLine)
            {
                // (AR) سطر فارغ = مخرج طوارئ: نفّذ المتراكم فورًا مهما كانت حالته
                // (EN) empty line = escape hatch: execute what we have immediately
                if (line.empty())
                {
                    runMultilineBuffer();
                    return true;
                }
                multilineBuffer_.push_back(line);
                // (AR) تكتمل الكتلة عندما لا يبقى في كامل المخزن كتلةٌ مفتوحة (… نهاية)
                //      ولا قوسٌ مفتوح. نُعيد الفحص على كامل المخزن، فلا يتراكم خطأٌ عبر
                //      الأسطر — هذا يُصلح ح-2 (العدّاد التراكميّ الذي كان يبقى > 0).
                // (EN) The block completes once nothing in the whole buffer is left open
                //      (… نهاية) and no bracket is open. We re-check the whole buffer, so
                //      no error accumulates across lines — this fixes H-2 (the stuck counter).
                std::string joined;
                for (const auto &l : multilineBuffer_)
                {
                    joined += l + "\n";
                }
                if (!isBufferIncomplete(joined))
                {
                    runMultilineBuffer();
                }
                return true;
            }

            // (AR) المستوى الأعلى، حالة الجاهزيّة / (EN) top level, Ready state
            if (line.empty())
            {
                return true;
            }
            if (isBufferIncomplete(line))
            {
                // (AR) كتلة/قوس مفتوح ⇒ ابدأ إدخالًا متعدّد الأسطر
                // (EN) an open block/bracket ⇒ begin multiline input
                multilineBuffer_.clear();
                multilineBuffer_.push_back(line);
                state_ = REPLState::MultiLine;
                return true;
            }

            // (AR) سطر مكتمل ⇒ نفّذه مباشرة / (EN) a complete single line ⇒ execute directly
            processCode(line);
            return true;
        }

        bool REPLEngine::processCommand(const std::string &command)
        {
            return commands_->process(command);
        }

        void REPLEngine::processCode(const std::string &code)
        {
            if (code.empty())
            {
                return;
            }

            state_ = REPLState::Executing;
            std::string result = evaluate(code);
            state_ = REPLState::Ready;

            // (AR) م-1: ميّز الأخطاء (⇒ printError: stderr أحمر، دائمًا) عن القيم (⇒ stdout
            //      عند printResults)، معتمِدًا علَم evaluate لا استنتاجًا نصّيًّا هشًّا من بادئة
            //      الرسالة. كان الكلّ يُطبع على stdout بلا تمييز.
            // (EN) م-1: distinguish errors (⇒ printError: red stderr, always) from values
            //      (⇒ stdout when printResults), using evaluate's flag rather than fragile
            //      string-sniffing. Previously everything went to stdout undistinguished.
            if (!result.empty())
            {
                if (lastResultIsError_)
                {
                    printError(result);
                }
                else if (config_.printResults)
                {
                    std::cout << result << std::endl;
                }
            }

            // (AR) غذِّ المُكمِّل بمُعرّفات المستخدم المُعرَّفة في هذا الكود
            // (EN) feed the completer with user identifiers defined in this code
            if (completer_)
            {
                extractIdentifiers(code);
            }
        }

        void REPLEngine::runMultilineBuffer()
        {
            if (!multilineBuffer_.empty())
            {
                std::string result = evaluateMultiline(multilineBuffer_);
                // (AR) م-1: توجيه الأخطاء إلى stderr كما في processCode / (EN) route errors to stderr, as in processCode
                if (!result.empty())
                {
                    if (lastResultIsError_)
                    {
                        printError(result);
                    }
                    else if (config_.printResults)
                    {
                        std::cout << result << std::endl;
                    }
                }
                if (completer_)
                {
                    std::string joined;
                    for (const auto &l : multilineBuffer_)
                    {
                        joined += l + "\n";
                    }
                    extractIdentifiers(joined);
                }
            }
            multilineBuffer_.clear();
            state_ = REPLState::Ready;
        }

        // (AR) هل يفتح «لامدا» (عند tokens[li]) كتلةً تنتظر «نهاية»؟ يُطابق قرار المحلّل
        //      (parser_expressions::parseLambda): بعد «لامدا(معاملات)» إن جاء «=>» متبوعًا
        //      بتعبير ⇒ لا «نهاية» (تعبيريّة، تكتمل بالسطر)؛ وإن جاء «=>» متبوعًا بكلمة جملة
        //      (إذا/بينما/لكل/ارجع/…) أو لم يأتِ «=>» أصلًا ⇒ كتلة تُغلَق بـ«نهاية». نتحفّظ
        //      (نعدّه كتلةً) متى انقطع المخزن قبل تحديد الصيغة فيبقى في وضع متعدّد الأسطر.
        // (EN) Does the «لامدا» at tokens[li] open a «نهاية»-terminated block? Mirrors the
        //      parser (parser_expressions::parseLambda): after «لامدا(params)», a «=>» followed
        //      by an expression ⇒ no «نهاية» (expression form, completes on the line); a «=>»
        //      followed by a statement keyword (if/while/for/return/…), or no «=>» at all ⇒ a
        //      block closed by «نهاية». When the buffer is cut before the form is decided we
        //      conservatively say "block" so the REPL keeps reading (stays multiline).
        static bool lambdaRequiresEnd(const std::vector<Lexer::Token> &tokens, size_t li)
        {
            using TT = Lexer::TokenType;
            const size_t n = tokens.size();

            // (AR) تخطّي التعليقات فقط (معاملات لامدا لا تحوي سلاسل) / skip comments only
            auto nextSignificant = [&](size_t k) -> size_t
            {
                while (k < n)
                {
                    const TT tt = tokens[k].getType();
                    if (tt == TT::COMMENT || tt == TT::DOC_COMMENT)
                        ++k;
                    else
                        break;
                }
                return k;
            };

            size_t j = nextSignificant(li + 1);
            // (AR) المعاملات إلزاميّة الأقواس؛ إن لم نرَ «(» بعدُ فالمخزن مقطوع ⇒ تحفّظ
            // (EN) params require parens; if we don't see «(» yet, the buffer is cut ⇒ conservative
            if (j >= n || tokens[j].getValue() != "(")
                return true;

            // (AR) تخطّي مجموعة أقواس المعاملات المتوازنة / skip the balanced param parens
            int depth = 0;
            for (; j < n; ++j)
            {
                const std::string &val = tokens[j].getValue();
                if (val == "(")
                    ++depth;
                else if (val == ")")
                {
                    --depth;
                    if (depth == 0)
                    {
                        ++j;
                        break;
                    }
                }
            }
            if (depth != 0)
                return true; // (AR) أقواس معاملات غير مغلقة ⇒ تحفّظ / unclosed params ⇒ conservative

            j = nextSignificant(j);
            if (j >= n)
                return true; // (AR) لا شيء بعد المعاملات بعدُ ⇒ انتظر / nothing after params yet ⇒ wait

            // (AR) الصيغة السهميّة «=>»: تعبيريّة (بلا «نهاية») إلا إن بدأ الجسم بكلمة جملة
            // (EN) arrow form «=>»: expression (no «نهاية») unless the body starts with a stmt keyword
            if (tokens[j].getType() == TT::FAT_ARROW)
            {
                const size_t k = nextSignificant(j + 1);
                if (k >= n)
                    return true; // (AR) «=>» بلا جسم بعدُ ⇒ انتظر / arrow with no body yet ⇒ wait
                const TT bt = tokens[k].getType();
                // (AR) نفس مجموعة كلمات جسم الكتلة في المحلّل (parseLambda) تمامًا
                // (EN) exactly the parser's block-body keyword set (parseLambda)
                return bt == TT::KEYWORD_IF || bt == TT::KEYWORD_WHILE || bt == TT::KEYWORD_FOR ||
                       bt == TT::KEYWORD_RETURN || bt == TT::KEYWORD_VAR || bt == TT::KEYWORD_CONST ||
                       bt == TT::KEYWORD_TRY || bt == TT::KEYWORD_THROW || bt == TT::KEYWORD_MATCH ||
                       bt == TT::KEYWORD_BREAK || bt == TT::KEYWORD_CONTINUE;
            }

            // (AR) بلا «=>» ⇒ صيغة كتليّة تُغلَق بـ«نهاية» / no «=>» ⇒ block form closed by «نهاية»
            return true;
        }

        bool REPLEngine::isBufferIncomplete(const std::string &src) const
        {
            // (AR) نقطّع المصدر بالمعجم ونعدّ عمق الكتل بكلمات الفتح/الإغلاق (من SoT)
            //      وتوازن الأقواس ‎(‎ ‎[‎. نتجاهل السلاسل والتعليقات كي لا تُعَدّ رموزها.
            // (EN) Tokenize via the lexer and count block depth by SoT opener/closer
            //      keywords plus ( / [ balance. Strings & comments are skipped.
            try
            {
                Lexer::LexerCore lexer(src);
                std::vector<Lexer::Token> tokens = lexer.tokenize();

                int blockDepth = 0; // (AR) كتل الكلمات (… نهاية) / keyword blocks
                int parenDepth = 0; // (AR) الأقواس ( و [ / parentheses & brackets

                // (AR) تتبّع الرمز المفيد السابق للتمييز بين «وإلا إذا» (else-if: فاتح
                //      كتلة يتبع كلمةَ inter_block على نفس السطر ⇒ استمرارٌ يشترك في
                //      «نهاية» واحدة، لا يُعَدّ) و«وإلا» ثمّ «إذا» متداخلة على سطر لاحق
                //      (كتلة جديدة تُعَدّ). القيد المعروف: else-if مكسور عبر سطرين.
                // (EN) Track the previous meaningful token to tell «وإلا إذا» (else-if:
                //      a block opener right after an inter_block keyword on the SAME line
                //      ⇒ a continuation sharing one «نهاية», not counted) apart from a
                //      nested «إذا» inside an «وإلا» body on a later line (a new block,
                //      counted). Known limit: an else-if split across two lines.
                bool havePrev = false;
                std::string prevValue;
                size_t prevLine = 0;

                for (size_t i = 0; i < tokens.size(); ++i)
                {
                    const Lexer::Token &tok = tokens[i];
                    const Lexer::TokenType t = tok.getType();
                    if (t == Lexer::TokenType::STRING_LITERAL ||
                        t == Lexer::TokenType::STRING_FSTRING ||
                        t == Lexer::TokenType::COMMENT ||
                        t == Lexer::TokenType::DOC_COMMENT)
                    {
                        continue;
                    }

                    const std::string &v = tok.getValue();
                    const size_t line = tok.getPosition().line;
                    if (lambdaWords_.count(v) != 0)
                    {
                        // (AR) «لامدا»: تُعَدّ فاتحةً فقط في صيغتها الكتليّة (التي تُغلَق بـ«نهاية»)؛
                        //      أمّا التعبيريّة «=> تعبير» فتكتمل بالسطر ولا تُعَدّ. نظرةٌ أماميّة تُميّز.
                        // (EN) «لامدا»: an opener only in its block form (closed by «نهاية»); the
                        //      expression form «=> expr» completes on the line and is not counted.
                        if (lambdaRequiresEnd(tokens, i))
                        {
                            ++blockDepth;
                        }
                    }
                    else if (blockOpeners_.count(v) != 0)
                    {
                        // (AR) استمرار كتلة (else-if) لا يفتح كتلة جديدة
                        // (EN) a block continuation (else-if) does not open a new block
                        const bool isContinuation =
                            havePrev && prevLine == line &&
                            blockContinuations_.count(prevValue) != 0;
                        if (!isContinuation)
                        {
                            ++blockDepth;
                        }
                    }
                    else if (blockClosers_.count(v) != 0)
                    {
                        --blockDepth;
                    }
                    else if (v == "(" || v == "[")
                    {
                        ++parenDepth;
                    }
                    else if (v == ")" || v == "]")
                    {
                        --parenDepth;
                    }

                    havePrev = true;
                    prevValue = v;
                    prevLine = line;
                }

                // (AR) غير مكتمل إذا بقيت كتلة مفتوحة أو قوس مفتوح
                // (EN) incomplete if a block or a bracket remains open
                return blockDepth > 0 || parenDepth > 0;
            }
            catch (...)
            {
                // (AR) عند تعذّر التقطيع نعدّه مكتملًا فيَظهر الخطأ بدل التعليق
                // (EN) if tokenizing fails, treat as complete so the error surfaces (no hang)
                return false;
            }
        }

        void REPLEngine::extractIdentifiers(const std::string &code)
        {
            if (!completer_)
            {
                return;
            }
            // (AR) استخراج بسيط: الرمز التالي لـ«متغير»/«ثابت»/«دالة» يُسجَّل مُعرّفًا للمُكمِّل
            // (EN) simple extraction: the token after متغير/ثابت/دالة is a user identifier
            auto findAfter = [&](const std::string &keyword)
            {
                size_t pos = 0;
                while ((pos = code.find(keyword, pos)) != std::string::npos)
                {
                    pos += keyword.size();
                    while (pos < code.size() && (code[pos] == ' ' || code[pos] == '\t'))
                    {
                        pos++;
                    }
                    size_t start = pos;
                    while (pos < code.size() && code[pos] != ' ' && code[pos] != '(' &&
                           code[pos] != '=' && code[pos] != '\n' && code[pos] != '\t')
                    {
                        pos++;
                    }
                    if (pos > start)
                    {
                        std::string ident = code.substr(start, pos - start);
                        if (!ident.empty())
                        {
                            completer_->addUserIdentifier(ident);
                        }
                    }
                }
            };
            // (AR) كلمات التصريح مأخوذة من SoT المعجم (لا سلاسل مباشرة) — التهجئة
            //      القانونيّة لـ«متغير/ثابت/دالة». نُلحق مسافةً كفاصل كلمة إلزاميّ يعتمده الفحص.
            // (EN) Declaration keywords sourced from the lexicon SoT (no raw literals) —
            //      canonical spellings for var/const/func; the trailing space is the
            //      mandatory word separator the scan relies on.
            using Lexer::KeywordTable;
            using Lexer::TokenType;
            findAfter(KeywordTable::getKeyword(TokenType::KEYWORD_VAR) + " ");
            findAfter(KeywordTable::getKeyword(TokenType::KEYWORD_CONST) + " ");
            findAfter(KeywordTable::getKeyword(TokenType::KEYWORD_FUNCTION) + " ");
        }

        std::string REPLEngine::promptText() const
        {
            const std::string &sym = (state_ == REPLState::MultiLine)
                                         ? config_.multilinePrompt
                                         : config_.promptSymbol;
            if (!config_.enableColor)
            {
                return sym;
            }
            const char *color = (state_ == REPLState::MultiLine) ? YELLOW : CYAN;
            return std::string(color) + sym + RESET;
        }

        void REPLEngine::printPrompt()
        {
            if (!config_.showPrompt)
                return;

            if (state_ == REPLState::MultiLine)
            {
                if (config_.enableColor)
                {
                    std::cout << YELLOW << config_.multilinePrompt << RESET;
                }
                else
                {
                    std::cout << config_.multilinePrompt;
                }
            }
            else
            {
                if (config_.enableColor)
                {
                    std::cout << CYAN << config_.promptSymbol << RESET;
                }
                else
                {
                    std::cout << config_.promptSymbol;
                }
            }
            std::cout.flush();
        }

        void REPLEngine::printResult(const Data::Value &result)
        {
            if (config_.enableColor)
            {
                std::cout << GREEN << result.toString() << RESET << std::endl;
            }
            else
            {
                std::cout << result.toString() << std::endl;
            }
        }

        void REPLEngine::printError(const std::string &message)
        {
            if (config_.enableColor)
            {
                std::cerr << RED << "✗ " << message << RESET << std::endl;
            }
            else
            {
                std::cerr << "✗ " << message << std::endl;
            }
        }

        void REPLEngine::printHeader()
        {
            // (AR) الترويسة كلّها من كتالوج SoT: اسم الأداة (سطرَي عربيّ/إنجليزيّ) +
            //      سطر الإصدار (تسمية VERSION_LABEL + رقم الإصدار) + سطرَي تلميح المساعدة
            //      (HELP_HINT بلغتَيه). خطّ الزينة «====» زخرفةٌ لا محتوى فيبقى محلّيًّا.
            // (EN) The whole banner comes from the SoT catalog: tool name (AR/EN lines) +
            //      version line (VERSION_LABEL + version) + two help-hint lines (HELP_HINT).
            //      The «====» rule is decoration (no content), kept local.
            static const char *const kRule =
                "========================================";
            const std::string versionLine =
                SoT::messageBoth(SoT::Message::VERSION_LABEL) +
                std::string(SoT::kDetailSeparator) + SoT::kVersion;

            if (config_.enableColor)
            {
                std::cout << BOLD << CYAN;
                std::cout << kRule << "\n";
                std::cout << SoT::kDisplayNameAr << "\n";
                std::cout << SoT::kDisplayNameEn << "\n";
                std::cout << versionLine << "\n";
                std::cout << kRule << "\n";
                std::cout << RESET << std::endl;

                std::cout << SoT::messageAr(SoT::Message::HELP_HINT) << "\n";
                std::cout << SoT::messageEn(SoT::Message::HELP_HINT) << "\n"
                          << std::endl;
            }
            else
            {
                std::cout << kRule << "\n";
                std::cout << SoT::kDisplayNameAr << "\n";
                std::cout << SoT::kDisplayNameEn << "\n";
                std::cout << versionLine << "\n";
                std::cout << kRule << "\n\n";

                std::cout << SoT::messageAr(SoT::Message::HELP_HINT) << "\n";
                std::cout << SoT::messageEn(SoT::Message::HELP_HINT) << "\n\n";
            }
        }

        std::string REPLEngine::readLine()
        {
            // (AR) الإدخال الغنيّ: محرّر السطر التفاعليّ (أسهم/Tab/تحرير) عند تفعيله
            // (EN) rich input: interactive line editor (arrows/Tab/editing) when enabled
            if (useLineEditor_ && lineEditor_)
            {
                if (history_)
                {
                    history_->resetCursor();
                }
                std::string line = lineEditor_->readLine(promptText());
                if (lineEditor_->isEof())
                {
                    state_ = REPLState::Exiting;
                    return "";
                }
                return line;
            }

            // (AR) المسار الافتراضيّ المُتحقَّق: std::getline / (EN) verified default path
            // (AR) نمط getline الصحيح: eofbit وحده لا يُفشِل التيّار. فإن قرأ getline
            //      محتوى السطر الأخير غير المُنهى بسطر جديد ثمّ بلغ EOF، أعِد المحتوى
            //      أوّلًا واخرج في القراءة التالية الفارغة — بدل إسقاط السطر الأخير
            //      (فحص std::cin.eof() المباشر كان يُسقطه عند الإدخال الأنبوبيّ بلا \n).
            // (EN) Correct getline idiom: eofbit alone does NOT fail the stream. If the
            //      final unterminated line has content before EOF, return it first and
            //      exit on the next empty read — instead of dropping that last line
            //      (the previous std::cin.eof() check dropped it on pipes lacking a \n).
            std::string line;
            if (!std::getline(std::cin, line))
            {
                state_ = REPLState::Exiting;
                return "";
            }

            return line;
        }

    } // namespace REPL
} // namespace Sad
