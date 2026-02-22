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

namespace Sad {
namespace Parser {

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
AccessModifier ParserCore::parseModifiers(bool& isStatic, bool& isVirtual, bool& isAbstract) {
    AccessModifier access = AccessModifier::PUBLIC; // (AR) افتراضياً عام / (EN) Default public
    
    // (AR) تحليل جميع المعدلات / (EN) Parse all modifiers
    while (true) {
        if (match(TT::KEYWORD_PUBLIC)) {
            access = AccessModifier::PUBLIC;
        }
        else if (match(TT::KEYWORD_PRIVATE)) {
            access = AccessModifier::PRIVATE;
        }
        else if (match(TT::KEYWORD_PROTECTED)) {
            access = AccessModifier::PROTECTED;
        }
        else if (match(TT::KEYWORD_STATIC)) {
            isStatic = true;
        }
        else if (match(TT::KEYWORD_ABSTRACT)) {
            isAbstract = true;
            isVirtual = true; // (AR) المجرد هو افتراضي / (EN) Abstract implies virtual
        }
        else {
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
std::unique_ptr<FieldDecl> ParserCore::parseFieldDeclaration(AccessModifier access, bool isStatic) {
#ifdef DEBUG_OOP
    std::cout << "[OOP] تحليل حقل\n";
#endif
    
    // (AR) النوع / (EN) Type
    // Check if current token is a type token
    if (!isTypeToken(current_.getType())) {
        error("(AR) توقع نوع الحقل (رقم، نص، منطقي، إلخ). (EN) Expected field type (number, string, boolean, etc).");
        synchronize();
        return nullptr;
    }
    
    Token typeToken = current_;
    advance(); // consume type
    std::string typeName = typeToken.getValue();
    Data::DataType fieldType = mapTokenTypeToDataType(typeToken.getType());
    
    // (AR) الاسم / (EN) Name
    Token nameToken = consume(TT::IDENTIFIER,
        "(AR) توقع اسم الحقل. (EN) Expected field name.");
    std::string fieldName = nameToken.getValue();
    
#ifdef DEBUG_OOP
    std::cout << "[OOP] حقل: " << typeName << " " << fieldName << "\n";
#endif
    
    // (AR) القيمة الافتراضية (اختياري) / (EN) Default value (optional)
    ExprPtr initializer = nullptr;
    if (match(TT::OP_ASSIGN)) {
        initializer = parseExpression();
    }
    
    // (AR) فاصلة منقوطة اختيارية (دعم العربية والإنجليزية)
    // (EN) Optional semicolon (support both Arabic and English)
    // Spec: docs\language_spec\rules\03_oop.md §1 - field_decl ::= ... [(';' | '؛')]?
    if (matchAny({TT::SEMICOLON, TT::ARABIC_SEMICOLON})) {
        // Semicolon consumed
    }
    
    return std::make_unique<FieldDecl>(fieldName, fieldType, std::move(initializer), 
                                        access, isStatic, nameToken.getPosition());
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
    AccessModifier access, bool isStatic, bool isVirtual, bool isAbstract) {
    
#ifdef DEBUG_OOP
    std::cout << "[OOP] تحليل طريقة\n";
#endif
    
    // (AR) نوع الإرجاع (اختياري - يأتي قبل اسم الطريقة)
    // (EN) Return type (optional - comes BEFORE method name)
    // Spec: docs\language_spec\rules\03_oop.md §1 - method_decl ::= ... 'دالة' [type] IDENTIFIER ...
    Data::DataType returnType = Data::DataType::NONE;
    
    // Check if next token is a type (keyword like رقم، نص) or identifier (for method name)
    Token nameToken = current_; // Initialize with current for line number tracking
    if (isTypeToken(current_.getType())) {
        // Has return type: رقم احصل_الرصيد()
        returnType = parseType();
        nameToken = consume(TT::IDENTIFIER,
            "(AR) توقع اسم الطريقة بعد نوع الإرجاع. (EN) Expected method name after return type.");
    } else {
        // No return type (void method): احصل_الرصيد()
        nameToken = consume(TT::IDENTIFIER,
            "(AR) توقع اسم الطريقة. (EN) Expected method name.");
    }
    
    std::string methodName = nameToken.getValue();
#ifdef DEBUG_OOP
    std::cout << "[OOP] طريقة: " << methodName << " (نوع الإرجاع: " 
              << static_cast<int>(returnType) << ")\n";
#endif
    
    // (AR) المعاملات / (EN) Parameters
    consume(TT::PAREN_LEFT,
        "(AR) توقع '(' بعد اسم الطريقة. (EN) Expected '(' after method name.");
    
    std::vector<Parameter> parameters;
    if (!check(TT::PAREN_RIGHT)) {
        do {
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
            Data::DataType paramType = Data::DataType::OBJECT;
            
            if (isTypeToken(current_.getType())) {
                // (AR) نوع صريح مدمج (رقم، نص، منطقي، إلخ)
                paramType = parseType();
                Token paramToken = consume(TT::IDENTIFIER,
                    "(AR) توقع اسم المعامل. (EN) Expected parameter name.");
                parameters.push_back(Parameter(paramToken.getValue(), paramType, nullptr));
            } else if (check(TT::IDENTIFIER)) {
                // (AR) التحقق: هل هذا معرّف متبوع بمعرّف آخر؟ (أي: نوع_صنف اسم)
                // (EN) Check: is this identifier followed by another? (i.e. class_type name)
                Token firstToken = current_;
                // (AR) نتحقق من الرمز التالي: إذا كان معرّفاً أيضاً فهذا "نوع اسم"
                if (peekNext().getType() == TT::IDENTIFIER) {
                    // (AR) صيغة: نوع_صنف اسم_المعامل (مثل: شخص ش)
                    advance(); // (AR) استهلاك اسم النوع
                    Token paramToken = current_;
                    advance(); // (AR) استهلاك اسم المعامل
                    parameters.push_back(Parameter(paramToken.getValue(), Data::DataType::OBJECT, nullptr));
                } else {
                    // (AR) صيغة: اسم_المعامل فقط (بدون نوع)
                    advance();
                    parameters.push_back(Parameter(firstToken.getValue(), paramType, nullptr));
                }
            } else {
                // (AR) نوع المعامل / (EN) Parameter type
                paramType = parseType();
                
                // (AR) اسم المعامل / (EN) Parameter name
                Token paramToken = consume(TT::IDENTIFIER,
                    "(AR) توقع اسم المعامل. (EN) Expected parameter name.");
                
                parameters.push_back(Parameter(paramToken.getValue(), paramType, nullptr));
            }
            
            // Spec: docs\language_spec\rules\03_oop.md §1 - param_list ::= param ((',' | '،') param)*
        } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));
    }
    
    consume(TT::PAREN_RIGHT,
        "(AR) توقع ')' بعد معاملات الطريقة. (EN) Expected ')' after method parameters.");
    
    // (AR) التحقق من عدم استخدام { } في تعريف الطريقة
    // (EN) Check for incorrect { } usage in method definition
    if (!isAbstract && check(TT::BRACE_LEFT)) {
        error(
            "(AR) ❌ خطأ نحوي: لا يمكن استخدام '{' في تعريف الطرق!\n"
            "في لغة ص، الطرق لا تستخدم الأقواس المعقوفة { }.\n"
            "يجب أن تبدأ جسم الطريقة مباشرة وتنتهي بكلمة 'نهاية'.\n\n"
            "❌ خطأ:\n"
            "  طريقة " + methodName + "() {\n"
            "    # الكود\n"
            "  }\n\n"
            "✅ صحيح:\n"
            "  طريقة " + methodName + "()\n"
            "    # الكود\n"
            "  نهاية\n\n"
            "(EN) ❌ Syntax error: Cannot use '{' in method definition!\n"
            "In Sad language, methods do not use curly braces { }.\n"
            "Method body must start directly and end with 'نهاية' keyword.\n"
        );
    }
    
    // (AR) جسم الطريقة / (EN) Method body
    StmtPtr body = nullptr;
    if (isAbstract) {
        // (AR) طريقة مجردة - بدون جسم، فاصلة منقوطة اختيارية
        // (EN) Abstract method - no body, optional semicolon
        matchAny({TT::SEMICOLON, TT::ARABIC_SEMICOLON});
    } else {
        // (AR) طريقة عادية - جسم كامل / (EN) Regular method - full body
        body = parseBlockStmt();
    }
    
    bool isOverride = false; 
    // (AR) ملاحظة: override غير مدعوم حالياً - سيتم إضافته لاحقاً
    // (EN) Note: override not currently supported - will be added later
    return std::make_unique<MethodDecl>(methodName, std::move(parameters), returnType,
                                         std::move(body), access, isStatic, isVirtual,
                                         isOverride, isAbstract, nameToken.getPosition());
}

// ======================================================================
// (AR) تحليل تصريح باني / (EN) Parse Constructor Declaration
// ======================================================================

/**
 * @brief (AR) يحلل تصريح باني في الصنف
 *        (EN) Parses constructor declaration in class
 * 
 * الصيغة / Syntax: ('باني' | 'منشئ') '(' [param_list] ')' [':' initializer_list] block
 * Spec: docs\language_spec\rules\03_oop.md §1 - constructor_decl
 * Note: Supports both 'باني' and 'منشئ' keywords
 */
std::unique_ptr<ConstructorDecl> ParserCore::parseConstructorDeclaration(const std::string& className, AccessModifier access) {
#ifdef DEBUG_OOP
    std::cout << "[OOP] تحليل باني: " << className << "\n";
#endif
    
    // Constructor name should match class name
    // (AR) المعاملات / (EN) Parameters
    consume(TT::PAREN_LEFT,
        "(AR) توقع '(' بعد اسم الباني. (EN) Expected '(' after constructor name.");
    
    std::vector<Parameter> parameters;
    if (!check(TT::PAREN_RIGHT)) {
        do {
            // (AR) نوع المعامل (اختياري) / (EN) Parameter type (optional)
            // (AR) ندعم صيغتين: باني(رقم س) أو باني(س) بدون نوع
            // (EN) Support two forms: constructor(int x) or constructor(x) without type
            Data::DataType paramType = Data::DataType::OBJECT;
            
            if (isTypeToken(current_.getType())) {
                // (AR) نوع صريح موجود / (EN) Explicit type present
                paramType = parseType();
                Token paramToken = consume(TT::IDENTIFIER,
                    "(AR) توقع اسم المعامل. (EN) Expected parameter name.");
                parameters.push_back(Parameter(paramToken.getValue(), paramType, nullptr));
            } else if (check(TT::IDENTIFIER)) {
                // (AR) لا يوجد نوع صريح - نعتبره OBJECT (ديناميكي)
                // (EN) No explicit type - treat as OBJECT (dynamic)
                // هذا يدعم أصناف القوالب حيث نوع المعامل هو معامل قالب
                Token paramToken = current_;
                advance();
                parameters.push_back(Parameter(paramToken.getValue(), paramType, nullptr));
            } else {
                error("(AR) توقع نوع أو اسم المعامل. (EN) Expected parameter type or name.");
                break;
            }
            
        } while (matchAny({TT::COMMA, TT::ARABIC_COMMA}));
    }
    
    consume(TT::PAREN_RIGHT,
        "(AR) توقع ')' بعد معاملات الباني. (EN) Expected ')' after constructor parameters.");
    
    // (AR) التحقق من عدم استخدام { } في تعريف الباني
    // (EN) Check for incorrect { } usage in constructor definition
    if (check(TT::BRACE_LEFT)) {
        error(
            "(AR) ❌ خطأ نحوي: لا يمكن استخدام '{' في تعريف الباني!\n"
            "في لغة ص، البناة لا تستخدم الأقواس المعقوفة { }.\n"
            "يجب أن يبدأ جسم الباني مباشرة ويختم بكلمة 'نهاية'.\n\n"
            "❌ خطأ:\n"
            "  باني " + className + "() {\n"
            "    # الكود\n"
            "  }\n\n"
            "✅ صحيح:\n"
            "  باني " + className + "()\n"
            "    # الكود\n"
            "  نهاية\n\n"
            "(EN) ❌ Syntax error: Cannot use '{' in constructor definition!\n"
            "In Sad language, constructors do not use curly braces { }.\n"
            "Constructor body must start directly and end with 'نهاية' keyword.\n"
        );
    }
    
    // (AR) قائمة التهيئة (اختياري): : الأساس(args)
    // (EN) Initializer list (optional): : super(args)
    // Spec: docs\language_spec\rules\03_oop.md - constructor can call base constructor
    // Syntax: باني(params) : الأساس(arg1, arg2)
    ExprList superArgs;
    if (match(TT::COLON)) {
        // (AR) توقع كلمة "الأساس" أو "super"
        // (EN) Expect "الأساس" or "super" keyword
        if (!match(TT::KEYWORD_SUPER)) {
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
                "Example: constructor(int x) : super(x)"
            );
        }
        
        // (AR) تحليل معاملات الباني الأساسي: (arg1, arg2, ...)
        // (EN) Parse super constructor arguments: (arg1, arg2, ...)
        consume(TT::PAREN_LEFT,
            "(AR) توقع '(' بعد 'الأساس'. (EN) Expected '(' after 'super'.");
        
        if (!check(TT::PAREN_RIGHT)) {
            do {
                superArgs.push_back(parseExpression());
            } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));
        }
        
