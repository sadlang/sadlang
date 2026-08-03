/**
 * @file parser_core_oop.cpp
 * @brief (AR) تنفيذ دوال تحليل البرمجة الكائنية في المحلل النحوي
 *        (EN) Implementation of OOP parsing functions in parser
 *
 * (AR) تم تعديل هذا الملف ليطابق مواصفات OOP في:
 *      docs\language_spec\rules\03_oop.md
 *      الأقسام: §1 (Basic Class Declaration), §2 (Inheritance), §4 (Destructor)
 *
 * (EN) This file was updated to match OOP specifications in:
 *      docs\language_spec\rules\03_oop.md
 *      Sections: §1 (Basic Class Declaration), §2 (Inheritance), §4 (Destructor)
 *
 * (AR) يحتوي هذا الملف على تنفيذات دوال تحليل البرمجة الكائنية:
 *      - تحليل تصريحات الأصناف
 *      - تحليل الحقول والطرق
 *      - تحليل البواني والهدامات
 *      - تحليل تعبيرات new, this, super
 *
 * (EN) This file contains implementations of OOP parsing functions:
 *      - Class declaration parsing
 *      - Field and method parsing
 *      - Constructor and destructor parsing
 *      - new, this, super expression parsing
 *
 * @author Sad Language Development Team
 * @date 2025-11-23 (Updated to match spec)
 * @license MIT License
 */

#include "parser_core.h"
#include <iostream>

namespace Sad
{
    namespace Parser
    {

        using namespace AST;
        using namespace Lexer;
        using TT = TokenType;

        // ======================================================================
        // (AR) تحليل معدلات الرؤية / (EN) Parse Visibility Modifiers
        // ======================================================================

        /**
         * @brief (AR) يحلل معدلات الرؤية (عام، خاص، محمي) والمعدلات الأخرى
         *        (EN) Parses visibility modifiers and other modifiers
         */
        AccessModifier ParserCore::parseModifiers(bool &isStatic, bool &isVirtual, bool &isAbstract)
        {
            AccessModifier access = AccessModifier::PUBLIC; // (AR) افتراضياً عام / (EN) Default public

            // (AR) تحليل جميع المعدلات / (EN) Parse all modifiers
            while (true)
            {
                if (match(TT::KEYWORD_PUBLIC))
                {
                    access = AccessModifier::PUBLIC;
                }
                else if (match(TT::KEYWORD_PRIVATE))
                {
                    access = AccessModifier::PRIVATE;
                }
                else if (match(TT::KEYWORD_PROTECTED))
                {
                    access = AccessModifier::PROTECTED;
                }
                else if (match(TT::KEYWORD_STATIC))
                {
                    isStatic = true;
                }
                else if (match(TT::KEYWORD_ABSTRACT))
                {
                    isAbstract = true;
                    isVirtual = true; // (AR) المجرد هو افتراضي / (EN) Abstract implies virtual
                }
                else
                {
                    // (AR) لا توجد معدلات أخرى / (EN) No more modifiers
                    break;
                }
            }

            return access;
        }

        // ======================================================================
        // (AR) تحليل تصريح حقل / (EN) Parse Field Declaration
        // ======================================================================

        /**
         * @brief (AR) يحلل تصريح حقل في الصنف
         *        (EN) Parses field declaration in class
         *
         * الصيغة / Syntax: [عام|خاص|محمي] type IDENTIFIER ['=' expression] [(';' | '؛')]?
         * Spec: docs\language_spec\rules\03_oop.md §1 - field_decl
         * Note: Semicolon is OPTIONAL (both Arabic ؛ and English ; supported)
         */
        std::unique_ptr<FieldDecl> ParserCore::parseFieldDeclaration(AccessModifier access, bool isStatic)
        {
#ifdef DEBUG_OOP
            std::cout << "[OOP] تحليل حقل\n";
#endif
            // (AR) التقاط تعليق التوثيق المعلق قبل تحليل الحقل لمنع تسربه
            //      إلى التصريح التالي. (EN) Consume pending doc comment up-front
            //      so it does not leak to the next declaration.
            std::string capturedDoc = consumePendingDocComment();

            // ═══════════════════════════════════════════════════════════════════
            // (AR) النوع — يدعم كلاً من الأنواع الأساسية والأصناف المخصصة
            //      الأصناف المخصصة تُكتب كـ IDENTIFIER وتُعامل كنوع OBJECT
            //      مثال: رقم س          → نوع أساسي
            //            خريطة_بت خ     → صنف مخصص (نوع OBJECT)
            //
            // (EN) Type — supports both primitive types and custom classes
            //      Custom classes are written as IDENTIFIER and treated as OBJECT type
            //      Example: رقم س          → primitive type
            //               Bitmap bm      → custom class (OBJECT type)
            // ═══════════════════════════════════════════════════════════════════

            Token typeToken = current_;
            Types::SadTypeKind fieldType;
            std::string typeName;

            if (isTypeToken(current_.getType()))
            {
                // (AR) نوع أساسي: رقم، نص، منطقي، إلخ
                // (EN) Primitive type: number, string, boolean, etc.
                advance(); // consume type
                typeName = typeToken.getValue();
                fieldType = mapTokenTypeToKind(typeToken.getType());
            }
            else if (check(TT::IDENTIFIER))
            {
                // ═══════════════════════════════════════════════════════════════
                // (AR) إصلاح المشكلة 11: دعم الأصناف المخصصة كأنواع للحقول
                //      مثال: خريطة_بت الخريطة — نقبل الاسم كنوع OBJECT
                //      التحقق من وجود الصنف يحدث لاحقاً في المدقق الدلالي
                //
                // (EN) Fix issue 11: support custom classes as field types
                //      Example: Bitmap myMap — accept name as OBJECT type
                //      Class existence checking happens later in semantic checker
                // ═══════════════════════════════════════════════════════════════
                advance(); // consume class name
                typeName = typeToken.getValue();
                fieldType = Types::SadTypeKind::Class; // Custom class → OBJECT type
            }
            else
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "نوع الحقل"}, {"what_en", "field type"}, {"ctx_ar", "في جسم البنية/الصنف"}, {"ctx_en", "in the struct/class body"}});
                synchronize();
                return nullptr;
            }

            // (AR) الاسم / (EN) Name
            Token nameToken = consume(TT::IDENTIFIER, "");
            std::string fieldName = nameToken.getValue();

