/**
 * @file parser_core.cpp
 * @brief (AR) تنفيذ المحلل النحوي الأساسي - تحويل الرموز إلى شجرة AST.
 *        (EN) Parser core implementation - converting tokens to AST.
 *
 * (AR) يحتوي هذا الملف على تنفيذات جميع دوال المحلل النحوي.
 *      يستخدم تقنية Recursive Descent Parsing مع معالجة الأخطاء والتعافي منها.
 *
 * (EN) This file contains implementations of all parser functions.
 *      Uses Recursive Descent Parsing with error handling and recovery.
 *
 * @author Sad Language Development Team
 * @date 2025-11-11
 * @license MIT License
 */

#include <string>
#include "parser_core.h"
#include "advanced_expr_nodes.h"
#include "directive_nodes.h"
#include "error_recovery.h" // (AR) kDiagStatsEnvVar لحجب آثار الاسترداد عن المستخدم
#include "asm_dialect_generated.h" // (AR) جدول لهجة التجميع المولَّد من SoT (م١ RFC اللهجات)
#include <algorithm>
#include <set>
#include <iostream>
#include <sstream>
#include <cstdlib> // (AR) std::getenv

namespace Sad
{
    namespace Parser
    {

        using namespace AST;
        using namespace Lexer;
        using TT = TokenType;

        // ======================================================================
        // (AR) البناء والهدم / (EN) Construction and Destruction
        // ======================================================================

        /**
         * @brief (AR) ينشئ محلل نحوي جديد مع مرجع للمحلل المعجمي.
         *        (EN) Creates new parser with reference to lexer.
         */
        ParserCore::ParserCore(LexerCore &lexer)
            : lexer_(lexer), current_(TT::END_OF_FILE, "") // Initialize with dummy token
              ,
              previous_(TT::END_OF_FILE, "") // Initialize with dummy token
              ,
              nextToken_(TT::END_OF_FILE, "") // Initialize with dummy token for lookahead
              ,
              panicMode_(false), filename_("<source>") // (AR) اسم ملف افتراضي / (EN) default filename
        {
            // Initialize by fetching first two tokens for proper lookahead
            // (AR) التهيئة بجلب أول رمزين للنظر المسبق الصحيح

            // Fetch first token into current_, skipping whitespace/comments/doc-comments
            current_ = lexer_.nextToken();
            while (current_.getType() == TT::WHITESPACE ||
                   current_.getType() == TT::COMMENT ||
                   current_.getType() == TT::DOC_COMMENT ||
                   current_.getType() == TT::NEWLINE)
            {
                // (AR) التقاط التعليق التوثيقي — يُرفق بأول تصريح
                // (EN) Capture doc comment — will attach to first declaration
                //
                // (AR) إصلاح BF-04: تجميع أسطر ## المتتالية بدل الكتابة فوقها
                //      كل سطر ## يولد رمز DOC_COMMENT منفصل من lexer،
                //      فإذا كان هناك تعليق سابق نضيف فاصل سطر ثم نلحق المحتوى.
                // (EN) BF-04 fix: accumulate consecutive ## lines instead of overwriting
                //      each ## line produces a separate DOC_COMMENT from the lexer;
                //      if a previous one exists, append with newline separator.
                if (current_.getType() == TT::DOC_COMMENT)
                {
                    // (AR) م٢: لا رمز كود قبل بداية الملف ⇒ لا فحص «ذيل سطر» هنا
                    // (EN) M2: no code token precedes file start ⇒ no trailing check here
                    appendDocComment(pendingDoc_, current_, nullptr);
                }
                current_ = lexer_.nextToken();
            }
            // (AR) م٢: توثيق انفصل عن أول رمز حقيقي في الملف (سطر فاصل أو نهايته) ⇒ يتيم
            // (EN) M2: doc detached from the file's first real token (separating line or EOF) ⇒ orphan
            cutDetachedDoc(pendingDoc_, current_);

            // Fetch second token into nextToken_, also skipping whitespace/comments/doc-comments
            nextToken_ = lexer_.nextToken();
            while (nextToken_.getType() == TT::WHITESPACE ||
                   nextToken_.getType() == TT::COMMENT ||
                   nextToken_.getType() == TT::DOC_COMMENT ||
                   nextToken_.getType() == TT::NEWLINE)
            {
                // (AR) إصلاح BF-04 (السبب الجذري): التعليقات التي تظهر بين current_
                //      و nextToken_ تأتي بعد current_ فعلياً — يجب ألا تُرفق بأول
                //      تصريح. نخزّنها في nextDoc_ ليتم ترحيلها لاحقاً.
                // (EN) BF-04 root-cause fix: doc comments appearing between
                //      current_ and nextToken_ are physically AFTER current_ —
                //      they must NOT attach to the first declaration. Buffer
                //      them in nextDoc_ for later promotion.
                if (nextToken_.getType() == TT::DOC_COMMENT)
                {
                    // (AR) م٢: current_ (أول رمز حقيقي) هو الكود السابق لهذا التوثيق —
                    //      لكشف توثيق ذيل السطر (SYN025).
                    // (EN) M2: current_ (first real token) is the code preceding this
                    //      doc — for trailing-doc detection (SYN025).
                    appendDocComment(nextDoc_, nextToken_, &current_);
                }
                nextToken_ = lexer_.nextToken();
            }
            // (AR) م٢: توثيق مُجمَّع هنا انفصل عمّا يليه (سطر فاصل أو نهاية الملف) ⇒ يتيم
            // (EN) M2: docs collected here detached from what follows (separating line or EOF) ⇒ orphan
            cutDetachedDoc(nextDoc_, nextToken_);

            // (AR) إضافة نقاط مزامنة إضافية لنظام التعافي
            // (EN) Add additional sync points for recovery system
            recoverySystem_.addSyncPoint("بينما"); // while
            recoverySystem_.addSyncPoint("حاول");  // try
            recoverySystem_.addSyncPoint("امسك");  // catch
            recoverySystem_.addSyncPoint("ارمي");  // throw
            recoverySystem_.addSyncPoint("بنية");  // struct
            recoverySystem_.addSyncPoint("تعداد"); // enum
            recoverySystem_.addSyncPoint("صدّر");   // export
            recoverySystem_.addSyncPoint("عام");   // public
            recoverySystem_.addSyncPoint("خاص");   // private
            recoverySystem_.addSyncPoint("محمي");  // protected
        }

        // ======================================================================
        // (AR) الدوال العامة / (EN) Public Interface
        // ======================================================================

        /**
         * @brief (AR) يحلل البرنامج الكامل ويرجع قائمة جمل.
         *        (EN) Parses complete program and returns statement list.
         */
        StmtList ParserCore::parseProgram()
        {
            StmtList statements;

            // (AR) اسم الملف يجب أن يُعين من خارج Parser
            // (EN) Filename should be set from outside Parser
            // if (filename_.empty()) {
            //     filename_ = "<source>";
            // }

            // (AR) عداد للدوال الرئيسية - للتأكد من وجود واحدة فقط
            // (EN) Counter for main functions - to ensure only one exists
            int mainFunctionCount = 0;

            // DEBUG: Disabled
            // std::cout << "[parser_core_impl.cpp] بدء parseProgram - current token: "
            //           << static_cast<int>(current_.getType()) << " = '"
            //           << current_.getValue() << "'\n";

            int statement_count = 0;
            int error_count = 0;

            // (AR) حماية ضد الحلقة اللانهائية - تتبع الموقع الحالي
            // (EN) Infinite loop protection - track current position
            size_t last_position = 0;
            int stuck_count = 0;
            const int MAX_STUCK_ITERATIONS = 3; // Allow 3 attempts at same position

            // Parse until EOF
            // (AR) التحليل حتى نهاية الملف
            while (!isAtEnd())
            {
                // (AR) فحص الحلقة اللانهائية: هل نحن عالقون في نفس الموقع؟
                // (EN) Infinite loop detection: are we stuck at the same position?
                size_t current_position = current_.getPosition().offset;

                if (current_position == last_position)
                {
                    stuck_count++;
                    if (stuck_count >= MAX_STUCK_ITERATIONS)
                    {
                        std::cerr << "\n";
                        std::cerr << "❌❌❌ ========================================\n";
                        std::cerr << "  🚨 (AR) اكتشاف حلقة لا نهائية!\n";
                        std::cerr << "  🚨 (EN) Infinite Loop Detected!\n";
                        std::cerr << "========================================\n";
                        std::cerr << "📍 (AR) عالق في السطر " << current_.getPosition().line
                                  << ", العمود " << current_.getPosition().column << "\n";
                        std::cerr << "📍 (EN) Stuck at line " << current_.getPosition().line
                                  << ", column " << current_.getPosition().column << "\n";
                        std::cerr << "🔎 (AR) الرمز: '" << current_.getValue() << "'\n";
                        std::cerr << "🔎 (EN) Token: '" << current_.getValue() << "'\n";
                        std::cerr << "⚠ (AR) القفز للرمز التالي لكسر الحلقة...\n";
                        std::cerr << "⚠ (EN) Forcing advance to break the loop...\n";
                        std::cerr << "========================================\n\n";

                        // Force advance to break infinite loop
                        advance();
                        stuck_count = 0;
                        error_count++;
                        continue;
                    }
                }
                else
                {
                    stuck_count = 0;
                    last_position = current_position;
                }

                try
                {
                    // DEBUG: Disabled
                    // std::cout << "[parser_core_impl.cpp] داخل حلقة parseProgram - current token: "
                    //           << static_cast<int>(current_.getType()) << " = '"
                    //           << current_.getValue() << "'\n";

                    auto stmt = parseDeclaration();
                    if (stmt)
                    {
                        // (AR) التحقق إذا كانت الجملة دالة رئيسية
                        // (EN) Check if statement is a main function
                        if (auto *funcDecl = dynamic_cast<FunctionDecl *>(stmt.get()))
                        {
                            if (funcDecl->isMainFunction)
                            {
                                mainFunctionCount++;
                                // (AR) التحقق من عدم تكرار الدالة الرئيسية
                                // (EN) Check for duplicate main function
                                if (mainFunctionCount > 1)
                                {
                                    std::cerr << "\n";
                                    std::cerr << "❌ ========================================\n";
                                    std::cerr << "  ⛔ (AR) خطأ: تم تعريف الدالة الرئيسية أكثر من مرة!\n";
                                    std::cerr << "  ⛔ (EN) Error: Main function defined more than once!\n";
                                    std::cerr << "========================================\n";
                                    std::cerr << "📍 (AR) السطر: " << current_.getPosition().line << "\n";
                                    std::cerr << "📍 (EN) Line: " << current_.getPosition().line << "\n";
                                    std::cerr << "💬 (AR) يجب أن يكون هناك دالة رئيسية واحدة فقط في البرنامج\n";
                                    std::cerr << "💬 (EN) There can only be one main function in a program\n";
                                    std::cerr << "========================================\n\n";

                                    error("(AR) تم تعريف الدالة الرئيسية أكثر من مرة. "
                                          "(EN) Main function defined more than once.");
                                }
                            }
                        }

                        statements.push_back(std::move(stmt));
                        statement_count++;
                        // DEBUG: Disabled
                        // std::cout << "[parser_core_impl.cpp] تمت إضافة جملة - العدد الكلي: "
                        //           << statements.size() << "\n";
                    }
                }
                catch (const std::exception &e)
                {
                    error_count++;

                    // (AR) عرض معلومات تفصيلية عن الخطأ
                    // (EN) Display detailed error information
                    std::cerr << "\n";
                    std::cerr << "❗❗❗ ========================================\n";
                    std::cerr << "  ⛔ (AR) خطأ في التحليل النحوي\n";
                    std::cerr << "  ⛔ (EN) Parsing Error\n";
                    std::cerr << "========================================\n";
                    std::cerr << "📄 (AR) الملف: " << (filename_.empty() ? "<source>" : filename_) << "\n";
                    std::cerr << "📄 (EN) File: " << (filename_.empty() ? "<source>" : filename_) << "\n";
                    std::cerr << "📍 (AR) السطر: " << current_.getPosition().line
                              << ", العمود: " << current_.getPosition().column << "\n";
                    std::cerr << "📍 (EN) Line: " << current_.getPosition().line
                              << ", Column: " << current_.getPosition().column << "\n";
                    std::cerr << "💬 (AR) الرسالة: " << e.what() << "\n";
                    std::cerr << "💬 (EN) Message: " << e.what() << "\n";
                    std::cerr << "========================================\n\n";

                    error(e.what());
                    synchronize();
                }
                catch (...)
                {
                    error_count++;

                    // (AR) معالجة أي استثناء غير معروف
                    // (EN) Handle any unknown exception
                    std::cerr << "\n";
                    std::cerr << "❗❗❗ ========================================\n";
                    std::cerr << "  ⛔ (AR) خطأ غير معروف في التحليل النحوي\n";
                    std::cerr << "  ⛔ (EN) Unknown Parsing Error\n";
                    std::cerr << "========================================\n";
                    std::cerr << "📄 (AR) الملف: " << (filename_.empty() ? "<source>" : filename_) << "\n";
                    std::cerr << "📄 (EN) File: " << (filename_.empty() ? "<source>" : filename_) << "\n";
                    std::cerr << "📍 (AR) السطر: " << current_.getPosition().line
                              << ", العمود: " << current_.getPosition().column << "\n";
                    std::cerr << "📍 (EN) Line: " << current_.getPosition().line
                              << ", Column: " << current_.getPosition().column << "\n";
                    std::cerr << "========================================\n\n";

                    errorCatalog(Errors::ErrorCode::SYN_PARSE_UNKNOWN_ERROR);
                    synchronize();
                }
            }

            // (AR) طباعة ملخص التحليل
            // (EN) Print parsing summary
            /*  std::cerr << "\n";
              std::cerr << "========================================\n";
              std::cerr << "  📊 (AR) ملخص التحليل / (EN) Parsing Summary\n";
              std::cerr << "========================================\n";
              std::cerr << "✓ (AR) عدد الجمل الناجحة: " << statement_count << "\n";
              std::cerr << "✓ (EN) Successful statements: " << statement_count << "\n";

              if (error_count > 0) {
                  std::cerr << "❌ (AR) عدد الأخطاء: " << error_count << "\n";
                  std::cerr << "❌ (EN) Error count: " << error_count << "\n";
              }
              std::cerr << "========================================\n\n";
              */
            // DEBUG: Disabled
            // std::cout << "[parser_core_impl.cpp] انتهى parseProgram - عدد الجمل: "
            //           << statements.size() << "\n";

            // (AR) إحصائيّات التعافي تشخيصٌ داخليّ للمطوّر لا للمستخدم النهائيّ: التقرير
            //      النهائيّ يعرض الأخطاء الفعليّة، أمّا «ناجح/فاشل» فيُربكه (يتناقض ظاهريًّا
            //      مع «❌ خطأ»). أظهرها فقط عند ضبط متغيّر البيئة التشخيصيّ. [تشخيص أنظف]
            // (EN) Recovery stats are internal dev diagnostics, not for the end user: the
            //      final report shows the real errors, while "success/fail" confuses them
            //      (seemingly contradicts "❌ error"). Emit only when the diag env var is set.
            if ((recoverySystem_.getRecoveryCount() > 0 || recoverySystem_.getFailedRecoveryCount() > 0)
                && std::getenv(Errors::kDiagStatsEnvVar) != nullptr)
            {
                std::cerr << recoverySystem_.formatStats(true);
            }

            return statements;
        }

