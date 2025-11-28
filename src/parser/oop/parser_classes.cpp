// ======================================================================
// parser_classes.cpp - تنفيذ محلل الأصناف / Class Parser Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ جميع دوال تحليل الأصناف والبرمجة الكائنية
//   يشمل تحليل الأصناف، الحقول، الدوال، البانيات، والتعبيرات
//
// English Description:
//   Implementation of all OOP parsing functions
//   Includes class, field, method, constructor, and expression parsing
// ======================================================================

#include "parser/parser_classes.h"
#include "data/managers/class_manager.h"
#include "errors/error_manager.h"
#include <iostream>

namespace Sad {
namespace Parser {

using namespace AST;
using namespace Lexer;

// ======================================================================
// تحليل تصريح صنف / Parse Class Declaration
// ======================================================================

std::unique_ptr<ClassDeclaration> ParserClasses::parseClassDeclaration() {
    // (AR) تحليل: صنف <اسم> [يرث <اسم_الأساس>] { ... }
    // (EN) Parse: class <name> [inherits <base_name>] { ... }
    
    // (AR) تخطي كلمة "صنف"
    // (EN) Skip "class" keyword
    expect(TokenType::CLASS);
    
    // (AR) اسم الصنف
    // (EN) Class name
    Token classNameToken = expect(TokenType::IDENTIFIER);
    std::string className = classNameToken.value;
    
    // (AR) الوراثة (اختياري)
    // (EN) Inheritance (optional)
    std::string baseClassName;
    if (match(TokenType::INHERITS)) {
        advance();
        Token baseNameToken = expect(TokenType::IDENTIFIER);
        baseClassName = baseNameToken.value;
    }
    
    // (AR) الجسم
    // (EN) Body
    expect(TokenType::LBRACE);
    
    auto classDecl = std::make_unique<ClassDeclaration>();
    classDecl->className = className;
    classDecl->baseClassName = baseClassName;
    
    // (AR) تحليل الأعضاء
    // (EN) Parse members
    while (!match(TokenType::RBRACE) && !isAtEnd()) {
        // (AR) تحديد نوع العضو
        // (EN) Determine member type
        
        Visibility visibility = Visibility::PUBLIC;
        bool isStatic = false;
        bool isVirtual = false;
        bool isAbstract = false;
        
        // (AR) تحليل المُعدِّلات
        // (EN) Parse modifiers
        parseModifiers(visibility, isStatic, isVirtual, isAbstract);
        
        // (AR) تحديد نوع العضو
        // (EN) Determine member type
        if (match(TokenType::CONSTRUCTOR)) {
            // (AR) باني
            // (EN) Constructor
            auto ctor = parseConstructorDeclaration(visibility);
            classDecl->constructors.push_back(std::move(ctor));
        }
        else if (match(TokenType::DESTRUCTOR)) {
            // (AR) هادم
            // (EN) Destructor
            auto dtor = parseDestructorDeclaration();
            classDecl->destructor = std::move(dtor);
        }
        else if (match(TokenType::FUNCTION)) {
            // (AR) دالة
            // (EN) Method
            auto method = parseMethodDeclaration(visibility, isStatic, isVirtual, isAbstract);
            classDecl->methods.push_back(std::move(method));
        }
        else {
            // (AR) حقل
            // (EN) Field
            auto field = parseFieldDeclaration(visibility, isStatic);
            classDecl->fields.push_back(std::move(field));
        }
    }
    
    expect(TokenType::RBRACE);
    
    return classDecl;
}

// ======================================================================
// تحليل المُعدِّلات / Parse Modifiers
// ======================================================================

void ParserClasses::parseModifiers(Visibility& visibility, bool& isStatic,
                                    bool& isVirtual, bool& isAbstract) {
    // (AR) تحليل المُعدِّلات: عام، خاص، محمي، ثابت، افتراضي، تجريدي
    // (EN) Parse modifiers: public, private, protected, static, virtual, abstract
    
    while (true) {
        if (match(TokenType::PUBLIC)) {
            advance();
            visibility = Visibility::PUBLIC;
        }
        else if (match(TokenType::PRIVATE)) {
            advance();
            visibility = Visibility::PRIVATE;
        }
        else if (match(TokenType::PROTECTED)) {
            advance();
            visibility = Visibility::PROTECTED;
        }
        else if (match(TokenType::STATIC)) {
            advance();
            isStatic = true;
        }
        else if (match(TokenType::VIRTUAL)) {
            advance();
            isVirtual = true;
        }
        else if (match(TokenType::ABSTRACT)) {
            advance();
            isAbstract = true;
        }
        else {
            break;
        }
    }
}

// ======================================================================
// تحليل تصريح حقل / Parse Field Declaration
// ======================================================================

std::unique_ptr<FieldDeclaration> ParserClasses::parseFieldDeclaration(
    Visibility visibility, bool isStatic) {
    // (AR) تحليل: <اسم>: <نوع> [= <قيمة_افتراضية>]
    // (EN) Parse: <name>: <type> [= <default_value>]
    
    auto field = std::make_unique<FieldDeclaration>();
    field->visibility = visibility;
    field->isStatic = isStatic;
    
    // (AR) اسم الحقل
    // (EN) Field name
    Token fieldNameToken = expect(TokenType::IDENTIFIER);
    field->fieldName = fieldNameToken.value;
    
    // (AR) النوع
    // (EN) Type
    expect(TokenType::COLON);
    Token typeToken = expect(TokenType::IDENTIFIER);
    field->typeName = typeToken.value;
    
    // (AR) القيمة الافتراضية (اختياري)
    // (EN) Default value (optional)
    if (match(TokenType::ASSIGN)) {
        advance();
        field->defaultValue = parseExpression();
    }
    
    return field;
}

// ======================================================================
// تحليل تصريح دالة / Parse Method Declaration
// ======================================================================

std::unique_ptr<MethodDeclaration> ParserClasses::parseMethodDeclaration(
    Visibility visibility, bool isStatic, bool isVirtual, bool isAbstract) {
    // (AR) تحليل: دالة <اسم>(<معاملات>) [-> <نوع_الإرجاع>] { ... }
    // (EN) Parse: function <name>(<params>) [-> <return_type>] { ... }
    
    expect(TokenType::FUNCTION);
    
    auto method = std::make_unique<MethodDeclaration>();
    method->visibility = visibility;
    method->isStatic = isStatic;
    method->isVirtual = isVirtual;
    method->isAbstract = isAbstract;
    
    // (AR) اسم الدالة
    // (EN) Method name
    Token methodNameToken = expect(TokenType::IDENTIFIER);
    method->methodName = methodNameToken.value;
    
    // (AR) المعاملات
    // (EN) Parameters
    expect(TokenType::LPAREN);
    
    if (!match(TokenType::RPAREN)) {
        do {
            // (AR) اسم المعامل
            // (EN) Parameter name
            Token paramNameToken = expect(TokenType::IDENTIFIER);
            std::string paramName = paramNameToken.value;
            
            // (AR) نوع المعامل
            // (EN) Parameter type
            expect(TokenType::COLON);
            Token paramTypeToken = expect(TokenType::IDENTIFIER);
            std::string paramType = paramTypeToken.value;
            
            method->parameters.push_back({paramName, paramType});
            
        } while (match(TokenType::COMMA) && advance());
    }
    
    expect(TokenType::RPAREN);
    
    // (AR) نوع الإرجاع (اختياري)
    // (EN) Return type (optional)
    if (match(TokenType::ARROW)) {
        advance();
        Token returnTypeToken = expect(TokenType::IDENTIFIER);
        method->returnTypeName = returnTypeToken.value;
    }
    
    // (AR) الجسم (إذا لم تكن تجريدية)
    // (EN) Body (if not abstract)
    if (!isAbstract) {
        method->body = parseBlockStatement();
    }
    
    return method;
}

// ======================================================================
// تحليل تصريح باني / Parse Constructor Declaration
// ======================================================================

std::unique_ptr<ConstructorDeclaration> ParserClasses::parseConstructorDeclaration(
    Visibility visibility) {
    // (AR) تحليل: باني(<معاملات>) { ... }
    // (EN) Parse: constructor(<params>) { ... }
    
    expect(TokenType::CONSTRUCTOR);
    
    auto ctor = std::make_unique<ConstructorDeclaration>();
    ctor->visibility = visibility;
    
    // (AR) المعاملات
    // (EN) Parameters
    expect(TokenType::LPAREN);
    
    if (!match(TokenType::RPAREN)) {
        do {
            // (AR) اسم المعامل
            // (EN) Parameter name
            Token paramNameToken = expect(TokenType::IDENTIFIER);
            std::string paramName = paramNameToken.value;
            
            // (AR) نوع المعامل
            // (EN) Parameter type
            expect(TokenType::COLON);
            Token paramTypeToken = expect(TokenType::IDENTIFIER);
            std::string paramType = paramTypeToken.value;
            
            ctor->parameters.push_back({paramName, paramType});
            
        } while (match(TokenType::COMMA) && advance());
    }
    
    expect(TokenType::RPAREN);
    
    // (AR) الجسم
    // (EN) Body
    ctor->body = parseBlockStatement();
    
    return ctor;
}

// ======================================================================
// تحليل تصريح هادم / Parse Destructor Declaration
// ======================================================================

std::unique_ptr<DestructorDeclaration> ParserClasses::parseDestructorDeclaration() {
    // (AR) تحليل: هادم() { ... }
    // (EN) Parse: destructor() { ... }
    
    expect(TokenType::DESTRUCTOR);
    
    auto dtor = std::make_unique<DestructorDeclaration>();
    
    // (AR) الأقواس
    // (EN) Parentheses
    expect(TokenType::LPAREN);
    expect(TokenType::RPAREN);
    
    // (AR) الجسم
    // (EN) Body
    dtor->body = parseBlockStatement();
    
    return dtor;
}

// ======================================================================
// تحليل تعبير إنشاء كائن / Parse New Expression
// ======================================================================

std::unique_ptr<NewExpression> ParserClasses::parseNewExpression() {
    // (AR) تحليل: جديد <اسم_الصنف>(<معاملات_الباني>)
    // (EN) Parse: new <class_name>(<constructor_args>)
    
    expect(TokenType::NEW);
    
    auto newExpr = std::make_unique<NewExpression>();
    
    // (AR) اسم الصنف
    // (EN) Class name
    Token classNameToken = expect(TokenType::IDENTIFIER);
    newExpr->className = classNameToken.value;
    
    // (AR) معاملات الباني
    // (EN) Constructor arguments
    expect(TokenType::LPAREN);
    
    if (!match(TokenType::RPAREN)) {
        do {
            newExpr->constructorArgs.push_back(parseExpression());
        } while (match(TokenType::COMMA) && advance());
    }
    
    expect(TokenType::RPAREN);
    
    return newExpr;
}

// ======================================================================
// تحليل تعبير الوصول للعضو / Parse Member Access Expression
// ======================================================================

std::unique_ptr<MemberAccessExpression> ParserClasses::parseMemberAccessExpression(
    std::unique_ptr<Expression> object) {
    // (AR) تحليل: <كائن>.<عضو>
    // (EN) Parse: <object>.<member>
    
    expect(TokenType::DOT);
    
    auto memberAccess = std::make_unique<MemberAccessExpression>();
    memberAccess->object = std::move(object);
    
    // (AR) اسم العضو
    // (EN) Member name
    Token memberNameToken = expect(TokenType::IDENTIFIER);
    memberAccess->memberName = memberNameToken.value;
    
    return memberAccess;
}

// ======================================================================
// تحليل تعبير استدعاء دالة / Parse Method Call Expression
// ======================================================================

std::unique_ptr<MethodCallExpression> ParserClasses::parseMethodCallExpression(
    std::unique_ptr<Expression> object) {
    // (AR) تحليل: <كائن>.<دالة>(<معاملات>)
    // (EN) Parse: <object>.<method>(<args>)
    
    expect(TokenType::DOT);
    
    auto methodCall = std::make_unique<MethodCallExpression>();
    methodCall->object = std::move(object);
    
    // (AR) اسم الدالة
    // (EN) Method name
    Token methodNameToken = expect(TokenType::IDENTIFIER);
    methodCall->methodName = methodNameToken.value;
    
    // (AR) المعاملات
    // (EN) Arguments
    expect(TokenType::LPAREN);
    
    if (!match(TokenType::RPAREN)) {
        do {
            methodCall->arguments.push_back(parseExpression());
        } while (match(TokenType::COMMA) && advance());
    }
    
    expect(TokenType::RPAREN);
    
    return methodCall;
}

// ======================================================================
// تحليل تعبير هذا / Parse This Expression
// ======================================================================

std::unique_ptr<ThisExpression> ParserClasses::parseThisExpression() {
    // (AR) تحليل: هذا
    // (EN) Parse: this
    
    expect(TokenType::THIS);
    
    return std::make_unique<ThisExpression>();
}

// ======================================================================
// تحليل تعبير الأساس / Parse Super Expression
// ======================================================================

std::unique_ptr<SuperExpression> ParserClasses::parseSuperExpression() {
    // (AR) تحليل: الأساس
    // (EN) Parse: super
    
    expect(TokenType::SUPER);
    
    return std::make_unique<SuperExpression>();
}

// ======================================================================
// دوال مساعدة / Helper Functions
// ======================================================================

Token ParserClasses::expect(TokenType type) {
    // (AR) توقع نوع رمز معين
    // (EN) Expect specific token type
    
    if (!match(type)) {
        Token current = currentToken();
        std::string message_ar = "توقعت " + tokenTypeToString(type);
        message_ar += " لكن وجدت " + tokenTypeToString(current.type);
        std::string message_en = "Expected " + tokenTypeToString(type);
        message_en += " but found " + tokenTypeToString(current.type);
        
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
            Sad::Errors::SourceLocation(current.filename, current.line, current.column),
            message_ar,
            message_en
        );
        
        // (AR) إرجاع رمز خطأ بدلاً من رمي استثناء
        // (EN) Return error token instead of throwing
        return current;
    }
    
