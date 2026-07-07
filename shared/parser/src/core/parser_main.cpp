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
                    if (!pendingDocComment_.empty())
                        pendingDocComment_ += '\n';
                    pendingDocComment_ += current_.getValue();
                }
                current_ = lexer_.nextToken();
            }

            // Fetch second token into nextToken_, also skipping whitespace/comments/doc-comments
            nextToken_ = lexer_.nextToken();
            while (nextToken_.getType() == TT::WHITESPACE ||
                   nextToken_.getType() == TT::COMMENT ||
                   nextToken_.getType() == TT::DOC_COMMENT ||
                   nextToken_.getType() == TT::NEWLINE)
            {
                // (AR) إصلاح BF-04 (السبب الجذري): التعليقات التي تظهر بين current_
                //      و nextToken_ تأتي بعد current_ فعلياً — يجب ألا تُرفق بأول
                //      تصريح. نخزّنها في nextDocComment_ ليتم ترحيلها لاحقاً.
                // (EN) BF-04 root-cause fix: doc comments appearing between
                //      current_ and nextToken_ are physically AFTER current_ —
                //      they must NOT attach to the first declaration. Buffer
                //      them in nextDocComment_ for later promotion.
                if (nextToken_.getType() == TT::DOC_COMMENT)
                {
                    if (!nextDocComment_.empty())
                        nextDocComment_ += '\n';
                    nextDocComment_ += nextToken_.getValue();
                }
                nextToken_ = lexer_.nextToken();
            }

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

                    errorBilingual(
                        "خطأ غير معروف أثناء التحليل النحوي",
                        "Unknown error during syntactic analysis");
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
                        errorBilingual(
                            "خطأ نحوي: توقعت اسم سمة داخل [[ ]].",
                            "Syntax error: expected attribute name inside [[ ]].");
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
                            errorBilingual(
                                "خطأ نحوي: توقعت ')' بعد معامل السمة.",
                                "Syntax error: expected ')' after attribute argument.");
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
                    errorBilingual(
                        "خطأ نحوي: توقعت ']' لإغلاق قائمة السمات.",
                        "Syntax error: expected ']' to close attribute list.");
                }
                if (!match(TT::BRACKET_RIGHT))
                {
                    errorBilingual(
                        "خطأ نحوي: توقعت ']' ثانية لإكمال [[...]].",
                        "Syntax error: expected second ']' to complete [[...]].");
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
            if (match(TT::KEYWORD_EXTERN))
            {
                if (!decorators.empty())
                {
                    error("(AR) المُزخرِفات لا تُستخدم مع الدوال الخارجية. (EN) Decorators cannot be used with extern functions.");
                }
                // (AR) تحليل اسم الربط الاختياري: خارجي("اسم_الربط") دالة ...
                // (EN) Parse optional link name: extern("link_name") function ...
                std::string ffiLinkName;
                if (check(TT::PAREN_LEFT))
                {
                    advance(); // (AR) استهلاك '(' / (EN) consume '('
                    Token linkNameToken = consume(TT::STRING_LITERAL,
                                                  "(AR) خطأ نحوي: توقع نص حرفي لاسم الربط بين الأقواس.\n"
                                                  "مثال: خارجي(\"c_function_name\") دالة ...\n"
                                                  "(EN) Syntax error: expected string literal for link name inside parentheses.\n"
                                                  "Example: extern(\"c_function_name\") function ...");
                    ffiLinkName = linkNameToken.getValue();
                    consume(TT::PAREN_RIGHT,
                            "(AR) خطأ نحوي: توقع ')' بعد اسم الربط.\n"
                            "(EN) Syntax error: expected ')' after link name.");
                }
                // (AR) [ISSUE-041] كتلة ربط أجنبيّ بلغة بلا أقواس: خارجي "C" <تصاريح> نهاية
                //      نُحوّلها لكتلة من تصاريح دوال خارجيّة (كلٌّ بلغة الربط نفسها). كان
                //      المُحلّل يتوقّع 'دالة' مباشرةً فيرفض صيغة الكتلة (gr.adv.ffi_extern_block).
                // (EN) [ISSUE-041] Foreign linkage block (string linkage, no parens):
                //      `extern "C" <decls> end`. Lower to a block of extern function decls
                //      (all sharing the linkage). The parser used to expect 'function'
                //      immediately and reject the block form (gr.adv.ffi_extern_block).
                else if (check(TT::STRING_LITERAL))
                {
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
                // (AR) توقع 'دالة' بعد 'خارجي' أو بعد 'خارجي("...")'
                // (EN) Expect 'function' after 'extern' or after 'extern("...")'
                if (!match(TT::KEYWORD_FUNCTION))
                {
                    error("(AR) خطأ نحوي: توقع 'دالة' بعد 'خارجي'. (EN) Syntax error: expected 'function' after 'extern'.");
                    return nullptr;
                }
                return parseExternFunctionDecl(ffiLinkName);
            }

            // (AR) 'غير_متزامن دالة' أُزيلت — استخدم 'دالة غير_متزامن' (الصفة بعد الاسم)
            // (EN) 'async function' removed — use 'function async' (Arabic adjective order)
            if (match(TT::KEYWORD_ASYNC) ||
                (checkContextual(TT::KEYWORD_ASYNC) && peekNext().getType() == TT::KEYWORD_FUNCTION))
            {
                errorBilingual(
                    "خطأ نحوي: 'غير_متزامن دالة' أُزيلت. استخدم 'دالة غير_متزامن' بدلاً منها.\n"
                    "💡 في العربية الصفة تأتي بعد الاسم: دالة غير_متزامن جلب()\n"
                    "💡 مثال:\n    دالة غير_متزامن جلب(رابط: نص)\n        ...\n    نهاية",
                    "Syntax error: 'async function' removed. Use 'function async' instead.\n"
                    "💡 In Arabic, adjectives follow nouns: دالة غير_متزامن name()\n"
                    "💡 Example:\n    دالة غير_متزامن fetch(url: نص)\n        ...\n    نهاية");
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
                    errorBilingual(
                        "خطأ نحوي: توقعت 'دالة' بعد 'مولد'.",
                        "Syntax error: Expected 'دالة' after 'مولد'.");
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
                    errorBilingual(
                        "خطأ نحوي: 'دالة مولد' أُزيلت. استخدم 'مولد دالة' بدلاً منها.\n"
                        "💡 مثال: مولد دالة عدّاد()\n    أنتج 1\n    أنتج 2\nنهاية",
                        "Syntax error: 'function generator' removed. Use 'generator function' instead.\n"
                        "💡 Example: مولد دالة counter()\n    yield 1\n    yield 2\nend");
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
                errorBilingual(
                    "خطأ نحوي: 'مجرد صنف' أُزيلت. في لغة ص، الصفة تأتي بعد الموصوف.\n"
                    "❌ خطأ: مجرد صنف اسم\n"
                    "✅ صحيح: صنف مجرد اسم\n"
                    "💡 صنف مجرد محكم اسم (لصنف مجرد ومحكم معاً)",
                    "Syntax error: 'مجرد صنف' removed. In Sad, adjectives follow nouns.\n"
                    "❌ Wrong: مجرد صنف Name\n"
                    "✅ Correct: صنف مجرد Name\n"
                    "💡 صنف مجرد محكم Name (for abstract + sealed)");
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
                errorBilingual(
                    "خطأ نحوي: 'محكم صنف' أُزيلت. في لغة ص، الصفة تأتي بعد الموصوف.\n"
                    "❌ خطأ: محكم صنف اسم\n"
                    "✅ صحيح: صنف محكم اسم\n"
                    "💡 صنف محكم مجرد اسم (لصنف محكم ومجرد معاً)",
                    "Syntax error: 'محكم صنف' removed. In Sad, adjectives follow nouns.\n"
                    "❌ Wrong: محكم صنف Name\n"
                    "✅ Correct: صنف محكم Name\n"
                    "💡 صنف محكم مجرد Name (for sealed + abstract)");
                advance(); // (AR) استهلاك 'محكم'
                bool isAlsoAbstract = false;
                if (match(TT::KEYWORD_ABSTRACT))
                {
                    isAlsoAbstract = true;
                }
                if (!match(TT::KEYWORD_CLASS))
                {
                    errorBilingual(
                        "توقع 'صنف' بعد 'محكم'.",
                        "Expected 'صنف' (class) after 'محكم' (sealed).");
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
                    errorBilingual(
                        "خطأ نحوي: توقعت اسم النوع المستعار بعد 'نوع'.",
                        "Syntax error: expected alias name after 'نوع'.");
                    return nullptr;
                }
                Token aliasName = current_;
                advance(); // (AR) استهلاك اسم المستعار
                if (!match(TT::OP_ASSIGN))
                {
                    errorBilingual(
                        "خطأ نحوي: توقعت '=' بعد اسم النوع المستعار.",
                        "Syntax error: expected '=' after type alias name.");
                    return nullptr;
                }
                auto target = parseExpression();
                if (!target)
                {
                    errorBilingual(
                        "خطأ نحوي: توقعت تعبيراً بعد '=' في تصريح الاسم المستعار للنوع.",
                        "Syntax error: expected expression after '=' in type alias declaration.");
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
                errorBilingual(
                    "(AR) خطأ نحوي: رمز غير متوقع '" + tokenVal + "' (" + tokenDesc + ") في بداية جملة أو تصريح.\n" +
                        "   تحقق من الكود - قد يكون هناك:\n" +
                        "   - فاصل منقوط أو فاصل غير ضروري\n" +
                        "   - قوس إغلاق بدون قوس فتح\n" +
                        "   - جملة ناقصة أو غير صحيحة\n" +
                        "   السطر: " + std::to_string(current_.getPosition().line),
                    "(EN) Syntax error: unexpected token '" + tokenVal + "' (" + tokenDesc + ") at statement start.\n" +
                        "   Check your code - there might be:\n" +
                        "   - unnecessary semicolon or separator\n" +
                        "   - closing bracket/brace without opening\n" +
                        "   - incomplete or malformed statement\n" +
                        "   Line: " + std::to_string(current_.getPosition().line));
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
                errorBilingual(
                    "خطأ نحوي: 'اعطِ' أُزيلت. استخدم 'أنتج' بدلاً منها.",
                    "Syntax error: 'اعطِ' removed. Use 'أنتج' instead.");
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
                    std::cout << "Failed to parse key, treating as block\n";
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
                        errorExpectedToken("تعبير قيمة", "value expression", "بعد ':' في list comprehension", "after ':' in list comprehension");
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
                    std::cout << "Regular map literal\n";
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
                            errorBilingual(
                                "خطأ: توقعت ':' أو '=' بعد مفتاح الخريطة.",
                                "Error: expected ':' or '=' after map key.");
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
                std::cout << "No colon found, treating as block with expression statement\n";

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
            bool isDirectiveKeyword = (nextType == TT::KEYWORD_UNSAFE ||
                                       nextType == TT::KEYWORD_COMPTIME ||
                                       nextType == TT::KEYWORD_VOLATILE ||
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

            // ─── @متطاير متغير/ثابت ... ───
            if (nextType == TT::KEYWORD_VOLATILE || name == "متطاير")
            {
                auto pos = current_.getPosition();
                advance(); // consume @
                advance(); // consume متطاير

                // (AR) التالي يجب أن يكون إعلان متغير (متغير أو ثابت)
                // (EN) Next must be a variable declaration (var or const)
                if (!check(TT::KEYWORD_VAR) && !check(TT::KEYWORD_CONST))
                {
                    error("(AR) خطأ نحوي: توقع 'متغير' أو 'ثابت' بعد @متطاير.\n"
                          "(EN) Syntax error: expected 'متغير' or 'ثابت' after @متطاير.");
                    return nullptr;
                }

                auto decl = parseDeclaration();
                return std::make_unique<VolatileVarDeclStmt>(std::move(decl), pos);
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

    } // namespace Parser
} // namespace Sad
