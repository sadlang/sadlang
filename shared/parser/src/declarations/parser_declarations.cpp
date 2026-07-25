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

#include "parser_core.h"
#include "advanced_expr_nodes.h"
#include "class_manager.h"
#include <iostream>
#include <sstream>

namespace Sad
{
    namespace Parser
    {

        using namespace AST;
        using namespace Lexer;
        using TT = TokenType;

        /**
         * @brief (AR) يحلل تصريح دالة مع مُزخرِفات اختيارية.
         *        (EN) Parses function declaration with optional decorators.
         *
         * @param decorators (AR) قائمة المُزخرِفات (إن وُجدت)
         *                   (EN) List of decorators (if any)
         *
         * Grammar / القواعد:
         *   function_decl → decorator* "function" IDENTIFIER "(" parameters ")" [ ":" type ] block
         *
         * @example Examples / أمثلة:
         * - function test() {}
         * - @staticmethod\nfunction test() {}
         * - @cache(100)\n@memoize\nfunction expensive() {}
         */
        StmtPtr ParserCore::parseFunctionDecl(ExprList decorators, bool is_async, bool is_generator)
        {
            // (AR) التقاط التعليق التوثيقي المعلق — يُرفق بالدالة بعد إنشاء العقدة
            // (EN) Capture pending doc comment — will be attached to function after node creation
            std::string docComment = consumePendingDocComment();

            // Spec: docs/language_spec/rules/02_functions.md - function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
            // Optional return type BEFORE function name: دالة [type] name(...)
            // (AR) نوع الإرجاع الاختياري قبل اسم الدالة: دالة [نوع] اسم(...)
            Types::SadTypeKind returnType = Types::SadTypeKind::Unknown;
            std::string returnTypeName; // (AR) [Phase 5e] لأنواع الأصناف المُعرَّفة من المستخدم

            // (AR) راية للإشارة إذا كانت هذه هي الدالة الرئيسية
            // (EN) Flag to indicate if this is the main function
            bool isMain = false;
            Token name(TT::IDENTIFIER, "", Lexer::Position()); // (AR) تعريف name مسبقاً / (EN) Define name upfront

            // ─────────────────────────────────────────────────────────────────────
            // (AR) اللبنة 3.15: مُعدِّل «لا_ترجع» يلي «دالة» قبل الاسم (نمط «متغير متطاير»):
            //      دالة لا_ترجع اسم(...) — يُعلن أنّ الدالّة لا تعود أبدًا ⇒ سمة LLVM NoReturn.
            //      «لا_ترجع» كلمة سياقيّة (CONTEXTUAL) فقد تُلفظ IDENTIFIER — نقبل الحالتين. لا سيجيل @.
            // (EN) Brick 3.15: 'لا_ترجع' modifier after 'دالة' before the name (mirrors 'متغير متطاير'):
            //      function noreturn name(...) — declares the function never returns ⇒ LLVM NoReturn.
            // ─────────────────────────────────────────────────────────────────────
            bool isNoReturn = false;
            if (check(TT::KEYWORD_NORETURN) ||
                (check(TT::IDENTIFIER) && current_.getValue() == "لا_ترجع"))
            {
                advance();
                isNoReturn = true;
            }

            // ─────────────────────────────────────────────────────────────────────
            // (AR) RFC 0034: 'دالة خارجية' — الصيغة المفردة الوحيدة لتصريح الربط الخارجيّ:
            //      دالة خارجية [ ("اسم_الربط") ] [نوع] اسم(معاملات)
            // (EN) RFC 0034: 'function extern' — the only single-decl extern form:
            //      function extern [ ("link_name") ] [type] name(params)
            // ─────────────────────────────────────────────────────────────────────
            if (check(TT::KEYWORD_EXTERN))
            {
                Token externTok = current_;
                advance(); // (AR) استهلاك 'خارجية'/'خارجي' / (EN) consume the extern lexeme

                // (AR) تمييز: إن تلاها '(' وليس بعده نصّ حرفيّ فهي دالة *اسمها* خارجي/خارجية
                //      (استعمال الكلمة الناعمة اسمًا — مشروع قبل RFC 0034 ويبقى) —
                //      نُثبّت الاسم ونُكمل المسار العاديّ (سلسلة اختيار الاسم أدناه تتخطّى
                //      الاسم المُثبَّت مسبقًا).
                // (EN) Disambiguation: if followed by '(' whose next token is not a string
                //      literal, this is a function *named* extern (soft-keyword-as-name,
                //      legal before RFC 0034 and still legal) — pin the name and continue
                //      down the regular path (the name-selection chain below skips a
                //      pre-pinned name).
                if (check(TT::PAREN_LEFT) && peekNext().getType() != TT::STRING_LITERAL)
                {
                    name = Token(TT::IDENTIFIER, externTok.getValue(), externTok.getPosition());
                }
                else
                {
                    // (AR) الصفة تطابق الموصوف جنسًا: 'دالة' مؤنّثة ⇒ 'خارجية' لا 'خارجي'.
                    //      اللفظة الأصليّة محفوظة في قيمة الرمز، والمذكّر هو primaryWord في
                    //      keywords.yaml — لا سلاسل خام.
                    // (EN) Adjective agrees in gender: 'دالة' is feminine ⇒ 'خارجية' not
                    //      'خارجي'. The original lexeme survives in the token value; the
                    //      masculine form is the catalog's primaryWord — no raw literals.
                    const auto *externEntry = Lexer::KeywordTable::getEntry(TT::KEYWORD_EXTERN);
                    if (externEntry && externTok.getValue() == externEntry->primaryWord)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_ADJECTIVE_GENDER, {{"wrong", kw(TT::KEYWORD_FUNCTION) + " " + kw(TT::KEYWORD_EXTERN)}, {"right", kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN)}, {"note_ar", "'" + kw(TT::KEYWORD_FUNCTION) + "' مؤنّثة فتلزمها الصفة المؤنّثة '" + kwAlias(TT::KEYWORD_EXTERN) + "'. مثال: " + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " printf(" + kw(TT::TYPE_STRING) + ")"}, {"note_en", "'" + kw(TT::KEYWORD_FUNCTION) + "' is feminine, so it takes the feminine adjective '" + kwAlias(TT::KEYWORD_EXTERN) + "'. Example: " + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " printf(" + kw(TT::TYPE_STRING) + ")"}});
                    }