        /**
         * @brief (AR) يتحقق من وجود أخطاء تحليل.
         *        (EN) Checks for parsing errors.
         */
        bool ParserCore::hasErrors() const
        {
            return Errors::ErrorManager::getInstance().hasErrors();
        }

        /**
         * @brief (AR) يطبع جميع الأخطاء.
         *        (EN) Prints all errors.
         */
        void ParserCore::printErrors() const
        {
            // (AR) استخدام ErrorManager لطباعة الأخطاء بشكل جميل
            // (EN) Use ErrorManager to print errors beautifully
            Errors::ErrorManager::getInstance().printAll();
        }

        /**
         * @brief (AR) يرجع قائمة الأخطاء.
         *        (EN) Returns error list.
         */
        std::vector<std::string> ParserCore::getErrors() const
        {
            // (AR) تحويل التشخيصات إلى قائمة نصية
            // (EN) Convert diagnostics to text list
            std::vector<std::string> result;
            const auto &diagnostics = Errors::ErrorManager::getInstance().getAllDiagnostics();

            for (const auto &diag : diagnostics)
            {
                result.push_back(diag.format(Errors::Language::ENGLISH, false));
            }

            return result;
        }

        // ======================================================================
        // (AR) دوال التحليل الأساسية / (EN) Core Parsing Functions
        // ======================================================================