        consume(TT::PAREN_RIGHT,
            "(AR) توقع ')' بعد معاملات الباني الأساسي. "
            "(EN) Expected ')' after super constructor arguments.");
    }
    
    // (AR) التحقق من استدعاء أساس() في بداية جسم الباني (بدون نقطتين)
    // (EN) Check for super() call at the start of constructor body (without colon)
    // يدعم: أساس(args) أو الأساس(args) كأول تعليمة في الباني
    if (superArgs.empty() && check(TT::KEYWORD_SUPER)) {
        advance(); // consume 'أساس' / 'الأساس'
        if (check(TT::PAREN_LEFT)) {
            advance(); // consume '('
            if (!check(TT::PAREN_RIGHT)) {
                do {
                    superArgs.push_back(parseExpression());
                } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));
            }
            consume(TT::PAREN_RIGHT,
                "(AR) توقع ')' بعد معاملات أساس(). "
                "(EN) Expected ')' after super() arguments.");
        }
    }
    
    // (AR) جسم الباني / (EN) Constructor body
    StmtPtr body = parseBlockStmt();
    
    return std::make_unique<ConstructorDecl>(std::move(parameters), std::move(body),
                                              std::move(superArgs));
}

// ======================================================================
// (AR) تحليل تصريح هدام / (EN) Parse Destructor Declaration
// ======================================================================