                    if (is_async || is_generator)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_INVALID_CONSTRUCT_FORM, {{"construct_ar", "تصريح الدالة الخارجية (لا يُجمع مع '" + kw(TT::KEYWORD_ASYNC) + "' أو '" + kw(TT::KEYWORD_GENERATOR) + "' — التصريح الخارجيّ بلا جسم)"}, {"construct_en", "extern function declaration (cannot combine with '" + kw(TT::KEYWORD_ASYNC) + "' or '" + kw(TT::KEYWORD_GENERATOR) + "' — extern declarations have no body)"}, {"form", kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " printf(" + kw(TT::TYPE_STRING) + ")"}});
                    }
                    if (!decorators.empty())
                    {
                        error("(AR) المُزخرِفات لا تُستخدم مع الدوال الخارجية. (EN) Decorators cannot be used with extern functions.");
                    }

                    // (AR) اسم الربط الاختياريّ: دالة خارجية("رمز") ...
                    // (EN) Optional link name: function extern("sym") ...
                    std::string ffiLinkName;
                    if (check(TT::PAREN_LEFT))
                    {
                        advance(); // (AR) استهلاك '(' / (EN) consume '('
                        Token linkNameToken = consume(TT::STRING_LITERAL,
                                                      "(AR) خطأ نحوي: توقع نص حرفي لاسم الربط بين الأقواس.\n"
                                                      "مثال: دالة خارجية(\"c_function_name\") ...\n"
                                                      "(EN) Syntax error: expected string literal for link name inside parentheses.\n"
                                                      "Example: function extern(\"c_function_name\") ...");
                        ffiLinkName = linkNameToken.getValue();
                        // (AR) نصّ فارغ ("") يُكافئ غياب اسم الربط فيربط بالاسم العربيّ صامتًا
                        //      — التباس خطر، نرفضه صراحةً.
                        // (EN) An empty string ("") silently degrades to name-based linking —
                        //      a dangerous ambiguity; reject it explicitly.
                        if (linkNameToken.getType() == TT::STRING_LITERAL && ffiLinkName.empty())
                        {
                            errorCatalog(Errors::ErrorCode::SYN_INVALID_CONSTRUCT_FORM, {{"construct_ar", "اسم الربط الخارجيّ (لا يكون نصًّا فارغًا \"\" — احذف القوسين أو ضع رمزًا فعليًّا)"}, {"construct_en", "extern link name (cannot be an empty string \"\" — drop the parentheses or provide a real symbol)"}, {"form", kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + "(\"cos\") " + kw(TT::TYPE_DOUBLE) + " جيب_التمام(" + kw(TT::TYPE_DOUBLE) + ")"}});
                        }
                        consume(TT::PAREN_RIGHT,
                                "(AR) خطأ نحوي: توقع ')' بعد اسم الربط.\n"
                                "(EN) Syntax error: expected ')' after link name.");
                    }

                    auto externDecl = parseExternFunctionDecl(ffiLinkName);
                    // (AR) التعليق التوثيقيّ التُقط أعلاه قبل تفويض التحليل — أعِد إرفاقه.
                    // (EN) The doc comment was captured above before delegating — reattach it.
                    if (auto *fd = dynamic_cast<AST::FunctionDecl *>(externDecl.get()))
                    {
                        if (fd->docComment.empty())
                            fd->docComment = std::move(docComment);
                        fd->isNoReturn = isNoReturn; // (AR) دالة لا_ترجع خارجية (مثل abort)
                    }
                    return externDecl;
                }
            }

            // Check if next token is a type keyword or built-in type identifier (before function name)
            // (AR) التحقق إذا كان الرمز التالي هو نوع (قبل اسم الدالة)
            // BUT only if it's NOT followed by '(' — otherwise the type keyword IS the function name
            // (AR) النوع الداخليّ لنوع إرجاع اختياريّ T؟ (NS-06)
            // (EN) Inner type of an optional return type T? (NS-06)
            Types::SadTypeKind returnInner = Types::SadTypeKind::Unknown;
            if (name.getValue().empty() &&
                isTypeToken(current_.getType()) &&
                nextToken_.getType() != TT::PAREN_LEFT)
            {
                returnType = parseType();
                if (returnType == Types::SadTypeKind::Optional)
                    returnInner = lastOptionalInner_;
            }

            // ─────────────────────────────────────────────────────────────────────
            // (AR) دعم 'غير_متزامنة' / 'غير_متزامن' بعد 'دالة': دالة غير_متزامنة اسم()
            // (EN) Support 'async' after 'function': function async name()
            // ─────────────────────────────────────────────────────────────────────
            // (AR) كلمة سياقية: تحقق مزدوج — KEYWORD_ASYNC أو مُعرّف
            // (EN) Contextual keyword: dual check
            if (check(TT::KEYWORD_ASYNC) || checkContextual(TT::KEYWORD_ASYNC))
            {
                is_async = true;
                advance(); // consume 'غير_متزامنة' / 'غير_متزامن'
            }

            // (AR) [Phase 5e] نوع إرجاع من صنف مُعرَّف من المستخدم: "دالة نقطة احصل_نقطة()"
            // (EN) [Phase 5e] User-defined class return type: "function نقطة getPoint()"
            // شرط: returnType لا يزال UNKNOWN (لم يُحدَّد بعد) + رمزان متتاليان من نوع IDENTIFIER
            if (returnType == Types::SadTypeKind::Unknown &&
                check(TT::IDENTIFIER) &&
                peekNext().getType() == TT::IDENTIFIER)
            {
                returnTypeName = current_.getValue();
                returnType = Types::SadTypeKind::Class;
                advance(); // (AR) استهلاك اسم الصنف / (EN) consume class name
            }

            // (AR) التحقق إذا كانت الدالة الرئيسية (كلمة سياقية — لم تعد محجوزة)
            // (EN) Check if this is the main function (contextual — no longer reserved)
            if (!name.getValue().empty())
            {
                // (AR) الاسم مُثبَّت مسبقًا (دالة اسمها خارجي/خارجية — تمييز RFC 0034 أعلاه)
                // (EN) Name pre-pinned (function named extern — RFC 0034 disambiguation above)
            }
            else if (check(TT::KEYWORD_MAIN) || checkContextual(TT::KEYWORD_MAIN))
            {
                // (AR) هذه هي الدالة الرئيسية - استخدام رمز KEYWORD_MAIN
                // (EN) This is the main function - consume KEYWORD_MAIN token
                Token mainToken = current_; // (AR) حفظ الرمز قبل advance / (EN) Save token before advance
                advance();                  // (AR) استهلاك الرمز / (EN) Consume token
                isMain = true;

                // (AR) إذا لم يتم تحديد نوع الإرجاع، استخدام صحيح (int) كإعداد افتراضي للدالة الرئيسية
                // (EN) If return type not specified, use INTEGER as default for main function
                if (returnType == Types::SadTypeKind::Unknown)
                {
                    returnType = Types::SadTypeKind::Integer;
                }

                // (AR) استخدام "رئيسية" كاسم للدالة
                // (EN) Use "main" as the function name
                name = Token(TT::IDENTIFIER, "رئيسية", mainToken.getPosition());
            }
            else if (isTypeToken(current_.getType()))
            {
                // Type keyword used as function name (e.g., دالة نص(props))
                // (AR) كلمة نوع مستخدمة كاسم دالة (مثلاً: دالة نص(props))
                auto tok = current_;
                advance();
                name = Token(TT::IDENTIFIER, tok.getValue(), tok.getPosition());
            }
            else if (isTokenUsableAsName(current_.getType()) ||
                     current_.getType() == TT::LITERAL_FALSE || current_.getType() == TT::LITERAL_TRUE ||
                     current_.getType() == TT::LITERAL_NULL)
            {
                // (AR) كلمة مفتاحية ناعمة أو حرفية كاسم دالة (مثلاً: دالة خطأ(...)، دالة صحيح(...))
                // (EN) Soft keyword or literal as function name (e.g., function خطأ(...), function صحيح(...))
                auto tok = current_;
                advance();
                name = Token(TT::IDENTIFIER, tok.getValue(), tok.getPosition());
            }
            else
            {
                // Expect function name (for regular functions)
                // (AR) توقع اسم الدالة (للدوال العادية)
                name = consume(TT::IDENTIFIER,
                               "(AR) خطأ نحوي: بعد كلمة 'دالة' (أو بعد نوع الإرجاع) يجب أن يأتي اسم الدالة.\n"
                               "مثال: دالة جمع(...) أو دالة رقم مربع(...)\n"
                               "(EN) Syntax error: After 'function' keyword (or return type) expected function name.\n"
                               "Example: function sum(...) or function int square(...)");
            }

            // (AR) التحقق من نمط وجود مسافة في اسم الدالة: دالة عدد طلاب()
            // (EN) Check for space-in-function-name pattern: function word1 word2()
            if (!isMain && check(TT::IDENTIFIER) && name.getPosition().line == current_.getPosition().line)
            {
                // (AR) التحقق إذا كان النمط: دالة كلمة1 كلمة2(...)
                if (peekNext().getType() == TT::PAREN_LEFT)
                {
                    std::string suggestedName = name.getValue() + "_" + current_.getValue();
                    errorCatalog(Errors::ErrorCode::SYN_NAME_HAS_SPACE, {{"what_ar", "الدالة"}, {"what_en", "function"}, {"name", name.getValue() + " " + current_.getValue()}, {"suggested", kw(TT::KEYWORD_FUNCTION) + " " + suggestedName + "(...)"}});
                    return nullptr;
                }
                // (AR) نمط: دالة كلمة1 كلمة2 كلمة3... → اسم متعدد الكلمات
                if (peekNext().getType() == TT::IDENTIFIER && peekNext().getPosition().line == name.getPosition().line)
                {
                    errorCatalog(Errors::ErrorCode::SYN_NAME_HAS_SPACE, {{"what_ar", "الدالة"}, {"what_en", "function"}, {"name", name.getValue() + " ..."}, {"suggested", name.getValue() + "_..."}});
                    return nullptr;
                }
            }

            // ─────────────────────────────────────────────────────────────────────
            // (AR) تحليل معاملات العمر <'أ, 'ب> بعد اسم الدالة (إن وجدت)
            // (EN) Parse lifetime parameters <'أ, 'ب> after function name (if present)
            // ─────────────────────────────────────────────────────────────────────
            std::vector<std::string> lifetimeParams = parseLifetimeParams();

            // Parse parameter list (now with type annotations)
            // (AR) تحليل قائمة المعاملات (الآن مع تصريحات الأنواع)
            consume(TT::PAREN_LEFT,
                    "(AR) خطأ نحوي: بعد اسم الدالة يجب أن يأتي قوس مفتوح '('.\n"
                    "مثال: دالة " +
                        name.getValue() + "(...) \n"
                                          "(EN) Syntax error: After function name expected '('.\n"
                                          "Example: function " +
                        name.getValue() + "(...)");
            auto paramObjs = parseTypedParameterList();
            consume(TT::PAREN_RIGHT,
                    "(AR) خطأ نحوي: بعد قائمة المعاملات يجب أن يأتي قوس مغلق ')'.\n"
                    "مثال: دالة " +
                        name.getValue() + "(معامل١، معامل٢) \n"
                                          "تأكد من أن قائمة المعاملات مكتملة ومفصولة بفواصل صحيحة.\n"
                                          "(EN) Syntax error: After parameter list expected ')'.\n"
                                          "Example: function " +
                        name.getValue() + "(param1, param2)\n"
                                          "Make sure parameter list is complete and properly separated.");

            // (AR) نوع الإرجاع يُحدد فقط قبل اسم الدالة: دالة رقم جمع()
            // (EN) Return type is specified only before function name: function int sum()
            // (AR) الصيغ المُزالة: `:` و `->` و `ترجع` بعد المعاملات
            // (EN) Removed forms: `:`, `->`, `ترجع` after parameters
            if (check(TT::COLON) || check(TT::ARROW) || check(TT::KEYWORD_RETURNS))
            {
                errorCatalog(Errors::ErrorCode::SYN_RETURN_TYPE_AFTER_PARAMS, {{"right_form", kw(TT::KEYWORD_FUNCTION) + " " + kw(TT::TYPE_INTEGER) + " " + name.getValue() + "(...)"}});
            }

            // (AR) التحقق من صحة توقيع الدالة الرئيسية
            // (EN) Validate main function signature
            if (isMain)
            {
                // (AR) التحقق من نوع الإرجاع - يجب أن يكون صحيح (int)
                // (EN) Check return type - must be INTEGER
                if (returnType != Types::SadTypeKind::Integer && returnType != Types::SadTypeKind::Unknown)
                {
                    std::cerr << "\n";
                    std::cerr << "⚠️ ========================================\n";
                    std::cerr << "  ⛔ (AR) تحذير: نوع إرجاع الدالة الرئيسية يجب أن يكون 'صحيح'\n";
                    std::cerr << "  ⛔ (EN) Warning: Main function return type must be 'int'\n";
                    std::cerr << "========================================\n";
                    std::cerr << "📍 (AR) السطر: " << name.getPosition().line << "\n";
                    std::cerr << "📍 (EN) Line: " << name.getPosition().line << "\n";
                    std::cerr << "💬 (AR) نوع الإرجاع الحالي غير صحيح\n";
                    std::cerr << "💬 (EN) Current return type is invalid\n";
                    std::cerr << "✓ (AR) التوقيع الصحيح: دالة صحيح رئيسية() أو دالة صحيح رئيسية(نص[] الوسائط)\n";
                    std::cerr << "✓ (EN) Correct signature: function int main() or function int main(string[] args)\n";
                    std::cerr << "========================================\n\n";
                }

                // (AR) التحقق من المعاملات - يجب أن تكون () أو (نص[] الوسائط)
                // (EN) Check parameters - must be () or (string[] args)
                if (paramObjs.size() > 1)
                {
                    std::cerr << "\n";
                    std::cerr << "⚠️ ========================================\n";
                    std::cerr << "  ⛔ (AR) خطأ: الدالة الرئيسية تقبل معامل واحد فقط أو لا شيء\n";
                    std::cerr << "  ⛔ (EN) Error: Main function accepts zero or one parameter only\n";
                    std::cerr << "========================================\n";
                    std::cerr << "📍 (AR) السطر: " << name.getPosition().line << "\n";
                    std::cerr << "📍 (EN) Line: " << name.getPosition().line << "\n";
                    std::cerr << "💬 (AR) عدد المعاملات الحالي: " << paramObjs.size() << "\n";
                    std::cerr << "💬 (EN) Current parameter count: " << paramObjs.size() << "\n";
                    std::cerr << "✓ (AR) التوقيع الصحيح: دالة صحيح رئيسية() أو دالة صحيح رئيسية(نص[] الوسائط)\n";
                    std::cerr << "✓ (EN) Correct signature: function int main() or function int main(string[] args)\n";
                    std::cerr << "========================================\n\n";

                    error("(AR) الدالة الرئيسية تقبل معامل واحد فقط (نص[] الوسائط) أو لا شيء. "
                          "(EN) Main function accepts zero or one parameter (string[] args) only.");
                }
                else if (paramObjs.size() == 1)
                {
                    // (AR) التحقق من أن المعامل من نوع نص[] (string array)
                    // (EN) Check that parameter is of type string[] (string array)
                    if (paramObjs[0].type != Types::SadTypeKind::Array)
                    {
                        std::cerr << "\n";
                        std::cerr << "⚠️ ========================================\n";
                        std::cerr << "  ⛔ (AR) تحذير: معامل الدالة الرئيسية يجب أن يكون من نوع نص[]\n";
                        std::cerr << "  ⛔ (EN) Warning: Main function parameter must be of type string[]\n";
                        std::cerr << "========================================\n";
                        std::cerr << "📍 (AR) السطر: " << name.getPosition().line << "\n";
                        std::cerr << "📍 (EN) Line: " << name.getPosition().line << "\n";
                        std::cerr << "💬 (AR) نوع المعامل الحالي غير صحيح\n";
                        std::cerr << "💬 (EN) Current parameter type is invalid\n";
                        std::cerr << "✓ (AR) التوقيع الصحيح: دالة صحيح رئيسية(نص[] الوسائط)\n";
                        std::cerr << "✓ (EN) Correct signature: function int main(string[] args)\n";
                        std::cerr << "========================================\n\n";
                    }
                }
            }

            // Parse function body - starts directly, ends with 'نهاية'
            // (AR) تحليل جسم الدالة - يبدأ مباشرة، ينتهي بـ 'نهاية'
            // Spec: docs/language_spec/rules/02_functions.md - function body ends with 'نهاية'

            // ─────────────────────────────────────────────────────────────────────
            // (AR) تحليل العقود البرمجية: يتطلب (شرط) / يضمن (شرط)
            // (EN) Parse Design by Contract: requires (condition) / ensures (condition)
            // (AR) هذه كلمات سياقية — تُحلل فقط قبل جسم الدالة مباشرة
            // ─────────────────────────────────────────────────────────────────────
            ExprList preconditions;
            ExprList postconditions;
            while (true)
            {
                bool isRequires = check(TT::KEYWORD_REQUIRES) ||
                                  checkContextual(TT::KEYWORD_REQUIRES); // يتطلب
                bool isEnsures = !isRequires && (check(TT::KEYWORD_ENSURES) ||
                                                 checkContextual(TT::KEYWORD_ENSURES)); // يضمن
                if (!isRequires && !isEnsures)
                    break;
                advance(); // (AR) استهلاك يتطلب/يضمن
                auto condExpr = parseExpression();
                if (condExpr)
                {
                    if (isRequires)
                    {
                        preconditions.push_back(std::move(condExpr));
                    }
                    else
                    {
                        postconditions.push_back(std::move(condExpr));
                    }
                }
                matchSemicolon(); // (AR) تخطي المنقوطة الاختيارية
            }

            // ─────────────────────────────────────────────────────────────────────
            // (AR) تحليل قيود الأنواع: حيث ت: سمة1 + سمة2، ك: سمة3
            // (EN) Parse type constraints: where T: Trait1 + Trait2, U: Trait3
            // (AR) هذه كلمة سياقية — تُحلل فقط قبل جسم الدالة
            // ─────────────────────────────────────────────────────────────────────
            std::vector<WhereConstraint> whereConstraints;
            if (check(TT::KEYWORD_WHERE) ||
                checkContextual(TT::KEYWORD_WHERE))
            {              // حيث
                advance(); // (AR) استهلاك حيث
                while (true)
                {
                    // (AR) قراءة اسم المعامل (ت)
                    if (!check(TT::IDENTIFIER))
                        break;
                    WhereConstraint constraint;
                    constraint.paramName = current_.getValue();
                    advance();
                    // (AR) توقع ':' بعد اسم المعامل
                    if (!match(TT::COLON))
                        break;
                    // (AR) قراءة أسماء السمات مع '+' للفصل
                    while (true)
                    {
                        if (!check(TT::IDENTIFIER))
                            break;
                        constraint.traits.push_back(current_.getValue());
                        advance();
                        // (AR) '+' يفصل بين السمات المتعددة
                        if (check(TT::OP_PLUS))
                        {
                            advance();
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (!constraint.traits.empty())
                    {
                        whereConstraints.push_back(std::move(constraint));
                    }
                    // (AR) فاصلة للفصل بين قيود متعددة
                    if (!matchComma())
                        break;
                }
            }

            auto body = parseBlockStmt();

            // (AR) التحقق من توافق نوع الإرجاع
            // (EN) Check return type compatibility
            // If function has UNKNOWN return type (no type specified), it should not have return statements with values
            if (returnType == Types::SadTypeKind::Unknown)
            {
                // Check if body contains return statements with values
                // This is a simplified check - proper semantic analysis would be better
                // For now, we just warn at parse time
                // Note: This check will be done at runtime by the interpreter
            }

            // (AR) إنشاء عقدة تصريح الدالة مع المُزخرِفات
            // (EN) Create function declaration node with decorators
            if (!decorators.empty())
            {
                auto funcDecl = std::make_unique<FunctionDecl>(
                    name.getValue(),
                    std::move(paramObjs),
                    returnType,
                    std::move(body),
                    std::move(decorators),
                    false,        // (AR) مُصدَّرة / (EN) exported
                    is_async,     // (AR) غير متزامنة / (EN) async
                    is_generator, // (AR) مولد / (EN) generator
                    name.getPosition());
                // (AR) تعيين راية الدالة الرئيسية
                // (EN) Set main function flag
                funcDecl->isMainFunction = isMain;
                funcDecl->isNoReturn = isNoReturn; // (AR) اللبنة 3.15: دالة لا_ترجع
                funcDecl->lifetimeParams = std::move(lifetimeParams);
                funcDecl->preconditions = std::move(preconditions);
                funcDecl->postconditions = std::move(postconditions);
                funcDecl->whereConstraints = std::move(whereConstraints);
                funcDecl->docComment = std::move(docComment);
                funcDecl->returnTypeName = returnTypeName; // (AR) [Phase 5e] حفظ اسم صنف الإرجاع
                // (AR) نوع إرجاع اختياريّ T؟: احفظ النوع الموحَّد بالنوع الداخليّ (NS-06)
                // (EN) Optional return type T?: store unified type with inner type (NS-06)
                if (returnType == Types::SadTypeKind::Optional)
                    funcDecl->sadReturnType = Types::SadTypeRegistry::instance().makeOptional(
                        Types::SadType::fromValueType(returnInner));
                return funcDecl;
            }

            // (AR) إنشاء عقدة تصريح الدالة بدون مُزخرِفات
            // (EN) Create function declaration node without decorators
            auto funcDecl = std::make_unique<FunctionDecl>(
                name.getValue(),
                std::move(paramObjs),
                returnType,
                std::move(body),
                false,        // (AR) مُصدَّرة / (EN) exported
                is_async,     // (AR) غير متزامنة / (EN) async
                is_generator, // (AR) مولد / (EN) generator
                name.getPosition());
            // (AR) تعيين راية الدالة الرئيسية
            // (EN) Set main function flag
            funcDecl->isMainFunction = isMain;
            funcDecl->isNoReturn = isNoReturn; // (AR) اللبنة 3.15: دالة لا_ترجع
            funcDecl->lifetimeParams = std::move(lifetimeParams);
            funcDecl->preconditions = std::move(preconditions);
            funcDecl->postconditions = std::move(postconditions);
            funcDecl->whereConstraints = std::move(whereConstraints);
            funcDecl->docComment = std::move(docComment);
            funcDecl->returnTypeName = returnTypeName; // (AR) [Phase 5e] حفظ اسم صنف الإرجاع
            // (AR) نوع إرجاع اختياريّ T؟: احفظ النوع الموحَّد بالنوع الداخليّ (NS-06)
            // (EN) Optional return type T?: store unified type with inner type (NS-06)
            if (returnType == Types::SadTypeKind::Optional)
                funcDecl->sadReturnType = Types::SadTypeRegistry::instance().makeOptional(
                    Types::SadType::fromValueType(returnInner));
            return funcDecl;
        }

        /**
         * @brief (AR) يحلل تصريح دالة خارجية (دالة خارجية) — بعد استهلاك الفاتحة
         *        (EN) Parses external function declaration (function extern) — opener consumed
         *
         * الصيغة / Syntax (RFC 0034):
         *   دالة خارجية [("اسم_الربط")] [نوع] اسم_الدالة(معاملات)
         *   وداخل كتلة الربط: دالة [نوع] اسم_الدالة(معاملات)
         *
         * @return (AR) مؤشر على عقدة تصريح الدالة الخارجية
         *         (EN) Pointer to external function declaration node
         */
        StmtPtr ParserCore::parseExternFunctionDecl(const std::string &linkName)
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            // (AR) نوع الإرجاع الاختياري قبل اسم الدالة
            // (EN) Optional return type before function name
            Types::SadTypeKind returnType = Types::SadTypeKind::Unknown;
            std::string returnTypeName; // (AR) لنوع إرجاع من صنف مُعرَّف (بنية @تمثيل_سي بالقيمة) [RFC #53 F2-ج]

            // Check if next token is a type keyword or built-in type identifier (before function name)
            if (isTypeToken(current_.getType()) &&
                nextToken_.getType() != TT::PAREN_LEFT)
            {
                returnType = parseType();
            }

            // (AR) [RFC #53 F2-ج] نوع إرجاع من صنف مُعرَّف من المستخدم في تصريح خارجيّ:
            //      «دالة خارجية("make_point") نقطة اصنع_نقطة()» — يماثل الدالة العاديّة.
            // (EN) [RFC #53 F2-ج] User-defined class return type in an extern decl —
            //      mirrors the regular function parser's two-identifier heuristic.
            // شرط: returnType لا يزال UNKNOWN + رمزان متتاليان من نوع IDENTIFIER
            if (returnType == Types::SadTypeKind::Unknown &&
                check(TT::IDENTIFIER) &&
                nextToken_.getType() == TT::IDENTIFIER)
            {
                returnTypeName = current_.getValue();
                returnType = Types::SadTypeKind::Class;
                advance(); // (AR) استهلاك اسم الصنف / (EN) consume class name
            }

            // (AR) توقع اسم الدالة
            // (EN) Expect function name
            Token name = consume(TT::IDENTIFIER,
                                 "(AR) خطأ نحوي: بعد 'دالة خارجية' (أو بعد نوع الإرجاع) يجب أن يأتي اسم الدالة.\n"
                                 "مثال: دالة خارجية رقم clear_screen()\n"
                                 "(EN) Syntax error: After 'function extern' (or return type) expected function name.\n"
                                 "Example: دالة خارجية رقم clear_screen()");

            // (AR) تحليل قائمة المعاملات
            // (EN) Parse parameter list
            consume(TT::PAREN_LEFT,
                    "(AR) خطأ نحوي: بعد اسم الدالة الخارجية يجب أن يأتي قوس مفتوح '('.\n"
                    "(EN) Syntax error: After extern function name expected '('.");
            auto paramObjs = parseTypedParameterList();
            consume(TT::PAREN_RIGHT,
                    "(AR) خطأ نحوي: بعد قائمة المعاملات يجب أن يأتي قوس مغلق ')'.\n"
                    "(EN) Syntax error: After parameter list expected ')'.");

            // (AR) نوع الإرجاع يُحدد فقط قبل اسم الدالة الخارجية: دالة خارجية رقم clear_screen()
            // (EN) Return type specified only before extern function name
            if (check(TT::COLON) || check(TT::ARROW) || check(TT::KEYWORD_RETURNS))
            {
                // (AR) RFC 0034: الصيغة المعتمدة «دالة خارجية [نوع] اسم(...)» —
                //      الصفة المؤنّثة هي البديل الأوّل في معجم SoT (لا سلاسل خام).
                // (EN) RFC 0034: canonical form is 'function extern [type] name(...)' —
                //      the feminine adjective is the SoT lexicon's first alias.
                errorCatalog(Errors::ErrorCode::SYN_RETURN_TYPE_AFTER_PARAMS, {{"right_form", kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " " + kw(TT::TYPE_INTEGER) + " " + name.getValue() + "(...)"}});
            }

            // (AR) إنشاء عقدة تصريح الدالة الخارجية (بدون جسم)
            // (EN) Create external function declaration node (no body)
            auto funcDecl = std::make_unique<FunctionDecl>(
                name.getValue(),
                std::move(paramObjs),
                returnType,
                nullptr, // (AR) لا جسم للدالة الخارجية / (EN) No body for extern function
                false,   // (AR) مُصدَّرة / (EN) exported
                false,   // (AR) غير متزامنة / (EN) async
                false,   // (AR) مولد / (EN) generator
                name.getPosition());
            funcDecl->isExtern = true;              // (AR) علامة الدالة الخارجية / (EN) Mark as external
            funcDecl->returnTypeName = returnTypeName; // (AR) اسم صنف الإرجاع (بنية @تمثيل_سي بالقيمة) [RFC #53 F2-ج]
            funcDecl->linkName = linkName;          // (AR) اسم الربط الخارجي (FFI) / (EN) FFI link name
            funcDecl->docComment = std::move(docComment);
            return funcDecl;
        }

        /**
         * @brief (AR) يحلل تصريح صنف كامل مع جميع الأعضاء
         *        (EN) Parses complete class declaration with all members
         *
         * الصيغة / Syntax:
         *   صنف اسم_الصنف [يرث اسم_الأساس] {
         *       أعضاء...
         *   }
         */
        StmtPtr ParserCore::parseClassDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