#ifdef DEBUG_OOP
            std::cout << "[OOP] حقل: " << typeName << " " << fieldName << "\n";
#endif

            // (AR) القيمة الافتراضية (اختياري) / (EN) Default value (optional)
            ExprPtr initializer = nullptr;
            if (match(TT::OP_ASSIGN))
            {
                initializer = parseExpression();
            }

            // (AR) فاصلة منقوطة اختيارية (دعم العربية والإنجليزية)
            // (EN) Optional semicolon (support both Arabic and English)
            // Spec: docs\language_spec\rules\03_oop.md §1 - field_decl ::= ... [(';' | '؛')]?
            if (matchSemicolon())
            {
                // Semicolon consumed
            }

            auto fieldDecl = std::make_unique<FieldDecl>(fieldName, fieldType, std::move(initializer),
                                                         access, isStatic, nameToken.getPosition());
            // (AR) إرفاق التوثيق الملتقط بالحقل / (EN) Attach captured doc to field
            fieldDecl->docComment = std::move(capturedDoc);
            return fieldDecl;
        }

        // ======================================================================
        // (AR) تحليل تصريح طريقة / (EN) Parse Method Declaration
        // ======================================================================

        /**
         * @brief (AR) يحلل تصريح طريقة في الصنف
         *        (EN) Parses method declaration in class
         *
         * الصيغة / Syntax: [عام|خاص|محمي] 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
         * Spec: docs\language_spec\rules\03_oop.md §1 - method_decl
         * Note: Return type is OPTIONAL and comes AFTER 'دالة' and BEFORE method name
         */
        std::unique_ptr<MethodDecl> ParserCore::parseMethodDeclaration(
            AccessModifier access, bool isStatic, bool isVirtual, bool isAbstract)
        {

#ifdef DEBUG_OOP
            std::cout << "[OOP] تحليل طريقة\n";
#endif
            // (AR) التقاط تعليق التوثيق المعلق فوراً لمنع تسربه إلى التصريحات
            //      اللاحقة. (EN) Capture pending doc comment immediately to prevent
            //      leakage into subsequent declarations.
            std::string capturedDoc = consumePendingDocComment();

            // (AR) نوع الإرجاع (اختياري - يأتي قبل اسم الطريقة)
            // (EN) Return type (optional - comes BEFORE method name)
            // Spec: docs\language_spec\rules\03_oop.md §1 - method_decl ::= ... 'دالة' [type] IDENTIFIER ...
            Types::SadTypeKind returnType = Types::SadTypeKind::Void;
            std::string returnTypeName; // (AR) [Phase 5e] لأنواع الأصناف المُعرَّفة من المستخدم

            // Check if next token is a type (keyword like رقم، نص) or identifier (for method name)
            Token nameToken = current_; // Initialize with current for line number tracking

            // ═══════════════════════════════════════════════════════════════════
            // (AR) معالجة المعدلات التي تأتي بعد 'دالة' (مثل: دالة ثابتة طريقة())
            //      الصيغة: دالة [ثابتة|ساكن] [غير_متزامن|غير_متزامنة] اسم_الطريقة()
            // (EN) Handle modifiers after 'function' (e.g., function static method())
            //      Syntax: function [static] [async] method_name()
            // ═══════════════════════════════════════════════════════════════════
            if (check(TT::KEYWORD_STATIC))
            {
                isStatic = true;
                advance(); // consume 'ثابتة' / 'ساكن'
            }
            // (AR) كلمة سياقية: تحقق مزدوج — KEYWORD_ASYNC أو مُعرّف
            // (EN) Contextual keyword: dual check
            if (check(TT::KEYWORD_ASYNC) ||
                (check(TT::IDENTIFIER) && checkContextual(TT::KEYWORD_ASYNC)))
            {
                advance(); // consume 'غير_متزامنة' / 'غير_متزامن' (parsed but ignored for now)
            }

            // ─────────────────────────────────────────────────────────────────
            // (AR) RFC 0034: 'دالة خارجية' داخل جسم صنف — التصريح الخارجيّ بلا جسم
            //      ولا «هذا»، فلا معنى له كطريقة. نرفضه برسالة واضحة بدل تعاقب
            //      أخطاء مضلِّل («توقع نوع بيانات...»). أمّا طريقة *اسمها*
            //      خارجي/خارجية ('(' يليها غير نصّ) فتبقى مشروعة كما قبل RFC 0034.
            // (EN) RFC 0034: 'دالة خارجية' inside a class body — extern decls have no
            //      body and no 'this'; reject with a clear message instead of a
            //      misleading cascade. A method *named* خارجي/خارجية ('(' followed
            //      by a non-string) stays legal as before RFC 0034.
            // ─────────────────────────────────────────────────────────────────
            bool namePinned = false;
            if (check(TT::KEYWORD_EXTERN))
            {
                Token externTok = current_;
                advance(); // (AR) استهلاك 'خارجية'/'خارجي' / (EN) consume the extern lexeme
                if (check(TT::PAREN_LEFT) && peekNext().getType() != TT::STRING_LITERAL)
                {
                    nameToken = Token(TT::IDENTIFIER, externTok.getValue(), externTok.getPosition());
                    namePinned = true;
                }
                else
                {
                    errorCatalog(Errors::ErrorCode::SYN_DECL_NOT_ALLOWED_HERE, {{"decl_ar", "التصريح الخارجيّ '" + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + "' (بلا جسم ولا يرتبط بكائن)"}, {"decl_en", "the extern declaration '" + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + "' (no body, no receiver)"}, {"where_ar", "صنف"}, {"where_en", "a class"}, {"example", kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " printf(" + kw(TT::TYPE_STRING) + ")"}});
                    // (AR) تعافٍ: استهلاك '("رمز")' الاختياريّ ثم بقيّة التوقيع — يمنع التعاقب.
                    // (EN) Recovery: consume optional '("sym")' then the rest of the signature.
                    if (check(TT::PAREN_LEFT) && peekNext().getType() == TT::STRING_LITERAL)
                    {
                        advance(); // '('
                        advance(); // "sym"
                        if (check(TT::PAREN_RIGHT))
                            advance();
                    }
                    parseExternFunctionDecl(std::string());
                    return nullptr;
                }
            }

            // (AR) التمييز بين نوع الإرجاع واسم الطريقة:
            //      دالة نص احصل() — نص هو نوع الإرجاع لأن يليه معرّف "احصل"
            //      دالة نص()      — نص هو اسم الطريقة لأن يليه "("
            // (EN) Distinguish return type from method name:
            //      function string getName() — string is return type (followed by identifier)
            //      function text()           — text is method name (followed by "(")
            if (namePinned)
            {
                // (AR) الاسم مُثبَّت مسبقًا (طريقة اسمها خارجي/خارجية — تمييز RFC 0034 أعلاه)
                // (EN) Name pre-pinned (method named extern — RFC 0034 disambiguation above)
            }
            else if (isTypeToken(current_.getType()) && peekNext().getType() == TT::IDENTIFIER)
            {
                // Has return type: رقم احصل_الرصيد()
                returnType = parseType();
                // (AR) دعم الكلمات المفتاحية الناعمة كأسماء طرق (مثل: احصل، عيّن)
                // (EN) Support soft keywords as method names (e.g., احصل, عيّن)
                if (check(TT::IDENTIFIER))
                {
                    nameToken = current_;
                    advance();
                }
                else if (isTokenUsableAsName(current_.getType()))
                {
                    nameToken = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                    advance();
                }
                else
                {
                    nameToken = consume(TT::IDENTIFIER, "");
                }
            }
            else if (isTypeToken(current_.getType()) && peekNext().getType() == TT::PAREN_LEFT)
            {
                // (AR) نوع مدمج كاسم طريقة: دالة نص() — نص هو اسم الطريقة
                // (EN) Built-in type as method name: function text() — text is method name
                nameToken = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                advance();
            }
            // (AR) [Phase 5e] نوع إرجاع من صنف مُعرَّف من المستخدم: "نقطة احصل_النقطة()"
            // (EN) [Phase 5e] User-defined class return type: "نقطة getPoint()"
            else if (check(TT::IDENTIFIER) && peekNext().getType() == TT::IDENTIFIER)
            {
                returnTypeName = current_.getValue();
                returnType = Types::SadTypeKind::Class;
                advance(); // (AR) استهلاك اسم الصنف / (EN) consume class name
                if (check(TT::IDENTIFIER))
                {
                    nameToken = current_;
                    advance();
                }
                else
                {
                    nameToken = consume(TT::IDENTIFIER, "");
                }
            }
            else if (isTokenUsableAsName(current_.getType()) ||
                     current_.getType() == TT::LITERAL_FALSE || current_.getType() == TT::LITERAL_TRUE ||
                     current_.getType() == TT::LITERAL_NULL)
            {
                // (AR) كلمة مفتاحية/حرف محجوز كاسم طريقة (مثل: دالة خطأ()، دالة صحيح()، دالة لاشيء())
                // (EN) Keyword/literal as method name (e.g., function خطأ(), function صحيح(), function لاشيء())
                nameToken = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                advance();
            }
            else
            {
                // No return type (void method): احصل_الرصيد()
                nameToken = consume(TT::IDENTIFIER, "");
            }

            std::string methodName = nameToken.getValue();
#ifdef DEBUG_OOP
            std::cout << "[OOP] طريقة: " << methodName << " (نوع الإرجاع: "
                      << static_cast<int>(returnType) << ")\n";
#endif

            // (AR) المعاملات / (EN) Parameters
            consume(TT::PAREN_LEFT, "");

            std::vector<Parameter> parameters;
            if (!check(TT::PAREN_RIGHT))
            {
                do
                {
                    // ─────────────────────────────────────────────────────────────
                    // (AR) نوع المعامل (اختياري) — ندعم صيغتين:
                    //   أ) دالة طريقة(رقم س، نص ن) — مع نوع صريح
                    //   ب) دالة طريقة(س، ن) — بدون نوع (يُعامل كـ OBJECT/ديناميكي)
                    //   ج) دالة طريقة(ت قيمة) — حيث ت معامل نوع قالب (يُعامل كـ OBJECT)
                    // هذا مهم جداً لدعم الأصناف القالبية حيث نوع المعامل
                    // قد يكون معامل نوع قالب (معرّف وليس كلمة نوع مدمجة)
                    // ─────────────────────────────────────────────────────────────
                    // (EN) Parameter type (optional) — supports two forms:
                    //   a) method(int x, string s) — with explicit type
                    //   b) method(x, s) — without type (treated as OBJECT/dynamic)
                    //   c) method(T value) — where T is template type param (OBJECT)
                    // ─────────────────────────────────────────────────────────────
                    Types::SadTypeKind paramType = Types::SadTypeKind::Class;

                    // (AR) تحقق: هل هذا نوع مدمج متبوع بمعرّف؟ أم أنه اسم معامل فقط؟
                    // (EN) Check: is this a built-in type followed by identifier? Or just a param name?
                    // مثال "نص اسم" vs "foo(نص)" حيث نص هو اسم المعامل
                    bool isTypeFollowedByName = isTypeToken(current_.getType()) &&
                                                (peekNext().getType() == TT::IDENTIFIER ||
                                                 isTokenUsableAsName(peekNext().getType()));

                    if (isTypeFollowedByName)
                    {
                        // (AR) نوع صريح مدمج (رقم، نص، منطقي، إلخ) متبوع باسم المعامل
                        paramType = parseType();
                        Token paramToken = consume(TT::IDENTIFIER, "");
                        // (AR) القيمة الافتراضية الاختيارية / (EN) Optional default value
                        ExprPtr defaultValue = nullptr;
                        if (match(TT::OP_ASSIGN))
                        {
                            defaultValue = parseExpression();
                        }
                        parameters.push_back(Parameter(paramToken.getValue(), paramType, std::move(defaultValue)));
                    }
                    else if (check(TT::IDENTIFIER) || isTokenUsableAsName(current_.getType()) || isTypeToken(current_.getType()))
                    {
                        // (AR) معرّف أو كلمة مفتاحية مستخدمة كاسم أو نوع مدمج كاسم معامل
                        // (EN) Identifier, keyword-as-name, or built-in type as param name
                        // (AR) التحقق: هل هذا معرّف متبوع بمعرّف آخر؟ (أي: نوع_صنف اسم)
                        // (EN) Check: is this identifier followed by another? (i.e. class_type name)
                        // (AR) ملاحظة: نفحص isTokenUsableAsName لدعم الكلمات المفتاحية مثل "من"/"و"/"أو" كأسماء معاملات
                        Token firstToken = current_;
                        // (AR) نتحقق من الرمز التالي: إذا كان معرّفاً أيضاً فهذا "نوع اسم"
                        TokenType nextType = peekNext().getType();
                        if (nextType == TT::IDENTIFIER || isTokenUsableAsName(nextType))
                        {
                            // (AR) صيغة: نوع_صنف اسم_المعامل (مثل: شخص ش)
                            advance(); // (AR) استهلاك اسم النوع
                            Token paramToken = current_;
                            advance(); // (AR) استهلاك اسم المعامل
                            // (AR) القيمة الافتراضية الاختيارية / (EN) Optional default value
                            ExprPtr defaultValue = nullptr;
                            if (match(TT::OP_ASSIGN))
                            {
                                defaultValue = parseExpression();
                            }
                            // (AR) تمرير اسم الصنف typeName — كان يُسقَط هنا فلا يصل
                            //      بذرُ أصناف المعاملات المصرَّحة إلى طرق الأصناف إطلاقًا
                            //      (`دالة سجل(حدث ح)` كطريقة) بينما يصل للدوالّ الحرّة.
                            // (EN) Pass the class name as typeName — it was dropped here,
                            //      so declared-param class seeding never reached class
                            //      methods (`دالة سجل(حدث ح)`) while free functions got it.
                            parameters.push_back(Parameter(paramToken.getValue(), Types::SadTypeKind::Class, std::move(defaultValue), firstToken.getValue()));
                        }
                        else
                        {
                            // (AR) صيغة: اسم_المعامل فقط (بدون نوع)
                            advance();
                            // (AR) القيمة الافتراضية الاختيارية / (EN) Optional default value
                            ExprPtr defaultValue = nullptr;
                            if (match(TT::OP_ASSIGN))
                            {
                                defaultValue = parseExpression();
                            }
                            parameters.push_back(Parameter(firstToken.getValue(), paramType, std::move(defaultValue)));
                        }
                    }
                    else
                    {
                        // (AR) نوع المعامل / (EN) Parameter type
                        paramType = parseType();

                        // (AR) اسم المعامل / (EN) Parameter name
                        Token paramToken = consume(TT::IDENTIFIER, "");

                        // (AR) القيمة الافتراضية الاختيارية / (EN) Optional default value
                        ExprPtr defaultValue = nullptr;
                        if (match(TT::OP_ASSIGN))
                        {
                            defaultValue = parseExpression();
                        }
                        parameters.push_back(Parameter(paramToken.getValue(), paramType, std::move(defaultValue)));
                    }

                    // Spec: docs\language_spec\rules\03_oop.md §1 - param_list ::= param ((',' | '،') param)*
                } while (matchComma());
            }

            consume(TT::PAREN_RIGHT, "");

            // (AR) التحقق من عدم استخدام { } في تعريف الطريقة
            // (EN) Check for incorrect { } usage in method definition
            if (!isAbstract && check(TT::BRACE_LEFT))
            {
                error(
                    "(AR) ❌ خطأ نحوي: لا يمكن استخدام '{' في تعريف الطرق!\n"
                    "في لغة ص، الطرق لا تستخدم الأقواس المعقوفة { }.\n"
                    "يجب أن تبدأ جسم الطريقة مباشرة وتنتهي بكلمة 'نهاية'.\n\n"
                    "❌ خطأ:\n"
                    "  طريقة " +
                    methodName + "() {\n"
                                 "    # الكود\n"
                                 "  }\n\n"
                                 "✅ صحيح:\n"
                                 "  طريقة " +
                    methodName + "()\n"
                                 "    # الكود\n"
                                 "  نهاية\n\n"
                                 "(EN) ❌ Syntax error: Cannot use '{' in method definition!\n"
                                 "In Sad language, methods do not use curly braces { }.\n"
                                 "Method body must start directly and end with 'نهاية' keyword.\n");
            }

            // (AR) جسم الطريقة / (EN) Method body
            StmtPtr body = nullptr;

            // ─────────────────────────────────────────────────────────────────────
            // (AR) تحليل العقود البرمجية: يتطلب (شرط) / يضمن (شرط) قبل جسم الطريقة
            // (EN) Parse Design by Contract: requires/ensures before method body
            // (AR) هذه كلمات سياقية — تُحلل فقط قبل جسم الطريقة مباشرة
            // ─────────────────────────────────────────────────────────────────────
            ExprList preconditions;
            ExprList postconditions;
            if (!isAbstract)
            {
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
            }

            if (isAbstract)
            {
                // (AR) طريقة مجردة - بدون جسم، فاصلة منقوطة اختيارية
                // (EN) Abstract method - no body, optional semicolon
                matchSemicolon();
            }
            else
            {
                // (AR) طريقة عادية - جسم كامل / (EN) Regular method - full body
                body = parseBlockStmt();
            }

            bool isOverride = false;
            // (AR) ملاحظة: override غير مدعوم حالياً - سيتم إضافته لاحقاً
            // (EN) Note: override not currently supported - will be added later
            auto methodDecl = std::make_unique<MethodDecl>(methodName, std::move(parameters), returnType,
                                                           std::move(body), access, isStatic, isVirtual,
                                                           isOverride, isAbstract, nameToken.getPosition());
            // (AR) [Phase 5e] حفظ اسم صنف الإرجاع (إن وُجد)
            // (EN) [Phase 5e] Store class return type name (if any)
            methodDecl->returnTypeName = returnTypeName;
            // (AR) تعيين العقود البرمجية على الطريقة
            // (EN) Set Design by Contract on the method
            methodDecl->preconditions = std::move(preconditions);
            methodDecl->postconditions = std::move(postconditions);
            // (AR) إرفاق التوثيق الملتقط بالطريقة / (EN) Attach captured doc to method
            methodDecl->docComment = std::move(capturedDoc);
            return methodDecl;
        }

        // ======================================================================
        // (AR) تحليل تصريح باني / (EN) Parse Constructor Declaration
        // ======================================================================

        /**
         * @brief (AR) يحلل تصريح باني في الصنف
         *        (EN) Parses constructor declaration in class
         *
         * الصيغة / Syntax: 'باني' '(' [param_list] ')' [':' initializer_list] block
         * Spec: docs\language_spec\rules\03_oop.md §1 - constructor_decl
         * Note: Only 'باني' keyword is supported
         */
        std::unique_ptr<ConstructorDecl> ParserCore::parseConstructorDeclaration(const std::string &className, AccessModifier access)
        {
#ifdef DEBUG_OOP
            std::cout << "[OOP] تحليل باني: " << className << "\n";
#endif
            // (AR) التقاط تعليق التوثيق المعلق قبل تحليل الباني / (EN) Capture pending doc comment
            std::string capturedDoc = consumePendingDocComment();

            // Constructor name should match class name
            // (AR) المعاملات / (EN) Parameters
            consume(TT::PAREN_LEFT, "");

            std::vector<Parameter> parameters;
            std::vector<std::string> thisParams; // (AR) معاملات هذا. للتعيين التلقائي / (EN) this-params for auto-assignment
            if (!check(TT::PAREN_RIGHT))
            {
                do
                {
                    // (AR) نوع المعامل (اختياري) / (EN) Parameter type (optional)
                    // (AR) ندعم ثلاث صيغ: باني(رقم س) أو باني(س) أو باني(هذا.س)
                    // (EN) Support three forms: constructor(int x) or constructor(x) or constructor(this.x)
                    Types::SadTypeKind paramType = Types::SadTypeKind::Class;
                    ExprPtr defaultValue = nullptr;

                    if (check(TT::KEYWORD_THIS) && peekNext().getType() == TT::DOT)
                    {
                        // (AR) صيغة اختصار: باني(هذا.اسم) — تعيين تلقائي للخاصية
                        // (EN) Shorthand: constructor(this.name) — auto-assign to field
                        advance(); // consume 'هذا'
                        advance(); // consume '.'
                        Token paramToken = consume(TT::IDENTIFIER, "");
                        // (AR) القيمة الافتراضية الاختيارية / (EN) Optional default value
                        if (match(TT::OP_ASSIGN))
                        {
                            defaultValue = parseExpression();
                        }
                        parameters.push_back(Parameter(paramToken.getValue(), paramType, std::move(defaultValue)));
                        thisParams.push_back(paramToken.getValue());
                    }
                    else if (isTypeToken(current_.getType()) &&
                             peekNext().getType() == TT::IDENTIFIER)
                    {
                        // (AR) نوع صريح موجود متبوع باسم معامل / (EN) Explicit type present followed by param name
                        paramType = parseType();
                        Token paramToken = consume(TT::IDENTIFIER, "");
                        // (AR) القيمة الافتراضية الاختيارية / (EN) Optional default value
                        if (match(TT::OP_ASSIGN))
                        {
                            defaultValue = parseExpression();
                        }
                        parameters.push_back(Parameter(paramToken.getValue(), paramType, std::move(defaultValue)));
                    }
                    else if (isTypeToken(current_.getType()))
                    {
                        // (AR) رمز نوع مُستخدم كاسم معامل (مثل: باني(نص = ""))
                        // (EN) Type token used as parameter name (e.g.: constructor(text = ""))
                        Token paramToken = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                        advance();
                        if (match(TT::OP_ASSIGN))
                        {
                            defaultValue = parseExpression();
                        }
                        parameters.push_back(Parameter(paramToken.getValue(), paramType, std::move(defaultValue)));
                    }
                    else if (check(TT::IDENTIFIER))
                    {
                        // (AR) اسم المعامل — ثم اختيارياً ": نوع" أو "= قيمة"
                        // (EN) Parameter name — then optionally ": type" or "= value"
                        Token paramToken = current_;
                        advance();
                        // (AR) تصريح النوع الاختياري: اسم : نوع
                        // (EN) Optional type annotation: name : type
                        if (match(TT::COLON))
                        {
                            paramType = parseType();
                        }
                        // (AR) القيمة الافتراضية الاختيارية / (EN) Optional default value
                        if (match(TT::OP_ASSIGN))
                        {
                            defaultValue = parseExpression();
                        }
                        parameters.push_back(Parameter(paramToken.getValue(), paramType, std::move(defaultValue)));
                    }
                    else if (isTokenUsableAsName(current_.getType()))
                    {
                        // (AR) كلمة سياقية مُستخدمة كاسم معامل (مثل: باني(سمة = لاشيء))
                        // (EN) Contextual keyword used as parameter name (e.g.: constructor(trait = null))
                        Token paramToken = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                        advance();
                        // (AR) تصريح النوع الاختياري: اسم : نوع
                        // (EN) Optional type annotation: name : type
                        if (match(TT::COLON))
                        {
                            paramType = parseType();
                        }
                        if (match(TT::OP_ASSIGN))
                        {
                            defaultValue = parseExpression();
                        }
                        parameters.push_back(Parameter(paramToken.getValue(), paramType, std::move(defaultValue)));
                    }
                    else
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "نوع أو اسم المعامل"}, {"what_en", "parameter type or name"}, {"ctx_ar", ""}, {"ctx_en", ""}});
                        break;
                    }

                } while (matchComma());
            }

            consume(TT::PAREN_RIGHT, "");

            // (AR) التحقق من عدم استخدام { } في تعريف الباني
            // (EN) Check for incorrect { } usage in constructor definition
            if (check(TT::BRACE_LEFT))
            {
                error(
                    "(AR) ❌ خطأ نحوي: لا يمكن استخدام '{' في تعريف الباني!\n"
                    "في لغة ص، البناة لا تستخدم الأقواس المعقوفة { }.\n"
                    "يجب أن يبدأ جسم الباني مباشرة ويختم بكلمة 'نهاية'.\n\n"
                    "❌ خطأ:\n"
                    "  باني " +
                    className + "() {\n"
                                "    # الكود\n"
                                "  }\n\n"
                                "✅ صحيح:\n"
                                "  باني " +
                    className + "()\n"
                                "    # الكود\n"
                                "  نهاية\n\n"
                                "(EN) ❌ Syntax error: Cannot use '{' in constructor definition!\n"
                                "In Sad language, constructors do not use curly braces { }.\n"
                                "Constructor body must start directly and end with 'نهاية' keyword.\n");
            }

            // (AR) قائمة التهيئة (اختياري): : الأساس(args)
            // (EN) Initializer list (optional): : super(args)
            // Spec: docs\language_spec\rules\03_oop.md - constructor can call base constructor
            // Syntax: باني(params) : الأساس(arg1, arg2)
            ExprList superArgs;
            if (match(TT::COLON))
            {
                // (AR) توقع كلمة "الأساس" أو "super"
                // (EN) Expect "الأساس" or "super" keyword
                if (!match(TT::KEYWORD_SUPER))
                {
                    // (AR) إذا لم تكن كلمة "الأساس"، قد تكون identifier للحقل
                    // (EN) If not "super", might be field identifier
                    // Note: Field initializers (: field1(val1), field2(val2)) are advanced feature
                    // Will be implemented in future phase when needed
                    // For now, we only support super constructor call
                    error(
                        "(AR) خطأ نحوي: بعد ':' في الباني، يجب استدعاء الباني الأساسي.\n"
                        "الصيغة: باني(المعاملات) : الأساس(القيم)\n"
                        "مثال: باني(رقم س) : الأساس(س)\n\n"
                        "(EN) Syntax error: After ':' in constructor, must call super constructor.\n"
                        "Syntax: constructor(params) : super(values)\n"
                        "Example: constructor(int x) : super(x)");
                }

                // (AR) تحليل معاملات الباني الأساسي: (arg1, arg2, ...)
                // (EN) Parse super constructor arguments: (arg1, arg2, ...)
                // (AR) نستخدم parseArgumentList لدعم الوسائط المسماة مثل: الأساس(تسمية: قيمة)
                // (EN) Use parseArgumentList to support named arguments like: super(label: value)
                consume(TT::PAREN_LEFT, "");

                superArgs = parseArgumentList();

                consume(TT::PAREN_RIGHT, "");
            }

            // (AR) التحقق من استدعاء الأساس() في بداية جسم الباني (بدون نقطتين)
            // (EN) Check for super() call at the start of constructor body (without colon)
            // الصيغة الوحيدة المدعومة: الأساس(args) أو الأساس.باني(args)
            // (EN) Only supported forms: الأساس(args) or الأساس.باني(args)
            if (superArgs.empty() && check(TT::KEYWORD_SUPER))
            {
                // (AR) نتحقق من الرمز التالي قبل الاستهلاك لتجنب استهلاك خاطئ
                // (EN) Check next token before consuming to avoid wrong consumption
                auto nextType = peekNext().getType();
                if (nextType == TT::PAREN_LEFT)
                {
                    // (AR) صيغة: الأساس(args)
                    // (EN) Syntax: super(args)
                    advance(); // consume 'الأساس'
                    advance(); // consume '('
                    superArgs = parseArgumentList();
                    consume(TT::PAREN_RIGHT, "");
                }
                else if (nextType == TT::DOT)
                {
                    // (AR) صيغة: الأساس.باني(args)
                    // (EN) Syntax: super.constructor(args)
                    advance(); // consume 'الأساس'
                    advance(); // consume '.'
                    // (AR) توقع كلمة 'باني' بعد 'الأساس.'
                    // (EN) Expect 'constructor' keyword after 'super.'
                    if (check(TT::KEYWORD_CONSTRUCTOR))
                    {
                        advance(); // consume 'باني'
                        consume(TT::PAREN_LEFT, "");
                        superArgs = parseArgumentList();
                        consume(TT::PAREN_RIGHT, "");
                    }
                    else
                    {
                        // (AR) الأساس. متبوعة بشيء آخر — سيتم تحليلها في جسم الباني
                        // (EN) super. followed by something else — will be parsed in body
                        // (AR) نعيد الموضع إلى ما قبل الأساس.
                        // (EN) Rewind position back before super.
                        // NOTE: Can't easily rewind, so we leave it to body parsing
                        // The body parser will see the remaining tokens
                    }
                }
                // (AR) إذا لم يكن ( أو . بعد الأساس، لا نستهلكها — تُترك لتحليل الجسم
                // (EN) If neither ( nor . after super, don't consume — leave for body parsing
            }

            // (AR) جسم الباني / (EN) Constructor body
            StmtPtr body = parseBlockStmt();

            // (AR) إدراج جمل تعيين تلقائي لمعاملات هذا.خاصية
            // (EN) Inject auto-assignment statements for this.property parameters
            // باني(هذا.اسم، هذا.عمر) → يُضاف تلقائياً: هذا.اسم = اسم; هذا.عمر = عمر
            if (!thisParams.empty())
            {
                auto *block = dynamic_cast<BlockStmt *>(body.get());
                if (block)
                {
                    for (auto it = thisParams.rbegin(); it != thisParams.rend(); ++it)
                    {
                        auto thisExpr = std::make_unique<ThisExpr>();
                        auto valueExpr = std::make_unique<VariableExpr>(*it, Lexer::Position());
                        auto assignExpr = std::make_unique<MemberAssignExpr>(
                            std::move(thisExpr), *it, std::move(valueExpr), Lexer::Position());
                        auto stmt = std::make_unique<ExprStmt>(std::move(assignExpr));
                        block->statements.insert(block->statements.begin(), std::move(stmt));
                    }
                }
            }

            auto ctorDecl = std::make_unique<ConstructorDecl>(std::move(parameters), std::move(body),
                                                              std::move(superArgs));
            // (AR) إرفاق التوثيق الملتقط بالباني / (EN) Attach captured doc to constructor
            ctorDecl->docComment = std::move(capturedDoc);
            return ctorDecl;
        }

        // ======================================================================
        // (AR) تحليل تصريح هدام / (EN) Parse Destructor Declaration
        // ======================================================================

        /**
         * @brief (AR) يحلل تصريح هدام في الصنف
         *        (EN) Parses destructor declaration in class
         *
         * الصيغة / Syntax: 'هدم' '()' block
         * Spec: docs\language_spec\rules\03_oop.md §1,4 - destructor_decl
         * Note: Only 'هدم' keyword is supported
         */
        std::unique_ptr<DestructorDecl> ParserCore::parseDestructorDeclaration(const std::string &className, AccessModifier access)
        {
#ifdef DEBUG_OOP
            std::cout << "[OOP] تحليل هدام: " << className << "\n";
#endif

            // (AR) لا يوجد معاملات / (EN) No parameters
            consume(TT::PAREN_LEFT, "");
            consume(TT::PAREN_RIGHT, "");

            // (AR) جسم الهدام / (EN) Destructor body
            StmtPtr body = parseBlockStmt();

            return std::make_unique<DestructorDecl>(std::move(body));
        }

        // ======================================================================
        // (AR) تحليل تعبير new / (EN) Parse new Expression
        // ======================================================================

        /**
         * @brief (AR) يحلل تعبير new لإنشاء كائن
         *        (EN) Parses new expression for object instantiation
         *
         * الصيغة / Syntax: جديد اسم_الصنف(معاملات)
         */
        ExprPtr ParserCore::parseNewExpr()
        {
#ifdef DEBUG_OOP
            std::cout << "[OOP] تحليل تعبير جديد (new)\n";
#endif

            // (AR) اسم الصنف / (EN) Class name
            Token classToken = consume(TT::IDENTIFIER, "");

            std::string className = classToken.getValue();

            // (AR) دعم القوالب: جديد صنف<نوع>(معاملات)
            // (EN) Template support: new Class<Type>(args)
            std::vector<Types::SadTypeKind> templateArgs;
            std::vector<std::string> templateArgNames;
            if (check(TT::OP_LESS))
            {
                advance(); // consume '<'
                do
                {
                    if (isTypeToken(current_.getType()))
                    {
                        templateArgs.push_back(mapTokenTypeToKind(current_.getType()));
                        templateArgNames.push_back(current_.getValue());
                        advance();
                    }
                    else if (check(TT::IDENTIFIER))
                    {
                        // (AR) نوع مخصص - نعتبره OBJECT
                        // (EN) Custom type - treat as OBJECT
                        templateArgs.push_back(Types::SadTypeKind::Class);
                        templateArgNames.push_back(current_.getValue());
                        advance();
                    }
                    else
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "نوع"}, {"what_en", "type"}, {"ctx_ar", "في معاملات القالب"}, {"ctx_en", "in the template parameters"}});
                        break;
                    }
                } while (matchComma());

                if (!match(TT::OP_GREATER))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ">"}, {"ctx_ar", "لإنهاء معاملات القالب"}, {"ctx_en", "to close the template parameters"}});
                }
            }