/**
 * @brief (AR) يحلل تصريح هدام في الصنف
 *        (EN) Parses destructor declaration in class
 * 
 * الصيغة / Syntax: ('~باني' | '~منشئ' | 'مدمر') '()' block
 * Spec: docs\language_spec\rules\03_oop.md §1,4 - destructor_decl
 * Note: Supports three forms: '~باني', '~منشئ', and 'مدمر'
 */
std::unique_ptr<DestructorDecl> ParserCore::parseDestructorDeclaration(const std::string& className, AccessModifier access) {
#ifdef DEBUG_OOP
    std::cout << "[OOP] تحليل هدام: " << className << "\n";
#endif
    
    // (AR) لا يوجد معاملات / (EN) No parameters
    consume(TT::PAREN_LEFT,
        "(AR) توقع '()' بعد 'هادم'. (EN) Expected '()' after 'destructor'.");
    consume(TT::PAREN_RIGHT,
        "(AR) توقع ')' بعد '('. (EN) Expected ')' after '('.");
    
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
ExprPtr ParserCore::parseNewExpr() {
#ifdef DEBUG_OOP
    std::cout << "[OOP] تحليل تعبير جديد (new)\n";
#endif
    
    // (AR) اسم الصنف / (EN) Class name
    Token classToken = consume(TT::IDENTIFIER,
        "(AR) توقع اسم الصنف بعد 'جديد'. (EN) Expected class name after 'new'.");
    
    std::string className = classToken.getValue();
    
    // (AR) دعم القوالب: جديد صنف<نوع>(معاملات)
    // (EN) Template support: new Class<Type>(args)
    std::vector<Data::DataType> templateArgs;
    if (check(TT::OP_LESS)) {
        advance(); // consume '<'
        do {
            if (isTypeToken(current_.getType())) {
                templateArgs.push_back(mapTokenTypeToDataType(current_.getType()));
                advance();
            } else if (check(TT::IDENTIFIER)) {
                // (AR) نوع مخصص - نعتبره OBJECT
                // (EN) Custom type - treat as OBJECT
                templateArgs.push_back(Data::DataType::OBJECT);
                advance();
            } else {
                error("(AR) توقع نوع في معاملات القالب. (EN) Expected type in template arguments.");
                break;
            }
        } while (matchAny({TT::COMMA, TT::ARABIC_COMMA}));
        
        if (!match(TT::OP_GREATER)) {
            error("(AR) توقع '>' لإنهاء معاملات القالب. (EN) Expected '>' to close template arguments.");
        }
    }
    
#ifdef DEBUG_OOP
    std::cout << "[OOP] إنشاء كائن من صنف: " << className;
    if (!templateArgs.empty()) std::cout << " (قالب بـ " << templateArgs.size() << " نوع)";
    std::cout << "\n";
#endif
    
    // (AR) معاملات الباني / (EN) Constructor arguments
    consume(TT::PAREN_LEFT,
        "(AR) توقع '(' بعد اسم الصنف. (EN) Expected '(' after class name.");
    
    ExprList arguments;
    if (!check(TT::PAREN_RIGHT)) {
        do {
            arguments.push_back(parseExpression());
        } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));
    }
    