#ifdef DEBUG_OOP
            std::cout << "[OOP] بدء تحليل تصريح صنف\n";
#endif

            // (AR) اسم الصنف / (EN) Class name
            // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ::= 'صنف' IDENTIFIER ...
            // (AR) السماح بأسماء الأنواع المدمجة (نص، رقم، إلخ) كأسماء أصناف
            // (EN) Allow built-in type names (نص, رقم, etc.) as class names
            Token nameToken = current_;
            if (check(TT::IDENTIFIER))
            {
                advance();
            }
            else if (isTypeToken(current_.getType()))
            {
                advance();
            }
            else
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "الصنف"}, {"what_en", "class"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_CLASS) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_CLASS) + "'"}});
                return nullptr;
            }
            std::string className = nameToken.getValue();

            // (AR) التحقق من نمط وجود مسافة في اسم الصنف: صنف اسم صنف يرث ...
            // (EN) Check for space-in-class-name pattern: class word1 word2 ...
            // (AR) استثناء الكلمات السياقية: نفّذ/نفذ (implements)
            // (EN) Exclude contextual keywords: نفّذ/نفذ (implements)
            if (check(TT::IDENTIFIER) && nameToken.getPosition().line == current_.getPosition().line)
            {
                const std::string &nextVal = current_.getValue();
                // (AR) نفّذ/نفذ ليست جزءاً من اسم الصنف — بل كلمة تنفيذ السمة
                // (EN) نفّذ/نفذ is not part of class name — it's the trait implementation keyword
                if (nextVal != "نفّذ" && nextVal != "نفذ")
                {
                    // (AR) التحقق إذا كان التالي ليس 'يرث' — أي أنه جزء من اسم متعدد الكلمات
                    //      بعد قاعدة الصفة بعد الموصوف: الأعضاء تبدأ بـ دالة/باني/خاصية/متغير/ثابت
                    //      وليس بـ عام/خاص/محمي (لأن المعدلات تأتي بعد الكلمة المفتاحية الآن)
                    Token nextId = current_;
                    if (peekNext().getType() == TT::KEYWORD_INHERITS ||
                        peekNext().getType() == TT::KEYWORD_END || peekNext().getType() == TT::IDENTIFIER ||
                        peekNext().getType() == TT::KEYWORD_FUNCTION || peekNext().getType() == TT::KEYWORD_CONSTRUCTOR ||
                        peekNext().getType() == TT::KEYWORD_DESTRUCTOR || peekNext().getType() == TT::KEYWORD_PROPERTY ||
                        peekNext().getType() == TT::KEYWORD_VAR || peekNext().getType() == TT::KEYWORD_CONST ||
                        peekNext().getType() == TT::KEYWORD_OPERATOR ||
                        peekNext().getType() == TT::KEYWORD_PUBLIC || peekNext().getType() == TT::KEYWORD_PRIVATE ||
                        peekNext().getType() == TT::KEYWORD_PROTECTED)
                    {
                        std::string suggestedName = className + "_" + nextId.getValue();
                        errorCatalog(Errors::ErrorCode::SYN_NAME_HAS_SPACE, {{"what_ar", "الصنف"}, {"what_en", "class"}, {"name", className + " " + nextId.getValue()}, {"suggested", kw(TT::KEYWORD_CLASS) + " " + suggestedName}});
                        return nullptr;
                    }
                }
            }

#ifdef DEBUG_OOP
            std::cout << "[OOP] اسم الصنف: " << className << "\n";
#endif

            // (AR) الوراثة (اختياري) — 'يرث' فقط
            // (EN) Inheritance (optional) — 'يرث' keyword only
            std::vector<std::string> baseClassNames;
            if (match(TT::KEYWORD_INHERITS))
            {
                // (AR) قائمة الأصناف الأساسية (دعم الفاصلة العربية والإنجليزية)
                // (EN) Base class list (support both Arabic and English commas)
                // Spec: docs\language_spec\rules\03_oop.md §1 - base_class_list ::= IDENTIFIER ((',' | '،') IDENTIFIER)*
                do
                {
                    Token baseToken = consume(TT::IDENTIFIER,
                                              "(AR) توقع اسم الصنف الأساسي. (EN) Expected base class name.");
                    baseClassNames.push_back(baseToken.getValue());
#ifdef DEBUG_OOP
                    std::cout << "[OOP] يرث من: " << baseToken.getValue() << "\n";
#endif
                } while (matchComma()); // Support both commas
            }

            // (AR) تنفيذ السمات (اختياري): صنف اسم نفّذ سمة1، سمة2
            // (EN) Trait implementation (optional): class Name نفّذ Trait1, Trait2
            // (AR) نستهلك أسماء السمات لكن لا نضيفها للأصناف الأساسية — السمات تُتحقق لاحقاً
            // (EN) Consume trait names but don't add to base classes — traits are checked separately
            if ((check(TT::KEYWORD_IMPL) ||
                 checkContextual(TT::KEYWORD_IMPL)))
            {
                advance(); // consume نفّذ/نفذ
                do
                {
                    Token traitToken = consume(TT::IDENTIFIER,
                                               "(AR) توقع اسم السمة بعد 'نفّذ'. (EN) Expected trait name after 'implements'.");
                    // (AR) لا نضيف للـ superclasses — السمات ليست أصناف أساسية
                    // (EN) Don't add to superclasses — traits are not base classes
#ifdef DEBUG_OOP
                    std::cout << "[OOP] ينفّذ سمة: " << traitToken.getValue() << "\n";
#endif
                } while (matchComma());
            }

            // (AR) جسم الصنف مباشرة بدون أقواس - ينتهي بـ 'نهاية'
            // (EN) Class body directly without braces - ends with 'نهاية'
            // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ends with 'نهاية' NOT '}'
            // Note: NO BRACE_LEFT here! Class body is parsed directly

            // (AR) تحليل أعضاء الصنف / (EN) Parse class members
            // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ends with 'نهاية'
            StmtList members;
            while (!check(TT::KEYWORD_END) && !isAtEnd())
            {
                // ═══════════════════════════════════════════════════════════════════
                // (AR) قاعدة الصفة بعد الموصوف — المعدلات تأتي بعد الكلمة المفتاحية
                //      الصيغة الجديدة: دالة عام ساكن طريقة()
                //      بدلاً من القديمة: عام ساكن دالة طريقة()
                // (EN) Arabic adjective-after-noun rule — modifiers come AFTER keyword
                //      New: دالة عام ساكن method()
                //      Old (removed): عام ساكن دالة method()
                // ═══════════════════════════════════════════════════════════════════

                // ─────────────────────────────────────────────────────────────
                // (AR) رسائل خطأ للصيغة القديمة: عام/خاص/محمي/ساكن/مجرد قبل الكلمة المفتاحية
                // (EN) Error messages for old syntax: modifier BEFORE keyword
                // ─────────────────────────────────────────────────────────────
                if (check(TT::KEYWORD_PUBLIC) || check(TT::KEYWORD_PRIVATE) ||
                    check(TT::KEYWORD_PROTECTED) || check(TT::KEYWORD_STATIC) || check(TT::KEYWORD_ABSTRACT))
                {
                    // (AR) حفظ المعدل لرسالة الخطأ
                    std::string modName = current_.getValue();
                    auto modPos = current_.getPosition();

                    // (AR) التطلع للأمام لكشف الكلمة المفتاحية التالية
                    auto nextTT = peekNext().getType();

                    // (AR) كشف النمط القديم: معدل + كلمة_مفتاحية (أو معدل + معدل + ...)
                    if (nextTT == TT::KEYWORD_FUNCTION || nextTT == TT::KEYWORD_CONSTRUCTOR ||
                        nextTT == TT::KEYWORD_DESTRUCTOR || nextTT == TT::KEYWORD_PROPERTY ||
                        nextTT == TT::KEYWORD_OPERATOR || nextTT == TT::KEYWORD_VAR ||
                        nextTT == TT::KEYWORD_CONST || nextTT == TT::KEYWORD_STATIC ||
                        nextTT == TT::KEYWORD_PUBLIC || nextTT == TT::KEYWORD_PRIVATE ||
                        nextTT == TT::KEYWORD_PROTECTED || nextTT == TT::KEYWORD_ABSTRACT ||
                        (nextTT == TT::IDENTIFIER && (peekNext().getValue() == "خاصية" ||
                                                      peekNext().getValue() == "عامل" || peekNext().getValue() == "هدم")))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_ADJECTIVE_ORDER, {{"wrong", modName + " " + kw(TT::KEYWORD_FUNCTION) + " ..."}, {"right", kw(TT::KEYWORD_FUNCTION) + " " + modName + " ..."}});
                        // (AR) محاولة استرداد: نبتلع كل المعدلات القديمة ونحللها بالصيغة الجديدة
                        bool recStatic = false, recVirtual = false, recAbstract = false;
                        AccessModifier recAccess = parseModifiers(recStatic, recVirtual, recAbstract);
                        // (AR) نستمر في التحليل — المعدلات ابتُلعت والرمز الحالي هو الكلمة المفتاحية
                        // سنترك الكود يسقط إلى الفروع التالية حيث سيُكشف عن الكلمة المفتاحية
                        // ثم سيقرأ parseModifiers مرة أخرى (لن يجد شيئاً فيعيد الافتراضي)
                    }
                    else
                    {
                        // (AR) المعدل متبوع بنوع أو معرّف — قد يكون حقلاً (عام نص اسم)
                        //      نبتلع المعدلات ونسقط إلى تحليل الحقول
                        bool recStatic = false, recVirtual = false, recAbstract = false;
                        AccessModifier recAccess = parseModifiers(recStatic, recVirtual, recAbstract);

                        // (AR) إذا كان حقلاً — نعالجه بالمعدلات المبتلعة
                        if (isTypeToken(current_.getType()) ||
                            (check(TT::IDENTIFIER) && isClassName(current_.getValue())))
                        {
                            errorCatalog(Errors::ErrorCode::SYN_ADJECTIVE_ORDER, {{"wrong", modName + " " + kw(TT::TYPE_STRING) + " اسم"}, {"right", kw(TT::KEYWORD_VAR) + " " + modName + " اسم = قيمة"}});
                            auto field = parseFieldDeclaration(recAccess, recStatic);
                            if (field)
                                members.push_back(std::move(field));
                            continue;
                        }
                        // (AR) غير معروف — نعطي خطأ عام
                        errorCatalog(Errors::ErrorCode::SYN_UNEXPECTED_TOKEN, {{"found", modName}, {"expected", "تصريح يقبل هذا المعدِّل (a declaration accepting this modifier)"}});
                        advance();
                        continue;
                    }
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) تحليل المعدلات المحلية — تُستخدم فقط بعد ابتلاع المعدلات القديمة
                // (EN) Local modifier variables — used only after swallowing old modifiers
                // ═══════════════════════════════════════════════════════════════════
                bool isStatic = false;
                bool isVirtual = false;
                bool isAbstract = false;
                AccessModifier access = AccessModifier::PUBLIC;

                // ─────────────────────────────────────────────────────────────
                // (AR) [1] خاصية: خاصية [عام|خاص|محمي] [ساكن] [نوع] اسم
                // (EN) [1] Property: property [public|private|protected] [static] [type] name
                // ─────────────────────────────────────────────────────────────
                if (check(TT::KEYWORD_PROPERTY) || checkContextual(TT::KEYWORD_PROPERTY))
                {
                    advance(); // consume 'خاصية'

                    // (AR) قراءة المعدلات بعد 'خاصية'
                    access = parseModifiers(isStatic, isVirtual, isAbstract);

                    // (AR) التحقق من النوع المدمج بعد 'خاصية' (مثال: خاصية عام نص الاسم)
                    // (EN) Check for built-in type after 'خاصية' (e.g., خاصية عام نص الاسم)
                    if (isTypeToken(current_.getType()))
                    {
                        auto property = parsePropertyDeclaration(access, isStatic);
                        if (property)
                        {
                            members.push_back(std::move(property));
                        }
                    }
                    else if (check(TT::IDENTIFIER))
                    {
                        auto property = parsePropertyDeclaration(access, isStatic);
                        if (property)
                        {
                            members.push_back(std::move(property));
                        }
                    }
                    else
                    {
                        error("(AR) توقع نوع أو اسم بعد 'خاصية'. (EN) Expected type or name after 'خاصية'.");
                        advance();
                    }
                    continue;
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) 'غير_متزامن دالة' أُزيلت في جسم الصنف — استخدم 'دالة غير_متزامن'
                // (EN) 'async function' removed in class body — use 'function async'
                // ─────────────────────────────────────────────────────────────
                if (check(TT::KEYWORD_ASYNC) ||
                    (check(TT::IDENTIFIER) && checkContextual(TT::KEYWORD_ASYNC)))
                {
                    errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", kw(TT::KEYWORD_ASYNC) + " " + kw(TT::KEYWORD_FUNCTION)}, {"new", kw(TT::KEYWORD_FUNCTION) + " " + kw(TT::KEYWORD_ASYNC)}, {"example", kw(TT::KEYWORD_FUNCTION) + " " + kw(TT::KEYWORD_ASYNC) + " جلب()"}});
                    advance(); // consume 'غير_متزامن'
                    // (AR) محاولة استرداد — نحلل كدالة عادية غير متزامنة
                    if (check(TT::KEYWORD_FUNCTION))
                    {
                        advance(); // consume 'دالة'
                        // (AR) قراءة المعدلات بعد 'دالة'
                        access = parseModifiers(isStatic, isVirtual, isAbstract);
                        auto method = parseMethodDeclaration(access, isStatic, isVirtual, isAbstract);
                        if (method)
                        {
                            auto *methodDecl = dynamic_cast<MethodDecl *>(method.get());
                            if (methodDecl)
                            {
                                methodDecl->isAsync = true;
                            }
                            members.push_back(std::move(method));
                        }
                        continue;
                    }
                    error("(AR) توقع 'دالة' بعد 'غير_متزامن'. (EN) Expected 'function' after 'async'.");
                    continue;
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) [2] طريقة: دالة [عام|خاص|محمي] [ساكن] [مجرد] [غير_متزامن] [نوع] اسم()
                // (EN) [2] Method: function [public|private|protected] [static] [abstract] [async] [type] name()
                // ─────────────────────────────────────────────────────────────
                if (check(TT::KEYWORD_FUNCTION))
                {
                    advance(); // consume 'دالة'
                    // (AR) قراءة المعدلات بعد 'دالة' (الصفة بعد الموصوف)
                    access = parseModifiers(isStatic, isVirtual, isAbstract);
                    auto method = parseMethodDeclaration(access, isStatic, isVirtual, isAbstract);
                    if (method)
                    {
                        members.push_back(std::move(method));
                    }
                    continue;
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) [3] باني: باني [عام|خاص|محمي] (...)
                // (EN) [3] Constructor: constructor [public|private|protected] (...)
                // ─────────────────────────────────────────────────────────────
                if (check(TT::KEYWORD_CONSTRUCTOR))
                {
                    advance(); // (AR) استهلاك كلمة الباني
                    // (AR) قراءة المعدلات بعد 'باني'
                    access = parseModifiers(isStatic, isVirtual, isAbstract);

                    auto constructor = parseConstructorDeclaration(className, access);
                    if (constructor)
                    {
                        members.push_back(std::move(constructor));
                    }
                    continue;
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) [4] هدم: هدم [عام|خاص|محمي] ()
                // (EN) [4] Destructor: destructor [public|private|protected] ()
                // ─────────────────────────────────────────────────────────────
                if (check(TT::KEYWORD_DESTRUCTOR) || checkContextual(TT::KEYWORD_DESTRUCTOR))
                {
                    advance(); // (AR) استهلاك كلمة الهدم / (EN) consume destructor keyword
                    // (AR) قراءة المعدلات بعد 'هدم'
                    access = parseModifiers(isStatic, isVirtual, isAbstract);
                    auto destructor = parseDestructorDeclaration(className, access);
                    if (destructor)
                    {
                        members.push_back(std::move(destructor));
                    }
                    continue;
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) [5] عامل: عامل [عام|خاص|محمي] + (...)
                // (EN) [5] Operator: operator [public|private|protected] + (...)
                // ─────────────────────────────────────────────────────────────
                if (check(TT::KEYWORD_OPERATOR) || checkContextual(TT::KEYWORD_OPERATOR))
                {
                    advance(); // consume 'عامل'
                    // (AR) قراءة المعدلات بعد 'عامل'
                    access = parseModifiers(isStatic, isVirtual, isAbstract);
                    auto operatorDecl = parseOperatorDecl();
                    if (operatorDecl)
                    {
                        // (AR) تعيين معدّل الوصول / (EN) Set access modifier
                        if (auto *opDecl = dynamic_cast<AST::OperatorDecl *>(operatorDecl.get()))
                        {
                            opDecl->access = access;
                        }
                        members.push_back(std::move(operatorDecl));
                    }
                    continue;
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) دعم 'ثابت' داخل الصنف: ثابت اسم = قيمة
                // (EN) Support 'const' inside class: const name = value
                // ─────────────────────────────────────────────────────────────
                if (check(TT::KEYWORD_CONST))
                {
                    // (AR) التقاط تعليق التوثيق المعلَّق قبل أي عمل آخر
                    //      حتى لا يتسرب إلى تصريح آخر (BF-04: ربط ## بحقل ثابت)
                    // (EN) Capture pending doc comment up-front so it doesn't
                    //      leak into another declaration
                    std::string capturedDoc = consumePendingDocComment();
                    advance(); // consume 'ثابت'
                    // (AR) قراءة المعدلات بعد 'ثابت' (الصفة بعد الموصوف)
                    access = parseModifiers(isStatic, isVirtual, isAbstract);
                    // (AR) دعم الكلمات المفتاحية والحروف المحجوزة كأسماء حقول (مثل: ثابت افتراضي = ...)
                    // (EN) Accept keywords/literals as field names (e.g., const default = ...)
                    Token fieldName(TT::IDENTIFIER, "", Lexer::Position());
                    if (check(TT::IDENTIFIER))
                    {
                        fieldName = current_;
                        advance();
                    }
                    else if (isTokenUsableAsName(current_.getType()))
                    {
                        fieldName = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                        advance();
                    }
                    else
                    {
                        error("(AR) توقعت اسم حقل بعد 'ثابت'. (EN) Expected field name after 'const'.");
                        advance();
                        continue;
                    }

                    ExprPtr initializer = nullptr;
                    if (match(TT::OP_ASSIGN))
                    {
                        initializer = parseExpression();
                    }

                    auto field = std::make_unique<FieldDecl>(
                        fieldName.getValue(), Types::SadTypeKind::Unknown,
                        std::move(initializer), access, true, fieldName.getPosition());
                    // (AR) إرفاق التوثيق الملتقط بالحقل الأول فقط — الحقول التالية
                    //      في نفس السطر (بفاصلة) لا تأخذ نفس التعليق
                    // (EN) Attach the captured doc to the first field only
                    field->docComment = std::move(capturedDoc);
                    members.push_back(std::move(field));

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) دعم ثوابت متعددة بفاصلة: ثابت س = 1 ، ص = 2 ، ع = 3
                    // (EN) Multi-const with comma: const x = 1 , y = 2 , z = 3
                    // ═══════════════════════════════════════════════════════════════
                    while (matchComma())
                    {

                        Token nextFieldName(TT::IDENTIFIER, "", Lexer::Position());
                        if (check(TT::IDENTIFIER))
                        {
                            nextFieldName = current_;
                            advance();
                        }
                        else if (isTokenUsableAsName(current_.getType()))
                        {
                            nextFieldName = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                            advance();
                        }
                        else
                        {
                            error("(AR) توقعت اسم حقل بعد الفاصلة. (EN) Expected field name after comma.");
                            break;
                        }

                        ExprPtr nextInit = nullptr;
                        if (match(TT::OP_ASSIGN))
                        {
                            nextInit = parseExpression();
                        }

                        auto nextField = std::make_unique<FieldDecl>(
                            nextFieldName.getValue(), Types::SadTypeKind::Unknown,
                            std::move(nextInit), access, true, nextFieldName.getPosition());
                        members.push_back(std::move(nextField));
                    }

                    matchSemicolon();
                    continue;
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) دعم 'متغير' داخل الصنف: متغير عام اسم: نوع = قيمة
                // (EN) Support 'var' inside class: var public name: type = value
                // (AR) دعم تعريف حقول متعددة بفاصلة: متغير عام اسم = "أحمد" ، عمر = 20
                // (EN) Support multi-field declarations: var public name = "Ahmed" , age = 20
                // ─────────────────────────────────────────────────────────────
                if (check(TT::KEYWORD_VAR))
                {
                    // (AR) التقاط تعليق التوثيق المعلَّق قبل أي عمل آخر
                    //      (BF-04: ربط ## بحقل متغير داخل الصنف)
                    // (EN) Capture pending doc comment up-front
                    std::string capturedDoc = consumePendingDocComment();
                    advance(); // consume 'متغير'
                    // (AR) قراءة المعدلات بعد 'متغير' (الصفة بعد الموصوف)
                    access = parseModifiers(isStatic, isVirtual, isAbstract);
                    // (AR) دعم الكلمات المفتاحية والحروف المحجوزة كأسماء حقول (مثل: متغير خطأ = ...)
                    // (EN) Accept keywords/literals as field names (e.g., var error = ...)
                    Token fieldName(TT::IDENTIFIER, "", Lexer::Position());
                    if (check(TT::IDENTIFIER))
                    {
                        fieldName = current_;
                        advance();
                    }
                    else if (isTokenUsableAsName(current_.getType()))
                    {
                        fieldName = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                        advance();
                    }
                    else
                    {
                        error("(AR) توقعت اسم حقل بعد 'متغير'. (EN) Expected field name after 'var'.");
                        advance();
                        continue;
                    }

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) دعم تحديد النوع: متغير اسم: نوع [= قيمة]
                    // (EN) Support type annotation: var name: type [= value]
                    // ═══════════════════════════════════════════════════════════════
                    Types::SadTypeKind fieldType = Types::SadTypeKind::Unknown;
                    std::string typeName;

                    if (match(TT::COLON))
                    {
                        // (AR) نوع محدد: متغير اسم: نوع
                        // (EN) Explicit type: var name: type
                        if (isTypeToken(current_.getType()))
                        {
                            // (AR) نوع أساسي: رقم، نص، منطقي، إلخ
                            Token typeToken = current_;
                            advance();
                            typeName = typeToken.getValue();
                            fieldType = mapTokenTypeToKind(typeToken.getType());
                        }
                        else if (check(TT::IDENTIFIER))
                        {
                            // (AR) صنف مخصص
                            Token typeToken = current_;
                            advance();
                            typeName = typeToken.getValue();
                            fieldType = Types::SadTypeKind::Class;
                        }
                        else
                        {
                            error("(AR) توقعت نوع الحقل بعد ':'. (EN) Expected field type after ':'.");
                        }
                    }

                    ExprPtr initializer = nullptr;
                    if (match(TT::OP_ASSIGN))
                    {
                        initializer = parseExpression();
                    }

                    auto field = std::make_unique<FieldDecl>(
                        fieldName.getValue(), fieldType,
                        std::move(initializer), access, isStatic, fieldName.getPosition());
                    // (AR) إرفاق التوثيق الملتقط بالحقل الأول فقط
                    // (EN) Attach the captured doc to the first field only
                    field->docComment = std::move(capturedDoc);
                    members.push_back(std::move(field));

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) دعم حقول متعددة بفاصلة: عام متغير اسم = "أحمد" ، عمر = 20
                    // (EN) Multi-field with comma: public var name = "Ahmed" , age = 20
                    // ═══════════════════════════════════════════════════════════════
                    while (matchComma())
                    {

                        Token nextFieldName(TT::IDENTIFIER, "", Lexer::Position());
                        if (check(TT::IDENTIFIER))
                        {
                            nextFieldName = current_;
                            advance();
                        }
                        else if (isTokenUsableAsName(current_.getType()))
                        {
                            nextFieldName = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                            advance();
                        }
                        else
                        {
                            error("(AR) توقعت اسم حقل بعد الفاصلة. (EN) Expected field name after comma.");
                            break;
                        }

                        // (AR) نوع اختياري / (EN) Optional type
                        Types::SadTypeKind nextFieldType = Types::SadTypeKind::Unknown;
                        if (match(TT::COLON))
                        {
                            if (isTypeToken(current_.getType()))
                            {
                                Token typeToken = current_;
                                advance();
                                nextFieldType = mapTokenTypeToKind(typeToken.getType());
                            }
                            else if (check(TT::IDENTIFIER))
                            {
                                advance();
                                nextFieldType = Types::SadTypeKind::Class;
                            }
                        }

                        // (AR) مُهيّئ اختياري / (EN) Optional initializer
                        ExprPtr nextInit = nullptr;
                        if (match(TT::OP_ASSIGN))
                        {
                            nextInit = parseExpression();
                        }

                        auto nextField = std::make_unique<FieldDecl>(
                            nextFieldName.getValue(), nextFieldType,
                            std::move(nextInit), access, isStatic, nextFieldName.getPosition());
                        members.push_back(std::move(nextField));
                    }

                    // (AR) تخطي الفاصلة المنقوطة الاختيارية
                    matchSemicolon();
                    continue;
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) حقل بدون 'متغير' لم يعد مدعوماً — يجب استخدام 'متغير'
                // (EN) Field without 'var' keyword no longer supported — must use 'var'
                // ─────────────────────────────────────────────────────────────
                if (check(TT::IDENTIFIER) && !isClassName(current_.getValue()) &&
                    (peekNext().getType() == TT::OP_ASSIGN || peekNext().getType() == TT::COLON))
                {
                    Token fieldName = current_;
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_VAR)}, {"ctx_ar", "قبل تعريف الحقل '" + fieldName.getValue() + "'"}, {"ctx_en", "before the field '" + fieldName.getValue() + "' definition"}});
                    // (AR) تخطي الحقل لمنع الحلقة اللانهائية
                    advance(); // skip name
                    if (match(TT::COLON))
                    {
                        if (isTypeToken(current_.getType()) || check(TT::IDENTIFIER))
                            advance();
                    }
                    if (match(TT::OP_ASSIGN))
                        parseExpression();
                    matchSemicolon();
                    continue;
                }

                // Otherwise, parse as field declaration
                // Field syntax: type name [= value] [;]
                // Support both built-in types (نص، رقم) and class types (شخص، حيوان)
                if (isTypeToken(current_.getType()) ||
                    (check(TT::IDENTIFIER) && isClassName(current_.getValue())))
                {
                    auto field = parseFieldDeclaration(access, isStatic);
                    if (field)
                    {
                        members.push_back(std::move(field));
                    }
                }
                else
                {
                    // Unknown member, skip to avoid infinite loop
                    error("(AR) عضو صنف غير معروف. (EN) Unknown class member.");
                    advance(); // Skip token
                }
            }

            // (AR) توقع 'نهاية' في نهاية تعريف الصنف
            // (EN) Expect 'نهاية' at end of class definition
            // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ends with 'نهاية'
            consume(TT::KEYWORD_END,
                    "(AR) توقع 'نهاية' بعد جسم الصنف. (EN) Expected 'نهاية' after class body.");