#ifdef DEBUG_OOP
            std::cout << "[OOP] إنشاء كائن من صنف: " << className;
            if (!templateArgs.empty())
                std::cout << " (قالب بـ " << templateArgs.size() << " نوع)";
            std::cout << "\n";
#endif

            // (AR) معاملات الباني (يدعم الوسائط المسماة مثل: جديد صنف(مفتاح: قيمة))
            // (EN) Constructor arguments (supports named args like: new Class(key: value))
            consume(TT::PAREN_LEFT, "");

            ExprList arguments = parseArgumentList();

#ifdef DEBUG_OOP
            std::cout << "[OOP] عدد معاملات الباني: " << arguments.size() << "\n";
#endif

            consume(TT::PAREN_RIGHT, "");

            // إنشاء عقدة NewExpr / Create NewExpr node
            auto newExpr = std::make_unique<NewExpr>(className);
            newExpr->arguments = std::move(arguments);
            newExpr->templateArguments = std::move(templateArgs);
            newExpr->templateArgumentNames = std::move(templateArgNames);

            return newExpr;
        }

        // ======================================================================
        // (AR) تحليل تعبير this / (EN) Parse this Expression
        // ======================================================================

        /**
         * @brief (AR) يحلل تعبير this (هذا)
         *        (EN) Parses this expression
         *
         * الصيغة / Syntax: 'هذا' (this keyword)
         * Spec: docs\language_spec\rules\03_oop.md §2 - this keyword
         *
         * @return (ExprPtr) (AR) مؤشر لعقدة ThisExpr
         *                  (EN) Pointer to ThisExpr node
         *
         * @example Examples / أمثلة:
         * هذا.خاصية = 5
         * هذا.طريقة()
         * this.property = 5
         * this.method()
         */
        ExprPtr ParserCore::parseThisExpression()
        {
#ifdef DEBUG_OOP
            std::cout << "[OOP] تحليل تعبير 'هذا' (this)\n";
#endif

            // (AR) إنشاء عقدة ThisExpr
            // (EN) Create ThisExpr node
            return std::make_unique<ThisExpr>();
        }

        // ======================================================================
        // (AR) تحليل تعبير super / (EN) Parse super Expression
        // ======================================================================

        /**
         * @brief (AR) يحلل تعبير super (الأساس)
         *        (EN) Parses super expression
         *
         * الصيغة / Syntax: 'الأساس' '.' method_name '(' [arg_list] ')'
         *                  | 'الأساس' '(' [arg_list] ')'  // Constructor call
         * Spec: docs\language_spec\rules\03_oop.md §2 - super_call
         *
         * @return (ExprPtr) (AR) مؤشر لعقدة SuperExpr
         *                  (EN) Pointer to SuperExpr node
         *
         * @note (AR) يُستخدم لاستدعاء طرق أو باني الصنف الأساسي
         *       (EN) Used for calling parent class methods or constructor
         *
         * @example Examples / أمثلة:
         * الأساس.طريقة()  // Call parent method
         * الأساس(args)     // Call parent constructor
         * super.method()    // Call parent method
         * super(args)       // Call parent constructor
         */
        ExprPtr ParserCore::parseSuperExpression()
        {
#ifdef DEBUG_OOP
            std::cout << "[OOP] تحليل تعبير 'الأساس' (super)\n";
#endif

            // Spec: docs\language_spec\rules\03_oop.md §2 - super keyword: `الأساس` refers to parent class
            // This can be followed by:
            // 1. '.methodName(args)' - calling parent method
            // 2. '(args)' - calling parent constructor (handled in initializer list)

            // (AR) إنشاء عقدة SuperExpr
            // (EN) Create SuperExpr node
            // Note: Member name will be set later during member access parsing
            return std::make_unique<SuperExpr>();
        }

        // ======================================================================
        // (AR) تحليل تصريح خاصية / (EN) Parse Property Declaration
        // ======================================================================

        /**
         * @brief (AR) يحلل تصريح خاصية مع getter/setter
         *        (EN) Parses property declaration with getter/setter
         *
         * الصيغة / Syntax: خاصية نوع اسم احصل ... نهاية [عيّن(...) ... نهاية] نهاية
         * Example: خاصية نص الاسم احصل ارجع قيمة نهاية عيّن(قيمة_جديدة) قيمة = قيمة_جديدة نهاية نهاية
         */
        std::unique_ptr<PropertyDecl> ParserCore::parsePropertyDeclaration(AccessModifier access, bool isStatic)
        {
#ifdef DEBUG_OOP
            std::cout << "[OOP] تحليل خاصية (property)\n";
#endif

            // (AR) النوع (اختياري) / (EN) Type (optional)
            // (AR) إذا لم يكن الرمز الحالي نوعاً، نستخدم UNKNOWN
            // (EN) If current token is not a type, use UNKNOWN
            Types::SadTypeKind propertyType = Types::SadTypeKind::Unknown;
            if (isTypeToken(current_.getType()))
            {
                propertyType = parseType();
            }

            // (AR) اسم الخاصية / (EN) Property name
            Token nameToken = consume(TT::IDENTIFIER, "");
            std::string propertyName = nameToken.getValue();

#ifdef DEBUG_OOP
            std::cout << "[OOP] خاصية: " << propertyName << " (نوع: "
                      << static_cast<int>(propertyType) << ")\n";
#endif

            // (AR) كتلة القراءة (getter) - إلزامية / (EN) Getter block - required
            std::unique_ptr<GetterBlock> getter = nullptr;
            if (!matchAny({TT::KEYWORD_GET}) && !matchContextual(TT::KEYWORD_GET))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_GET)}, {"ctx_ar", "بعد اسم الخاصية"}, {"ctx_en", "after the property name"}});
                synchronize();
                return nullptr;
            }

            // Parse getter body - statements until 'نهاية'
            StmtList getterStatements;
            while (!check(TT::KEYWORD_END) && !isAtEnd())
            {
                auto stmt = parseStatement();
                if (stmt)
                {
                    getterStatements.push_back(std::move(stmt));
                }
            }
            consume(TT::KEYWORD_END, "");

            auto getterBody = std::make_unique<BlockStmt>(
                std::move(getterStatements),
                nameToken.getPosition());
            getter = std::make_unique<GetterBlock>(std::move(getterBody));

            // (AR) كتلة الكتابة (setter) - اختيارية / (EN) Setter block - optional
            std::unique_ptr<SetterBlock> setter = nullptr;
            if (matchAny({TT::KEYWORD_SET}) || matchContextual(TT::KEYWORD_SET))
            {
                // (AR) معامل setter / (EN) Setter parameter
                consume(TT::PAREN_LEFT, "");

                Token paramToken = consume(TT::IDENTIFIER, "");
                std::string paramName = paramToken.getValue();

                consume(TT::PAREN_RIGHT, "");

                // Parse setter body - statements until 'نهاية'
                StmtList setterStatements;
                while (!check(TT::KEYWORD_END) && !isAtEnd())
                {
                    auto stmt = parseStatement();
                    if (stmt)
                    {
                        setterStatements.push_back(std::move(stmt));
                    }
                }
                consume(TT::KEYWORD_END, "");

                auto setterBody = std::make_unique<BlockStmt>(
                    std::move(setterStatements),
                    nameToken.getPosition());
                setter = std::make_unique<SetterBlock>(
                    paramName,
                    propertyType,
                    std::move(setterBody));
            }

            // (AR) نهاية الخاصية / (EN) End of property
            consume(TT::KEYWORD_END, "");

#ifdef DEBUG_OOP
            std::cout << "[OOP] انتهى تحليل خاصية '" << propertyName << "'\n";
#endif

            return std::make_unique<PropertyDecl>(
                propertyName,
                propertyType,
                std::move(getter),
                std::move(setter),
                access,
                isStatic);
        }

    } // namespace Parser
} // namespace Sad