    Token token = currentToken();
    advance();
    return token;
}

bool ParserClasses::match(TokenType type) const {
    // (AR) فحص نوع الرمز الحالي
    // (EN) Check current token type
    
    return currentToken().type == type;
}

Token ParserClasses::currentToken() const {
    // (AR) الرمز الحالي
    // (EN) Current token
    
    return tokens_[currentIndex_];
}

Token ParserClasses::advance() {
    // (AR) التقدم للرمز التالي
    // (EN) Advance to next token
    
    if (!isAtEnd()) {
        currentIndex_++;
    }
    return tokens_[currentIndex_ - 1];
}

bool ParserClasses::isAtEnd() const {
    // (AR) فحص ما إذا وصلنا لنهاية الرموز
    // (EN) Check if at end of tokens
    
    return currentIndex_ >= tokens_.size() || 
           currentToken().type == TokenType::END_OF_FILE;
}

std::string ParserClasses::tokenTypeToString(TokenType type) const {
    // (AR) تحويل نوع الرمز إلى نص
    // (EN) Convert token type to string
    
    // (AR) يمكن إضافة قاموس كامل هنا
    // (EN) Can add complete dictionary here
    switch (type) {
        case TokenType::CLASS: return "صنف";
        case TokenType::INHERITS: return "يرث";
        case TokenType::CONSTRUCTOR: return "باني";
        case TokenType::DESTRUCTOR: return "هادم";
        case TokenType::FUNCTION: return "دالة";
        case TokenType::NEW: return "جديد";
        case TokenType::THIS: return "هذا";
        case TokenType::SUPER: return "الأساس";
        case TokenType::PUBLIC: return "عام";
        case TokenType::PRIVATE: return "خاص";
        case TokenType::PROTECTED: return "محمي";
        case TokenType::STATIC: return "ثابت";
        case TokenType::VIRTUAL: return "افتراضي";
        case TokenType::ABSTRACT: return "تجريدي";
        default: return "<رمز>";
    }
}

// ======================================================================
// دوال التحليل المساعدة / Helper Parsing Functions
// ======================================================================

std::unique_ptr<Expression> ParserClasses::parseExpression() {
    // (AR) تحليل تعبير عام
    // (EN) Parse general expression
    
    // (AR) ملاحظة: هذه دالة بسيطة، يجب ربطها بالمحلل الرئيسي
    // (EN) Note: This is simplified, should connect to main parser
    
    // (AR) فحص التعبيرات الخاصة بالأصناف
    // (EN) Check class-specific expressions
    if (match(TokenType::NEW)) {
        return parseNewExpression();
    }
    else if (match(TokenType::THIS)) {
        return parseThisExpression();
    }
    else if (match(TokenType::SUPER)) {
        return parseSuperExpression();
    }
    
    // (AR) تحليل تعبير عادي (يجب الربط بالمحلل الرئيسي)
    // (EN) Parse regular expression (should connect to main parser)
    auto expr = parsePrimaryExpression();
    
    // (AR) فحص الوصول للأعضاء واستدعاء الدوال
    // (EN) Check member access and method calls
    while (match(TokenType::DOT)) {
        size_t savedIndex = currentIndex_;
        advance(); // تخطي النقطة / Skip dot
        
        if (match(TokenType::IDENTIFIER)) {
            Token memberToken = currentToken();
            advance();
            
            if (match(TokenType::LPAREN)) {
                // (AR) استدعاء دالة
                // (EN) Method call
                currentIndex_ = savedIndex; // إعادة للنقطة / Back to dot
                expr = parseMethodCallExpression(std::move(expr));
            }
            else {
                // (AR) وصول لحقل
                // (EN) Field access
                currentIndex_ = savedIndex; // إعادة للنقطة / Back to dot
                expr = parseMemberAccessExpression(std::move(expr));
            }
        }
    }
    
    return expr;
}

std::unique_ptr<Expression> ParserClasses::parsePrimaryExpression() {
    // (AR) تحليل تعبير أولي
    // (EN) Parse primary expression
    
    // (AR) ملاحظة: دالة بسيطة، يجب توسيعها
    // (EN) Note: Simplified, should be expanded
    
    if (match(TokenType::IDENTIFIER)) {
        Token idToken = advance();
        // (AR) إنشاء تعبير متغير (يجب إضافة VariableExpression)
        // (EN) Create variable expression (should add VariableExpression)
        // return std::make_unique<VariableExpression>(idToken.value);
    }
    
    // (AR) تعبير غير مدعوم - إبلاغ ErrorManager بدلاً من رمي استثناء
    // (EN) Unsupported expression - report to ErrorManager instead of throwing
    Token current = currentToken();
    Sad::Errors::ErrorManager::getInstance().reportError(
        Sad::Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
        Sad::Errors::SourceLocation(current.filename, current.line, current.column),
        "تعبير أولي غير مدعوم",
        "Unsupported primary expression"
    );
    
    // (AR) إرجاع nullptr بدلاً من رمي استثناء
    // (EN) Return nullptr instead of throwing
    return nullptr;
}

std::unique_ptr<Statement> ParserClasses::parseBlockStatement() {
    // (AR) تحليل جملة كتلة
    // (EN) Parse block statement
    
    // (AR) ملاحظة: دالة بسيطة، يجب الربط بالمحلل الرئيسي
    // (EN) Note: Simplified, should connect to main parser
    
    expect(TokenType::LBRACE);
    
    // (AR) تحليل الجمل داخل الكتلة
    // (EN) Parse statements inside block
    // auto block = std::make_unique<BlockStatement>();
    
    while (!match(TokenType::RBRACE) && !isAtEnd()) {
        // (AR) تحليل جملة
        // (EN) Parse statement
        // block->statements.push_back(parseStatement());
        
        // (AR) مؤقتاً: تخطي
        // (EN) Temporarily: skip
        advance();
    }
    
    expect(TokenType::RBRACE);
    
    // return block;
    return nullptr; // مؤقتاً / Temporarily
}

} // namespace Parser
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