#ifdef DEBUG_OOP
            std::cout << "[OOP] انتهى تحليل صنف '" << className << "' - "
                      << members.size() << " أعضاء\n";
#endif

            // (AR) تسجيل الصنف مبكراً في ClassManager للسماح بمتغيرات من هذا النوع لاحقاً في نفس الملف
            // (EN) Register class early in ClassManager to allow variables of this type later in same file
            // هذا تسجيل مؤقت - سيتم تحديثه عند التنفيذ بكامل التفاصيل
            // This is temporary registration - will be updated during execution with full details
            auto *classManager = Data::ClassManager::getInstance();
            if (!classManager->hasClass(className))
            {
                auto tempClassType = std::make_unique<Data::ClassType>(className);
                classManager->registerClass(std::move(tempClassType));
#ifdef DEBUG_OOP
                std::cout << "[OOP] ✅ تسجيل مؤقت للصنف: " << className << " (أثناء التحليل)\n";
#endif
            }

            // (AR) استخدام جميع الأصناف الأساسية بدلاً من الأول فقط / (EN) Use all base classes instead of just first
            auto classDecl = std::make_unique<ClassDecl>(
                className,
                baseClassNames, // (AR) دعم الوراثة المتعددة الكامل / (EN) Full multiple inheritance support
                std::move(members),
                false,
                nameToken.getPosition());
            classDecl->docComment = std::move(docComment);
            // (AR) تخزين مسار الملف المصدري — ضروري لفحص الوراثة من الأصناف المحكمة
            // (EN) Store source file path — needed for sealed class inheritance checking
            classDecl->sourceFile = filename_;
            return classDecl;
        }

        /**
         * @brief (AR) يحلل تصريح متغير بصيغتين:
         *             1. var/let/const name : type = value;
         *             2. type name = value;
         *        (EN) Parses variable declaration in two formats:
         *             1. var/let/const name : type = value;
         *             2. type name = value;
         */
        StmtPtr ParserCore::parseVarDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            // (AR) اللبنة 3.14: لصيقة «متطاير» اللاحقة (عربيّ سليم: الموصوف ثمّ الصفة)
            //      متغير متطاير عداد = 0  ⇒  volatile. تحلّ محلّ توجيه @متطاير المحذوف.
            //      «متطاير» كلمة سياقيّة (CONTEXTUAL) فقد تُلفظ IDENTIFIER — نقبل الحالتين.
            bool declVolatile = false;
            if (check(TT::KEYWORD_VOLATILE) ||
                (check(TT::IDENTIFIER) && current_.getValue() == "متطاير"))
            {
                advance();
                declVolatile = true;
            }

            // =====================================================================
            // (AR) اللبنة 3.16: مصفوفة تخزين ساكن مصفَّرة في .bss —
            //      «متغير ساكن اسم مصفوفة[N]» ⇒ [N x i8] zeroinitializer.
            //      لا مُهيّئ (مصفَّرة تلقائيًّا)؛ N ثابت موجب زمن-ترجمة؛ تُصدَّر عبر @رمز
            //      (linkSymbol). البايتات i8 — نهلة تعنونها بـعنوان_رمز + اكتب_ذاكرة32.
            // (EN) Brick 3.16: named zero-filled static .bss array.
            // =====================================================================
            if (check(TT::KEYWORD_STATIC))
            {
                advance(); // (AR) استهلاك «ساكن» / (EN) consume 'static'
                if (!check(TT::IDENTIFIER))
                    errorCatalog(Errors::ErrorCode::SEM_STATIC_ARRAY_SIZE,
                                 {{"detail", "توقّع اسم المخزن بعد «ساكن»: متغير ساكن اسم مصفوفة[N]"}});
                Token saNameTok = current_;
                advance(); // (AR) الاسم / (EN) the name
                // (AR) «مصفوفة» نوع مدمج (builtin_types، emittedByLexer=false) فقد يُلفَظ
                //      IDENTIFIER — نقبل الحالتين (نظير الأنواع السياقيّة).
                if (check(TT::TYPE_ARRAY) ||
                    (check(TT::IDENTIFIER) && current_.getValue() == "مصفوفة"))
                    advance();
                else
                    errorCatalog(Errors::ErrorCode::SEM_STATIC_ARRAY_SIZE,
                                 {{"detail", "توقّع «مصفوفة» بعد اسم المخزن الساكن: متغير ساكن اسم مصفوفة[N]"}});
                consume(TT::BRACKET_LEFT,
                        "(AR) توقّع '[' لحجم المصفوفة الساكنة / (EN) expected '[' for static array size");
                Token saSizeTok = consume(TT::NUMBER_INTEGER,
                        "(AR) توقّع عددًا صحيحًا حرفيًّا لحجم المصفوفة الساكنة\n"
                        "(EN) expected an integer literal for the static array size");
                consume(TT::BRACKET_RIGHT,
                        "(AR) توقّع ']' بعد حجم المصفوفة الساكنة / (EN) expected ']' after size");
                uint64_t saCount = 0;
                try { saCount = std::stoull(saSizeTok.getValue()); }
                catch (...) { saCount = 0; }
                // (AR) SEM023: الحجم عدد صحيح موجب زمن-ترجمة
                if (saCount == 0)
                    errorCatalog(Errors::ErrorCode::SEM_STATIC_ARRAY_SIZE,
                                 {{"detail", "الحجم '" + saSizeTok.getValue() + "' ليس عددًا صحيحًا موجبًا"}});
                // (AR) النوع cosmetic هنا (الخلفيّة تُصدر [N x i8] من zeroArrayCount لا من
                //      النوع)؛ Integer يتجنّب تحذير astTypeToSIRType على الأنواع غير المعالَجة.
                auto saDecl = std::make_unique<VarDeclStmt>(
                    saNameTok.getValue(), Types::SadTypeKind::Integer, nullptr, false,
                    saNameTok.getPosition());
                saDecl->isStaticArray = true;
                saDecl->staticArrayCount = saCount;
                saDecl->isVolatile = declVolatile;
                return saDecl;
            }

            // =====================================================================
            // (AR) تفكيك الصف: متغير (أ، ب، ج) = تعبير_صف
            // (EN) Tuple destructuring: var (a, b, c) = tuple_expression
            // =====================================================================
            if (check(TT::PAREN_LEFT))
            {
                auto startPos = current_.getPosition();
                advance(); // (AR) تخطي '(' / (EN) Skip '('

                std::vector<std::string> names;

                // (AR) تحليل أسماء المتغيرات المفصولة بفواصل
                // (EN) Parse comma-separated variable names
                if (!check(TT::PAREN_RIGHT))
                {
                    do
                    {
                        if (!check(TT::IDENTIFIER))
                        {
                            errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "متغير"}, {"what_en", "variable"}, {"ctx_ar", "داخل تفكيك الصف"}, {"ctx_en", "inside tuple destructuring"}});
                            return nullptr;
                        }
                        names.push_back(current_.getValue());
                        advance();
                    } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));
                }

                if (!match(TT::PAREN_RIGHT))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ")"}, {"ctx_ar", "بعد أسماء المتغيرات في تفكيك الصف"}, {"ctx_en", "after the variable names in tuple destructuring"}});
                    return nullptr;
                }

                if (!match(TT::OP_ASSIGN))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "="}, {"ctx_ar", "بعد ')' في تفكيك الصف"}, {"ctx_en", "after ')' in tuple destructuring"}});
                    return nullptr;
                }

                auto initializer = parseExpression();
                // (AR) تخطي الفاصلة المنقوطة الاختيارية / (EN) Skip optional semicolon
                if (check(TT::SEMICOLON) || check(TT::ARABIC_SEMICOLON))
                    advance();

                bool isConst = pendingConst_;
                return std::make_unique<TupleDestructureStmt>(names, isConst, std::move(initializer), startPos);
            }

            Types::SadTypeKind varType = Types::SadTypeKind::Unknown;
            // (AR) [NS-06 موجة 2] النوع الداخليّ T إن كان varType اختياريًّا `T؟`.
            // (EN) [NS-06 wave 2] inner T when varType is optional `T?`.
            Types::SadTypeKind varInnerKind = Types::SadTypeKind::Unknown;
            std::string className = "";     // For class-typed variables
            Token name(TT::IDENTIFIER, ""); // Initialize with default

            // Check if we have type-first syntax: TYPE IDENTIFIER = value;
            // (AR) التحقق من صيغة النوع أولاً: نوع معرّف = قيمة;
            // (AR) لكن فقط إذا كان بعد النوع معرّف — وإلا النوع هو اسم المتغير
            // (EN) BUT only if there's an IDENTIFIER after the type — otherwise the type IS the var name
            if (isTypeToken(current_.getType()) && peekNext().getType() == TT::IDENTIFIER)
            {
                // Format 2: TYPE IDENTIFIER = value;
                // Current token is already a type token (TYPE_INTEGER, TYPE_STRING, etc.)
                // (AR) الصيغة 2: نوع معرّف = قيمة;
                // الرمز الحالي هو بالفعل رمز نوع
                varType = parseType();
                varInnerKind = lastOptionalInner_;

                // Check if we have an identifier after the type
                // (AR) تحقق مما إذا كان لدينا معرّف بعد النوع
                if (!check(TT::IDENTIFIER))
                {
                    // Missing identifier after type specification
                    // (AR) معرّف مفقود بعد تحديد النوع
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "متغير"}, {"what_en", "variable"}, {"ctx_ar", "بعد تحديد النوع"}, {"ctx_en", "after the type specification"}});
                    return nullptr;
                }

                name = peek();
                advance();
            }
            else if (isTypeToken(current_.getType()))
            {
                // (AR) كلمة نوع مستخدمة كاسم متغير: متغير نص = "hello"
                // (EN) Type keyword used as variable name: var text = "hello"
                name = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                advance();
            }
            else if (check(TT::IDENTIFIER))
            {
                // Check if this identifier is a class name (for class-typed variables)
                // (AR) التحقق مما إذا كان هذا المعرّف هو اسم صنف
                // (AR) لكن فقط إذا كان متبوعاً بمعرّف آخر — إذا كان متبوعاً بـ = فهو اسم المتغير
                // (EN) BUT only if followed by another IDENTIFIER — if followed by = it IS the var name
                if (isClassName(current_.getValue()) && peekNext().getType() == TT::IDENTIFIER)
                {
                    // Class-typed variable: ClassName varName = ...;
                    // (AR) متغير من نوع صنف: اسم_الصنف اسم_المتغير = ...;
                    className = current_.getValue();
                    varType = Types::SadTypeKind::Class;
                    advance(); // Consume class name

                    // Check if identifier follows the class name
                    // (AR) تحقق مما إذا كان معرّف يتبع اسم الصنف
                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "متغير"}, {"what_en", "variable"}, {"ctx_ar", "من نوع الصنف '" + className + "'"}, {"ctx_en", "of class type '" + className + "'"}});
                        return nullptr;
                    }

                    name = peek();
                    advance();
                }
                else
                {
                    // Format 1: var/let/const IDENTIFIER : type = value;
                    // or just: IDENTIFIER = value; (type inference)
                    // (AR) الصيغة 1: var/let/const معرّف : نوع = قيمة;
                    // أو فقط: معرّف = قيمة; (استنتاج النوع)
                    name = peek();
                    advance();

                    // (AR) التحقق من نمط وجود مسافة في اسم المتغير: متغير عدد طلاب = 10
                    // (EN) Check for space-in-variable-name pattern: var word1 word2 = 10
                    if (check(TT::IDENTIFIER) && name.getPosition().line == current_.getPosition().line)
                    {
                        // (AR) نمط: معرّف معرّف = قيمة → مسافة في اسم المتغير
                        if (peekNext().getType() == TT::OP_ASSIGN)
                        {
                            std::string suggestedName = name.getValue() + "_" + current_.getValue();
                            errorCatalog(Errors::ErrorCode::SYN_NAME_HAS_SPACE, {{"what_ar", "المتغير"}, {"what_en", "variable"}, {"name", name.getValue() + " " + current_.getValue()}, {"suggested", kw(TT::KEYWORD_VAR) + " " + suggestedName + " = ..."}});
                            return nullptr;
                        }
                        // (AR) نمط: معرّف معرّف معرّف... → اسم متعدد الكلمات
                        if (peekNext().getType() == TT::IDENTIFIER && peekNext().getPosition().line == name.getPosition().line)
                        {
                            errorCatalog(Errors::ErrorCode::SYN_NAME_HAS_SPACE, {{"what_ar", "المتغير"}, {"what_en", "variable"}, {"name", name.getValue() + " ..."}, {"suggested", name.getValue() + "_" + current_.getValue() + "_..."}});
                            return nullptr;
                        }
                    }

                    // Optional type annotation: name : type
                    // (AR) تصريح النوع الاختياري: اسم : نوع
                    if (match(TT::COLON))
                    {
                        // We have a type annotation, parse it
                        // (AR) لدينا تصريح نوع، قم بتحليله
                        Types::SadTypeKind annotatedType = parseType();

                        // Check if the type was parsed successfully
                        // (AR) تحقق مما إذا تم تحليل النوع بنجاح
                        if (annotatedType == Types::SadTypeKind::Unknown)
                        {
                            errorCatalog(Errors::ErrorCode::SYN_UNKNOWN_ELEMENT, {{"what_ar", "النوع بعد ':' في تصريح المتغير '" + name.getValue() + "'"}, {"what_en", "type after ':' in the declaration of '" + name.getValue() + "'"}, {"found", current_.getValue()}, {"allowed", kw(TT::TYPE_INTEGER) + "، " + kw(TT::TYPE_DOUBLE) + "، " + kw(TT::TYPE_STRING) + "، " + kw(TT::TYPE_BOOLEAN) + "، ..."}});
                            return nullptr;
                        }

                        varType = annotatedType;
                        varInnerKind = lastOptionalInner_;
                    }
                }
            }
            else if (isTokenUsableAsName(current_.getType()))
            {
                // (AR) كلمة محجوزة مسموح بها كاسم متغير (مثل: جديد، نهاية، من، باني، استمر)
                //      يمكن استخدامها بعد 'متغير' لأن السياق واضح
                // (EN) Allowed reserved word as variable name (e.g., new, end, from, constructor, continue)
                //      Can be used after 'var' because context is unambiguous
                name = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                advance();

                // Optional type annotation: name : type
                if (match(TT::COLON))
                {
                    Types::SadTypeKind annotatedType = parseType();
                    if (annotatedType != Types::SadTypeKind::Unknown)
                    {
                        varType = annotatedType;
                        varInnerKind = lastOptionalInner_;
                    }
                }
            }
            else
            {
                // Neither type token nor identifier - this is an error
                // (AR) لا رمز نوع ولا معرّف - هذا خطأ
                Token currentToken = peek();

                // Provide more helpful error messages based on what we found — via the central catalog
                // (AR) قدّم رسائل خطأ أكثر فائدة بناءً على ما وجدنا — عبر الكتالوج المركزي
                if (currentToken.getType() == TT::NUMBER_INTEGER || currentToken.getType() == TT::NUMBER_DOUBLE)
                {
                    // (AR) رقم بعد متغير: متغير 42 = ...
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME,
                                 {{"what_ar", "متغير"},
                                  {"what_en", "variable"},
                                  {"ctx_ar", "لا رقمًا — لا يمكن استخدام '" + currentToken.getValue() + "' اسمًا"},
                                  {"ctx_en", "not a number — cannot use '" + currentToken.getValue() + "' as a name"}});
                }
                else if (currentToken.getType() == TT::STRING_LITERAL)
                {
                    // (AR) نص بعد متغير: متغير "اسم" = ...
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME,
                                 {{"what_ar", "متغير"},
                                  {"what_en", "variable"},
                                  {"ctx_ar", "لا نصًا حرفيًا — المعرّف يُكتب بلا علامتي اقتباس"},
                                  {"ctx_en", "not a string literal — identifiers are written without quotes"}});
                }
                else if (currentToken.isKeyword() || currentToken.getType() == TT::LITERAL_TRUE ||
                         currentToken.getType() == TT::LITERAL_FALSE || currentToken.getType() == TT::LITERAL_NULL)
                {
                    // (AR) كلمة محجوزة بعد متغير: متغير إذا = ...
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME,
                                 {{"what_ar", "متغير"},
                                  {"what_en", "variable"},
                                  {"ctx_ar", "لا كلمة محجوزة — '" + currentToken.getValue() + "' لا تصلح اسمًا"},
                                  {"ctx_en", "not a reserved keyword — '" + currentToken.getValue() + "' cannot be a name"}});
                }
                else if (currentToken.getType() == TT::OP_ASSIGN)
                {
                    // (AR) = مباشرة بعد متغير: متغير = ...
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME,
                                 {{"what_ar", "متغير"},
                                  {"what_en", "variable"},
                                  {"ctx_ar", "قبل '=' — التصريح بلا اسم"},
                                  {"ctx_en", "before '=' — the declaration has no name"}});
                }
                else
                {
                    // (AR) فاصلة منقوطة/أقواس/غير ذلك: صيغة تصريح غير صحيحة
                    // (EN) Semicolon/braces/other: invalid declaration form
                    errorCatalog(Errors::ErrorCode::SYN_INVALID_CONSTRUCT_FORM,
                                 {{"construct_ar", "تصريح المتغير"},
                                  {"construct_en", "variable declaration"},
                                  {"form", kw(TT::KEYWORD_VAR) + " اسم = قيمة"}});
                }
                return nullptr;
            }

            // Optional initializer
            // (AR) المُهيّئ الاختياري
            ExprPtr initializer = nullptr;
            if (match(TT::OP_ASSIGN))
            {
                // We have an assignment operator, parse the expression
                // (AR) لدينا عامل إسناد، قم بتحليل التعبير
                initializer = parseExpression();

                // Check if expression parsing was successful
                // (AR) تحقق مما إذا كان تحليل التعبير ناجحاً
                if (!initializer)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "صالحًا في قيمة تهيئة المتغير '" + name.getValue() + "'"}, {"ctx_en", "(valid) in the initializer of variable '" + name.getValue() + "'"}});
                    return nullptr;
                }
            }

            // ═══════════════════════════════════════════════════════════════════════
            // (AR) دعم تعريف متغيرات متعددة في نفس السطر بفاصلة:
            //      متغير اسم = "أحمد" ، عمر = 20 ، وظيفة = "مهندس"
            //      ثابت س = 1 ، ص = 2 ، ع = 3
            // (EN) Support multiple variable declarations in same line with comma:
            //      var name = "Ahmed" , age = 20 , job = "Engineer"
            //      const x = 1 , y = 2 , z = 3
            // ═══════════════════════════════════════════════════════════════════════

            // (AR) حفظ موقع أول متغير / (EN) Save position of first variable
            Lexer::Position firstPos = name.getPosition();

            // (AR) إنشاء أول تصريح متغير / (EN) Create first variable declaration
            auto firstDecl = std::make_unique<VarDeclStmt>(
                name.getValue(),
                varType,
                std::move(initializer),
                pendingConst_,
                firstPos);
            firstDecl->docComment = std::move(docComment);
            firstDecl->isVolatile = declVolatile; // (AR) لصيقة «متطاير» (اللبنة 3.14)

            // (AR) [NS-06 موجة 2] لنوع اختياريّ `T؟`: ابنِ sadType غنيًّا = Optional<T>
            //      (المُنشئ يضع fromValueType(Optional) بلا T داخليّ). هذا يمكّن codegen
            //      المترجم من حفظ النوع الداخليّ (نص لا i64) — انظر buildLocalVariable.
            // (EN) [NS-06 wave 2] For optional `T?`: build a rich sadType = Optional<T>
            //      so the compiler can keep the inner type at codegen.
            if (varType == Types::SadTypeKind::Optional &&
                varInnerKind != Types::SadTypeKind::Unknown)
            {
                firstDecl->sadType = Types::SadTypeRegistry::instance().makeOptional(
                    Types::SadType::fromValueType(varInnerKind));
            }

            // (AR) التحقق من وجود فاصلة (تعريف متغيرات متعددة)
            // (EN) Check for comma (multiple variable declarations)
            if (checkComma())
            {
                StmtList declarations;
                declarations.push_back(std::move(firstDecl));

                while (matchComma())
                {
                    // (AR) تحليل المتغير التالي: اسم [: نوع] [= قيمة]
                    // (EN) Parse next variable: name [: type] [= value]
                    Token nextName(TT::IDENTIFIER, "");
                    Types::SadTypeKind nextType = Types::SadTypeKind::Unknown;

                    if (check(TT::IDENTIFIER))
                    {
                        nextName = peek();
                        advance();
                    }
                    else if (isTypeToken(current_.getType()) && peekNext().getType() != TT::IDENTIFIER)
                    {
                        // (AR) كلمة نوع مستخدمة كاسم متغير / (EN) Type keyword used as variable name
                        nextName = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                        advance();
                    }
                    else
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "متغير"}, {"what_en", "variable"}, {"ctx_ar", "بعد الفاصلة في تصريح المتغيرات المتعددة"}, {"ctx_en", "after the comma in a multi-variable declaration"}});
                        break;
                    }

                    // (AR) تصريح النوع الاختياري: اسم : نوع
                    // (EN) Optional type annotation: name : type
                    // (AR) النوع الداخليّ T لنوع اختياريّ T؟ في هذا المتغيّر (NS-06)
                    // (EN) Inner type T for an optional T? on this variable (NS-06)
                    Types::SadTypeKind nextInnerKind = Types::SadTypeKind::Unknown;
                    if (match(TT::COLON))
                    {
                        nextType = parseType();
                        if (nextType == Types::SadTypeKind::Optional)
                            nextInnerKind = lastOptionalInner_;
                        if (nextType == Types::SadTypeKind::Unknown)
                        {
                            errorCatalog(Errors::ErrorCode::SYN_UNKNOWN_ELEMENT, {{"what_ar", "النوع بعد ':' في تصريح المتغير '" + nextName.getValue() + "'"}, {"what_en", "type after ':' in the declaration of '" + nextName.getValue() + "'"}, {"found", current_.getValue()}, {"allowed", kw(TT::TYPE_INTEGER) + "، " + kw(TT::TYPE_DOUBLE) + "، " + kw(TT::TYPE_STRING) + "، " + kw(TT::TYPE_BOOLEAN) + "، ..."}});
                        }
                    }

                    // (AR) المُهيّئ الاختياري / (EN) Optional initializer
                    ExprPtr nextInit = nullptr;
                    if (match(TT::OP_ASSIGN))
                    {
                        nextInit = parseExpression();
                        if (!nextInit)
                        {
                            errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "صالحًا في قيمة تهيئة المتغير '" + nextName.getValue() + "'"}, {"ctx_en", "(valid) in the initializer of variable '" + nextName.getValue() + "'"}});
                            break;
                        }
                    }

                    auto nextDecl = std::make_unique<VarDeclStmt>(
                        nextName.getValue(),
                        nextType,
                        std::move(nextInit),
                        pendingConst_,
                        nextName.getPosition());
                    nextDecl->isVolatile = declVolatile; // (AR) «متطاير» (اللبنة 3.14)
                    // (AR) [NS-06] سباكة Optional<T> للمتغيّر التالي كما في الأول
                    // (EN) [NS-06] Plumb Optional<T> for the next variable like the first
                    if (nextType == Types::SadTypeKind::Optional &&
                        nextInnerKind != Types::SadTypeKind::Unknown)
                    {
                        nextDecl->sadType = Types::SadTypeRegistry::instance().makeOptional(
                            Types::SadType::fromValueType(nextInnerKind));
                    }
                    declarations.push_back(std::move(nextDecl));
                }

                // (AR) فاصلة منقوطة اختيارية / (EN) Optional semicolon
                matchSemicolon();

                // (AR) إرجاع تصريح متغيرات متعددة (بدون نطاق جديد)
                // (EN) Return multi-variable declaration (no new scope)
                return std::make_unique<MultiVarDeclStmt>(
                    std::move(declarations),
                    firstPos);
            }

            // (AR) فاصلة منقوطة اختيارية (متغير واحد فقط)
            // (EN) Optional semicolon (single variable only)
            matchSemicolon();

            return std::move(firstDecl);
        }

        /**
         * @brief (AR) يحلل تصريح enum: enum اسم { عضو1، عضو2 }.
         *        (EN) Parses enum declaration: enum name { member1, member2 }.
         */
        StmtPtr ParserCore::parseEnumDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            // Expect enum name
            // (AR) توقع اسم Enum
            if (!check(TT::IDENTIFIER))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "التعداد"}, {"what_en", "enum"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_ENUM) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_ENUM) + "'"}});
                return nullptr;
            }
            Token name = peek();
            advance();

            // (AR) يدعم صيغة: تعداد اسم ... نهاية (الأقواس {} مُزالة)
            // (EN) Supports: enum name ... end (braces {} removed)
            if (check(TT::BRACE_LEFT))
            {
                errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", kw(TT::KEYWORD_ENUM) + " { ... }"}, {"new", kw(TT::KEYWORD_END)}, {"example", kw(TT::KEYWORD_ENUM) + " " + name.getValue() + "\n ...\n" + kw(TT::KEYWORD_END)}});
            }

            // Parse enum members
            // (AR) تحليل أعضاء Enum
            std::vector<EnumMember> members;

            // (AR) دالة مساعدة: هل وصلنا نهاية التعداد؟
            auto isEnumEnd = [&]() -> bool
            {
                return check(TT::KEYWORD_END);
            };

            // Check for empty enum
            if (isEnumEnd())
            {
                errorCatalog(Errors::ErrorCode::SYN_EMPTY_CONSTRUCT, {{"construct_ar", "التعداد"}, {"construct_en", "enum"}, {"required_ar", "عضو واحد"}, {"required_en", "one member"}});
                advance(); // consume } or نهاية
                return nullptr;
            }

            // (AR) دالة مساعدة: هل الرمز الحالي يصلح كاسم عضو تعداد؟
            //      في سياق التعداد، نقبل المُعرّفات + الكلمات المحجوزة والأنواع المدمجة
            //      لأنها أسماء أعضاء وليست تعليمات بنيوية
            //      مثال: تعداد حالة { خطأ، نص، خارجي، نجاح } — كلها صالحة
            // (EN) Helper: is current token valid as enum member name?
            //      Inside enum context, we accept identifiers + reserved words + types
            //      since they are member names, not structural instructions
            auto isValidEnumMember = [&]() -> bool
            {
                if (isEnumEnd())
                    return false;
                auto tt = current_.getType();
                // (AR) المُعرّفات العادية / (EN) Regular identifiers
                if (tt == TT::IDENTIFIER)
                    return true;
                // (AR) القيم الحرفية كأسماء / (EN) Literals as names
                if (tt == TT::LITERAL_TRUE || tt == TT::LITERAL_FALSE || tt == TT::LITERAL_NULL)
                    return true;
                // (AR) أنواع مدمجة (TYPE_*) / (EN) Built-in types
                if (tt == TT::TYPE_INTEGER || tt == TT::TYPE_DOUBLE || tt == TT::TYPE_STRING ||
                    tt == TT::TYPE_BOOLEAN || tt == TT::TYPE_VOID || tt == TT::TYPE_NULL ||
                    tt == TT::TYPE_ARRAY || tt == TT::TYPE_MAP)
                    return true;
                // (AR) كلمات محجوزة مسموح بها كأعضاء تعداد (ليست بنيوية)
                // (EN) Allowed reserved words as enum members (non-structural)
                switch (tt)
                {
                case TT::KEYWORD_EXTERN:      // خارجي
                case TT::KEYWORD_PUBLIC:      // عام
                case TT::KEYWORD_PRIVATE:     // خاص
                case TT::KEYWORD_PROTECTED:   // محمي
                case TT::KEYWORD_ABSTRACT:    // مجرد
                case TT::KEYWORD_STATIC:      // ساكن
                case TT::KEYWORD_NEW:         // جديد
                case TT::KEYWORD_SUPER:       // الأساس
                case TT::KEYWORD_FROM:        // من
                case TT::KEYWORD_IN:          // في
                case TT::KEYWORD_AS:          // كـ
                case TT::KEYWORD_DEFAULT:     // افتراضي
                case TT::KEYWORD_RETURN:      // ارجع
                case TT::KEYWORD_THROW:       // ارمي
                case TT::KEYWORD_CATCH:       // امسك
                case TT::KEYWORD_IMPORT:      // استورد
                case TT::KEYWORD_EXPORT:      // صدّر
                case TT::KEYWORD_CONSTRUCTOR: // باني
                case TT::KEYWORD_BREAK:       // توقف
                case TT::KEYWORD_CONTINUE:    // استمر
                case TT::OP_AND:              // و
                case TT::OP_OR:               // أو
                case TT::OP_NOT:              // ليس
                    return true;
                default:
                    return false;
                }
            };

            while (!isEnumEnd() && !isAtEnd())
            {
                // (AR) قبول المُعرّفات والكلمات المحجوزة كأعضاء تعداد
                //      مثال: خطأ، صحيح، نص، خارجي — كلها أسماء أعضاء صالحة
                // (EN) Accept identifiers AND reserved keywords as enum member names
                //      Example: خطأ, صحيح, نص, خارجي — all valid member names
                if (!isValidEnumMember())
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "عضو التعداد"}, {"what_en", "enum member"}, {"ctx_ar", "داخل التعداد"}, {"ctx_en", "inside the enum"}});
                    return nullptr;
                }
                Token memberName = peek();
                advance();

                ExprPtr value = nullptr;
                std::vector<std::string> adtFields;     // (AR) أسماء حقول الحمولة (ADT) / (EN) ADT payload field names
                std::vector<std::string> adtFieldTypes; // (AR) أنواع حقول الحمولة الموازية / (EN) parallel payload field types

                // (AR) تحليل حقلٍ واحدٍ من الحمولة — نوعٌ اختياريّ (النوع قبل الاسم: قاعدة ص)
                //      يليه اسم الحقل. يدعم: «رقم قيمة» (نوع مدمج)، «عقدة يسار» (نوع صنف)،
                //      و«نصف_القطر» (اسم فقط — غير مُصنَّف، توافق خلفيّ).
                // (EN) Parse one payload field — optional type (type-before-name: Sad rule)
                //      then field name. Supports: «رقم قيمة» (built-in), «عقدة يسار» (class
                //      type), and «نصف_القطر» (name only — untyped, backward compatible).
                auto parseAdtField = [&]() -> bool
                {
                    std::string fieldType;

                    // (AR) نوعٌ مدمجٌ متبوعٌ باسم؟ (مثل «رقم قيمة») / (EN) built-in type then a name?
                    bool builtinTypeThenName = isTypeToken(current_.getType()) &&
                                               (peekNext().getType() == TT::IDENTIFIER ||
                                                isTokenUsableAsName(peekNext().getType()));
                    // (AR) اسمُ صنفٍ متبوعٌ باسمٍ؟ (مثل «عقدة يسار») / (EN) class-type name then a name?
                    bool classTypeThenName = (check(TT::IDENTIFIER) || isTokenUsableAsName(current_.getType())) &&
                                             (peekNext().getType() == TT::IDENTIFIER ||
                                              isTokenUsableAsName(peekNext().getType()));

                    if (builtinTypeThenName)
                    {
                        fieldType = current_.getValue();
                        // (AR) استهلاك النوع المدمج. حدّ أ-م١: بوّابة peekNext تشترط اسمًا بعد النوع،
                        //      فالأنواع العامّة/المصفوفات في الحمولة (قائمة<رقم> قيمة / رقم[] قيمة) غير مدعومة بعد.
                        // (EN) consume built-in type. أ-م١ limit: the peekNext gate requires a name after the
                        //      type, so generic/array payload types are not yet supported.
                        parseType();
                    }
                    else if (classTypeThenName)
                    {
                        fieldType = current_.getValue();
                        advance(); // (AR) استهلاك اسم نوع الصنف / (EN) consume class-type name
                    }

                    // (AR) اسم الحقل / (EN) field name
                    if (!check(TT::IDENTIFIER) && !isValidEnumMember())
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "الحقل"}, {"what_en", "field"}, {"ctx_ar", "داخل حمولة التعداد الجبري — مثال: عدد(رقم قيمة)"}, {"ctx_en", "inside the ADT enum payload — e.g., عدد(رقم قيمة)"}});
                        return false;
                    }
                    adtFields.push_back(peek().getValue());
                    adtFieldTypes.push_back(fieldType);
                    advance();
                    return true;
                };

                // (AR) كشف أقواس الحمولة للتعدادات الجبرية (ADT)
                //      مثال: عدد(رقم قيمة) أو مستطيل(عرض، ارتفاع) أو دائرة(نصف_القطر)
                // (EN) Detect payload parentheses for Algebraic Data Types (ADT)
                //      Example: عدد(رقم قيمة) or Rectangle(width, height) or Circle(radius)
                if (check(TT::PAREN_LEFT))
                {
                    advance(); // (AR) استهلاك '(' / (EN) consume '('

                    // (AR) تحليل حقول الحمولة المفصولة بفواصل
                    // (EN) Parse comma-separated payload fields
                    if (!check(TT::PAREN_RIGHT))
                    {
                        // (AR) الحقل الأول / (EN) First field
                        if (!parseAdtField())
                        {
                            return nullptr;
                        }

                        // (AR) الحقول التالية مفصولة بفواصل
                        // (EN) Remaining fields separated by commas
                        while (checkComma())
                        {
                            advance(); // (AR) استهلاك الفاصلة / (EN) consume comma
                            if (!parseAdtField())
                            {
                                return nullptr;
                            }
                        }
                    }

                    // (AR) استهلاك ')' / (EN) consume ')'
                    if (!check(TT::PAREN_RIGHT))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ")"}, {"ctx_ar", "لإغلاق حمولة التعداد الجبري"}, {"ctx_en", "to close the ADT enum payload"}});
                        return nullptr;
                    }
                    advance();
                }
                else if (match(TT::OP_ASSIGN))
                {
                    // Explicit value
                    // (AR) قيمة صريحة
                    value = parseExpression();
                    if (!value)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "صالحًا في قيمة عضو التعداد"}, {"ctx_en", "(valid) in the enum member value"}});
                        return nullptr;
                    }
                }

                // (AR) إنشاء عضو التعداد — جبري (ADT) أو بسيط
                // (EN) Create enum member — ADT or simple
                if (!adtFields.empty())
                {
                    members.push_back(EnumMember(memberName.getValue(), std::move(adtFields), std::move(adtFieldTypes)));
                }
                else
                {
                    members.push_back(EnumMember(memberName.getValue(), std::move(value)));
                }

                // (AR) الفاصلة اختيارية بين الأعضاء (الأسطر الجديدة تفصل تلقائياً)
                //      دعم كلتا الفاصلتين: ، (عربية) و , (لاتينية)
                // (EN) Commas are optional between members (newlines separate automatically)
                //      Support both Arabic comma (،) and Latin comma (,)
                if (!isEnumEnd())
                {
                    if (checkComma())
                    {
                        advance(); // consume optional comma
                    }
                }
            }

            if (!isEnumEnd())
            {
                errorCatalog(Errors::ErrorCode::SYN_UNCLOSED_CONSTRUCT, {{"construct_ar", "كتلة التعداد"}, {"construct_en", "enum block"}, {"closer", kw(TT::KEYWORD_END)}});
                return nullptr;
            }

            consume(TT::KEYWORD_END, "");

            // Create enum declaration node
            // (AR) إنشاء عقدة تصريح Enum
            auto enumDecl = std::make_unique<EnumDecl>(
                name.getValue(),
                std::move(members),
                false,
                name.getPosition());
            enumDecl->docComment = std::move(docComment);
            return enumDecl;
        }

        /**
         * @brief (AR) يحلل تصريح بنية: بنية اسم ... نهاية
         *        (EN) Parses struct declaration: struct name ... end
         */
        StmtPtr ParserCore::parseStructDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            // (AR) اسم البنية / (EN) Struct name
            if (!check(TT::IDENTIFIER))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "البنية"}, {"what_en", "struct"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_STRUCT) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_STRUCT) + "'"}});
                return nullptr;
            }
            Token name = peek();
            advance();

            // (AR) تحليل معاملات العمر الاختيارية: بنية اسم<'أ، 'ب>
            // (EN) Parse optional lifetime parameters: struct name<'a, 'b>
            auto lifetimeParams = parseLifetimeParams();

            // (AR) يدعم صيغة: بنية اسم ... نهاية (الأقواس {} مُزالة)
            // (EN) Supports: struct name ... end (braces {} removed)
            if (check(TT::BRACE_LEFT))
            {
                errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", kw(TT::KEYWORD_STRUCT) + " { ... }"}, {"new", kw(TT::KEYWORD_END)}, {"example", kw(TT::KEYWORD_STRUCT) + " " + name.getValue() + "\n ...\n" + kw(TT::KEYWORD_END)}});
            }

            std::vector<StructField> fields;
            StmtList methods;

            auto isStructEnd = [&]() -> bool
            {
                return check(TT::KEYWORD_END);
            };

            while (!isStructEnd() && !isAtEnd())
            {
                // (AR) تخطي محددات الوصول إن وجدت (عام/خاص/محمي)
                // (EN) Skip access modifiers if present (public/private/protected)
                // ═══════════════════════════════════════════════════════════════════
                // (AR) قاعدة الصفة بعد الموصوف — المعدلات تأتي بعد الكلمة المفتاحية
                //      الصيغة الجديدة: باني عام (...) / دالة خاص طريقة()
                //      بدلاً من القديمة: عام باني (...) / خاص دالة طريقة()
                // (EN) Arabic adjective-after-noun rule for struct members
                // ═══════════════════════════════════════════════════════════════════
                AST::AccessModifier memberAccess = AST::AccessModifier::PUBLIC;

                // (AR) رسالة خطأ للصيغة القديمة: معدل قبل الكلمة المفتاحية
                if (check(TT::KEYWORD_PUBLIC) || check(TT::KEYWORD_PRIVATE) || check(TT::KEYWORD_PROTECTED))
                {
                    std::string modName = current_.getValue();
                    auto nextTT = peekNext().getType();
                    if (nextTT == TT::KEYWORD_CONSTRUCTOR || nextTT == TT::KEYWORD_FUNCTION)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_ADJECTIVE_ORDER, {{"wrong", modName + " " + kw(TT::KEYWORD_CONSTRUCTOR) + "/" + kw(TT::KEYWORD_FUNCTION) + " ..."}, {"right", kw(TT::KEYWORD_CONSTRUCTOR) + "/" + kw(TT::KEYWORD_FUNCTION) + " " + modName + " ..."}});
                    }
                    // (AR) ابتلاع المعدل للاسترداد
                    if (check(TT::KEYWORD_PUBLIC))
                    {
                        memberAccess = AST::AccessModifier::PUBLIC;
                        advance();
                    }
                    else if (check(TT::KEYWORD_PRIVATE))
                    {
                        memberAccess = AST::AccessModifier::PRIVATE;
                        advance();
                    }
                    else if (check(TT::KEYWORD_PROTECTED))
                    {
                        memberAccess = AST::AccessModifier::PROTECTED;
                        advance();
                    }
                }

                // (AR) التحقق من باني داخل البنية / (EN) Check for constructor inside struct
                if (check(TT::KEYWORD_CONSTRUCTOR))
                {
                    advance(); // (AR) استهلاك 'باني' / (EN) consume 'باني'
                    // (AR) قراءة المعدلات بعد 'باني' (الصفة بعد الموصوف)
                    bool _s = false, _v = false, _a = false;
                    memberAccess = parseModifiers(_s, _v, _a);
                    auto constructor = parseConstructorDeclaration(name.getValue(), memberAccess);
                    if (constructor)
                    {
                        methods.push_back(std::move(constructor));
                    }
                    continue;
                }

                // (AR) التحقق من دالة داخل البنية / (EN) Check for method inside struct
                if (check(TT::KEYWORD_FUNCTION))
                {
                    advance(); // consume 'دالة'
                    // (AR) قراءة المعدلات بعد 'دالة' (الصفة بعد الموصوف)
                    bool isStatic = false, isVirtual = false, isAbstract = false;
                    memberAccess = parseModifiers(isStatic, isVirtual, isAbstract);
                    auto method = parseMethodDeclaration(memberAccess, isStatic, isVirtual, isAbstract);
                    if (method)
                    {
                        methods.push_back(std::move(method));
                    }
                    continue;
                }

                // (AR) حقل بنية: [متغير|ثابت] [عام|خاص|محمي] [نوع] اسم [= قيمة]
                // (EN) Struct field: [var|const] [public|private|protected] [type] name [= value]
                Types::SadTypeKind fieldType = Types::SadTypeKind::Unknown;
                bool fieldIsMutable = true;

                // ================================================================
                // (AR) [Fix #54] دعم صيغة: متغير عام س = 0 / ثابت خاص ص = 0
                //      المفسر يقبل هذه الصيغة وهي الأكثر شيوعاً في لغة ص.
                //      بدون هذا: بنية نقطة; متغير عام س = 0; نهاية → خطأ نحوي
                // (EN) [Fix #54] Support: متغير عام س = 0 / ثابت خاص ص = 0
                //      The interpreter accepts this syntax and it's the most common in Sad.
                // ================================================================
                if (check(TT::KEYWORD_VAR) || check(TT::KEYWORD_CONST))
                {
                    fieldIsMutable = check(TT::KEYWORD_VAR);
                    advance(); // (AR) استهلاك متغير/ثابت

                    // (AR) قراءة معدل الوصول الاختياري بعد متغير/ثابت
                    // (EN) Read optional access modifier after var/const
                    if (check(TT::KEYWORD_PUBLIC))
                    {
                        memberAccess = AST::AccessModifier::PUBLIC;
                        advance();
                    }
                    else if (check(TT::KEYWORD_PRIVATE))
                    {
                        memberAccess = AST::AccessModifier::PRIVATE;
                        advance();
                    }
                    else if (check(TT::KEYWORD_PROTECTED))
                    {
                        memberAccess = AST::AccessModifier::PROTECTED;
                        advance();
                    }

                    // (AR) تخطي كلمة ساكن إن وجدت
                    // (EN) Skip static keyword if present
                    if (check(TT::KEYWORD_STATIC))
                    {
                        advance();
                    }
                }

                // (AR) إذا كان الرمز الحالي نوعاً
                if (isTypeToken(current_.getType()))
                {
                    fieldType = parseType();
                }

                if (!check(TT::IDENTIFIER))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "الحقل"}, {"what_en", "field"}, {"ctx_ar", "داخل البنية"}, {"ctx_en", "inside the struct"}});
                    advance();
                    continue;
                }

                Token fieldName = peek();
                advance();

                ExprPtr defaultValue = nullptr;
                if (match(TT::OP_ASSIGN))
                {
                    defaultValue = parseExpression();
                }

                // (AR) تخطي الفاصلة أو الفاصلة المنقوطة الاختيارية
                matchComma(); // also accept comma as separator
                matchSemicolon();

                fields.push_back(StructField(fieldName.getValue(), fieldType, std::move(defaultValue), fieldIsMutable));
            }

            if (!isStructEnd())
            {
                errorCatalog(Errors::ErrorCode::SYN_UNCLOSED_CONSTRUCT, {{"construct_ar", "كتلة البنية"}, {"construct_en", "struct block"}, {"closer", kw(TT::KEYWORD_END)}});
                return nullptr;
            }

            consume(TT::KEYWORD_END, "");

            auto structDecl = std::make_unique<StructDecl>(
                name.getValue(),
                std::move(fields),
                std::vector<TypeParameter>{},
                false,
                false,
                name.getPosition());
            structDecl->lifetimeParams = std::move(lifetimeParams);
            structDecl->docComment = std::move(docComment);
            // (AR) إصلاح حرج: نقل الدوال والباني إلى عقدة البنية
            // (EN) Critical fix: transfer methods and constructor to struct node
            structDecl->methods = std::move(methods);
            return structDecl;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // (AR) تحليل تصريح اختبار: اختبر("اسم") ... نهاية / اختبر "اسم" ... نهاية
        // (EN) Parse test declaration: test("name") ... end / test "name" ... end
        // ═══════════════════════════════════════════════════════════════════════════
        StmtPtr ParserCore::parseTestDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            using TT = Lexer::TokenType;
            auto startPos = previous_.getPosition();

            // (AR) اسم الاختبار: نص بين أقواس — الأقواس إلزامية
            // (EN) Test name: string in parens — parens required
            std::string testName;

            if (!match(TT::PAREN_LEFT))
            {
                error("(AR) خطأ نحوي: توقع '(' بعد 'اختبر'.\n"
                      "💡 مثال: اختبر(\"اسم الاختبار\")\n"
                      "(EN) Syntax error: expected '(' after 'test'.\n"
                      "💡 Example: test(\"test name\")");
                // (AR) محاولة استرداد: إذا وجدنا نصاً مباشرة نحلله
                if (check(TT::STRING_LITERAL))
                {
                    testName = current_.getValue();
                    advance();
                }
                else
                {
                    return nullptr;
                }
            }
            else
            {
                if (check(TT::STRING_LITERAL))
                {
                    testName = current_.getValue();
                    auto strToken = current_;
                    advance();

                    // (AR) تمييز بين تصريح اختبار واستدعاء دالة:
                    //      اختبر("اسم") ... نهاية  → تصريح اختبار (يتبعه ')')
                    //      اختبر("اسم"، قيمة، ...) → استدعاء دالة (يتبعه ',' أو '،')
                    // (EN) Distinguish test declaration from function call:
                    //      test("name") ... end  → test declaration (followed by ')')
                    //      test("name", val, ...) → function call (followed by ',' or '،')
                    if (check(TT::COMMA) || check(TT::ARABIC_COMMA))
                    {
                        // (AR) هذا استدعاء دالة وليس تصريح اختبار
                        // (EN) This is a function call, not a test declaration
                        advance(); // consume comma

                        // (AR) بناء قائمة الوسائط: الوسيط الأول هو النص الذي قرأناه
                        AST::ExprList args;
                        args.push_back(std::make_unique<AST::LiteralExpr>(strToken));

                        // (AR) قراءة باقي الوسائط
                        // (EN) Read remaining arguments
                        do
                        {
                            args.push_back(parseExpression());
                        } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));

                        if (!match(TT::PAREN_RIGHT))
                        {
                            error("(AR) خطأ نحوي: توقع ')' بعد وسائط الدالة. (EN) Expected ')' after function arguments.");
                        }

                        // (AR) بناء CallExpr مع اسم الدالة "اختبر"
                        auto callee = std::make_unique<AST::VariableExpr>(
                            "\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xA8\xD8\xB1", startPos); // "اختبر"
                        auto callExpr = std::make_unique<AST::CallExpr>(
                            std::move(callee), std::move(args), startPos);

                        return std::make_unique<AST::ExprStmt>(std::move(callExpr));
                    }
                }
                else
                {
                    error("(AR) خطأ نحوي: توقع اسم الاختبار (نص). (EN) Syntax error: expected test name (string).");
                    return nullptr;
                }

                if (!match(TT::PAREN_RIGHT))
                {
                    error("(AR) خطأ نحوي: توقع ')' بعد اسم الاختبار. (EN) Syntax error: expected ')' after test name.");
                }
            }

            // (AR) جسم الاختبار حتى 'نهاية'
            // (EN) Test body until 'end'
            // (AR) تحليل إعدادات اختبار الخصائص (اختياري)
            // (EN) Parse optional property test config
            // الصيغة: اختبر("اسم") تكرارات 100 بذرة 42 حد_أقصى 50
            bool isPropertyTest = false;
            int iterations = 0;
            int seed = -1;
            int maxSize = 100;

            // (AR) تحقق من كلمات سياقية لإعدادات الخصائص
            while (check(TT::IDENTIFIER) && !check(TT::KEYWORD_END))
            {
                std::string kw = current_.getValue();
                // تكرارات
                if (kw == "\xD8\xAA\xD9\x83\xD8\xB1\xD8\xA7\xD8\xB1\xD8\xA7\xD8\xAA" || kw == "iterations")
                {
                    advance();
                    if (check(TT::NUMBER_INTEGER))
                    {
                        iterations = std::stoi(current_.getValue());
                        isPropertyTest = true;
                        advance();
                    }
                    else
                    {
                        error("(AR) توقعت عدداً بعد 'تكرارات'. (EN) Expected number after 'iterations'.");
                    }
                }
                // بذرة
                else if (kw == "\xD8\xA8\xD8\xB0\xD8\xB1\xD8\xA9" || kw == "seed")
                {
                    advance();
                    if (check(TT::NUMBER_INTEGER))
                    {
                        seed = std::stoi(current_.getValue());
                        advance();
                    }
                    else
                    {
                        error("(AR) توقعت عدداً بعد 'بذرة'. (EN) Expected number after 'seed'.");
                    }
                }
                // حد_أقصى
                else if (kw == "\xD8\xAD\xD8\xAF_\xD8\xA3\xD9\x82\xD8\xB5\xD9\x89" || kw == "max_size")
                {
                    advance();
                    if (check(TT::NUMBER_INTEGER))
                    {
                        maxSize = std::stoi(current_.getValue());
                        advance();
                    }
                    else
                    {
                        error("(AR) توقعت عدداً بعد 'حد_أقصى'. (EN) Expected number after 'max_size'.");
                    }
                }
                else
                {
                    break; // (AR) ليست إعداداً — بداية جسم الاختبار
                }
            }

            AST::StmtList bodyStmts;
            while (!check(TT::KEYWORD_END) && !isAtEnd())
            {
                auto stmt = parseDeclaration();
                if (stmt)
                {
                    bodyStmts.push_back(std::move(stmt));
                }
            }

            if (!match(TT::KEYWORD_END))
            {
                error("(AR) خطأ نحوي: توقع 'نهاية' لإنهاء الاختبار. (EN) Syntax error: expected 'end' to close test.");
            }

            auto body = std::make_unique<AST::BlockStmt>(std::move(bodyStmts), startPos);
            auto testDecl = std::make_unique<AST::TestDecl>(testName, std::move(body), false, startPos);
            testDecl->docComment = std::move(docComment);

            // (AR) تعيين إعدادات اختبار الخصائص
            if (isPropertyTest)
            {
                testDecl->isPropertyTest = true;
                testDecl->iterations = iterations;
                testDecl->seed = seed;
                testDecl->maxSize = maxSize;
            }

            return testDecl;
        }

        /**
         * @brief (AR) يحلل جملة تصدير: صدّر identifier;
         *        (EN) Parses export statement: export identifier;
         */
        StmtPtr ParserCore::parseExportStmt()
        {
            // Parse the declaration to be exported
            // (AR) تحليل التصريح المُصدّر
            auto declaration = parseDeclaration();
            if (!declaration)
            {
                errorCatalog(Errors::ErrorCode::SYN_INVALID_CONSTRUCT_FORM, {{"construct_ar", "التصريح المُصدَّر"}, {"construct_en", "exported declaration"}, {"form", kw(TT::KEYWORD_EXPORT) + " " + kw(TT::KEYWORD_FUNCTION) + " ... | " + kw(TT::KEYWORD_EXPORT) + " " + kw(TT::KEYWORD_CLASS) + " ..."}});
                return nullptr;
            }

            // Create export statement node
            // (AR) إنشاء عقدة جملة التصدير
            return std::make_unique<ExportStmt>(
                std::move(declaration),
                previous().getPosition());
        }

        // ======================================================================
        // (AR) تحليل تصريح واجهة/سمة / (EN) Parse Trait/Interface Declaration
        // ======================================================================
        // الصيغة: سمة اسم_السمة [يرث سمة_أخرى]
        //              دالة اسم_دالة(معاملات) [نوع_إرجاع]
        //              [    جسم_افتراضي    ]
        //              نهاية
        //          نهاية
        // ======================================================================
        StmtPtr ParserCore::parseTraitDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            // (AR) اسم الواجهة
            Token nameToken = consume(TT::IDENTIFIER,
                                      "(AR) توقع اسم الواجهة/السمة بعد 'سمة'. (EN) Expected trait name after 'trait'.");
            std::string traitName = nameToken.getValue();

            // (AR) وراثة واجهات أخرى (اختياري) — 'يرث' فقط
            std::vector<std::string> superTraits;
            if (match(TT::KEYWORD_INHERITS))
            {
                do
                {
                    Token superToken = consume(TT::IDENTIFIER,
                                               "(AR) توقع اسم الواجهة الأساسية. (EN) Expected parent trait name.");
                    superTraits.push_back(superToken.getValue());
                } while (matchComma());
            }

            // (AR) تحليل دوال الواجهة
            std::vector<AST::TraitMethod> methods;
            while (!check(TT::KEYWORD_END) && !isAtEnd())
            {
                // (AR) تخطي الفواصل المنقوطة
                skipSemicolons();
                if (check(TT::KEYWORD_END))
                    break;

                // (AR) توقع 'دالة'
                if (match(TT::KEYWORD_FUNCTION))
                {
                    // (AR) استهلاك 'مجرد' الاختياري بعد 'دالة' في السمة
                    //      يدعم الصيغة: دالة مجرد اسم() — isAbstract للتوثيق فقط
                    // (EN) Optionally consume 'مجرد' after 'دالة' in trait body
                    //      Supports: دالة مجرد name() — isAbstract for documentation only
                    bool methodIsAbstract = false;
                    if (match(TT::KEYWORD_ABSTRACT))
                    {
                        methodIsAbstract = true;
                    }
                    (void)methodIsAbstract; // (AR) جميع دوال السمة مجردة ضمنياً إن لم يكن لها جسم
                    // (AR) نوع الإرجاع (اختياري قبل الاسم)
                    Types::SadTypeKind returnType = Types::SadTypeKind::Void;
                    std::string returnTypeName; // (AR) [Phase 5e] لأنواع الأصناف المُعرَّفة من المستخدم

                    // (AR) التحقق من نوع الإرجاع (دعم الأنواع كمُعرّفات مدمجة)
                    if (isTypeToken(current_.getType()))
                    {
                        returnType = parseType();
                    }
                    // (AR) [Phase 5e] نوع إرجاع من صنف مُعرَّف من المستخدم: "نقطة اسم_الدالة(...)"
                    // (EN) [Phase 5e] User-defined class return type: "نقطة methodName(...)"
                    else if (check(TT::IDENTIFIER) && peekNext().getType() == TT::IDENTIFIER)
                    {
                        returnTypeName = current_.getValue();
                        returnType = Types::SadTypeKind::Class;
                        advance(); // (AR) استهلاك اسم الصنف / (EN) consume class name
                    }

                    // (AR) اسم الدالة
                    Token methodName = consume(TT::IDENTIFIER,
                                               "(AR) توقع اسم الدالة في السمة. (EN) Expected method name in trait.");

                    // (AR) المعاملات — نستخدم parseTypedParameterList() لدعم الأنواع المدمجة كمُعرّفات
                    // (EN) Parameters — use parseTypedParameterList() to support built-in type identifiers
                    consume(TT::PAREN_LEFT, "(AR) توقع '('. (EN) Expected '('.");
                    auto params = parseTypedParameterList();
                    consume(TT::PAREN_RIGHT, "(AR) توقع ')'. (EN) Expected ')'.");

                    // (AR) جسم افتراضي (اختياري) — إذا السطر التالي ليس 'دالة' أو 'نهاية'
                    // (EN) Optional default body — if next line isn't 'دالة' or 'نهاية'
                    StmtPtr defaultImpl = nullptr;
                    skipSemicolons();
                    if (!check(TT::KEYWORD_FUNCTION) && !check(TT::KEYWORD_END) && !isAtEnd())
                    {
                        // (AR) هناك جسم افتراضي — نحلله كعبارات حتى 'نهاية'
                        // (EN) There's a default body — parse statements until 'نهاية'
                        std::vector<StmtPtr> bodyStmts;
                        while (!check(TT::KEYWORD_END) && !isAtEnd())
                        {
                            auto stmt = parseStatement();
                            if (stmt)
                                bodyStmts.push_back(std::move(stmt));
                        }
                        consume(TT::KEYWORD_END,
                                "(AR) توقع 'نهاية' بعد جسم الدالة الافتراضية في السمة. (EN) Expected 'end' after default method body in trait.");
                        if (!bodyStmts.empty())
                        {
                            auto block = std::make_unique<AST::BlockStmt>(std::move(bodyStmts), Position());
                            defaultImpl = std::move(block);
                        }
                    }

                    methods.emplace_back(methodName.getValue(), std::move(params), returnType, std::move(defaultImpl));
                    methods.back().returnTypeName = returnTypeName; // (AR) [Phase 5e] حفظ اسم الصنف المُرجَع
                }
                else
                {
                    // (AR) تخطي عنصر غير معروف
                    advance();
                }
            }

            consume(TT::KEYWORD_END,
                    "(AR) توقع 'نهاية' بعد جسم السمة. (EN) Expected 'end' after trait body.");

            auto traitDecl = std::make_unique<AST::TraitDecl>(traitName, std::move(methods),
                                                              std::vector<AST::TypeParameter>{}, std::move(superTraits), false, nameToken.getPosition());
            traitDecl->docComment = std::move(docComment);
            return traitDecl;
        }

        // ======================================================================
        // (AR) تحليل كتلة تنفيذ (impl) / (EN) Parse Impl Block
        // ======================================================================
        // الصيغة: نفّذ اسم_السمة لـ اسم_الصنف
        //              دالة اسم_دالة(معاملات)
        //                  جسم
        //              نهاية
        //          نهاية
        // ======================================================================
        StmtPtr ParserCore::parseImplDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            // (AR) يمكن أن يكون: نفّذ سمة لـ صنف  أو  نفّذ صنف
            Token firstToken = consume(TT::IDENTIFIER,
                                       "(AR) توقع اسم السمة أو الصنف بعد 'نفّذ'. (EN) Expected trait/class name after 'impl'.");

            std::string traitName;
            std::string targetType;

            // (AR) فحص كلمة 'لـ' (for)
            if (check(TT::IDENTIFIER) && (previous().getValue() != ""))
            {
                // (AR) نبحث عن 'لـ' أو 'ل'
                Token nextTok = peek();
                if (nextTok.getValue() == "\xd9\x84\xd9\x80" || nextTok.getValue() == "\xd9\x84" ||
                    nextTok.getValue() == "for")
                {
                    traitName = firstToken.getValue();
                    advance(); // skip لـ/ل/for
                    Token target = consume(TT::IDENTIFIER,
                                           "(AR) توقع اسم الصنف بعد 'لـ'. (EN) Expected class name after 'for'.");
                    targetType = target.getValue();
                }
                else
                {
                    // (AR) نفّذ صنف (بدون سمة)
                    targetType = firstToken.getValue();
                }
            }
            else
            {
                targetType = firstToken.getValue();
            }

            // (AR) تحليل الدوال
            StmtList methods;
            while (!check(TT::KEYWORD_END) && !isAtEnd())
            {
                skipSemicolons();
                if (check(TT::KEYWORD_END))
                    break;

                if (match(TT::KEYWORD_FUNCTION))
                {
                    // (AR) match() يستهلك 'دالة' — parseFunctionDecl يتوقع أنه مستهلك بالفعل
                    // (EN) match() consumes 'دالة' — parseFunctionDecl expects it already consumed
                    auto fn = parseFunctionDecl();
                    // (AR) RFC 0034: 'دالة خارجية' صارت تُحلَّل داخل parseFunctionDecl،
                    //      فقد تصل هنا عبر كتلة تنفيذ — التصريح الخارجيّ بلا جسم لا يصلح
                    //      طريقةً (كان يُقبل صامتًا ويُستدعى كلا-شيء). نرفضه صراحةً.
                    // (EN) RFC 0034: 'دالة خارجية' now parses inside parseFunctionDecl and
                    //      can reach impl blocks — a body-less extern is not a method
                    //      (was silently accepted and called as a no-op). Reject explicitly.
                    auto *fnDecl = dynamic_cast<AST::FunctionDecl *>(fn.get());
                    if (fnDecl && fnDecl->isExtern)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_DECL_NOT_ALLOWED_HERE, {{"decl_ar", "التصريح الخارجيّ '" + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + "' (بلا جسم)"}, {"decl_en", "the body-less extern declaration '" + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + "'"}, {"where_ar", "كتلة تنفيذ"}, {"where_en", "an impl block"}, {"example", kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " printf(" + kw(TT::TYPE_STRING) + ")"}});
                    }
                    else
                    {
                        methods.push_back(std::move(fn));
                    }
                }
                else
                {
                    advance();
                }
            }

            consume(TT::KEYWORD_END,
                    "(AR) توقع 'نهاية' بعد كتلة التنفيذ. (EN) Expected 'end' after impl block.");

            auto implDecl = std::make_unique<AST::ImplDecl>(traitName, targetType, std::move(methods),
                                                            std::vector<AST::TypeParameter>{}, firstToken.getPosition());
            implDecl->docComment = std::move(docComment);
            return implDecl;
        }

        // ======================================================================
        // (AR) تحليل كتلة الامتداد / (EN) Parse Extension Block
        // ======================================================================
        //
        // (AR) الصيغة النحوية:
        //      امتداد اسم_النوع
        //          دالة اسم_الدالة(هذا، ...)
        //              ...
        //          نهاية
        //      نهاية
        //
        // (EN) Syntax:
        //      extension TypeName
        //          function methodName(this, ...)
        //              ...
        //          end
        //      end
        //
        // (AR) طرق الامتداد تعيد استخدام نفس آلية ImplDecl (إضافة دوال لصنف)
        //      لكن بدون الحاجة لتحديد سمة. هي اختصار لـ: نفّذ نوع { ... }
        // (EN) Extension methods reuse the same mechanism as ImplDecl (adding methods to class)
        //      but without requiring a trait. Shorthand for: impl Type { ... }
        // ======================================================================

        StmtPtr ParserCore::parseExtensionDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            auto pos = previous().getPosition();

            // (AR) تحليل اسم النوع المستهدف
            // (EN) Parse target type name
            Token targetToken = consume(TT::IDENTIFIER,
                                        "(AR) توقع اسم النوع بعد 'امتداد'.\n"
                                        "مثال: امتداد نص ... نهاية\n"
                                        "(EN) Expected type name after 'extension'.\n"
                                        "Example: extension String ... end");

            std::string targetType = targetToken.getValue();

            // (AR) تحليل الدوال داخل كتلة الامتداد
            // (EN) Parse methods inside extension block
            StmtList methods;
            while (!check(TT::KEYWORD_END) && !isAtEnd())
            {
                skipSemicolons();
                if (check(TT::KEYWORD_END))
                    break;

                if (match(TT::KEYWORD_FUNCTION))
                {
                    // (AR) match() يستهلك 'دالة' — parseFunctionDecl يتوقع أنه مستهلك بالفعل
                    // (EN) match() consumes 'دالة' — parseFunctionDecl expects it already consumed
                    auto fn = parseFunctionDecl();
                    // (AR) RFC 0034: التصريح الخارجيّ بلا جسم لا يصلح طريقة امتداد — رفض صريح.
                    // (EN) RFC 0034: a body-less extern is not an extension method — reject.
                    auto *fnDecl = dynamic_cast<AST::FunctionDecl *>(fn.get());
                    if (fnDecl && fnDecl->isExtern)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_DECL_NOT_ALLOWED_HERE, {{"decl_ar", "التصريح الخارجيّ '" + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + "' (بلا جسم)"}, {"decl_en", "the body-less extern declaration '" + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + "'"}, {"where_ar", "كتلة امتداد"}, {"where_en", "an extension block"}, {"example", kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " printf(" + kw(TT::TYPE_STRING) + ")"}});
                    }
                    else
                    {
                        methods.push_back(std::move(fn));
                    }
                }
                else
                {
                    // (AR) تخطي الرموز غير المتوقعة مع رسالة خطأ
                    // (EN) Skip unexpected tokens with error message
                    error("(AR) توقع 'دالة' داخل كتلة الامتداد. (EN) Expected 'function' inside extension block.");
                    advance();
                }
            }

            consume(TT::KEYWORD_END,
                    "(AR) توقع 'نهاية' بعد كتلة الامتداد.\n"
                    "مثال:\n    امتداد نص\n        دالة حروف_كبيرة(هذا) ...\n        نهاية\n    نهاية\n"
                    "(EN) Expected 'end' after extension block.");

            auto extensionDecl = std::make_unique<AST::ExtensionDecl>(
                targetType, std::move(methods),
                std::vector<AST::TypeParameter>{}, pos);
            extensionDecl->docComment = std::move(docComment);
            return extensionDecl;
        }

        // ======================================================================
        // (AR) تحليل تصريح ماكرو / (EN) Parse Macro Declaration
        // ======================================================================
        //
        // (AR) الصيغة النحوية:
        //      ماكرو اسم_الماكرو(معامل1، معامل2، ...)
        //          جسم الماكرو
        //      نهاية
        //
        // (EN) Syntax:
        //      macro macro_name(param1, param2, ...)
        //          macro body
        //      end
        //
        // (AR) الماكرو يختلف عن الدالة في أنه يُنفّذ في نطاق المُستدعي (inline).
        //      يُستدعى بأسلوب: اسم!(معاملات) مع علامة التعجب
        // (EN) Macros differ from functions in that they execute in caller's scope (inline).
        //      Invoked with: name!(arguments) using the exclamation mark
        // ======================================================================

        StmtPtr ParserCore::parseMacroDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            auto pos = previous().getPosition();

            // (AR) تحليل اسم الماكرو
            // (EN) Parse macro name
            Token nameToken = consume(TT::IDENTIFIER,
                                      "(AR) توقع اسم الماكرو بعد 'ماكرو'.\n"
                                      "مثال: ماكرو تبديل(أ، ب) ... نهاية\n"
                                      "(EN) Expected macro name after 'macro'.\n"
                                      "Example: macro swap(a, b) ... end");

            std::string macroName = nameToken.getValue();

            // (AR) تحليل قائمة المعاملات
            //      دعم المعاملات المتغيرة: ماكرو اسم(أ، ب، ...باقي)
            //      الرمز `...` قبل اسم المعامل الأخير يجعله يجمع الوسائط الزائدة في مصفوفة
            // (EN) Parse parameter list
            //      Variadic support: macro name(a, b, ...rest)
            //      `...` before last param name collects extra args into array
            consume(TT::PAREN_LEFT,
                    "(AR) توقع '(' بعد اسم الماكرو.\n"
                    "(EN) Expected '(' after macro name.");

            std::vector<std::string> params;
            bool isVariadic = false;
            if (!check(TT::PAREN_RIGHT))
            {
                do
                {
                    // (AR) تحقق من `...` للمعامل المتغير
                    // (EN) Check for `...` for variadic parameter
                    if (match(TT::ELLIPSIS))
                    {
                        Token paramToken = consume(TT::IDENTIFIER,
                                                   "(AR) توقع اسم المعامل المتغير بعد '...'.\n"
                                                   "مثال: ماكرو سجّل(...رسائل)\n"
                                                   "(EN) Expected variadic parameter name after '...'.\n"
                                                   "Example: macro log(...messages)");
                        params.push_back(paramToken.getValue());
                        isVariadic = true;
                        // (AR) المعامل المتغير يجب أن يكون الأخير
                        // (EN) Variadic parameter must be last
                        if (!check(TT::PAREN_RIGHT))
                        {
                            error("(AR) المعامل المتغير '..." + paramToken.getValue() +
                                  "' يجب أن يكون آخر معامل.\n"
                                  "(EN) Variadic parameter '..." +
                                  paramToken.getValue() +
                                  "' must be the last parameter.");
                        }
                        break;
                    }
                    Token paramToken = consume(TT::IDENTIFIER,
                                               "(AR) توقع اسم معامل في تعريف الماكرو.\n"
                                               "(EN) Expected parameter name in macro definition.");
                    params.push_back(paramToken.getValue());
                } while (matchComma());
            }

            consume(TT::PAREN_RIGHT,
                    "(AR) توقع ')' بعد معاملات الماكرو.\n"
                    "(EN) Expected ')' after macro parameters.");

            // (AR) تحليل جسم الماكرو (كتلة من الجمل حتى 'نهاية')
            // (EN) Parse macro body (block of statements until 'end')
            StmtList bodyStmts;
            while (!check(TT::KEYWORD_END) && !isAtEnd())
            {
                skipSemicolons();
                if (check(TT::KEYWORD_END))
                    break;
                auto stmt = parseDeclaration();
                if (stmt)
                {
                    bodyStmts.push_back(std::move(stmt));
                }
            }

            consume(TT::KEYWORD_END,
                    "(AR) توقع 'نهاية' بعد جسم الماكرو.\n"
                    "مثال:\n    ماكرو تبديل(أ، ب)\n        متغير _م = أ\n        أ = ب\n        ب = _م\n    نهاية\n"
                    "(EN) Expected 'end' after macro body.");

            auto body = std::make_unique<AST::BlockStmt>(std::move(bodyStmts), pos);

            auto macroDecl = std::make_unique<AST::MacroDecl>(
                macroName, std::move(params), std::move(body), pos, isVariadic);
            macroDecl->docComment = std::move(docComment);
            return macroDecl;
        }

        // ======================================================================
        // (AR) تحليل معاملات العمر (Lifetime Parameters)
        // (EN) Parse Lifetime Parameters
        // ======================================================================
        // (AR) الصيغة: <'أ, 'ب, 'ثابت>
        //      يُستدعى بعد اسم الدالة أو البنية عندما يكون الرمز التالي '<'
        //      متبوعاً بتعليق عمر (LIFETIME token)
        // (EN) Syntax: <'a, 'b, 'static>
        //      Called after function/struct name when next token is '<' followed by LIFETIME
        // ======================================================================

        std::vector<std::string> ParserCore::parseLifetimeParams()
        {
            std::vector<std::string> lifetimes;

            // (AR) يجب أن يبدأ بـ < متبوعاً بـ LIFETIME
            // (EN) Must start with < followed by LIFETIME
            if (!check(TT::OP_LESS))
                return lifetimes;

            // (AR) نظرة مسبقة: هل بعد '<' يوجد LIFETIME?
            // (EN) Lookahead: is there a LIFETIME after '<'?
            if (peekNext().getType() != TT::LIFETIME)
                return lifetimes;

            advance(); // (AR) استهلك '<' / consume '<'

            // (AR) قراءة الأعمار مفصولة بفواصل
            // (EN) Read comma-separated lifetimes
            do
            {
                if (!check(TT::LIFETIME))
                {
                    error("(AR) توقع تعليق عمر بعد '<' أو ','.\n"
                          "مثال: دالة أطول<'أ, 'ب>(...)\n"
                          "(EN) Expected lifetime annotation after '<' or ','.");
                    break;
                }
                lifetimes.push_back(current_.getValue());
                advance(); // (AR) استهلك LIFETIME / consume LIFETIME
            } while (matchComma());

            consume(TT::OP_GREATER,
                    "(AR) توقع '>' بعد معاملات العمر.\n"
                    "مثال: دالة اسم<'أ>(...)\n"
                    "(EN) Expected '>' after lifetime parameters.");

            return lifetimes;
        }

    } // namespace Parser
} // namespace Sad