#ifdef DEBUG_OOP
    std::cout << "[OOP] عدد معاملات الباني: " << arguments.size() << "\n";
#endif
    
    consume(TT::PAREN_RIGHT,
        "(AR) توقع ')' بعد معاملات الباني. (EN) Expected ')' after constructor arguments.");
    
    // إنشاء عقدة NewExpr / Create NewExpr node
    auto newExpr = std::make_unique<NewExpr>(className);
    newExpr->arguments = std::move(arguments);
    newExpr->templateArguments = std::move(templateArgs);
    
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
ExprPtr ParserCore::parseThisExpression() {
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
ExprPtr ParserCore::parseSuperExpression() {
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
std::unique_ptr<PropertyDecl> ParserCore::parsePropertyDeclaration(AccessModifier access, bool isStatic) {
#ifdef DEBUG_OOP
    std::cout << "[OOP] تحليل خاصية (property)\n";
#endif
    
    // (AR) النوع (اختياري) / (EN) Type (optional)
    // (AR) إذا لم يكن الرمز الحالي نوعاً، نستخدم UNKNOWN
    // (EN) If current token is not a type, use UNKNOWN
    Data::DataType propertyType = Data::DataType::UNKNOWN;
    if (isTypeToken(current_.getType())) {
        propertyType = parseType();
    }
    
    // (AR) اسم الخاصية / (EN) Property name
    Token nameToken = consume(TT::IDENTIFIER,
        "(AR) توقع اسم الخاصية. (EN) Expected property name.");
    std::string propertyName = nameToken.getValue();
    
#ifdef DEBUG_OOP
    std::cout << "[OOP] خاصية: " << propertyName << " (نوع: " 
              << static_cast<int>(propertyType) << ")\n";
#endif
    
    // (AR) كتلة القراءة (getter) - إلزامية / (EN) Getter block - required
    std::unique_ptr<GetterBlock> getter = nullptr;
    if (!matchAny({TT::KEYWORD_GET})) {
        error("(AR) توقع 'احصل' (get) بعد اسم الخاصية. (EN) Expected 'احصل' (get) after property name.");
        synchronize();
        return nullptr;
    }
    
    // Parse getter body - statements until 'نهاية'
    StmtList getterStatements;
    while (!check(TT::KEYWORD_END) && !isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) {
            getterStatements.push_back(std::move(stmt));
        }
    }
    consume(TT::KEYWORD_END,
        "(AR) توقع 'نهاية' في نهاية كتلة getter. (EN) Expected 'نهاية' at end of getter block.");
    
    auto getterBody = std::make_unique<BlockStmt>(
        std::move(getterStatements),
        nameToken.getPosition()
    );
    getter = std::make_unique<GetterBlock>(std::move(getterBody));
    
    // (AR) كتلة الكتابة (setter) - اختيارية / (EN) Setter block - optional
    std::unique_ptr<SetterBlock> setter = nullptr;
    if (matchAny({TT::KEYWORD_SET})) {
        // (AR) معامل setter / (EN) Setter parameter
        consume(TT::PAREN_LEFT,
            "(AR) توقع '(' بعد 'عيّن'. (EN) Expected '(' after 'set'.");
        
        Token paramToken = consume(TT::IDENTIFIER,
            "(AR) توقع اسم معامل setter. (EN) Expected setter parameter name.");
        std::string paramName = paramToken.getValue();
        
        consume(TT::PAREN_RIGHT,
            "(AR) توقع ')' بعد معامل setter. (EN) Expected ')' after setter parameter.");
        
        // Parse setter body - statements until 'نهاية'
        StmtList setterStatements;
        while (!check(TT::KEYWORD_END) && !isAtEnd()) {
            auto stmt = parseStatement();
            if (stmt) {
                setterStatements.push_back(std::move(stmt));
            }
        }
        consume(TT::KEYWORD_END,
            "(AR) توقع 'نهاية' في نهاية كتلة setter. (EN) Expected 'نهاية' at end of setter block.");
        
        auto setterBody = std::make_unique<BlockStmt>(
            std::move(setterStatements),
            nameToken.getPosition()
        );
        setter = std::make_unique<SetterBlock>(
            paramName,
            propertyType,
            std::move(setterBody)
        );
    }
    
    // (AR) نهاية الخاصية / (EN) End of property
    consume(TT::KEYWORD_END,
        "(AR) توقع 'نهاية' في نهاية تصريح الخاصية. (EN) Expected 'نهاية' at end of property declaration.");
    
#ifdef DEBUG_OOP
    std::cout << "[OOP] انتهى تحليل خاصية '" << propertyName << "'\n";
#endif
    
    return std::make_unique<PropertyDecl>(
        propertyName,
        propertyType,
        std::move(getter),
        std::move(setter),
        access,
        isStatic
    );
}

} // namespace Parser
} // namespace Sad
