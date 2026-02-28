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

namespace Sad {
namespace Parser {

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
StmtPtr ParserCore::parseFunctionDecl(ExprList decorators, bool is_async, bool is_generator) {
    // Spec: docs/language_spec/rules/02_functions.md - function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
    // Optional return type BEFORE function name: دالة [type] name(...)
    // (AR) نوع الإرجاع الاختياري قبل اسم الدالة: دالة [نوع] اسم(...)
    Data::DataType returnType = Data::DataType::UNKNOWN;
    
    // (AR) راية للإشارة إذا كانت هذه هي الدالة الرئيسية
    // (EN) Flag to indicate if this is the main function
    bool isMain = false;
    Token name(TT::IDENTIFIER, "", Lexer::Position());  // (AR) تعريف name مسبقاً / (EN) Define name upfront
    
    // Check if next token is a type keyword (before function name)
    // (AR) التحقق إذا كان الرمز التالي هو نوع (قبل اسم الدالة)
    // BUT only if it's NOT followed by '(' — otherwise the type keyword IS the function name
    // e.g., "دالة نص مربع(...)" = return type string, name مربع
    //       "دالة نص(...)"       = function named نص (no return type)
    if ((check(TT::TYPE_INTEGER) || check(TT::TYPE_DOUBLE) || 
         check(TT::TYPE_STRING) || check(TT::TYPE_BOOLEAN) ||
         check(TT::TYPE_ARRAY) || check(TT::TYPE_MAP)) &&
        nextToken_.getType() != TT::PAREN_LEFT) {
        returnType = parseType();
    }
    
    // (AR) التحقق إذا كانت الدالة الرئيسية (قبل توقع اسم الدالة)
    // (EN) Check if this is the main function (before expecting function name)
    if (check(TT::KEYWORD_MAIN)) {
        // (AR) هذه هي الدالة الرئيسية - استخدام رمز KEYWORD_MAIN
        // (EN) This is the main function - consume KEYWORD_MAIN token
        Token mainToken = current_;  // (AR) حفظ الرمز قبل advance / (EN) Save token before advance
        advance();  // (AR) استهلاك الرمز / (EN) Consume token
        isMain = true;
        
        // (AR) إذا لم يتم تحديد نوع الإرجاع، استخدام صحيح (int) كإعداد افتراضي للدالة الرئيسية
        // (EN) If return type not specified, use INTEGER as default for main function
        if (returnType == Data::DataType::UNKNOWN) {
            returnType = Data::DataType::INTEGER;
        }
        
        // (AR) استخدام "رئيسية" كاسم للدالة
        // (EN) Use "main" as the function name
        name = Token(TT::IDENTIFIER, "رئيسية", mainToken.getPosition());
    } else if (isTypeToken(current_.getType())) {
        // Type keyword used as function name (e.g., دالة نص(props))
        // (AR) كلمة نوع مستخدمة كاسم دالة (مثلاً: دالة نص(props))
        auto tok = current_;
        advance();
        name = Token(TT::IDENTIFIER, tok.getValue(), tok.getPosition());
    } else if (isKeywordUsableAsName(current_.getType())) {
        // (AR) كلمة مفتاحية ناعمة مستخدمة كاسم دالة (مثلاً: دالة احصل(...))
        // (EN) Soft keyword used as function name (e.g., function احصل(...))
        auto tok = current_;
        advance();
        name = Token(TT::IDENTIFIER, tok.getValue(), tok.getPosition());
    } else {
        // Expect function name (for regular functions)
        // (AR) توقع اسم الدالة (للدوال العادية)
        name = consume(TT::IDENTIFIER, 
            "(AR) خطأ نحوي: بعد كلمة 'دالة' (أو بعد نوع الإرجاع) يجب أن يأتي اسم الدالة.\n"
            "مثال: دالة جمع(...) أو دالة رقم مربع(...)\n"
            "(EN) Syntax error: After 'function' keyword (or return type) expected function name.\n"
            "Example: function sum(...) or function int square(...)");
    }
    
    // Parse parameter list (now with type annotations)
    // (AR) تحليل قائمة المعاملات (الآن مع تصريحات الأنواع)
    consume(TT::PAREN_LEFT, 
        "(AR) خطأ نحوي: بعد اسم الدالة يجب أن يأتي قوس مفتوح '('.\n"
        "مثال: دالة " + name.getValue() + "(...) \n"
        "(EN) Syntax error: After function name expected '('.\n"
        "Example: function " + name.getValue() + "(...)");
    auto paramObjs = parseTypedParameterList();
    consume(TT::PAREN_RIGHT, 
        "(AR) خطأ نحوي: بعد قائمة المعاملات يجب أن يأتي قوس مغلق ')'.\n"
        "مثال: دالة " + name.getValue() + "(معامل١، معامل٢) \n"
        "تأكد من أن قائمة المعاملات مكتملة ومفصولة بفواصل صحيحة.\n"
        "(EN) Syntax error: After parameter list expected ')'.\n"
        "Example: function " + name.getValue() + "(param1, param2)\n"
        "Make sure parameter list is complete and properly separated.");
    
    // Optional return type annotation AFTER parameters: function name(...) : type
    // OR with "ترجع" keyword: function name(...) ترجع type
    // (AR) تصريح نوع الإرجاع الاختياري بعد المعاملات: دالة اسم(...) : نوع
    // (AR) أو مع كلمة "ترجع": دالة اسم(...) ترجع نوع
    // Note: This overrides the type specified before the function name (if any)
    if (match(TT::COLON)) {
        returnType = parseType();
    } else if (match(TT::KEYWORD_RETURNS) || match(TT::ARROW)) {
        // Support "ترجع" keyword or "->" for return type
        if (isTypeToken(current_.getType())) {
            returnType = mapTokenTypeToDataType(current_.getType());
            advance();
        } else if (check(TT::IDENTIFIER)) {
            // Could be a custom type or class name
            returnType = Data::DataType::OBJECT;
            advance();
        }
    } else if (returnType == Data::DataType::UNKNOWN && isTypeToken(current_.getType())) {
        // Type without "ترجع" keyword (for backward compatibility)
        // BUT only if it's NOT a variable declaration (type followed by identifier and =)
        // (AR) لكن فقط إذا لم يكن إعلان متغير (نوع متبوع بمعرف و=)
        // Check: if nextToken_ is IDENTIFIER and the token after that is '=', this is a VarDecl, not return type
        if (nextToken_.getType() != TT::IDENTIFIER) {
            returnType = mapTokenTypeToDataType(current_.getType());
            advance();
        }
        // Otherwise, leave current_ as the type token for parseBlockStmt to parse as VarDecl
    }
    
    // (AR) التحقق من صحة توقيع الدالة الرئيسية
    // (EN) Validate main function signature
    if (isMain) {
        // (AR) التحقق من نوع الإرجاع - يجب أن يكون صحيح (int)
        // (EN) Check return type - must be INTEGER
        if (returnType != Data::DataType::INTEGER && returnType != Data::DataType::UNKNOWN) {
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
        if (paramObjs.size() > 1) {
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
        } else if (paramObjs.size() == 1) {
            // (AR) التحقق من أن المعامل من نوع نص[] (string array)
            // (EN) Check that parameter is of type string[] (string array)
            if (paramObjs[0].type != Data::DataType::ARRAY) {
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
    auto body = parseBlockStmt();
    
    // (AR) التحقق من توافق نوع الإرجاع
    // (EN) Check return type compatibility
    // If function has UNKNOWN return type (no type specified), it should not have return statements with values
    if (returnType == Data::DataType::UNKNOWN) {
        // Check if body contains return statements with values
        // This is a simplified check - proper semantic analysis would be better
        // For now, we just warn at parse time
        // Note: This check will be done at runtime by the interpreter
    }
    
    // (AR) إنشاء عقدة تصريح الدالة مع المُزخرِفات
    // (EN) Create function declaration node with decorators
    if (!decorators.empty()) {
        auto funcDecl = std::make_unique<FunctionDecl>(
            name.getValue(),
            std::move(paramObjs),
            returnType,
            std::move(body),
            std::move(decorators),
            false,  // (AR) مُصدَّرة / (EN) exported
            is_async,  // (AR) غير متزامنة / (EN) async
            is_generator,  // (AR) مولد / (EN) generator
            name.getPosition()
        );
        // (AR) تعيين راية الدالة الرئيسية
        // (EN) Set main function flag
        funcDecl->isMainFunction = isMain;
        return funcDecl;
    }
    
    // (AR) إنشاء عقدة تصريح الدالة بدون مُزخرِفات
    // (EN) Create function declaration node without decorators
    auto funcDecl = std::make_unique<FunctionDecl>(
        name.getValue(),
        std::move(paramObjs),
        returnType,
        std::move(body),
        false,  // (AR) مُصدَّرة / (EN) exported
        is_async,  // (AR) غير متزامنة / (EN) async
        is_generator,  // (AR) مولد / (EN) generator
        name.getPosition()
    );
    // (AR) تعيين راية الدالة الرئيسية
    // (EN) Set main function flag
    funcDecl->isMainFunction = isMain;
    return funcDecl;
}

/**
 * @brief (AR) يحلل تصريح دالة خارجية (خارجي دالة)
 *        (EN) Parses external function declaration (extern function)
 * 
 * الصيغة / Syntax:
 *   خارجي دالة [نوع] اسم_الدالة(معاملات)
 * 
 * @return (AR) مؤشر على عقدة تصريح الدالة الخارجية
 *         (EN) Pointer to external function declaration node
 */
StmtPtr ParserCore::parseExternFunctionDecl() {
    // (AR) نوع الإرجاع الاختياري قبل اسم الدالة
    // (EN) Optional return type before function name
    Data::DataType returnType = Data::DataType::UNKNOWN;
    
    // Check if next token is a type keyword (before function name)
    if ((check(TT::TYPE_INTEGER) || check(TT::TYPE_DOUBLE) || 
         check(TT::TYPE_STRING) || check(TT::TYPE_BOOLEAN) ||
         check(TT::TYPE_ARRAY) || check(TT::TYPE_MAP)) &&
        nextToken_.getType() != TT::PAREN_LEFT) {
        returnType = parseType();
    }
    
    // (AR) توقع اسم الدالة
    // (EN) Expect function name
    Token name = consume(TT::IDENTIFIER, 
        "(AR) خطأ نحوي: بعد 'خارجي دالة' (أو بعد نوع الإرجاع) يجب أن يأتي اسم الدالة.\n"
        "مثال: خارجي دالة رقم clear_screen()\n"
        "(EN) Syntax error: After 'extern function' (or return type) expected function name.\n"
        "Example: extern function int clear_screen()");
    
    // (AR) تحليل قائمة المعاملات
    // (EN) Parse parameter list
    consume(TT::PAREN_LEFT, 
        "(AR) خطأ نحوي: بعد اسم الدالة الخارجية يجب أن يأتي قوس مفتوح '('.\n"
        "(EN) Syntax error: After extern function name expected '('.");
    auto paramObjs = parseTypedParameterList();
    consume(TT::PAREN_RIGHT, 
        "(AR) خطأ نحوي: بعد قائمة المعاملات يجب أن يأتي قوس مغلق ')'.\n"
        "(EN) Syntax error: After parameter list expected ')'.");
    
    // (AR) إنشاء عقدة تصريح الدالة الخارجية (بدون جسم)
    // (EN) Create external function declaration node (no body)
    auto funcDecl = std::make_unique<FunctionDecl>(
        name.getValue(),
        std::move(paramObjs),
        returnType,
        nullptr,  // (AR) لا جسم للدالة الخارجية / (EN) No body for extern function
        false,    // (AR) مُصدَّرة / (EN) exported
        false,    // (AR) غير متزامنة / (EN) async
        false,    // (AR) مولد / (EN) generator
        name.getPosition()
    );
    funcDecl->isExtern = true;  // (AR) علامة الدالة الخارجية / (EN) Mark as external
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
StmtPtr ParserCore::parseClassDecl() {
#ifdef DEBUG_OOP
    std::cout << "[OOP] بدء تحليل تصريح صنف\n";
#endif
    
    // (AR) اسم الصنف / (EN) Class name
    // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ::= 'صنف' IDENTIFIER ...
    Token nameToken = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم الصنف بعد 'صنف'. (EN) Expected class name after 'class'.");
    std::string className = nameToken.getValue();
    
#ifdef DEBUG_OOP
    std::cout << "[OOP] اسم الصنف: " << className << "\n";
#endif
    
    // (AR) الوراثة (اختياري) - دعم كلا من ':' و'يرث'
    // (EN) Inheritance (optional) - support both ':' and 'يرث'
    // Spec: docs\language_spec\rules\03_oop.md §1,2 - [(':' | 'يرث') base_class_list]
    std::vector<std::string> baseClassNames;
    if (matchAny({TT::COLON, TT::KEYWORD_INHERITS})) {
        // (AR) قائمة الأصناف الأساسية (دعم الفاصلة العربية والإنجليزية)
        // (EN) Base class list (support both Arabic and English commas)
        // Spec: docs\language_spec\rules\03_oop.md §1 - base_class_list ::= IDENTIFIER ((',' | '،') IDENTIFIER)*
        do {
            Token baseToken = consume(TT::IDENTIFIER,
                "(AR) توقع اسم الصنف الأساسي. (EN) Expected base class name.");
            baseClassNames.push_back(baseToken.getValue());
#ifdef DEBUG_OOP
            std::cout << "[OOP] يرث من: " << baseToken.getValue() << "\n";
#endif
        } while (matchAny({TT::COMMA, TT::ARABIC_COMMA})); // Support both commas
    }
    
    // (AR) جسم الصنف مباشرة بدون أقواس - ينتهي بـ 'نهاية'
    // (EN) Class body directly without braces - ends with 'نهاية'
    // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ends with 'نهاية' NOT '}'
    // Note: NO BRACE_LEFT here! Class body is parsed directly
    
    // (AR) تحليل أعضاء الصنف / (EN) Parse class members
    // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ends with 'نهاية'
    StmtList members;
    while (!check(TT::KEYWORD_END) && !isAtEnd()) {
        // (AR) تحليل حقول الصنف: نوع اسم_الحقل [؛]
        // (EN) Parse class fields: type field_name [;]
        
        // Check for visibility modifiers or field declaration
        bool isStatic = false;
        bool isVirtual = false;
        bool isAbstract = false;
        AccessModifier access = parseModifiers(isStatic, isVirtual, isAbstract);
        
        // Check if it's a property (starts with 'خاصية' or 'property')
        if (check(TT::KEYWORD_PROPERTY)) {
            advance(); // consume 'خاصية'
            auto property = parsePropertyDeclaration(access, isStatic);
            if (property) {
                members.push_back(std::move(property));
            }
            continue;
        }
        
        // Check if it's a method (starts with 'دالة' or 'function')
        if (check(TT::KEYWORD_FUNCTION)) {
            advance(); // consume 'دالة'
            auto method = parseMethodDeclaration(access, isStatic, isVirtual, isAbstract);
            if (method) {
                members.push_back(std::move(method));
            }
            continue;
        }
        
        // ───────────────────────────────────────────────────────────────────────
        // (AR) التحقق من دالة الباني - يدعم: باني، منشئ، بناء، أو اسم الصنف()
        // (EN) Check for constructor - supports: باني، منشئ، بناء, or ClassName()
        // ───────────────────────────────────────────────────────────────────────
        if (check(TT::KEYWORD_CONSTRUCTOR) ||
            (check(TT::IDENTIFIER) && current_.getValue() == className && 
             peekNext().getType() == TT::PAREN_LEFT)) {
            
            if (check(TT::KEYWORD_CONSTRUCTOR)) {
                advance(); // (AR) استهلاك كلمة الباني
            } else {
                advance(); // (AR) استهلاك اسم الصنف
            }
            
            auto constructor = parseConstructorDeclaration(className, access);
            if (constructor) {
                members.push_back(std::move(constructor));
            }
            continue;
        }
        
        // Check for destructor (keyword 'مدمر')
        if (check(TT::KEYWORD_DESTRUCTOR)) {
            auto destructor = parseDestructorDeclaration(className, access);
            if (destructor) {
                members.push_back(std::move(destructor));
            }
            continue;
        }
        
        // Check for operator overload (keyword 'عامل')
        // (AR) التحقق من تحميل العامل الزائد / (EN) Check for operator overload
        if (check(TT::KEYWORD_OPERATOR)) {
            advance(); // consume 'عامل'
            auto operatorDecl = parseOperatorDecl();
            if (operatorDecl) {
                // (AR) تعيين معدّل الوصول / (EN) Set access modifier
                if (auto* opDecl = dynamic_cast<AST::OperatorDecl*>(operatorDecl.get())) {
                    opDecl->access = access;
                }
                members.push_back(std::move(operatorDecl));
            }
            continue;
        }
        
        // ─────────────────────────────────────────────────────────────
        // (AR) دعم 'متغير' داخل الصنف: عام متغير اسم = قيمة
        // (EN) Support 'var' inside class: public var name = value
        // ─────────────────────────────────────────────────────────────
        if (check(TT::KEYWORD_VAR)) {
            advance(); // consume 'متغير'
            if (!check(TT::IDENTIFIER)) {
                error("(AR) توقعت اسم حقل بعد 'متغير'. (EN) Expected field name after 'var'.");
                advance();
                continue;
            }
            Token fieldName = peek();
            advance();
            
            ExprPtr initializer = nullptr;
            if (match(TT::OP_ASSIGN)) {
                initializer = parseExpression();
            }
            // (AR) تخطي الفاصلة المنقوطة الاختيارية
            matchAny({TT::SEMICOLON, TT::ARABIC_SEMICOLON});
            
            auto field = std::make_unique<FieldDecl>(
                fieldName.getValue(), Data::DataType::UNKNOWN,
                std::move(initializer), access, isStatic, fieldName.getPosition());
            members.push_back(std::move(field));
            continue;
        }
        
        // Otherwise, parse as field declaration
        // Field syntax: type name [= value] [;]
        // Support both built-in types (نص، رقم) and class types (شخص، حيوان)
        if (isTypeToken(current_.getType()) || 
            (check(TT::IDENTIFIER) && isClassName(current_.getValue()))) {
            auto field = parseFieldDeclaration(access, isStatic);
            if (field) {
                members.push_back(std::move(field));
            }
        } else {
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
    auto* classManager = Data::ClassManager::getInstance();
    if (!classManager->hasClass(className)) {
        auto tempClassType = std::make_unique<Data::ClassType>(className);
        classManager->registerClass(std::move(tempClassType));
#ifdef DEBUG_OOP
        std::cout << "[OOP] ✅ تسجيل مؤقت للصنف: " << className << " (أثناء التحليل)\n";
#endif
    }
    
    // (AR) استخدام جميع الأصناف الأساسية بدلاً من الأول فقط / (EN) Use all base classes instead of just first
    return std::make_unique<ClassDecl>(
        className,
        baseClassNames, // (AR) دعم الوراثة المتعددة الكامل / (EN) Full multiple inheritance support
        std::move(members),
        false,
        nameToken.getPosition()
    );
}

/**
 * @brief (AR) يحلل تصريح متغير بصيغتين:
 *             1. var/let/const name : type = value;
 *             2. type name = value;
 *        (EN) Parses variable declaration in two formats:
 *             1. var/let/const name : type = value;
 *             2. type name = value;
 */
StmtPtr ParserCore::parseVarDecl() {
    Data::DataType varType = Data::DataType::UNKNOWN;
    std::string className = "";  // For class-typed variables
    Token name(TT::IDENTIFIER, "");  // Initialize with default
    
    // Check if we have type-first syntax: TYPE IDENTIFIER = value;
    // (AR) التحقق من صيغة النوع أولاً: نوع معرّف = قيمة;
    if (isTypeToken(current_.getType())) {
        // Format 2: TYPE IDENTIFIER = value;
        // Current token is already a type token (TYPE_INTEGER, TYPE_STRING, etc.)
        // (AR) الصيغة 2: نوع معرّف = قيمة;
        // الرمز الحالي هو بالفعل رمز نوع
        varType = parseType();
        
        // Check if we have an identifier after the type
        // (AR) تحقق مما إذا كان لدينا معرّف بعد النوع
        if (!check(TT::IDENTIFIER)) {
            // Missing identifier after type specification
            // (AR) معرّف مفقود بعد تحديد النوع
            errorBilingual(
                "توقع اسم متغير بعد تحديد النوع. اسم المتغير يجب أن يكون معرّف صحيح.",
                "Expected variable name after type specification. Variable name must be a valid identifier."
            );
            return nullptr;
        }
        
        name = peek();
        advance();
    } else if (check(TT::IDENTIFIER)) {
        // Check if this identifier is a class name (for class-typed variables)
        // (AR) التحقق مما إذا كان هذا المعرّف هو اسم صنف
        if (isClassName(current_.getValue())) {
            // Class-typed variable: ClassName varName = ...;
            // (AR) متغير من نوع صنف: اسم_الصنف اسم_المتغير = ...;
            className = current_.getValue();
            varType = Data::DataType::OBJECT;
            advance();  // Consume class name
            
            // Check if identifier follows the class name
            // (AR) تحقق مما إذا كان معرّف يتبع اسم الصنف
            if (!check(TT::IDENTIFIER)) {
                errorBilingual(
                    "توقع اسم متغير من نوع الصنف '" + className + "'. اسم المتغير يجب أن يكون معرّف صحيح.",
                    "Expected variable name of class type '" + className + "'. Variable name must be a valid identifier."
                );
                return nullptr;
            }
            
            name = peek();
            advance();
        } else {
            // Format 1: var/let/const IDENTIFIER : type = value;
            // or just: IDENTIFIER = value; (type inference)
            // (AR) الصيغة 1: var/let/const معرّف : نوع = قيمة;
            // أو فقط: معرّف = قيمة; (استنتاج النوع)
            name = peek();
            advance();
            
            // Optional type annotation: name : type
            // (AR) تصريح النوع الاختياري: اسم : نوع
            if (match(TT::COLON)) {
                // We have a type annotation, parse it
                // (AR) لدينا تصريح نوع، قم بتحليله
                Data::DataType annotatedType = parseType();
                
                // Check if the type was parsed successfully
                // (AR) تحقق مما إذا تم تحليل النوع بنجاح
                if (annotatedType == Data::DataType::UNKNOWN) {
                    errorBilingual(
                        "نوع غير صحيح أو غير معروف بعد ':' في تصريح المتغير '" + name.getValue() + "'.",
                        "Invalid or unknown type after ':' in variable declaration '" + name.getValue() + "'."
                    );
                    return nullptr;
                }
                
                varType = annotatedType;
            }
        }
    } else {
        // Neither type token nor identifier - this is an error
        // (AR) لا رمز نوع ولا معرّف - هذا خطأ
        Token currentToken = peek();
        
        // Check what kind of token we got instead
        // (AR) تحقق من نوع الرمز الذي حصلنا عليه بدلاً من ذلك
        std::string errorMsg_ar = "صيغة تصريح متغير غير صحيحة.";
        std::string errorMsg_en = "Invalid variable declaration syntax.";
        
        // Provide more helpful error messages based on what we found
        // (AR) قدّم رسائل خطأ أكثر فائدة بناءً على ما وجدنا
        if (currentToken.getType() == TT::SEMICOLON) {
            errorMsg_ar = "لا يمكن تصريح متغير بدون اسم أو نوع.";
            errorMsg_en = "Cannot declare variable without name or type.";
        } else if (currentToken.getType() == TT::BRACE_LEFT || currentToken.getType() == TT::BRACE_RIGHT) {
            errorMsg_ar = "لا يمكن تصريح متغير في هذا الموقع. هل قصدت البحث عن شيء آخر؟";
            errorMsg_en = "Cannot declare variable at this location. Did you mean something else?";
        }
        
        errorBilingual(
            errorMsg_ar,
            errorMsg_en
        );
        return nullptr;
    }
    
    // Optional initializer
    // (AR) المُهيّئ الاختياري
    ExprPtr initializer = nullptr;
    if (match(TT::OP_ASSIGN)) {
        // We have an assignment operator, parse the expression
        // (AR) لدينا عامل إسناد، قم بتحليل التعبير
        initializer = parseExpression();
        
        // Check if expression parsing was successful
        // (AR) تحقق مما إذا كان تحليل التعبير ناجحاً
        if (!initializer) {
            errorBilingual(
                "تعبير غير صحيح في قيمة تهيئة المتغير '" + name.getValue() + "'. تأكد من أن التعبير صحيح.",
                "Invalid expression in variable initializer for '" + name.getValue() + "'. Ensure the expression is correct."
            );
            return nullptr;
        }
    }
    
    // Optional semicolon (support both Arabic and English)
    // (AR) فاصلة منقوطة اختيارية (دعم العربية والإنجليزية)
    if (check(TT::SEMICOLON) || check(TT::ARABIC_SEMICOLON)) {
        advance();
    }
    
    // Create variable declaration statement
    // (AR) إنشاء جملة تصريح المتغير
    return std::make_unique<VarDeclStmt>(
        name.getValue(),
        varType,
        std::move(initializer),
        pendingConst_,
        name.getPosition()
    );
}

/**
 * @brief (AR) يحلل تصريح enum: enum اسم { عضو1، عضو2 }.
 *        (EN) Parses enum declaration: enum name { member1, member2 }.
 */
StmtPtr ParserCore::parseEnumDecl() {
    // Expect enum name
    // (AR) توقع اسم Enum
    if (!check(TT::IDENTIFIER)) {
        errorBilingual(
            "خطأ: توقعت اسم Enum بعد كلمة 'تعداد'. مثال: تعداد اللون",
            "Error: expected enum name after 'enum' keyword. Example: enum Color"
        );
        return nullptr;
    }
    Token name = peek();
    advance();
    
    // (AR) يدعم كلا الصيغتين: تعداد اسم { أعضاء } أو تعداد اسم ... نهاية
    // (EN) Supports both styles: enum name { members } or enum name ... end
    bool useBraces = check(TT::BRACE_LEFT);
    if (useBraces) {
        consume(TT::BRACE_LEFT, "");
    }
    
    // Parse enum members
    // (AR) تحليل أعضاء Enum
    std::vector<EnumMember> members;
    
    // (AR) دالة مساعدة: هل وصلنا نهاية التعداد؟
    auto isEnumEnd = [&]() -> bool {
        if (useBraces) return check(TT::BRACE_RIGHT);
        return check(TT::KEYWORD_END);
    };
    
    // Check for empty enum
    if (isEnumEnd()) {
        errorBilingual(
            "خطأ: تعداد فارغ. يجب أن يحتوي التعداد على عضو واحد على الأقل.",
            "Error: empty enum. Enum must have at least one member."
        );
        advance(); // consume } or نهاية
        return nullptr;
    }
    
    while (!isEnumEnd() && !isAtEnd()) {
        if (!check(TT::IDENTIFIER)) {
            errorBilingual(
                "خطأ: توقعت اسم عضو في Enum. الأعضاء يجب أن تكون معرفات.",
                "Error: expected member name in enum. Members must be identifiers."
            );
            return nullptr;
        }
        Token memberName = peek();
        advance();
        
        ExprPtr value = nullptr;
        if (match(TT::OP_ASSIGN)) {
            // Explicit value
            // (AR) قيمة صريحة
            value = parseExpression();
            if (!value) {
                errorBilingual(
                    "خطأ: قيمة عضو Enum غير صحيحة. يجب أن تكون القيمة تعبيراً صحيحاً.",
                    "Error: invalid enum member value. Value must be a valid expression."
                );
                return nullptr;
            }
        }
        
        members.push_back(EnumMember(memberName.getValue(), std::move(value)));
        
        // (AR) الفاصلة اختيارية بين الأعضاء (الأسطر الجديدة تفصل تلقائياً)
        // (EN) Commas are optional between members (newlines separate automatically)
        if (!isEnumEnd()) {
            if (check(TT::COMMA)) {
                advance(); // consume optional comma
            }
        }
    }
    
    if (!isEnumEnd()) {
        if (useBraces) {
            errorBilingual(
                "خطأ: لم يتم إغلاق Enum. توقعت '}' في النهاية.",
                "Error: enum not closed. Expected '}' at end."
            );
        } else {
            errorBilingual(
                "خطأ: لم يتم إغلاق التعداد. توقعت 'نهاية' في النهاية.",
                "Error: enum not closed. Expected 'end' at end."
            );
        }
        return nullptr;
    }
    
    if (useBraces) {
        consume(TT::BRACE_RIGHT, "");
    } else {
        consume(TT::KEYWORD_END, "");
    }
    
    // Create enum declaration node
    // (AR) إنشاء عقدة تصريح Enum
    return std::make_unique<EnumDecl>(
        name.getValue(),
        std::move(members),
        false,
        name.getPosition()
    );
}

/**
 * @brief (AR) يحلل تصريح بنية: بنية اسم ... نهاية
 *        (EN) Parses struct declaration: struct name ... end
 */
StmtPtr ParserCore::parseStructDecl() {
    // (AR) اسم البنية / (EN) Struct name
    if (!check(TT::IDENTIFIER)) {
        errorBilingual(
            "خطأ: توقعت اسم البنية بعد كلمة 'بنية'. مثال: بنية نقطة",
            "Error: expected struct name after 'struct' keyword. Example: struct Point"
        );
        return nullptr;
    }
    Token name = peek();
    advance();
    
    // (AR) يدعم كلا الصيغتين: بنية اسم { حقول } أو بنية اسم ... نهاية
    // (EN) Supports both: struct name { fields } or struct name ... end
    bool useBraces = check(TT::BRACE_LEFT);
    if (useBraces) {
        consume(TT::BRACE_LEFT, "");
    }
    
    std::vector<StructField> fields;
    StmtList methods;
    
    auto isStructEnd = [&]() -> bool {
        if (useBraces) return check(TT::BRACE_RIGHT);
        return check(TT::KEYWORD_END);
    };
    
    while (!isStructEnd() && !isAtEnd()) {
        // (AR) التحقق من دالة داخل البنية / (EN) Check for method inside struct
        if (check(TT::KEYWORD_FUNCTION)) {
            advance(); // consume 'دالة'
            auto method = parseMethodDeclaration(AST::AccessModifier::PUBLIC, false, false, false);
            if (method) {
                methods.push_back(std::move(method));
            }
            continue;
        }
        
        // (AR) حقل بنية: [نوع] اسم [= قيمة]
        // (EN) Struct field: [type] name [= value]
        Data::DataType fieldType = Data::DataType::UNKNOWN;
        
        // (AR) إذا كان الرمز الحالي نوعاً
        if (isTypeToken(current_.getType())) {
            fieldType = parseType();
        }
        
        if (!check(TT::IDENTIFIER)) {
            errorBilingual(
                "خطأ: توقعت اسم حقل في البنية.",
                "Error: expected field name in struct."
            );
            advance();
            continue;
        }
        
        Token fieldName = peek();
        advance();
        
        ExprPtr defaultValue = nullptr;
        if (match(TT::OP_ASSIGN)) {
            defaultValue = parseExpression();
        }
        
        // (AR) تخطي الفاصلة أو الفاصلة المنقوطة الاختيارية
        matchAny({TT::COMMA, TT::SEMICOLON, TT::ARABIC_SEMICOLON});
        
        fields.push_back(StructField(fieldName.getValue(), fieldType, std::move(defaultValue), true));
    }
    
    if (!isStructEnd()) {
        errorBilingual(
            "خطأ: لم يتم إغلاق البنية. توقعت 'نهاية'.",
            "Error: struct not closed. Expected 'end'."
        );
        return nullptr;
    }
    
    if (useBraces) {
        consume(TT::BRACE_RIGHT, "");
    } else {
        consume(TT::KEYWORD_END, "");
    }
    
    return std::make_unique<StructDecl>(
        name.getValue(),
        std::move(fields),
        std::vector<TypeParameter>{},
        false,
        false,
        name.getPosition()
    );
}

// ═══════════════════════════════════════════════════════════════════════════
// (AR) تحليل تصريح اختبار: اختبر("اسم") ... نهاية / اختبر "اسم" ... نهاية
// (EN) Parse test declaration: test("name") ... end / test "name" ... end
// ═══════════════════════════════════════════════════════════════════════════
StmtPtr ParserCore::parseTestDecl() {
    using TT = Lexer::TokenType;
    auto startPos = previous_.getPosition();
    
    // (AR) اسم الاختبار: نص أو نص بين أقواس
    // (EN) Test name: string or string in parens
    std::string testName;
    
    bool hasParen = match(TT::PAREN_LEFT);
    
    if (check(TT::STRING_LITERAL)) {
        testName = current_.getValue();
        advance();
    } else {
        error("(AR) خطأ نحوي: توقع اسم الاختبار (نص). (EN) Syntax error: expected test name (string).");
        return nullptr;
    }
    
    if (hasParen) {
        if (!match(TT::PAREN_RIGHT)) {
            error("(AR) خطأ نحوي: توقع ')' بعد اسم الاختبار. (EN) Syntax error: expected ')' after test name.");
        }
    }
    
    // (AR) جسم الاختبار حتى 'نهاية'
    // (EN) Test body until 'end'
    AST::StmtList bodyStmts;
    while (!check(TT::KEYWORD_END) && !isAtEnd()) {
        auto stmt = parseDeclaration();
        if (stmt) {
            bodyStmts.push_back(std::move(stmt));
        }
    }
    
    if (!match(TT::KEYWORD_END)) {
        error("(AR) خطأ نحوي: توقع 'نهاية' لإنهاء الاختبار. (EN) Syntax error: expected 'end' to close test.");
    }
    
    auto body = std::make_unique<AST::BlockStmt>(std::move(bodyStmts), startPos);
    return std::make_unique<AST::TestDecl>(testName, std::move(body), false, startPos);
}

/**
 * @brief (AR) يحلل جملة تصدير: صدّر identifier;
 *        (EN) Parses export statement: export identifier;
 */
StmtPtr ParserCore::parseExportStmt() {
    // Parse the declaration to be exported
    // (AR) تحليل التصريح المُصدّر
    auto declaration = parseDeclaration();
    if (!declaration) {
        errorBilingual(
            "خطأ: فشل تحليل التصريح المُصدّر. يجب أن يتبع 'صدّر' تصريح صحيح (دالة، صنف، إلخ).",
            "Error: failed to parse export declaration. 'export' must be followed by a valid declaration (function, class, etc.)."
        );
        return nullptr;
    }
    
    // Create export statement node
    // (AR) إنشاء عقدة جملة التصدير
    return std::make_unique<ExportStmt>(
        std::move(declaration),
        previous().getPosition()
    );
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
StmtPtr ParserCore::parseTraitDecl() {
    // (AR) اسم الواجهة
    Token nameToken = consume(TT::IDENTIFIER,
        "(AR) توقع اسم الواجهة/السمة بعد 'سمة'. (EN) Expected trait name after 'trait'.");
    std::string traitName = nameToken.getValue();
    
    // (AR) وراثة واجهات أخرى (اختياري)
    std::vector<std::string> superTraits;
    if (matchAny({TT::COLON, TT::KEYWORD_INHERITS})) {
        do {
            Token superToken = consume(TT::IDENTIFIER,
                "(AR) توقع اسم الواجهة الأساسية. (EN) Expected parent trait name.");
            superTraits.push_back(superToken.getValue());
        } while (matchAny({TT::COMMA, TT::ARABIC_COMMA}));
    }
    
    // (AR) تحليل دوال الواجهة
    std::vector<AST::TraitMethod> methods;
    while (!check(TT::KEYWORD_END) && !isAtEnd()) {
        // (AR) تخطي الفواصل المنقوطة
        while (matchAny({TT::SEMICOLON, TT::ARABIC_SEMICOLON})) {}
        if (check(TT::KEYWORD_END)) break;
        
        // (AR) توقع 'دالة'
        if (match(TT::KEYWORD_FUNCTION)) {
            // (AR) نوع الإرجاع (اختياري قبل الاسم)
            Data::DataType returnType = Data::DataType::NONE;
            
            // (AR) التحقق من نوع الإرجاع
            if (check(TT::TYPE_INTEGER) || check(TT::TYPE_DOUBLE) || 
                check(TT::TYPE_STRING) || check(TT::TYPE_BOOLEAN)) {
                if (match(TT::TYPE_INTEGER)) returnType = Data::DataType::INTEGER;
                else if (match(TT::TYPE_DOUBLE)) returnType = Data::DataType::FLOAT;
                else if (match(TT::TYPE_STRING)) returnType = Data::DataType::STRING;
                else if (match(TT::TYPE_BOOLEAN)) returnType = Data::DataType::BOOLEAN;
            }
            
            // (AR) اسم الدالة
            Token methodName = consume(TT::IDENTIFIER,
                "(AR) توقع اسم الدالة في السمة. (EN) Expected method name in trait.");
            
            // (AR) المعاملات
            consume(TT::PAREN_LEFT, "(AR) توقع '('. (EN) Expected '('.");
            std::vector<AST::Parameter> params;
            if (!check(TT::PAREN_RIGHT)) {
                do {
                    Data::DataType pType = Data::DataType::UNKNOWN;
                    if (check(TT::TYPE_INTEGER)) { advance(); pType = Data::DataType::INTEGER; }
                    else if (check(TT::TYPE_DOUBLE)) { advance(); pType = Data::DataType::FLOAT; }
                    else if (check(TT::TYPE_STRING)) { advance(); pType = Data::DataType::STRING; }
                    else if (check(TT::TYPE_BOOLEAN)) { advance(); pType = Data::DataType::BOOLEAN; }
                    
                    Token pName = consume(TT::IDENTIFIER, "(AR) توقع اسم المعامل. (EN) Expected parameter name.");
                    params.push_back(AST::Parameter(pName.getValue(), pType));
                } while (matchAny({TT::COMMA, TT::ARABIC_COMMA}));
            }
            consume(TT::PAREN_RIGHT, "(AR) توقع ')'. (EN) Expected ')'.");
            
            // (AR) جسم افتراضي (اختياري) — إذا لم يكن هناك 'نهاية' مباشرة
            StmtPtr defaultImpl = nullptr;
            
            methods.emplace_back(methodName.getValue(), std::move(params), returnType, std::move(defaultImpl));
        } else {
            // (AR) تخطي عنصر غير معروف
            advance();
        }
    }
    
    consume(TT::KEYWORD_END,
        "(AR) توقع 'نهاية' بعد جسم السمة. (EN) Expected 'end' after trait body.");
    
    return std::make_unique<AST::TraitDecl>(traitName, std::move(methods),
        std::vector<AST::TypeParameter>{}, std::move(superTraits), false, nameToken.getPosition());
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
StmtPtr ParserCore::parseImplDecl() {
    // (AR) يمكن أن يكون: نفّذ سمة لـ صنف  أو  نفّذ صنف
    Token firstToken = consume(TT::IDENTIFIER,
        "(AR) توقع اسم السمة أو الصنف بعد 'نفّذ'. (EN) Expected trait/class name after 'impl'.");
    
    std::string traitName;
    std::string targetType;
    
    // (AR) فحص كلمة 'لـ' (for)
    if (check(TT::IDENTIFIER) && (previous().getValue() != "")) {
        // (AR) نبحث عن 'لـ' أو 'ل'
        Token nextTok = peek();
        if (nextTok.getValue() == "\xd9\x84\xd9\x80" || nextTok.getValue() == "\xd9\x84" || 
            nextTok.getValue() == "for") {
            traitName = firstToken.getValue();
            advance(); // skip لـ/ل/for
            Token target = consume(TT::IDENTIFIER,
                "(AR) توقع اسم الصنف بعد 'لـ'. (EN) Expected class name after 'for'.");
            targetType = target.getValue();
        } else {
            // (AR) نفّذ صنف (بدون سمة)
            targetType = firstToken.getValue();
        }
    } else {
        targetType = firstToken.getValue();
    }
    
    // (AR) تحليل الدوال
    StmtList methods;
    while (!check(TT::KEYWORD_END) && !isAtEnd()) {
        while (matchAny({TT::SEMICOLON, TT::ARABIC_SEMICOLON})) {}
        if (check(TT::KEYWORD_END)) break;
        
        if (match(TT::KEYWORD_FUNCTION)) {
            // (AR) match() يستهلك 'دالة' — parseFunctionDecl يتوقع أنه مستهلك بالفعل
            // (EN) match() consumes 'دالة' — parseFunctionDecl expects it already consumed
            methods.push_back(parseFunctionDecl());
        } else {
            advance();
        }
    }
    
    consume(TT::KEYWORD_END,
        "(AR) توقع 'نهاية' بعد كتلة التنفيذ. (EN) Expected 'end' after impl block.");
    
    return std::make_unique<AST::ImplDecl>(traitName, targetType, std::move(methods),
        std::vector<AST::TypeParameter>{}, firstToken.getPosition());
}

} // namespace Parser
} // namespace Sad