        /**
         * @brief (AR) يحلل تصريح واحد (دالة، صنف، متغير، import، export).
         *        (EN) Parses single declaration (function, class, variable, import, export).
         */
        StmtPtr ParserCore::parseDeclaration()
        {
            // ══════════════════════════════════════════════════════════════════
            // (AR) التحقق من التوجيهات @ (directives) قبل المُزخرِفات
            // (EN) Check for @ directives before decorators
            // ══════════════════════════════════════════════════════════════════
            if (check(TT::AT_SIGN))
            {
                auto directiveResult = tryParseDirective();
                if (directiveResult)
                {
                    return directiveResult;
                }
            }

            // ─────────────────────────────────────────────────────────────────────
            // (AR) كتلة لهجة التجميع «تجميع … نهاية» (م١ RFC اللهجات الأصيلة)
            //      تُكتشَف هنا قبل منطق تصريح المتغيّر كي لا يُلتقط «تجميع» اسمَ نوع/
            //      متغيّر. كلمة سياقية: تُفتح كتلةً فقط إن لم يلها ما يجعلها تعبيرًا.
            // (EN) Assembly dialect block — detected before variable-declaration logic
            //      so "تجميع" is not mistaken for a type/variable name.
            // ─────────────────────────────────────────────────────────────────────
            if (check(TT::KEYWORD_ASM) || checkContextual(TT::KEYWORD_ASM))
            {
                TT nxtAsm = peekNext().getType();
                bool isExprCtxAsm =
                    nxtAsm == TT::OP_ASSIGN ||
                    nxtAsm == TT::OP_PLUS_ASSIGN ||
                    nxtAsm == TT::OP_MINUS_ASSIGN ||
                    nxtAsm == TT::OP_MULTIPLY_ASSIGN ||
                    nxtAsm == TT::OP_DIVIDE_ASSIGN ||
                    nxtAsm == TT::DOT ||
                    nxtAsm == TT::PAREN_LEFT ||
                    nxtAsm == TT::BRACKET_LEFT;
                if (!isExprCtxAsm)
                    return parseAsmBlockStmt();
            }

            // (AR) التحقق من المُزخرِفات قبل التصريح
            // (EN) Check for decorators before declaration
            ExprList decorators;
            while (check(TT::AT_SIGN))
            {
                match(TT::AT_SIGN); // consume @
                decorators.push_back(parseDecorator());
            }

            // ══════════════════════════════════════════════════════════════════
            // (AR) التحقق من سمات الدوال [[سمة، سمة(N)، ...]]
            //      تُجمع كنصوص خام ثم تُمرّر إلى FunctionDecl لتُترجم
            //      إلى LLVM function attributes في codegen.
            //      الأمثلة: [[مضمن_دائماً]]، [[بارد، لا_تعرّج]]، [[محاذاة(64)]]
            // (EN) Function attributes [[attr, attr(N), ...]]
            //      Collected as raw strings, attached to FunctionDecl, then
            //      lowered to LLVM function attributes during codegen.
            // ══════════════════════════════════════════════════════════════════
            std::vector<std::string> functionAttributes;
            if (check(TT::BRACKET_LEFT) && peekNext().getType() == TT::BRACKET_LEFT)
            {
                advance(); // consume first '['
                advance(); // consume second '['
                while (!check(TT::BRACKET_RIGHT) && !isAtEnd())
                {
                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "السمة"}, {"what_en", "attribute"}, {"ctx_ar", "داخل [[ ]]"}, {"ctx_en", "inside [[ ]]"}});
                        break;
                    }
                    std::string attrName = current_.getValue();
                    advance(); // consume identifier
                    // (AR) سمة بمعامل مثل محاذاة(64) → نخزّن "محاذاة(64)"
                    // (EN) Attribute with argument like محاذاة(64) → store "محاذاة(64)"
                    if (check(TT::PAREN_LEFT))
                    {
                        advance(); // consume '('
                        std::string argStr;
                        while (!check(TT::PAREN_RIGHT) && !isAtEnd())
                        {
                            argStr += current_.getValue();
                            advance();
                        }
                        if (!match(TT::PAREN_RIGHT))
                        {
                            errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ")"}, {"ctx_ar", "بعد معامل السمة"}, {"ctx_en", "after the attribute argument"}});
                            break;
                        }
                        attrName += "(" + argStr + ")";
                    }
                    functionAttributes.push_back(attrName);
                    // (AR) فاصلة اختيارية بين السمات (عربية أو لاتينية)
                    if (check(TT::COMMA) || check(TT::ARABIC_COMMA))
                        advance();
                }
                if (!match(TT::BRACKET_RIGHT))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "]"}, {"ctx_ar", "لإغلاق قائمة السمات"}, {"ctx_en", "to close the attribute list"}});
                }
                if (!match(TT::BRACKET_RIGHT))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "]"}, {"ctx_ar", "ثانيةً لإكمال [[...]]"}, {"ctx_en", "again to complete [[...]]"}});
                }
            }

            // Check for declaration keywords
            // (AR) التحقق من كلمات التصريح المفتاحية

            // (AR) استيراد / (EN) Import
            if (match(TT::KEYWORD_IMPORT))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع الاستيراد. (EN) Decorators cannot be used with imports.");
                }
                return parseImportStmt();
            }

            // (AR) استيراد انتقائي (من...استورد) / (EN) From-import
            if (match(TT::KEYWORD_FROM))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع الاستيراد. (EN) Decorators cannot be used with imports.");
                }
                return parseFromImportStmt();
            }

            // (AR) تصدير / (EN) Export
            if (match(TT::KEYWORD_EXPORT))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مباشرة مع التصدير. (EN) Decorators cannot be used directly with export.");
                }
                return parseExportDecl();
            }

            // ======================================================================
            // (AR) دعم الدوال الخارجية (External Functions / FFI)
            // (EN) External function support (FFI)
            // ======================================================================
            // (AR) RFC 0034: «خارجي/خارجية» كلمة ناعمة — لا ندخل مسار extern إلّا حين
            //      يكون ما بعدها شكل extern فعلًا: نصّ ربط (كتلة)، أو 'دالة' (حاجز
            //      legacy)، أو '(' (يُحسم بعد الاستهلاك: نصّ = حاجز legacy،
            //      غير نصّ = استدعاء دالة *اسمها* خارجي/خارجية). أيّ تتمّة أخرى
            //      ('='، '.'، '['، نهاية سطر...) تسقط لمسار جملة التعبير حيث يقبلها
            //      parsePrimary اسمًا (isTokenUsableAsName) — يحفظ برامج المستخدم
            //      المعرِّفة دالّةً باسم «خارجية» (ctest ‏test_throw_catch_comprehensive).
            // (EN) RFC 0034: 'خارجي/خارجية' is a soft keyword — enter the extern path
            //      only when the continuation is actually extern-shaped: a linkage
            //      string (block), 'دالة' (legacy barrier), or '(' (resolved after
            //      consuming: string = legacy barrier, non-string = a call to a
            //      function *named* extern). Any other continuation falls through to
            //      the expression-statement path, where parsePrimary accepts it as a
            //      name — preserving user programs that define a function named
            //      «خارجية» (ctest test_throw_catch_comprehensive).
            if (check(TT::KEYWORD_EXTERN) &&
                (peekNext().getType() == TT::STRING_LITERAL ||
                 peekNext().getType() == TT::KEYWORD_FUNCTION ||
                 peekNext().getType() == TT::PAREN_LEFT))
            {
                Token externTok = current_;
                advance(); // (AR) استهلاك 'خارجي'/'خارجية' / (EN) consume the extern lexeme

                // (AR) '(' يليه غير نصّ ⇒ جملة استدعاء لدالة اسمها خارجي/خارجية —
                //      نبني الاسم المستهلَك أوّليًّا ونكمل سلسلة اللواحق والتعبير.
                // (EN) '(' followed by a non-string ⇒ a call statement to a function
                //      named extern — rebuild the consumed name as a primary and
                //      continue the postfix chain.
                if (check(TT::PAREN_LEFT) && peekNext().getType() != TT::STRING_LITERAL)
                {
                    auto callee = std::make_unique<VariableExpr>(externTok.getValue(), externTok.getPosition());
                    auto expr = parsePostfixFrom(std::move(callee));
                    if (!expr)
                        return nullptr;
                    matchSemicolon();
                    return std::make_unique<ExprStmt>(std::move(expr));
                }

                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع الدوال الخارجية. (EN) Decorators cannot be used with extern functions.");
                }
                // (AR) [ISSUE-041] كتلة ربط أجنبيّ بلغة بلا أقواس: خارجي "C" <تصاريح> نهاية
                //      نُحوّلها لكتلة من تصاريح دوال خارجيّة (كلٌّ بلغة الربط نفسها). كان
                //      المُحلّل يتوقّع 'دالة' مباشرةً فيرفض صيغة الكتلة (gr.adv.ffi_extern_block).
                // (EN) [ISSUE-041] Foreign linkage block (string linkage, no parens):
                //      `extern "C" <decls> end`. Lower to a block of extern function decls
                //      (all sharing the linkage). The parser used to expect 'function'
                //      immediately and reject the block form (gr.adv.ffi_extern_block).
                if (check(TT::STRING_LITERAL))
                {
                    // (AR) RFC 0034: فاتحة الكتلة مذكّرة حصرًا — «خارجي "C"» لا «خارجية "C"»
                    //      (لا موصوف مؤنّث هنا؛ SoT ‏gr.adv.ffi_extern_block ينصّ على المذكّر).
                    //      اللفظة الأصليّة في previous() (استهلكها match أعلاه) والمذكّر هو
                    //      primaryWord في الكتالوج — لا سلاسل خام. نُبلغ ثم نُكمل تحليل
                    //      الكتلة تعافيًا لمنع تعاقب الأخطاء.
                    // (EN) RFC 0034: the block opener is the masculine form only —
                    //      'خارجي "C"', never the feminine alias (no feminine noun to agree
                    //      with; SoT gr.adv.ffi_extern_block mandates the masculine). The
                    //      original lexeme is in previous(); masculine = catalog primaryWord.
                    //      Report, then keep parsing the block as recovery.
                    {
                        const auto *externEntry = Lexer::KeywordTable::getEntry(TT::KEYWORD_EXTERN);
                        if (externEntry && previous().getValue() != externEntry->primaryWord)
                        {
                            errorCatalog(Errors::ErrorCode::SYN_ADJECTIVE_GENDER, {{"wrong", previous().getValue() + " \"C\""}, {"right", externEntry->primaryWord + " \"C\""}, {"note_ar", "كتلة الربط تُفتح بالمذكّر '" + externEntry->primaryWord + "' — لا موصوف مؤنّث هنا. مثال: " + externEntry->primaryWord + " \"C\"\n    " + kw(TT::KEYWORD_FUNCTION) + " printf(" + kw(TT::TYPE_STRING) + ")\n" + kw(TT::KEYWORD_END)}, {"note_en", "the linkage block opens with the masculine '" + externEntry->primaryWord + "' — there is no feminine noun to agree with. Example: " + externEntry->primaryWord + " \"C\"\n    " + kw(TT::KEYWORD_FUNCTION) + " printf(" + kw(TT::TYPE_STRING) + ")\n" + kw(TT::KEYWORD_END)}});
                        }
                    }
                    std::string ffiLinkName;
                    // (AR) موقع نصّ الربط — يُستخدم كموقع عقدة الكتلة في التشخيصات.
                    // (EN) Linkage-string position — used as the block node's position in diagnostics.
                    auto blockPos = current_.getPosition();
                    ffiLinkName = current_.getValue();
                    advance(); // (AR) استهلاك "C" / (EN) consume linkage string
                    StmtList externBody;
                    while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
                    {
                        // (AR) كل دورة تستهلك 'دالة' عبر match قبل استدعاء parseExternFunctionDecl،
                        //      فالتقدّم مضمون حتى لو أعاد المُحلّل nullptr — لا حلقة لا نهائية.
                        // (EN) Each iteration consumes 'function' via match before calling
                        //      parseExternFunctionDecl, so progress is guaranteed even if the
                        //      parser returns nullptr — no infinite loop is possible.
                        if (!match(TT::KEYWORD_FUNCTION))
                        {
                            // (AR) رمز دخيل غير 'دالة' داخل الكتلة — نُبلِغ ونُرجِع فورًا (كنظير
                            //      الصيغة المفردة أدناه) تجنّبًا لتعاقب خطأ ثانٍ من consume('نهاية').
                            // (EN) Stray non-'function' token inside the block — report and return
                            //      immediately (mirroring the single-decl path below) to avoid a
                            //      cascading second error from the consume('end') call.
                            error("(AR) خطأ نحوي: توقع 'دالة' داخل كتلة 'خارجي'. (EN) Syntax error: expected 'function' inside extern block.");
                            return nullptr;
                        }
                        auto externFn = parseExternFunctionDecl(ffiLinkName);
                        if (externFn)
                            externBody.push_back(std::move(externFn));
                    }
                    consume(TT::KEYWORD_END,
                            "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق كتلة 'خارجي'.\n"
                            "(EN) Syntax error: expected 'نهاية' to close extern block.");
                    return std::make_unique<BlockStmt>(std::move(externBody), blockPos);
                }
                // (AR) RFC 0034: الصيغتان المفردتان القديمتان 'خارجي دالة' و'خارجي("رمز") دالة'
                //      أُزيلتا بلا توافق خلفيّ — حاجز خطأ توجيهيّ على نمط حاجز 'غير_متزامن دالة'.
                //      بلوغ هذه النقطة يعني حتمًا 'دالة' أو '("رمز")' (بوّابة الدخول أعلاه
                //      تُسقط كلّ تتمّة أخرى لمسار التعبير) — فرسالة الإزالة دقيقة دومًا.
                // (EN) RFC 0034: legacy single-decl forms 'extern function' and
                //      'extern("sym") function' removed — guidance barrier (async-style guard).
                //      Reaching here necessarily means 'دالة' or '("sym")' (the entry gate
                //      above routes every other continuation to the expression path), so
                //      the removal message is always accurate.
                errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", kw(TT::KEYWORD_EXTERN) + " " + kw(TT::KEYWORD_FUNCTION)}, {"new", kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN)}, {"example", kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " printf(" + kw(TT::TYPE_STRING) + ")\n" + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + "(\"cos\") " + kw(TT::TYPE_DOUBLE) + " جيب_التمام(" + kw(TT::TYPE_DOUBLE) + ")"}});
                // (AR) تعافٍ: استهلاك اسم الربط الاختياريّ '("رمز")' ثم 'دالة' ثم التصريح
                //      نفسه — يمنع تعاقب أخطاء لاحقة كما يفعل حاجز 'غير_متزامن دالة'.
                // (EN) Recovery: consume optional '("sym")' then 'function' then the decl
                //      itself — prevents cascading errors (mirrors the async guard).
                std::string ffiLinkName;
                if (check(TT::PAREN_LEFT))
                {
                    advance(); // (AR) استهلاك '(' / (EN) consume '('
                    if (check(TT::STRING_LITERAL))
                    {
                        ffiLinkName = current_.getValue();
                        advance();
                    }
                    if (check(TT::PAREN_RIGHT))
                        advance();
                }
                if (!match(TT::KEYWORD_FUNCTION))
                {
                    return nullptr;
                }
                return parseExternFunctionDecl(ffiLinkName);
            }

            // (AR) 'غير_متزامن دالة' أُزيلت — استخدم 'دالة غير_متزامن' (الصفة بعد الاسم)
            // (EN) 'async function' removed — use 'function async' (Arabic adjective order)
            if (match(TT::KEYWORD_ASYNC) ||
                (checkContextual(TT::KEYWORD_ASYNC) && peekNext().getType() == TT::KEYWORD_FUNCTION))
            {
                errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", kw(TT::KEYWORD_ASYNC) + " " + kw(TT::KEYWORD_FUNCTION)}, {"new", kw(TT::KEYWORD_FUNCTION) + " " + kw(TT::KEYWORD_ASYNC)}, {"example", kw(TT::KEYWORD_FUNCTION) + " " + kw(TT::KEYWORD_ASYNC) + " جلب(رابط: " + kw(TT::TYPE_STRING) + ")\n ...\n" + kw(TT::KEYWORD_END)}});
                if (checkContextual(TT::KEYWORD_ASYNC))
                    advance();
                if (!check(TT::KEYWORD_FUNCTION))
                {
                    return nullptr;
                }
                advance(); // consume 'دالة'
                auto __fd = parseFunctionDecl(std::move(decorators), true, false);
                if (auto *fp = dynamic_cast<AST::FunctionDecl *>(__fd.get()))
                    fp->attributes = std::move(functionAttributes);
                return __fd;
            }

            // (AR) مولد دالة — الصيغة العربية المعتمدة (مضاف + مضاف إليه)
            // (EN) Generator function — Arabic approved form (construct state)
            if (match(TT::KEYWORD_GENERATOR) ||
                (checkContextual(TT::KEYWORD_GENERATOR) && peekNext().getType() == TT::KEYWORD_FUNCTION))
            {
                if (checkContextual(TT::KEYWORD_GENERATOR))
                    advance(); // consume contextual
                if (!match(TT::KEYWORD_FUNCTION))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_FUNCTION)}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_GENERATOR) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_GENERATOR) + "'"}});
                    return nullptr;
                }
                auto __fd = parseFunctionDecl(std::move(decorators), false, true);
                if (auto *fp = dynamic_cast<AST::FunctionDecl *>(__fd.get()))
                    fp->attributes = std::move(functionAttributes);
                return __fd;
            }

            if (match(TT::KEYWORD_FUNCTION))
            {
                // (AR) 'دالة مولد' أُزيلت — استخدم 'مولد دالة'
                // (EN) 'function generator' removed — use 'generator function'
                if (match(TT::KEYWORD_GENERATOR) ||
                    (checkContextual(TT::KEYWORD_GENERATOR)))
                {
                    errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", kw(TT::KEYWORD_FUNCTION) + " " + kw(TT::KEYWORD_GENERATOR)}, {"new", kw(TT::KEYWORD_GENERATOR) + " " + kw(TT::KEYWORD_FUNCTION)}, {"example", kw(TT::KEYWORD_GENERATOR) + " " + kw(TT::KEYWORD_FUNCTION) + " عدّاد()\n " + kw(TT::KEYWORD_YIELD) + " 1\n " + kw(TT::KEYWORD_YIELD) + " 2\n" + kw(TT::KEYWORD_END)}});
                    if (checkContextual(TT::KEYWORD_GENERATOR))
                        advance();
                    auto __fd = parseFunctionDecl(std::move(decorators), false, true);
                    if (auto *fp = dynamic_cast<AST::FunctionDecl *>(__fd.get()))
                        fp->attributes = std::move(functionAttributes);
                    return __fd;
                }
                auto __fd = parseFunctionDecl(std::move(decorators), false, false);
                if (auto *fp = dynamic_cast<AST::FunctionDecl *>(__fd.get()))
                    fp->attributes = std::move(functionAttributes);
                return __fd;
            }

            // ======================================================================
            // (AR) دعم القوالب (Templates - Phase 7B)
            // (EN) Template support (Phase 7B)
            // ======================================================================
            if (match(TT::KEYWORD_TEMPLATE) ||
                matchContextual(TT::KEYWORD_TEMPLATE))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات للقوالب غير مدعومة بعد. (EN) Template decorators not yet supported.");
                }
                return parseTemplateDecl();
            }

            // (AR) دعم فضاء الأسماء (Namespaces - Phase 7B.5)
            // (EN) Namespace support (Phase 7B.5)
            if (match(TT::KEYWORD_NAMESPACE) ||
                matchContextual(TT::KEYWORD_NAMESPACE))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لفضاء الأسماء غير مدعومة. (EN) Namespace decorators not supported.");
                }
                return parseNamespaceDecl();
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) الصيغة القديمة: مجرد صنف — أُزيلت. استخدم: صنف مجرد
            // (EN) Old syntax: مجرد صنف — removed. Use: صنف مجرد
            // ═══════════════════════════════════════════════════════════════════
            if (match(TT::KEYWORD_ABSTRACT))
            {
                errorCatalog(Errors::ErrorCode::SYN_ADJECTIVE_ORDER, {{"wrong", kw(TT::KEYWORD_ABSTRACT) + " " + kw(TT::KEYWORD_CLASS)}, {"right", kw(TT::KEYWORD_CLASS) + " " + kw(TT::KEYWORD_ABSTRACT)}});
                // (AR) محاولة استرداد: نبتلع 'محكم' إن وجدت ثم 'صنف'
                bool isAlsoSealed = false;
                if (checkContextual(TT::KEYWORD_SEALED))
                {
                    advance();
                    isAlsoSealed = true;
                }
                if (!match(TT::KEYWORD_CLASS))
                {
                    error("(AR) توقع 'صنف' بعد 'مجرد'. (EN) Expected 'class' after 'abstract'.");
                }
                auto classDecl = parseClassDecl();
                if (auto *cd = dynamic_cast<AST::ClassDecl *>(classDecl.get()))
                {
                    cd->isAbstract = true;
                    if (isAlsoSealed)
                    {
                        cd->isSealed = true;
                        cd->sourceFile = filename_;
                    }
                }
                return classDecl;
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) الصيغة القديمة: محكم صنف — أُزيلت. استخدم: صنف محكم
            // (EN) Old syntax: محكم صنف — removed. Use: صنف محكم
            // ═══════════════════════════════════════════════════════════════════
            if (checkContextual(TT::KEYWORD_SEALED))
            {
                errorCatalog(Errors::ErrorCode::SYN_ADJECTIVE_ORDER, {{"wrong", kw(TT::KEYWORD_SEALED) + " " + kw(TT::KEYWORD_CLASS)}, {"right", kw(TT::KEYWORD_CLASS) + " " + kw(TT::KEYWORD_SEALED)}});
                advance(); // (AR) استهلاك 'محكم'
                bool isAlsoAbstract = false;
                if (match(TT::KEYWORD_ABSTRACT))
                {
                    isAlsoAbstract = true;
                }
                if (!match(TT::KEYWORD_CLASS))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_CLASS)}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_SEALED) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_SEALED) + "'"}});
                }
                auto classDecl = parseClassDecl();
                if (auto *cd = dynamic_cast<AST::ClassDecl *>(classDecl.get()))
                {
                    cd->isSealed = true;
                    cd->sourceFile = filename_;
                    if (isAlsoAbstract)
                    {
                        cd->isAbstract = true;
                    }
                }
                return classDecl;
            }

            if (match(TT::KEYWORD_CLASS))
            {
                // ═══════════════════════════════════════════════════════════════
                // (AR) الصيغة الجديدة: صنف [مجرد] [محكم] اسم ... نهاية
                //      المعدلات تأتي بعد كلمة 'صنف' (الصفة بعد الموصوف)
                // (EN) New syntax: class [abstract] [sealed] Name ... end
                //      Modifiers come AFTER 'class' keyword (adjective after noun)
                // ═══════════════════════════════════════════════════════════════
                bool isAbstract = false;
                bool isSealed = false;

                // (AR) قراءة معدلات الصنف بعد 'صنف': مجرد و/أو محكم
                // (EN) Read class modifiers after 'صنف': abstract and/or sealed
                while (true)
                {
                    if (match(TT::KEYWORD_ABSTRACT))
                    {
                        isAbstract = true;
                    }
                    else if (checkContextual(TT::KEYWORD_SEALED))
                    {
                        advance();
                        isSealed = true;
                    }
                    else
                    {
                        break;
                    }
                }

                auto classDecl = parseClassDecl();
                if (auto *cd = dynamic_cast<AST::ClassDecl *>(classDecl.get()))
                {
                    cd->decorators = std::move(decorators);
                    if (isAbstract)
                    {
                        cd->isAbstract = true;
                    }
                    if (isSealed)
                    {
                        cd->isSealed = true;
                        cd->sourceFile = filename_;
                    }
                }
                return classDecl;
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دعم السمات: سمة اسم_السمة ... نهاية
            // (EN) Trait support: trait TraitName ... end
            // ═══════════════════════════════════════════════════════════════════
            // (AR) سمة كلمة سياقية — تُعامل كإعلان سمة فقط إذا تلاها مُعرّف (اسم السمة)
            //      وإلا تُعامل كمُعرّف عادي (متغير/دالة): سمة.لون = ...
            // (EN) سمة is contextual — only treated as trait decl if followed by identifier
            if (match(TT::KEYWORD_TRAIT) ||
                (checkContextual(TT::KEYWORD_TRAIT) && peekNext().getType() == TT::IDENTIFIER && (advance(), true)))
            {
                return parseTraitDecl();
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دعم الواجهات التصريحية (UI): واجهة اسم ... نهاية
            // (EN) Declarative UI support: واجهة name ... end
            // (AR) واجهة كلمة سياقية — تُعامل كتعريف مكون واجهة إذا تلاها مُعرّف
            // (EN) واجهة is contextual — treated as UI component decl if followed by identifier
            // ═══════════════════════════════════════════════════════════════════
            if (match(TT::KEYWORD_UI_DECL) ||
                (checkContextual(TT::KEYWORD_UI_DECL) &&
                 peekNext().getType() == TT::IDENTIFIER && (advance(), true)))
            {
                return parseUIDeclaration();
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دعم تنفيذ الواجهات: نفّذ اسم_الواجهة لـ اسم_الصنف ... نهاية
            // (EN) Impl block: impl TraitName for ClassName ... end
            // (AR) نفّذ/نفذ كلمة سياقية — تُعامل كـ impl فقط إذا تلاها مُعرّف (اسم السمة)
            //      وإلا تُعامل كمُعرّف عادي (متغير/دالة)
            // (EN) نفّذ/نفذ is contextual — treated as impl only if followed by identifier (trait name)
            //      otherwise treated as regular identifier (variable/function)
            // ═══════════════════════════════════════════════════════════════════
            if (match(TT::KEYWORD_IMPL) ||
                (checkContextual(TT::KEYWORD_IMPL) &&
                 peekNext().getType() == TT::IDENTIFIER && (advance(), true)))
            {
                return parseImplDecl();
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دعم طرق الامتداد: امتداد اسم_النوع ... نهاية
            // (EN) Extension methods: extension TypeName ... end
            // (AR) امتداد كلمة سياقية — تُعامل كإعلان امتداد فقط إذا تلاها مُعرّف (اسم النوع)
            //      وإلا تُعامل كمُعرّف عادي (متغير/دالة)
            // (EN) امتداد is contextual — treated as extension only if followed by identifier (type name)
            // ═══════════════════════════════════════════════════════════════════
            if (match(TT::KEYWORD_EXTENSION) ||
                (checkContextual(TT::KEYWORD_EXTENSION) &&
                 peekNext().getType() == TT::IDENTIFIER && (advance(), true)))
            {
                return parseExtensionDecl();
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دعم الماكروز: ماكرو اسم(معاملات) ... نهاية
            // (EN) Macro support: macro name(params) ... end
            // (AR) ماكرو كلمة سياقية — تُعامل كإعلان ماكرو فقط إذا تلاها مُعرّف (اسم الماكرو)
            // (EN) ماكرو is contextual — treated as macro only if followed by identifier (macro name)
            // ═══════════════════════════════════════════════════════════════════
            if (match(TT::KEYWORD_MACRO) ||
                (checkContextual(TT::KEYWORD_MACRO) &&
                 peekNext().getType() == TT::IDENTIFIER && (advance(), true)))
            {
                return parseMacroDecl();
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دعم الأسماء المستعارة للأنواع: نوع اسم = نوع_هدف
            //      مثال: نوع عدد = رقم
            //      مثال: نوع مستخدم = شخص
            //      نوع كلمة سياقية — تُعامل كتصريح نوع مستعار فقط إذا تلاها مُعرّف
            // (EN) Type alias support: type name = target_type
            //      نوع is contextual — treated as type alias only if followed by identifier
            // ═══════════════════════════════════════════════════════════════════
            if (match(TT::KEYWORD_TYPENAME) ||
                (checkContextual(TT::KEYWORD_TYPENAME) &&
                 peekNext().getType() == TT::IDENTIFIER && (advance(), true)))
            {
                // (AR) الآن الرمز الحالي هو اسم النوع المستعار
                // (EN) Now current token is the alias name
                if (!check(TT::IDENTIFIER))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "النوع المستعار"}, {"what_en", "type alias"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_TYPENAME) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_TYPENAME) + "'"}});
                    return nullptr;
                }
                Token aliasName = current_;
                advance(); // (AR) استهلاك اسم المستعار
                if (!match(TT::OP_ASSIGN))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "="}, {"ctx_ar", "بعد اسم النوع المستعار"}, {"ctx_en", "after the type alias name"}});
                    return nullptr;
                }
                auto target = parseExpression();
                if (!target)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "بعد '=' في تصريح الاسم المستعار للنوع"}, {"ctx_en", "after '=' in a type-alias declaration"}});
                    return nullptr;
                }
                return std::make_unique<TypeAliasDecl>(
                    aliasName.getValue(), std::move(target), aliasName.getPosition());
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دعم العقود الذكية: عقد اسم ... نهاية (يُعامل كصنف مع isContract=true)
            // (EN) Smart contracts: contract name ... end (treated as class with isContract=true)
            // ═══════════════════════════════════════════════════════════════════
            if (match(TT::KEYWORD_CONTRACT) ||
                (checkContextual(TT::KEYWORD_CONTRACT) &&
                 peekNext().getType() == TT::IDENTIFIER && (advance(), true)))
            {
                auto classDecl = parseClassDecl();
                if (classDecl)
                {
                    auto *cd = dynamic_cast<ClassDecl *>(classDecl.get());
                    if (cd)
                        cd->isContract = true;
                }
                return classDecl;
            }

            // (AR) دعم المتغيرات مع كلمة متغير / (EN) Support variables with var keyword
            if (match(TT::KEYWORD_VAR))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع المتغيرات. (EN) Decorators cannot be used with variables.");
                }
                return parseVarDecl();
            }

            // (AR) دعم الثوابت مع كلمة ثابت / (EN) Support constants with const keyword
            if (match(TT::KEYWORD_CONST))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع المتغيرات. (EN) Decorators cannot be used with variables.");
                }
                pendingConst_ = true;
                auto result = parseVarDecl();
                pendingConst_ = false;
                return result;
            }

            // Check for type-first variable declaration: type IDENTIFIER = value;
            // Use proper lookahead to verify TYPE is followed by IDENTIFIER
            // (AR) التحقق من تصريح المتغير ببدء النوع: نوع معرّف = قيمة;
            // استخدام النظر المسبق الصحيح للتحقق أن TYPE متبوع بـ IDENTIFIER

            if (isTypeToken(current_.getType()))
            {
                // Look ahead to see if next token is IDENTIFIER
                const Token &nextTok = peekNext();

                if (nextTok.getType() == TT::IDENTIFIER)
                {
                    // Valid variable declaration: TYPE IDENTIFIER
                    if (!decorators.empty())
                    {
                        error("(AR) المُزخرِفات لا تُستخدم مع المتغيرات. (EN) Decorators cannot be used with variables.");
                    }
                    return parseVarDecl();
                }
                else if (nextTok.getType() == TT::PAREN_LEFT)
                {
                    // Type keyword used as function call: نص(...) → treat as expression statement
                    // (AR) كلمة نوع مستخدمة كاستدعاء دالة: نص(...) → تعامل كجملة تعبير
                    return parseStatement();
                }
                else
                {
                    // TYPE token not followed by IDENTIFIER - this is an error
                    // The user wrote a type keyword but didn't follow it with a variable name
                    // (AR) رمز النوع غير متبوع بمعرّف - خطأ نحوي
                    std::string typeStr = current_.getValue();
                    error(
                        "(AR) خطأ نحوي: بعد كلمة النوع '" + typeStr + "' يجب أن يأتي اسم المتغير.\n" +
                        "مثال صحيح: " + typeStr + " اسم_المتغير = قيمة\n" +
                        "الموقع: السطر " + std::to_string(current_.getPosition().line) + "\n" +
                        "(EN) Syntax error: After type keyword '" + typeStr + "' expected variable name.\n" +
                        "Correct example: " + typeStr + " variable_name = value\n" +
                        "Location: line " + std::to_string(current_.getPosition().line));
                    // Try to recover by synchronizing to next statement
                    synchronize();
                    return nullptr;
                }
            }

            // Check for class-typed variable declaration: ClassName varName = ...
            // (AR) التحقق من تصريح متغير من نوع صنف: اسم_الصنف اسم_المتغير = ...
            if (check(TT::IDENTIFIER))
            {
                // Peek to see if this looks like: IDENTIFIER IDENTIFIER ASSIGN
                // or: IDENTIFIER IDENTIFIER = جديد
                // (AR) استثناء الكلمات السياقية التي تبدأ جملاً (أنتج، انتظر، باستخدام)
                // (EN) Exclude contextual keywords that start statements (yield, await, with)
                const std::string &identVal = current_.getValue();
                const Token &nextTok = peekNext();
                if (nextTok.getType() == TT::IDENTIFIER &&
                    identVal != "أنتج" &&
                    identVal != "\xD9\x85\xD8\xAD\xD9\x83\xD9\x85" &&
                    identVal != "\xD8\xA3\xD8\xAC\xD9\x84" &&         // أجل (defer with hamza)
                    identVal != "\xD8\xA7\xD8\xAC\xD9\x84" &&         // اجل (defer with plain alef)
                    identVal != "\xD8\xA3\xD8\xB7\xD9\x84\xD9\x82" && // أطلق (go with hamza)
                    identVal != "\xD8\xA7\xD8\xB7\xD9\x84\xD9\x82" && // اطلق (go without hamza)
                    identVal != "\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xB1" && // اختر (select)
                    identVal != "\xD8\xA7\xD8\xB9\xD8\xB1\xD8\xB6" && // اعرض (UI widget expression)
                    identVal != "انتظر" && identVal != "باستخدام" && identVal != "حالة")
                {
                    // Could be class-typed variable, but we can't check ClassManager during parsing
                    // So we use a heuristic: if current looks like a type name (starts with capital letter
                    // or is registered as a class), treat as variable declaration
                    // For now, always treat IDENTIFIER IDENTIFIER pattern as variable declaration
                    // (AR) نستخدم قاعدة: إذا وجدنا معرّف متبوع بمعرّف، نعتبره تصريح متغير
                    if (!decorators.empty())
                    {
                        error("(AR) المُزخرِفات لا تُستخدم مع المتغيرات. (EN) Decorators cannot be used with variables.");
                    }
                    return parseVarDecl();
                }
            }

            // Check if IDENTIFIER - could be function call or expression statement
            // (AR) التحقق إذا كان معرّف - قد يكون استدعاء دالة أو جملة تعبير
            // (AR) أولاً: تحقق من الكلمات السياقية (اختبر، حالة) قبل المعالجة العامة
            // (EN) First: check contextual keywords (test, case) before generic handling
            if (checkContextual(TT::KEYWORD_TEST))
            {
                advance(); // consume contextual keyword
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع الاختبارات. (EN) Decorators cannot be used with tests.");
                }
                return parseTestDecl();
            }

            // (AR) حالة (switch) — كلمة سياقية بدون أقواس
            // (AR) فارق: إذا تلاها '(' مباشرةً → constructor call/function call (ليس switch)
            //            إذا تلاها '.' مباشرةً → وصول لعضو صنف (ليس switch)
            //            إذا تلاها '=' أو عوامل تعيين مركَّبة → تعيين متغير (ليس switch)
            //            إذا تلاها '[' → فهرسة (ليس switch)
            //            مثال: حالة(42).method() → constructor call صحيح
            //            مثال: حالة س عندما ... → switch statement
            //            مثال: حالة = 5 → variable assignment (إصلاح VE-004)
            //            مثال: حالة += 1 → compound assignment (إصلاح VE-004)
            // (EN) Disambiguation:
            //      حالة + ( . [ → expression / member access / index → not switch
            //      حالة + = += -= *= /= %= //= → assignment to variable named حالة
            //      حالة + IDENTIFIER → switch statement
            // (EN) [BF-04 fix VE-004] previously حالة was always treated as switch
            //      unless followed by ( or . — now also exclude assignments and indexing.
            if (checkContextual(TT::KEYWORD_CASE))
            {
                TT nextTT = peekNext().getType();
                bool isExpressionContext =
                    nextTT == TT::PAREN_LEFT ||         // حالة(...)
                    nextTT == TT::DOT ||                // حالة.x
                    nextTT == TT::BRACKET_LEFT ||       // حالة[i]
                    nextTT == TT::OP_ASSIGN ||          // حالة = ...
                    nextTT == TT::OP_PLUS_ASSIGN ||     // حالة += ...
                    nextTT == TT::OP_MINUS_ASSIGN ||    // حالة -= ...
                    nextTT == TT::OP_MULTIPLY_ASSIGN || // حالة *= ...
                    nextTT == TT::OP_DIVIDE_ASSIGN ||   // حالة /= ...
                    nextTT == TT::OP_FLOOR_DIVIDE_ASSIGN ||
                    nextTT == TT::OP_MODULO_ASSIGN;

                if (!isExpressionContext)
                {
                    advance(); // consume "حالة"
                    return parseSwitchStmt();
                }
                // (AR) سقوط: لا نتقدم — `حالة` تُعامل كمُعرّف عادي عبر expression statement أدناه
                // (EN) Fall through — حالة is treated as a regular identifier
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دعم تعبيرات الواجهة التصريحية على المستوى العلوي:
            //      اعرض عنصر_واجهة(...).معدّل ... نهاية
            //      مثال: اعرض عمود
            //                نص("مرحبا").حجم(32)
            //                زر("اضغط").عند_النقر => اطبع("تم!")
            //            نهاية
            //      مثال: اعرض نص("مرحبا").حجم(32)
            //
            //      'اعرض' كلمة سياقية — تُعامل كتعبير واجهة فقط إذا تلاها
            //      اسم عنصر واجهة مسجل. وإلا تُعامل كمُعرّف عادي.
            //
            // (EN) Top-level declarative UI expression support:
            //      اعرض widget_name(...).modifier ... end
            //      'اعرض' is a contextual keyword — treated as UI expression only
            //      if followed by a known widget name. Otherwise treated as identifier.
            // ═══════════════════════════════════════════════════════════════════
            if (checkContextual(TT::KEYWORD_SHOW) &&
                peekNext().getType() == TT::IDENTIFIER && isKnownWidget(peekNext().getValue()))
            {
                advance(); // (AR) استهلاك 'اعرض' / (EN) consume 'اعرض'
                auto widgetExpr = parseWidgetExpression();
                return std::make_unique<ExprStmt>(std::move(widgetExpr));
            }

            if (check(TT::IDENTIFIER))
            {
                // This is either a function call or expression statement
                // Let parseStatement handle it
                return parseStatement();
            }

            if (match(TT::KEYWORD_ENUM))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع التعدادات. (EN) Decorators cannot be used with enums.");
                }
                return parseEnumDecl();
            }

            if (match(TT::KEYWORD_STRUCT))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع البنى. (EN) Decorators cannot be used with structs.");
                }
                return parseStructDecl();
            }

            if (match(TT::KEYWORD_TEST) || matchContextual(TT::KEYWORD_TEST))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع الاختبارات. (EN) Decorators cannot be used with tests.");
                }
                return parseTestDecl();
            }

            if (match(TT::KEYWORD_IMPORT))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع الاستيراد. (EN) Decorators cannot be used with import.");
                }
                return parseImportStmt();
            }

            if (match(TT::KEYWORD_EXPORT))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع التصدير. (EN) Decorators cannot be used with export.");
                }
                return parseExportStmt();
            }

            // If decorators without valid target
            // (AR) إذا وُجدت مُزخرِفات بدون هدف صالح
            if (!decorators.empty())
            {
                error("(AR) المُزخرِفات يجب أن تسبق تصريح دالة. (EN) Decorators must precede a function declaration.");
                synchronize();
                return nullptr;
            }

            // Check for unexpected tokens that shouldn't appear at statement start
            // (AR) التحقق من الرموز غير المتوقعة في بداية الجملة
            if (check(TT::COLON) || checkSemicolon() ||
                checkComma() ||
                check(TT::BRACE_RIGHT) || check(TT::BRACKET_RIGHT) || check(TT::PAREN_RIGHT))
            {

                std::string tokenVal = current_.getValue();
                std::string tokenDesc;

                if (current_.getType() == TT::COLON)
                {
                    tokenDesc = "colon ':'";
                }
                else if (current_.getType() == TT::SEMICOLON || current_.getType() == TT::ARABIC_SEMICOLON)
                {
                    tokenDesc = "semicolon ';'";
                }
                else if (current_.getType() == TT::COMMA || current_.getType() == TT::ARABIC_COMMA)
                {
                    tokenDesc = "comma ','";
                }
                else if (current_.getType() == TT::BRACE_RIGHT)
                {
                    tokenDesc = "closing brace '}'";
                }
                else if (current_.getType() == TT::BRACKET_RIGHT)
                {
                    tokenDesc = "closing bracket ']'";
                }
                else if (current_.getType() == TT::PAREN_RIGHT)
                {
                    tokenDesc = "closing parenthesis ')'";
                }

                // Show detailed error message in both languages
                errorCatalog(Errors::ErrorCode::SYN_UNEXPECTED_TOKEN, {{"found", tokenVal + " (" + tokenDesc + ")"}, {"expected", "بداية جملة أو تصريح (statement/declaration start)"}});
                synchronize();
                return nullptr;
            }

            // If no declaration keyword, parse as statement
            // (AR) إذا لم توجد كلمة تصريح، حلل كجملة
            return parseStatement();
        }

        /**
         * @brief (AR) يحلل جملة (if، while، for، return، إلخ).
         *        (EN) Parses statement (if, while, for, return, etc.).
         */
        StmtPtr ParserCore::parseStatement()
        {
            // Control flow statements
            // (AR) جمل التحكم في التدفق

            if (match(TT::KEYWORD_IF))
            {
                return parseIfStmt();
            }

            if (match(TT::KEYWORD_WHILE))
            {
                return parseWhileStmt();
            }

            if (match(TT::KEYWORD_FOR))
            {
                return parseForStmt();
            }

            // ─────────────────────────────────────────────────────────────────────
            // (AR) حالة (switch/case) — بدون أقواس: `حالة تعبير`
            // (AR) لا تطابق إذا تلاها '(' أو '.' أو '[' أو عوامل تعيين — فهي expression
            //      [إصلاح BF-04 VE-004] إضافة استثناءات لعوامل التعيين والفهرسة
            //      حتى يعمل `حالة = 5` و `حالة += 1` و `حالة[0] = ...` كتعيين متغير
            // (EN) Skip if followed by '(' '.' '[' or assignment operators
            // ─────────────────────────────────────────────────────────────────────
            if (match(TT::KEYWORD_CASE))
            {
                return parseSwitchStmt();
            }
            if (checkContextual(TT::KEYWORD_CASE))
            {
                TT nextTT2 = peekNext().getType();
                bool isExprCtx2 =
                    nextTT2 == TT::PAREN_LEFT ||
                    nextTT2 == TT::DOT ||
                    nextTT2 == TT::BRACKET_LEFT ||
                    nextTT2 == TT::OP_ASSIGN ||
                    nextTT2 == TT::OP_PLUS_ASSIGN ||
                    nextTT2 == TT::OP_MINUS_ASSIGN ||
                    nextTT2 == TT::OP_MULTIPLY_ASSIGN ||
                    nextTT2 == TT::OP_DIVIDE_ASSIGN ||
                    nextTT2 == TT::OP_FLOOR_DIVIDE_ASSIGN ||
                    nextTT2 == TT::OP_MODULO_ASSIGN;
                if (!isExprCtx2)
                {
                    advance(); // consume "حالة"
                    return parseSwitchStmt();
                }
                // (AR) سقوط: `حالة` كمُعرّف عادي — يُعالَج بمعالجة الجمل التعبيرية أدناه
            }

            if (match(TT::KEYWORD_MATCH))
            {
                return parseMatchStmt();
            }

            if (match(TT::KEYWORD_RETURN))
            {
                return parseReturnStmt();
            }

            if (match(TT::KEYWORD_YIELD) || matchContextual(TT::KEYWORD_YIELD))
            {
                return parseYieldStmt();
            }

            // (AR) رسالة خطأ لـ 'اعطِ' المُزالة
            if (checkContextual(TT::KEYWORD_GIVE_DEPRECATED))
            {
                errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", kw(TT::KEYWORD_GIVE_DEPRECATED)}, {"new", kw(TT::KEYWORD_YIELD)}, {"example", kw(TT::KEYWORD_YIELD) + " 1"}});
                advance();
                return parseYieldStmt();
            }

            if (match(TT::KEYWORD_WITH) || matchContextual(TT::KEYWORD_WITH))
            {
                return parseWithStmt();
            }

            // ─────────────────────────────────────────────────────────────────────
            // (AR) أجّل (defer) — تنظيف مضمون عند خروج الدالة
            //      كلمة سياقية: IDENTIFIER بقيمة "أجل" (الشدة تُجرَّد تلقائياً)
            //      نتحقق من كلا الشكلين: أجل (بهمزة) و اجل (بألف عادية)
            //      الصيغة: أجّل جملة  أو  أجّل ... نهاية
            // (EN) defer — guaranteed cleanup on function exit
            //      Contextual keyword: IDENTIFIER with value "أجل" (shadda stripped)
            //      Check both forms: أجل (with hamza) and اجل (plain alef)
            //      Syntax: أجّل statement  or  أجّل ... نهاية
            // ─────────────────────────────────────────────────────────────────────
            if (match(TT::KEYWORD_DEFER) ||
                (checkContextual(TT::KEYWORD_DEFER)
                 && peekNext().getType() != TT::OP_ASSIGN                      // ليس إسناد (اجل = ...)
                 && peekNext().getType() != TT::OP_PLUS_ASSIGN                 // ليس += (اجل += ...)
                 && peekNext().getType() != TT::DOT                            // ليس وصول (اجل.شيء)
                 && (advance(), true)))
            {
                return parseDeferStmt();
            }

            // ─────────────────────────────────────────────────────────────────────
            // (AR) أطلق (go) — إطلاق goroutine (مهمة متزامنة خفيفة)
            //      كلمة سياقية: IDENTIFIER بقيمة "أطلق"
            //      الصيغة: أطلق تعبير  أو  أطلق ... نهاية
            // (EN) go — launch a goroutine (lightweight concurrent task)
            //      Contextual keyword: IDENTIFIER with value "أطلق"
            //      Syntax: أطلق expression  or  أطلق ... نهاية
            // ─────────────────────────────────────────────────────────────────────
            if (match(TT::KEYWORD_GO) ||
                (checkContextual(TT::KEYWORD_GO)
                 && peekNext().getType() != TT::OP_ASSIGN                      // ليس إسناد
                 && peekNext().getType() != TT::OP_PLUS_ASSIGN                 // ليس +=
                 && peekNext().getType() != TT::DOT                            // ليس وصول
                 && (advance(), true)))
            {
                return parseGoStmt();
            }

            // ─────────────────────────────────────────────────────────────────────
            // (AR) اختر (select) — اختيار من قنوات متعددة
            //      كلمة سياقية: IDENTIFIER بقيمة "اختر"
            //      الصيغة: اختر عندما ق.استقبل(): ... افتراضي: ... نهاية
            // (EN) select — multiplex on multiple channels
            //      Contextual keyword: IDENTIFIER with value "اختر"
            //      Syntax: اختر عندما ch.receive(): ... افتراضي: ... نهاية
            // ─────────────────────────────────────────────────────────────────────
            if (match(TT::KEYWORD_SELECT) ||
                (checkContextual(TT::KEYWORD_SELECT)
                 && peekNext().getType() != TT::OP_ASSIGN && peekNext().getType() != TT::OP_PLUS_ASSIGN && peekNext().getType() != TT::DOT && (advance(), true)))
            {
                return parseSelectStmt();
            }

            if (match(TT::KEYWORD_BREAK))
            {
                return parseBreakStmt();
            }

            if (match(TT::KEYWORD_CONTINUE))
            {
                return parseContinueStmt();
            }

            // Check for block vs map literal
            // Strategy: { followed by expression is likely map if we find : early
            // Block: { stmt; stmt; }
            // Map: {k: v} or comprehension {لكل x في src أنتج ...}
            // (AR) التحقق من block أو خريطة حرفية
            if (check(TT::BRACE_LEFT))
            {
                // Save position
                Token brace = current_;
                advance(); // consume {

                // Check for empty map
                if (check(TT::BRACE_RIGHT))
                {
                    consume(TT::BRACE_RIGHT, "Expected }");
                    auto mapExpr = std::make_unique<MapExpr>(std::vector<MapPair>{}, brace.getPosition());
                    return std::make_unique<ExprStmt>(std::move(mapExpr));
                }

                // Try to parse first expression (key)
                // This could be: identifier, number, string, or complex expression
                ExprPtr firstKey = parseExpression();

                if (!firstKey)
                {
                    // (AR) أثر تشخيصيّ إلى stderr لا stdout: stdout قد يكون قناة آليّة نظيفة
                    //      (مثل sad-check --json) فتلويثه يُفسد التحليل. (EN) diagnostic trace to
                    //      stderr, never stdout — stdout may be a clean machine channel (JSON).
                    std::cerr << "Failed to parse key, treating as block\n";
                    auto block = parseBlockStmt();
                    return block;
                }

                // Check if followed by colon or = (map syntax)
                if (check(TT::COLON) || check(TT::OP_ASSIGN))
                {
                    advance(); // consume ':' or '='

                    // Parse first value
                    ExprPtr firstValue = parseExpression();

                    if (!firstValue)
                    {
                        errorCatalogExpected(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "(قيمة) بعد ':' في استيعاب القائمة"}, {"ctx_en", "(value) after ':' in a list comprehension"}});
                        return nullptr;
                    }

                    // Check for dict comprehension
                    if (check(TT::KEYWORD_FOR))
                    {
                        advance(); // consume 'for'

                        Token loopVar = consume(TT::IDENTIFIER, "Expected loop variable");
                        consume(TT::KEYWORD_IN, "Expected 'in'");
                        auto iterable = parseExpression();

                        ExprPtr condition = nullptr;
                        if (match(TT::KEYWORD_IF))
                        {
                            condition = parseExpression();
                        }

                        consume(TT::BRACE_RIGHT, "Expected }");

                        // Create dict comprehension
                        auto dictComp = std::make_unique<DictComprehensionExpr>(
                            std::move(firstKey),
                            std::move(firstValue),
                            loopVar.getValue(),
                            std::move(iterable),
                            std::move(condition),
                            brace.getPosition());

                        return std::make_unique<ExprStmt>(std::move(dictComp));
                    }

                    // Regular map literal
                    // (AR) أثر تشخيصيّ إلى stderr لا stdout (يمنع تلويث قناة JSON الآليّة).
                    // (EN) diagnostic trace to stderr, not stdout (keeps machine JSON clean).
                    std::cerr << "Regular map literal\n";
                    std::vector<MapPair> pairs;
                    pairs.emplace_back(std::move(firstKey), std::move(firstValue));

                    while (matchComma())
                    {
                        if (check(TT::BRACE_RIGHT))
                            break;

                        // (AR) استخدم parseTernary لتجنب تفسير = كإسناد
                        auto key = parseTernary();
                        if (!check(TT::COLON) && !check(TT::OP_ASSIGN))
                        {
                            errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ":"}, {"ctx_ar", "بعد مفتاح الخريطة (أو '=')"}, {"ctx_en", "after the map key (or '=')"}});
                            return nullptr;
                        }
                        advance(); // consume ':' or '='
                        auto value = parseExpression();
                        pairs.emplace_back(std::move(key), std::move(value));
                    }

                    consume(TT::BRACE_RIGHT, "Expected }");

                    auto mapExpr = std::make_unique<MapExpr>(std::move(pairs), brace.getPosition());
                    return std::make_unique<ExprStmt>(std::move(mapExpr));
                }

                // No colon found - this must be a block statement with expression
                // But we've already consumed { and parsed an expression
                // This is problematic - we need to handle this as expression statement in block
                // (AR) أثر تشخيصيّ إلى stderr لا stdout (انظر أعلاه) — يمنع تلويث قناة JSON.
                // (EN) diagnostic trace to stderr, not stdout (see above) — keeps JSON clean.
                std::cerr << "No colon found, treating as block with expression statement\n";

                // We have an expression, make it an expression statement
                auto exprStmt = std::make_unique<ExprStmt>(std::move(firstKey));

                // Continue parsing rest of block
                StmtList statements;
                statements.push_back(std::move(exprStmt));

                // Parse remaining statements
                while (!check(TT::BRACE_RIGHT) && !isAtEnd())
                {
                    auto stmt = parseDeclaration();
                    if (stmt)
                    {
                        statements.push_back(std::move(stmt));
                    }
                }

                consume(TT::BRACE_RIGHT, "Expected }");

                return std::make_unique<BlockStmt>(std::move(statements), brace.getPosition());
            }

            if (match(TT::KEYWORD_TRY))
            {
                return parseTryStmt();
            }

            if (match(TT::KEYWORD_THROW))
            {
                return parseRaiseStmt();
            }

            // Default: expression statement
            // (AR) الافتراضي: جملة تعبير
            return parseExpressionStmt();
        }

        // ======================================================================
        // (AR) تحليل التصريحات / (EN) Declaration Parsing
        // ======================================================================

        // ======================================================================
        // (AR) تحليل التوجيهات @ / (EN) @ Directive Parsing
        // ======================================================================

        /**
         * @brief (AR) محاولة تحليل توجيه @ — يعيد nullptr إذا لم يكن توجيهاً
         * @brief (EN) Try parsing an @ directive — returns nullptr if not a directive
         *
         * (AR) الأسماء المعروفة: غير_آمن، وقت_الترجمة، متطاير، تجميع، حجم، ذري
         * (EN) Known names: غير_آمن, وقت_الترجمة, متطاير, تجميع, حجم, ذري
         */
        StmtPtr ParserCore::tryParseDirective()
        {
            // (AR) نتحقق أن الرمز الحالي هو @ والتالي هو معرّف أو كلمة مفتاحية بأحد أسماء التوجيهات
            // (EN) Check current is @ and next is an identifier/keyword with a known directive name
            if (!check(TT::AT_SIGN))
                return nullptr;

            // (AR) النظر المسبق: ما بعد @ يجب أن يكون معرّفاً أو كلمة مفتاحية خاصة
            const auto &next = peekNext();
            TT nextType = next.getType();

            // (AR) قائمة الكلمات المفتاحية المسموحة كتوجيهات
            // (EN) List of keywords allowed as directives
            // (AR) KEYWORD_VOLATILE أُزيل من قائمة توجيهات @ (اللبنة 3.14): «متطاير»
            //      صار لصيقة لاحقة «متغير متطاير» لا توجيهًا سابقًا @متطاير.
            bool isDirectiveKeyword = (nextType == TT::KEYWORD_UNSAFE ||
                                       nextType == TT::KEYWORD_COMPTIME ||
                                       nextType == TT::KEYWORD_SIZEOF ||
                                       nextType == TT::KEYWORD_ATOMIC ||
                                       nextType == TT::KEYWORD_ASM);

            if (nextType != TT::IDENTIFIER && !isDirectiveKeyword)
                return nullptr;

            const std::string &name = next.getValue();

            // ─── @غير_آمن ... نهاية ───
            if (nextType == TT::KEYWORD_UNSAFE || name == "غير_آمن")
            {
                auto pos = current_.getPosition();
                advance(); // consume @
                advance(); // consume غير_آمن

                StmtList body;
                while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
                {
                    auto stmt = parseDeclaration();
                    if (stmt)
                        body.push_back(std::move(stmt));
                }
                consume(TT::KEYWORD_END,
                        "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق كتلة @غير_آمن.\n"
                        "(EN) Syntax error: expected 'نهاية' to close @غير_آمن block.");

                return std::make_unique<UnsafeBlockStmt>(std::move(body), pos);
            }

            // ─── @وقت_الترجمة ... نهاية ───
            if (nextType == TT::KEYWORD_COMPTIME || name == "وقت_الترجمة")
            {
                auto pos = current_.getPosition();
                advance(); // consume @
                advance(); // consume وقت_الترجمة

                StmtList body;
                while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
                {
                    auto stmt = parseDeclaration();
                    if (stmt)
                        body.push_back(std::move(stmt));
                }
                consume(TT::KEYWORD_END,
                        "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق كتلة @وقت_الترجمة.\n"
                        "(EN) Syntax error: expected 'نهاية' to close @وقت_الترجمة block.");

                return std::make_unique<ComptimeBlockStmt>(std::move(body), pos);
            }

            // (AR) توجيه @متطاير حُذف (اللبنة 3.14): الصياغة العربيّة الصحيحة لصيقة
            //      لاحقة «متغير متطاير» (الموصوف ثمّ الصفة) — تُعالَج في parseVarDecl.
            //      @رمز يبقى توجيهًا سابقًا (سمة اسميّة ذات وسيط لا صفة نحويّة).

            // ─── @رمز("اسم") متغير/ثابت — رمز رابط مُصدَّر ثابت (اللبنة 3.14) ───
            if (name == "رمز")
            {
                auto pos = current_.getPosition();
                advance(); // consume @
                advance(); // consume رمز

                consume(TT::PAREN_LEFT,
                        "(AR) خطأ نحوي: توقع '(' بعد @رمز.\n"
                        "(EN) Syntax error: expected '(' after @رمز.");
                Token symTok = consume(TT::STRING_LITERAL,
                                       "(AR) خطأ نحوي: توقع اسم الرمز كنص حرفي بعد @رمز.\n"
                                       "(EN) Syntax error: expected symbol name as string literal after @رمز.");
                consume(TT::PAREN_RIGHT,
                        "(AR) خطأ نحوي: توقع ')' بعد اسم الرمز في @رمز.\n"
                        "(EN) Syntax error: expected ')' after symbol name in @رمز.");

                // (AR) التالي تصريح متغيّر/ثابت (قد يسبقه @متطاير — يلتقطه parseDeclaration)
                auto decl = parseDeclaration();
                // (AR) درِّل إلى VarDeclStmt (مباشرةً أو داخل غلاف @متطاير) واضبط الرمز
                VarDeclStmt *vd = dynamic_cast<VarDeclStmt *>(decl.get());
                if (!vd)
                {
                    if (auto *vol = dynamic_cast<VolatileVarDeclStmt *>(decl.get()))
                        vd = dynamic_cast<VarDeclStmt *>(vol->declaration.get());
                }
                if (vd)
                    vd->linkSymbol = symTok.getValue();
                else if (auto *fd = dynamic_cast<FunctionDecl *>(decl.get()))
                    // (AR) (اللبنة 3.17) @رمز على دالّة: تُصدَّر تحت اسم الرمز عبر
                    //      linkName بربط خارجيّ (ExternalLinkage) — يستدعيها كود C/
                    //      المترجم بالاسم ASCII حرفيًّا. تُطفئ تعريفات C مثل
                    //      __sad_panic بتعريف ص مكافئ (getLinkName يُرجِع linkName).
                    fd->linkName = symTok.getValue();
                else
                    error("(AR) خطأ نحوي: @رمز يتطلّب تصريح 'متغير' أو 'ثابت' أو 'دالة'.\n"
                          "(EN) Syntax error: @رمز requires a 'متغير', 'ثابت', or 'دالة' declaration.");
                return decl;
            }

            // ─── @تجميع("code") — inline assembly statement ───
            if (nextType == TT::KEYWORD_ASM || name == "تجميع")
            {
                auto pos = current_.getPosition();
                advance(); // consume @
                advance(); // consume تجميع

                consume(TT::PAREN_LEFT,
                        "(AR) خطأ نحوي: توقع '(' بعد @تجميع.\n"
                        "(EN) Syntax error: expected '(' after @تجميع.");

                Token asmCode = consume(TT::STRING_LITERAL,
                                        "(AR) خطأ نحوي: توقع نص التجميع كنص حرفي.\n"
                                        "(EN) Syntax error: expected assembly code as string literal.");

                // (AR) تحليل اختياري: المخرجات والمدخلات والقيود
                // (EN) Optional: output constraints, input constraints, clobbers
                std::string output, input, clobbers;
                bool isVolatile = false;

                if (match(TT::COMMA))
                {
                    // (AR) وسيط ثاني: قيود المخرجات أو "متطاير"
                    if (check(TT::STRING_LITERAL))
                    {
                        output = current_.getValue();
                        advance();
                    }
                    else if (checkContextual(TT::KEYWORD_VOLATILE))
                    {
                        isVolatile = true;
                        advance();
                    }
                }
                if (match(TT::COMMA))
                {
                    if (check(TT::STRING_LITERAL))
                    {
                        input = current_.getValue();
                        advance();
                    }
                }
                if (match(TT::COMMA))
                {
                    if (check(TT::STRING_LITERAL))
                    {
                        clobbers = current_.getValue();
                        advance();
                    }
                }

                consume(TT::PAREN_RIGHT,
                        "(AR) خطأ نحوي: توقع ')' لإغلاق @تجميع.\n"
                        "(EN) Syntax error: expected ')' to close @تجميع.");

                auto asmExpr = std::make_unique<InlineAsmExpr>(
                    asmCode.getValue(), output, input, clobbers, isVolatile, pos);

                // (AR) نلفه في ExprStmt
                // (EN) Wrap in ExprStmt
                return std::make_unique<ExprStmt>(std::move(asmExpr));
            }

            // ─── @حجم(type) و @ذري(op, ...) — تعبيرات → نلفها في ExprStmt ───
            if (nextType == TT::KEYWORD_SIZEOF || name == "حجم" ||
                nextType == TT::KEYWORD_ATOMIC || name == "ذري")
            {
                auto expr = parseDirectiveExpr();
                if (expr)
                {
                    return std::make_unique<ExprStmt>(std::move(expr));
                }
            }

            // (AR) ليس توجيهاً معروفاً — تُرجع nullptr ليتم التعامل معه كمُزخرِف
            // (EN) Not a known directive — return nullptr so it's treated as a decorator
            return nullptr;
        }

        /**
         * @brief (AR) تحليل تعبير @ — @حجم(نوع) أو @ذري(عملية, ...)
         * @brief (EN) Parse @ expression — @حجم(type) or @ذري(op, ...)
         */
        ExprPtr ParserCore::parseDirectiveExpr()
        {
            if (!check(TT::AT_SIGN))
                return nullptr;

            const auto &next = peekNext();
            TT nextType = next.getType();

            // ─── @حجم(نوع) ───
            // (AR) التحقق من KEYWORD_SIZEOF أو معرّف "حجم"
            // (EN) Check for KEYWORD_SIZEOF or identifier "حجم"
            if (nextType == TT::KEYWORD_SIZEOF ||
                (nextType == TT::IDENTIFIER && next.getValue() == "حجم"))
            {
                auto pos = current_.getPosition();
                advance(); // consume @
                advance(); // consume حجم

                consume(TT::PAREN_LEFT,
                        "(AR) خطأ نحوي: توقع '(' بعد @حجم.\n"
                        "(EN) Syntax error: expected '(' after @حجم.");

                // (AR) اسم النوع: معرّف أو كلمة نوع
                std::string typeName;
                if (check(TT::IDENTIFIER))
                {
                    typeName = current_.getValue();
                    advance();
                }
                else
                {
                    // (AR) محاولة قراءة اسم النوع من أي رمز
                    typeName = current_.getValue();
                    advance();
                }

                consume(TT::PAREN_RIGHT,
                        "(AR) خطأ نحوي: توقع ')' بعد اسم النوع في @حجم.\n"
                        "(EN) Syntax error: expected ')' after type name in @حجم.");

                return std::make_unique<SizeofExpr>(typeName, pos);
            }

            // ─── @ذري(عملية, وسائط...) ───
            // (AR) التحقق من KEYWORD_ATOMIC أو معرّف "ذري"
            // (EN) Check for KEYWORD_ATOMIC or identifier "ذري"
            if (nextType == TT::KEYWORD_ATOMIC ||
                (nextType == TT::IDENTIFIER && next.getValue() == "ذري"))
            {
                auto pos = current_.getPosition();
                advance(); // consume @
                advance(); // consume ذري

                consume(TT::PAREN_LEFT,
                        "(AR) خطأ نحوي: توقع '(' بعد @ذري.\n"
                        "(EN) Syntax error: expected '(' after @ذري.");

                // (AR) أول وسيط: اسم العملية (معرّف)
                // (EN) First arg: operation name (identifier)
                std::string opName;
                if (check(TT::IDENTIFIER))
                {
                    opName = current_.getValue();
                    advance();
                }
                else
                {
                    error("(AR) خطأ نحوي: توقع اسم العملية بعد @ذري(.\n"
                          "(EN) Syntax error: expected operation name after @ذري(.");
                    return nullptr;
                }

                // (AR) بقية الوسائط: تعبيرات مفصولة بفاصلة
                // (EN) Remaining args: comma-separated expressions
                ExprList operands;
                while (matchComma())
                {
                    operands.push_back(parseExpression());
                }

                consume(TT::PAREN_RIGHT,
                        "(AR) خطأ نحوي: توقع ')' لإغلاق @ذري.\n"
                        "(EN) Syntax error: expected ')' to close @ذري.");

                return std::make_unique<AtomicExpr>(opName, std::move(operands), pos);
            }

            return nullptr;
        }

        // ═════════════════════════════════════════════════════════════════════
        // (AR) كتلة لهجة التجميع العربيّ «تجميع … نهاية» (م١ RFC اللهجات الأصيلة)
        // (EN) Arabic assembly dialect block (native-dialects RFC M1)
        // ═════════════════════════════════════════════════════════════════════

        namespace
        {
            // (AR) مسافة تحرير (Levenshtein) بايتيّة — كافية لترتيب الاقتراحات.
            // (EN) Byte-wise Levenshtein distance — sufficient for ranking suggestions.
            std::size_t asmEditDistance(const std::string &a, const std::string &b)
            {
                const std::size_t n = a.size(), m = b.size();
                std::vector<std::size_t> prev(m + 1), cur(m + 1);
                for (std::size_t j = 0; j <= m; ++j)
                    prev[j] = j;
                for (std::size_t i = 1; i <= n; ++i)
                {
                    cur[0] = i;
                    for (std::size_t j = 1; j <= m; ++j)
                    {
                        const std::size_t sub = prev[j - 1] + (a[i - 1] == b[j - 1] ? 0 : 1);
                        cur[j] = std::min({prev[j] + 1, cur[j - 1] + 1, sub});
                    }
                    std::swap(prev, cur);
                }
                return prev[m];
            }

            // (AR) أقرب ثلاثة أسماء معجميّة للاسم المجهول (لقائمة «هل تقصد؟»). المرشّحون
            //      من المنمنمات و(اختياريًّا) السجلّات — كي يُقترَح لسجلّ مُخطأ سجلٌّ لا منمنمة.
            // (EN) Three nearest lexicon names. Candidates from mnemonics and (optionally)
            //      registers, so a mistyped register suggests a register, not a mnemonic.
            std::string asmNearestNames(const std::string &name, bool includeRegisters)
            {
                using namespace ::Sad::Dialects::Asm;
                std::vector<std::pair<std::size_t, const char *>> ranked;
                ranked.reserve(kMnemonicCount + kRegisterCount);
                for (std::size_t i = 0; i < kMnemonicCount; ++i)
                    ranked.emplace_back(asmEditDistance(name, kMnemonics[i].ar), kMnemonics[i].ar);
                if (includeRegisters)
                    for (std::size_t i = 0; i < kRegisterCount; ++i)
                        ranked.emplace_back(asmEditDistance(name, kRegisters[i].ar), kRegisters[i].ar);
                std::sort(ranked.begin(), ranked.end(),
                          [](const auto &x, const auto &y) { return x.first < y.first; });
                std::string out;
                const std::size_t take = std::min<std::size_t>(3, ranked.size());
                for (std::size_t i = 0; i < take; ++i)
                {
                    if (i)
                        out += "\xd8\x8c "; // (AR) فاصلة عربيّة بين الاقتراحات / Arabic comma separator
                    out += ranked[i].second;
                }
                return out;
            }

            // (AR) للمنمنمات فقط (موضع بداية التعليمة). / mnemonic-only (instruction head).
            std::string asmNearestMnemonics(const std::string &name)
            {
                return asmNearestNames(name, /*includeRegisters=*/false);
            }
        } // namespace

        AST::AsmOperand ParserCore::parseAsmOperand(char operandClass)
        {
            using namespace ::Sad::Dialects::Asm;
            AST::AsmOperand op;

            // ── {متغيّر ص} — ربط تلقائيّ بقيد InlineAsm ──
            if (check(TT::BRACE_LEFT))
            {
                advance(); // {
                if (check(TT::IDENTIFIER))
                {
                    op.kind = AST::AsmOperand::Kind::SadVariable;
                    op.text = current_.getValue();
                    op.varExpr = std::make_unique<AST::VariableExpr>(
                        current_.getValue(), current_.getPosition());
                    advance();
                }
                else
                {
                    errorCatalogExpected(Errors::ErrorCode::SYN_MISSING_IDENTIFIER);
                }
                if (check(TT::BRACE_RIGHT))
                    advance();
                else
                    errorCatalogExpected(Errors::ErrorCode::SYN_EXPECTED_SYMBOL,
                                         {{"symbol", "}"}});
                return op;
            }

            // ── [تعبير عنونة] ──
            if (check(TT::BRACKET_LEFT))
            {
                advance(); // [
                op.kind = AST::AsmOperand::Kind::Memory;
                while (!check(TT::BRACKET_RIGHT) && !check(TT::END_OF_FILE))
                {
                    AST::AsmOperand piece;
                    if (check(TT::BRACE_LEFT))
                    {
                        advance();
                        if (check(TT::IDENTIFIER))
                        {
                            piece.kind = AST::AsmOperand::Kind::SadVariable;
                            piece.text = current_.getValue();
                            piece.varExpr = std::make_unique<AST::VariableExpr>(
                                current_.getValue(), current_.getPosition());
                            advance();
                        }
                        else
                        {
                            errorCatalogExpected(Errors::ErrorCode::SYN_MISSING_IDENTIFIER);
                            break;
                        }
                        if (check(TT::BRACE_RIGHT))
                            advance();
                    }
                    else if (check(TT::IDENTIFIER))
                    {
                        const Register *reg = findRegister(current_.getValue().c_str());
                        if (!reg)
                        {
                            // (AR) موضع سجلّ (داخل عنونة) ⇒ خطأ سجلّ SEM033 واقتراحات
                            //      تشمل السجلّات (تصحيح ٦) — لا يُسمّى السجلّ «منمنمة».
                            errorCatalog(Errors::ErrorCode::SEM_ASM_UNKNOWN_REGISTER,
                                         {{"register", current_.getValue()},
                                          {"suggestions", asmNearestNames(current_.getValue(), true)}});
                            advance();
                            continue;
                        }
                        piece.kind = AST::AsmOperand::Kind::Register;
                        piece.text = current_.getValue();
                        piece.loweredText = reg->en;
                        advance();
                    }
                    else if (check(TT::NUMBER_INTEGER))
                    {
                        piece.kind = AST::AsmOperand::Kind::Immediate;
                        piece.text = current_.getValue();
                        piece.loweredText = current_.getValue();
                        advance();
                    }
                    else if (check(TT::OP_PLUS) || check(TT::OP_MINUS) || check(TT::OP_MULTIPLY))
                    {
                        piece.kind = AST::AsmOperand::Kind::Punct;
                        piece.loweredText = (current_.getType() == TT::OP_PLUS)    ? "+"
                                            : (current_.getType() == TT::OP_MINUS) ? "-"
                                                                                   : "*";
                        advance();
                    }
                    else
                    {
                        errorCatalogExpected(Errors::ErrorCode::SYN_EXPECTED_SYMBOL,
                                             {{"symbol", "]"}});
                        break;
                    }
                    op.pieces.push_back(std::move(piece));
                }
                if (check(TT::BRACKET_RIGHT))
                    advance();

                // (AR) [تصحيح ١] فحص صيغة العنونة: قاعدة واحدة فقط (سجلّ أو متغيّر ص)
                //      وإزاحة اختياريّة؛ قاعدتان أو فهرس×حجم (×) ⇒ SEM032 (لا إسقاط صامت).
                // (EN) [Fix 1] one base + optional disp; two bases or index*scale ⇒ SEM032.
                {
                    int baseCount = 0;
                    bool indexScale = false;
                    for (const auto &p : op.pieces)
                    {
                        if (p.kind == AST::AsmOperand::Kind::Register ||
                            p.kind == AST::AsmOperand::Kind::SadVariable)
                            ++baseCount;
                        else if (p.kind == AST::AsmOperand::Kind::Punct && p.loweredText == "*")
                            indexScale = true;
                    }
                    if (baseCount != 1 || indexScale)
                        errorCatalog(Errors::ErrorCode::SEM_ASM_MEMORY_FORM, {});
                }
                return op;
            }

            // ── عدد صحيح (قد تسبقه إشارة سالب) ──
            if (check(TT::NUMBER_INTEGER) ||
                (check(TT::OP_MINUS) && peekNext().getType() == TT::NUMBER_INTEGER))
            {
                std::string digits;
                if (check(TT::OP_MINUS))
                {
                    digits += "-";
                    advance();
                }
                digits += current_.getValue();
                advance();
                op.kind = AST::AsmOperand::Kind::Immediate;
                op.text = digits;
                op.loweredText = digits;
                return op;
            }

            // ── معرّف: سجلّ أو لصيقة أو منمنمة تابعة (حسب صنف المعامل من المعجم) ──
            if (check(TT::IDENTIFIER))
            {
                const std::string name = current_.getValue();
                if (operandClass == 'l')
                {
                    op.kind = AST::AsmOperand::Kind::LabelRef;
                    op.text = name;
                    advance();
                    return op;
                }
                if (operandClass == 'a')
                {
                    const Mnemonic *sub = findMnemonic(name.c_str());
                    if (!sub || sub->operandClasses[0] != '\0')
                    {
                        // (AR) بادئة «كرّر» تتبعها منمنمة معجميّة بلا معاملات فقط
                        errorCatalog(Errors::ErrorCode::SEM_ASM_UNKNOWN_MNEMONIC,
                                     {{"mnemonic", name},
                                      {"suggestions", asmNearestMnemonics(name)}});
                        advance();
                        return op;
                    }
                    op.kind = AST::AsmOperand::Kind::SubMnemonic;
                    op.text = name;
                    op.loweredText = sub->en;
                    advance();
                    return op;
                }
                const Register *reg = findRegister(name.c_str());
                if (reg)
                {
                    op.kind = AST::AsmOperand::Kind::Register;
                    op.text = name;
                    op.loweredText = reg->en;
                    advance();
                    return op;
                }
                // (AR) معرّف ليس سجلًّا في موضع سجلّ/قيمة — خطأ سجلّ SEM033.
                //      الاقتراحات تشمل السجلّات (تصحيح ٦: سجلّ مُخطأ يُقترَح له سجلّ).
                errorCatalog(Errors::ErrorCode::SEM_ASM_UNKNOWN_REGISTER,
                             {{"register", name},
                              {"suggestions", asmNearestNames(name, true)}});
                advance();
                return op;
            }

            errorCatalogExpected(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION);
            if (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
                advance();
            return op;
        }

        AST::StmtPtr ParserCore::parseAsmBlockStmt()
        {
            using namespace ::Sad::Dialects::Asm;

            auto pos = current_.getPosition();
            advance(); // (AR) استهلاك «تجميع»

            auto block = std::make_unique<AST::AsmBlockStmt>(pos);

            // (AR) «تجميع متطاير» — مُعدِّل اختياريّ (الكتلة متطايرة دومًا في م١)
            if (checkContextual(TT::KEYWORD_VOLATILE))
                advance();

            // (AR) متتبِّعات فحص بنيويّ زمن التحليل (تفعل في المحرّكين معًا كـSEM025/026):
            //      لصائق معرَّفة، مراجع قفز، متغيّرات ص مكتوبة.
            // (EN) Parse-time structural trackers (dual-engine, like SEM025/026):
            //      defined labels, jump refs, written sad vars.
            std::set<std::string> definedLabelNames;
            std::vector<std::string> labelRefs;
            std::set<std::string> writtenVars;
            bool multiOutputReported = false;

            while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
            {
                // ── بند يلوّث(هدف {، هدف}) ──
                if (check(TT::IDENTIFIER) && current_.getValue() == kClobberKeyword)
                {
                    advance();
                    if (!check(TT::PAREN_LEFT))
                    {
                        errorCatalogExpected(Errors::ErrorCode::SYN_EXPECTED_SYMBOL,
                                             {{"symbol", "("}});
                        break;
                    }
                    advance(); // (
                    while (!check(TT::PAREN_RIGHT) && !check(TT::END_OF_FILE))
                    {
                        if (!check(TT::IDENTIFIER))
                        {
                            errorCatalogExpected(Errors::ErrorCode::SYN_MISSING_IDENTIFIER);
                            break;
                        }
                        const std::string target = current_.getValue();
                        AST::AsmClobber clob;
                        clob.ar = target;
                        if (const ClobberSpecial *sp = findClobberSpecial(target.c_str()))
                            clob.llvm = sp->llvm;
                        else if (const Register *reg = findRegister(target.c_str()))
                            clob.llvm = std::string("~{") + reg->en + "}";
                        else
                            // (AR) هدف يلوّث = سجلّ أو هدف مخصوص ⇒ خطأ سجلّ SEM033
                            //      واقتراحات تشمل السجلّات (لا اقتراح منمنمات هنا).
                            errorCatalog(Errors::ErrorCode::SEM_ASM_UNKNOWN_REGISTER,
                                         {{"register", target},
                                          {"suggestions", asmNearestNames(target, true)}});
                        if (!clob.llvm.empty())
                            block->clobbers.push_back(std::move(clob));
                        advance();
                        if (check(TT::ARABIC_COMMA) || check(TT::COMMA))
                            advance();
                        else
                            break;
                    }
                    if (check(TT::PAREN_RIGHT))
                        advance();
                    else
                        errorCatalogExpected(Errors::ErrorCode::SYN_EXPECTED_SYMBOL,
                                             {{"symbol", ")"}});
                    continue;
                }

                // ── لصيقة «اسم:» ──
                if (check(TT::IDENTIFIER) && peekNext().getType() == TT::COLON)
                {
                    AST::AsmItem item;
                    item.isLabel = true;
                    item.labelName = current_.getValue();
                    item.pos = current_.getPosition();
                    // (AR) [تصحيح ٤] لصيقة مكرَّرة ⇒ SEM030 (لا خفض صامت لرقم واحد).
                    if (!definedLabelNames.insert(item.labelName).second)
                        errorCatalog(Errors::ErrorCode::SEM_ASM_DUPLICATE_LABEL,
                                     {{"label", item.labelName}});
                    advance(); // الاسم
                    advance(); // :
                    block->items.push_back(std::move(item));
                    continue;
                }

                // ── تعليمة: منمنمة [معامل {، معامل}] ──
                if (!check(TT::IDENTIFIER))
                {
                    errorCatalogExpected(Errors::ErrorCode::SYN_UNEXPECTED_TOKEN);
                    advance();
                    continue;
                }

                const std::string mnemonicName = current_.getValue();
                const Lexer::Position mnemonicPos = current_.getPosition();
                const Mnemonic *info = findMnemonic(mnemonicName.c_str());
                if (!info)
                {
                    errorCatalog(Errors::ErrorCode::SEM_ASM_UNKNOWN_MNEMONIC,
                                 {{"mnemonic", mnemonicName},
                                  {"suggestions", asmNearestMnemonics(mnemonicName)}});
                    // (AR) استرداد: تخطَّ حتى «نهاية» — لا معنى لمواصلة كتلة فاسدة
                    while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
                        advance();
                    break;
                }
                advance(); // المنمنمة

                AST::AsmItem item;
                item.isLabel = false;
                item.mnemonicAr = mnemonicName;
                item.mnemonicEn = info->en;
                item.operandClasses = info->operandClasses;
                item.readsDest = info->readsDest;
                item.pos = mnemonicPos;

                const std::size_t expected = std::strlen(info->operandClasses);
                for (std::size_t i = 0; i < expected; ++i)
                {
                    if (i > 0)
                    {
                        if (check(TT::ARABIC_COMMA) || check(TT::COMMA))
                        {
                            advance();
                        }
                        else
                        {
                            // (AR) معاملات أقلّ من المعجم
                            errorCatalog(Errors::ErrorCode::SEM_ASM_OPERAND_COUNT,
                                         {{"mnemonic", mnemonicName},
                                          {"expected", std::to_string(expected)},
                                          {"found", std::to_string(i)}});
                            break;
                        }
                    }
                    item.operands.push_back(parseAsmOperand(info->operandClasses[i]));
                }

                // (AR) معاملات أكثر من المعجم: فاصلة زائدة بعد المعامل الأخير
                if ((check(TT::ARABIC_COMMA) || check(TT::COMMA)))
                {
                    errorCatalog(Errors::ErrorCode::SEM_ASM_OPERAND_COUNT,
                                 {{"mnemonic", mnemonicName},
                                  {"expected", std::to_string(expected)},
                                  {"found", std::to_string(expected + 1)}});
                    // (AR) استرداد: تخطَّ الفواصل والمعاملات الزائدة
                    while (check(TT::ARABIC_COMMA) || check(TT::COMMA))
                    {
                        advance();
                        if (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
                            advance();
                    }
                }

                // (AR) [تصحيح ٥/٣] تتبُّع بنيويّ: متغيّرات ص المكتوبة (خرج) ومراجع القفز.
                //      «انقل» يكتب دون قراءة؛ بقيّة موضع 'w' inout — كلاهما خرجٌ يُخزَّن.
                // (EN) [Fix 5/3] track written sad vars (outputs) and jump refs.
                for (std::size_t i = 0; i < item.operands.size(); ++i)
                {
                    const char cls = (i < item.operandClasses.size()) ? item.operandClasses[i] : 'r';
                    const AST::AsmOperand &op = item.operands[i];
                    if (cls == 'w' && op.kind == AST::AsmOperand::Kind::SadVariable)
                        writtenVars.insert(op.text);
                    if (cls == 'l' && op.kind == AST::AsmOperand::Kind::LabelRef)
                        labelRefs.push_back(op.text);
                }
                // (AR) [تصحيح ٥] أكثر من متغيّر ص مكتوب ⇒ SEM028 (مرّة واحدة).
                if (writtenVars.size() > 1 && !multiOutputReported)
                {
                    errorCatalog(Errors::ErrorCode::SEM_ASM_MULTIPLE_OUTPUTS,
                                 {{"count", std::to_string(writtenVars.size())}});
                    multiOutputReported = true;
                }

                block->items.push_back(std::move(item));
            }

            if (!match(TT::KEYWORD_END))
            {
                errorCatalog(Errors::ErrorCode::SYN_UNCLOSED_CONSTRUCT,
                             {{"construct_ar", kBlockOpener},
                              {"construct_en", "assembly dialect block"},
                              {"closer", kw(TT::KEYWORD_END)}});
            }

            // (AR) [تصحيح ٣] كلّ هدف قفز/نداء يجب أن يكون لصيقةً معرَّفةً في الكتلة نفسها
            //      ⇒ SEM029 (النداء إلى رمز خارجيّ حقيقيّ خارج نطاق م١).
            // (EN) [Fix 3] every jump/call target must be a label defined in this block ⇒ SEM029.
            for (const auto &ref : labelRefs)
                if (definedLabelNames.find(ref) == definedLabelNames.end())
                    errorCatalog(Errors::ErrorCode::SEM_ASM_UNDEFINED_LABEL, {{"label", ref}});

            // (AR) عزل التوثيق: لا يتسرّب `##` من داخل الكتلة إلى التصريح التالي
            pendingDoc_ = DocBuffer{};
            nextDoc_ = DocBuffer{};

            return block;
        }

    } // namespace Parser
} // namespace Sad
