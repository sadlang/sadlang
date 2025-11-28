# 🏗️ المرحلة 1: الأصناف الأساسية / Phase 1: Basic Classes

**المدة:** 5 أيام  
**الأولوية:** P0 (حرجة)  
**الحالة:** قادمة

---

## 🎯 الأهداف / Objectives

### (AR) الأهداف الرئيسية
1. تنفيذ تعريف الأصناف الأساسية
2. دعم الخصائص (Fields) بأنواعها المختلفة
3. تنفيذ الطرق (Methods) الأساسية
4. دعم البواني (Constructors)
5. تكامل كامل مع Lexer وParser

### (EN) Main Objectives
1. Implement basic class definitions
2. Support fields with different types
3. Implement basic methods
4. Support constructors
5. Full integration with Lexer and Parser

---

## 📋 المهام التفصيلية / Detailed Tasks

### اليوم 1: تحديث Lexer (8 ساعات)

#### المهمة 1.1: إضافة كلمات مفتاحية جديدة (3 ساعات)

**الملف:** `include/lexer/token.h`

```cpp
// إضافة أنواع رموز جديدة في enum TokenType
enum class TokenType {
    // ... الرموز الموجودة ...
    
    // كلمات OOP المفتاحية
    KEYWORD_CLASS,          // صنف
    KEYWORD_NEW,            // جديد
    KEYWORD_THIS,           // هذا
    KEYWORD_CONSTRUCTOR,    // باني
    KEYWORD_CONSTRUCTOR_ALT,// منشئ (بديل)
    KEYWORD_DESTRUCTOR,     // مدمر
    KEYWORD_DESTRUCTOR_ALT1,// ~باني (بديل 1)
    KEYWORD_DESTRUCTOR_ALT2,// ~منشئ (بديل 2)
    KEYWORD_INHERITS,       // يرث
    
    // رموز الوصول
    KEYWORD_PUBLIC,         // عام
    KEYWORD_PRIVATE,        // خاص
    KEYWORD_PROTECTED,      // محمي
    KEYWORD_STATIC,         // ثابت
    
    // رموز إضافية
    DOT,                    // . (للوصول للأعضاء)
    ARROW,                  // -> (اختياري)
    DOUBLE_COLON,           // :: (للأعضاء الثابتة)
    
    // ... بقية الرموز ...
};
```

**الملف:** `src/lexer/lexer_keywords.cpp`

```cpp
void LexerKeywords::initializeKeywords() {
    // ... الكلمات الموجودة ...
    
    // كلمات OOP
    keywords["صنف"] = TokenType::KEYWORD_CLASS;
    keywords["class"] = TokenType::KEYWORD_CLASS;  // دعم إنجليزي اختياري
    
    keywords["جديد"] = TokenType::KEYWORD_NEW;
    keywords["new"] = TokenType::KEYWORD_NEW;
    
    keywords["هذا"] = TokenType::KEYWORD_THIS;
    keywords["this"] = TokenType::KEYWORD_THIS;
    
    keywords["باني"] = TokenType::KEYWORD_CONSTRUCTOR;
    keywords["منشئ"] = TokenType::KEYWORD_CONSTRUCTOR_ALT;
    keywords["constructor"] = TokenType::KEYWORD_CONSTRUCTOR;
    
    keywords["مدمر"] = TokenType::KEYWORD_DESTRUCTOR;
    keywords["~باني"] = TokenType::KEYWORD_DESTRUCTOR_ALT1;
    keywords["~منشئ"] = TokenType::KEYWORD_DESTRUCTOR_ALT2;
    keywords["destructor"] = TokenType::KEYWORD_DESTRUCTOR;
    
    keywords["يرث"] = TokenType::KEYWORD_INHERITS;
    keywords["inherits"] = TokenType::KEYWORD_INHERITS;
    
    // كلمات الوصول
    keywords["عام"] = TokenType::KEYWORD_PUBLIC;
    keywords["public"] = TokenType::KEYWORD_PUBLIC;
    
    keywords["خاص"] = TokenType::KEYWORD_PRIVATE;
    keywords["private"] = TokenType::KEYWORD_PRIVATE;
    
    keywords["محمي"] = TokenType::KEYWORD_PROTECTED;
    keywords["protected"] = TokenType::KEYWORD_PROTECTED;
    
    keywords["ثابت"] = TokenType::KEYWORD_STATIC;
    keywords["static"] = TokenType::KEYWORD_STATIC;
}
```

#### المهمة 1.2: تحديث معالجة الرموز الخاصة (2 ساعات)

**الملف:** `src/lexer/lexer_operators.cpp`

```cpp
Token LexerCore::scanOperator() {
    char current = advance();
    
    switch (current) {
        case '.':
            return makeToken(TokenType::DOT, ".");
            
        case ':':
            // التحقق من :: للأعضاء الثابتة
            if (peek() == ':') {
                advance();
                return makeToken(TokenType::DOUBLE_COLON, "::");
            }
            return makeToken(TokenType::COLON, ":");
            
        case '-':
            // التحقق من -> (سهم)
            if (peek() == '>') {
                advance();
                return makeToken(TokenType::ARROW, "->");
            }
            // ... معالجة باقي حالات -
            break;
            
        // ... باقي الرموز ...
    }
}
```

#### المهمة 1.3: اختبارات Lexer (3 ساعات)

**الملف:** `tests/lexer_tests/oop_keywords_test.cpp`

```cpp
#include <gtest/gtest.h>
#include "lexer/lexer_core.h"

TEST(OOPLexerTest, RecognizeClassKeyword) {
    std::string source = "صنف شخص\nنهاية";
    LexerCore lexer(source);
    auto tokens = lexer.tokenize();
    
    ASSERT_GE(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_CLASS);
    EXPECT_EQ(tokens[0].lexeme, "صنف");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "شخص");
}

TEST(OOPLexerTest, RecognizeInheritsKeyword) {
    std::string source = "صنف كلب يرث حيوان\nنهاية";
    LexerCore lexer(source);
    auto tokens = lexer.tokenize();
    
    ASSERT_GE(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_CLASS);
    EXPECT_EQ(tokens[2].type, TokenType::KEYWORD_INHERITS);
}

TEST(OOPLexerTest, RecognizeVisibilityKeywords) {
    std::string source = "عام خاص محمي";
    LexerCore lexer(source);
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(tokens.size(), 4);  // 3 كلمات + EOF
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_PUBLIC);
    EXPECT_EQ(tokens[1].type, TokenType::KEYWORD_PRIVATE);
    EXPECT_EQ(tokens[2].type, TokenType::KEYWORD_PROTECTED);
}

TEST(OOPLexerTest, RecognizeMemberAccessOperator) {
    std::string source = "كائن.خاصية";
    LexerCore lexer(source);
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(tokens.size(), 4);  // ID . ID EOF
    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].type, TokenType::DOT);
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
}

TEST(OOPLexerTest, RecognizeStaticMemberOperator) {
    std::string source = "صنف::خاصية_ثابتة";
    LexerCore lexer(source);
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[1].type, TokenType::DOUBLE_COLON);
}
```

#### المخرجات - اليوم 1
- [ ] جميع الكلمات المفتاحية الجديدة مضافة
- [ ] الرموز الخاصة (. : ::) تعمل بشكل صحيح
- [ ] 10+ اختبار Lexer يمر بنجاح

---

### اليوم 2: إنشاء عقد AST للأصناف (8 ساعات)

#### المهمة 2.1: تعريف عقد الصنف (3 ساعات)

**الملف:** `include/parser/ast/class_nodes.h`

```cpp
#pragma once
#include "ast_node.h"
#include "ast_statements.h"
#include "ast_expressions.h"
#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace AST {

// رؤية العضو
enum class Visibility {
    PUBLIC,
    PRIVATE,
    PROTECTED
};

// معامل
struct Parameter {
    std::string name;
    Type* type;
    std::unique_ptr<Expr> defaultValue;
};

// تهيئة خاصية في الباني
struct FieldInitializer {
    std::string fieldName;
    std::unique_ptr<Expr> value;
};

// تعريف خاصية
class FieldDecl : public ASTNode {
public:
    Visibility visibility;
    Type* type;
    std::string name;
    std::unique_ptr<Expr> initializer;
    bool isStatic;
    
    FieldDecl(Visibility vis, Type* t, const std::string& n)
        : visibility(vis), type(t), name(n), isStatic(false) {}
    
    NodeType getType() const override { return NodeType::FIELD_DECL; }
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// تعريف طريقة
class MethodDecl : public ASTNode {
public:
    Visibility visibility;
    std::string name;
    Type* returnType;  // يأتي بعد 'دالة' وقبل اسم الطريقة
    std::vector<Parameter> parameters;
    std::unique_ptr<BlockStmt> body;
    bool isStatic;
    bool isVirtual;
    bool isAbstract;
    
    MethodDecl(Visibility vis, const std::string& n, Type* ret = nullptr)
        : visibility(vis), name(n), returnType(ret), 
          isStatic(false), isVirtual(false), isAbstract(false) {}
    
    NodeType getType() const override { return NodeType::METHOD_DECL; }
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// تعريف باني
class ConstructorDecl : public ASTNode {
public:
    std::vector<Parameter> parameters;
    std::vector<FieldInitializer> initializers;
    std::unique_ptr<BlockStmt> body;
    
    ConstructorDecl() = default;
    
    NodeType getType() const override { return NodeType::CONSTRUCTOR_DECL; }
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// تعريف هدام
class DestructorDecl : public ASTNode {
public:
    std::unique_ptr<BlockStmt> body;
    
    DestructorDecl() = default;
    
    NodeType getType() const override { return NodeType::DESTRUCTOR_DECL; }
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// تعريف صنف
class ClassDeclStmt : public Statement {
public:
    std::string name;
    std::vector<std::string> baseClasses;
    std::vector<std::unique_ptr<FieldDecl>> fields;
    std::unique_ptr<ConstructorDecl> constructor;
    std::unique_ptr<DestructorDecl> destructor;
    std::vector<std::unique_ptr<MethodDecl>> methods;
    
    explicit ClassDeclStmt(const std::string& n)
        : name(n) {}
    
    NodeType getType() const override { return NodeType::CLASS_DECL; }
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// إنشاء كائن
class NewExpr : public Expression {
public:
    std::string className;
    std::vector<std::unique_ptr<Expr>> arguments;
    
    explicit NewExpr(const std::string& cls)
        : className(cls) {}
    
    NodeType getType() const override { return NodeType::NEW_EXPR; }
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// الوصول لعضو
class MemberAccessExpr : public Expression {
public:
    std::unique_ptr<Expr> object;
    std::string memberName;
    
    MemberAccessExpr(std::unique_ptr<Expr> obj, const std::string& member)
        : object(std::move(obj)), memberName(member) {}
    
    NodeType getType() const override { return NodeType::MEMBER_ACCESS; }
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// استدعاء طريقة
class MethodCallExpr : public Expression {
public:
    std::unique_ptr<Expr> object;
    std::string methodName;
    std::vector<std::unique_ptr<Expr>> arguments;
    
    MethodCallExpr(std::unique_ptr<Expr> obj, const std::string& method)
        : object(std::move(obj)), methodName(method) {}
    
    NodeType getType() const override { return NodeType::METHOD_CALL; }
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

// this (هذا)
class ThisExpr : public Expression {
public:
    ThisExpr() = default;
    
    NodeType getType() const override { return NodeType::THIS_EXPR; }
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

} // namespace AST
} // namespace Sad
```

#### المهمة 2.2: تنفيذ عقد AST (2 ساعات)

**الملف:** `src/parser/ast/class_nodes.cpp`

```cpp
#include "parser/ast/class_nodes.h"
#include "parser/ast/ast_visitor.h"

namespace Sad {
namespace AST {

// FieldDecl
void FieldDecl::accept(ASTVisitor& visitor) {
    visitor.visitFieldDecl(this);
}

std::string FieldDecl::toString() const {
    std::string vis = (visibility == Visibility::PUBLIC) ? "عام" :
                      (visibility == Visibility::PRIVATE) ? "خاص" : "محمي";
    return vis + " " + name + ": " + type->toString();
}

// MethodDecl
void MethodDecl::accept(ASTVisitor& visitor) {
    visitor.visitMethodDecl(this);
}

std::string MethodDecl::toString() const {
    std::string sig = name + "(";
    for (size_t i = 0; i < parameters.size(); i++) {
        if (i > 0) sig += ", ";
        sig += parameters[i].name + ": " + parameters[i].type->toString();
    }
    sig += ")";
    if (returnType) {
        sig += " -> " + returnType->toString();
    }
    return sig;
}

// ConstructorDecl
void ConstructorDecl::accept(ASTVisitor& visitor) {
    visitor.visitConstructorDecl(this);
}

std::string ConstructorDecl::toString() const {
    return "باني(...)";
}

// DestructorDecl
void DestructorDecl::accept(ASTVisitor& visitor) {
    visitor.visitDestructorDecl(this);
}

std::string DestructorDecl::toString() const {
    return "مدمر()";
}

// ClassDeclStmt
void ClassDeclStmt::accept(ASTVisitor& visitor) {
    visitor.visitClassDeclStmt(this);
}

std::string ClassDeclStmt::toString() const {
    std::string result = "صنف " + name;
    if (!baseClasses.empty()) {
        result += " يرث ";
        for (size_t i = 0; i < baseClasses.size(); i++) {
            if (i > 0) result += ", ";
            result += baseClasses[i];
        }
    }
    return result;
}

// NewExpr
void NewExpr::accept(ASTVisitor& visitor) {
    visitor.visitNewExpr(this);
}

std::string NewExpr::toString() const {
    return className + "(...)";
}

// MemberAccessExpr
void MemberAccessExpr::accept(ASTVisitor& visitor) {
    visitor.visitMemberAccessExpr(this);
}

std::string MemberAccessExpr::toString() const {
    return object->toString() + "." + memberName;
}

// MethodCallExpr
void MethodCallExpr::accept(ASTVisitor& visitor) {
    visitor.visitMethodCallExpr(this);
}

std::string MethodCallExpr::toString() const {
    return object->toString() + "." + methodName + "(...)";
}

// ThisExpr
void ThisExpr::accept(ASTVisitor& visitor) {
    visitor.visitThisExpr(this);
}

std::string ThisExpr::toString() const {
    return "هذا";
}

} // namespace AST
} // namespace Sad
```

#### المهمة 2.3: تحديث ASTVisitor (3 ساعات)

**الملف:** `include/parser/ast/ast_visitor.h`

```cpp
#pragma once

namespace Sad {
namespace AST {

// Forward declarations
class FieldDecl;
class MethodDecl;
class ConstructorDecl;
class DestructorDecl;
class ClassDeclStmt;
class NewExpr;
class MemberAccessExpr;
class MethodCallExpr;
class ThisExpr;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // ... زيارات موجودة ...
    
    // زيارات OOP الجديدة
    virtual void visitFieldDecl(FieldDecl* node) = 0;
    virtual void visitMethodDecl(MethodDecl* node) = 0;
    virtual void visitConstructorDecl(ConstructorDecl* node) = 0;
    virtual void visitDestructorDecl(DestructorDecl* node) = 0;
    virtual void visitClassDeclStmt(ClassDeclStmt* node) = 0;
    virtual void visitNewExpr(NewExpr* node) = 0;
    virtual void visitMemberAccessExpr(MemberAccessExpr* node) = 0;
    virtual void visitMethodCallExpr(MethodCallExpr* node) = 0;
    virtual void visitThisExpr(ThisExpr* node) = 0;
};

} // namespace AST
} // namespace Sad
```

#### المخرجات - اليوم 2
- [ ] جميع عقد AST للأصناف معرفة
- [ ] تنفيذ الدوال الأساسية
- [ ] ASTVisitor محدث
- [ ] البناء ينجح بدون أخطاء

---

### اليوم 3: تنفيذ Parser للأصناف (8 ساعات)

#### المهمة 3.1: تحليل تعريف الصنف (4 ساعات)

**الملف:** `src/parser/parser_classes.cpp`

```cpp
#include "parser/parser_core.h"
#include "parser/ast/class_nodes.h"

namespace Sad {
namespace Parser {

std::unique_ptr<AST::ClassDeclStmt> ParserCore::parseClassDecl() {
    // صنف
    consume(TokenType::KEYWORD_CLASS, "توقع 'صنف'");
    
    // اسم الصنف
    Token className = consume(TokenType::IDENTIFIER, "توقع اسم الصنف");
    
    auto classDecl = std::make_unique<AST::ClassDeclStmt>(className.lexeme);
    
    // الوراثة (اختياري)
    // دعم كل من ':' و 'يرث'
    if (match(TokenType::COLON) || match(TokenType::KEYWORD_INHERITS)) {
        do {
            Token baseName = consume(TokenType::IDENTIFIER, "توقع اسم الصنف الأساس");
            classDecl->baseClasses.push_back(baseName.lexeme);
        } while (match(TokenType::COMMA) || match(TokenType::ARABIC_COMMA));
    }
    
    // جسم الصنف (بدون أقواس - مباشرةً)
    AST::Visibility currentVisibility = AST::Visibility::PUBLIC;  // افتراضياً عام
    
    while (!check(TokenType::KEYWORD_END) && !isAtEnd()) {
        // فحص تغيير الرؤية
        if (match(TokenType::KEYWORD_PUBLIC)) {
            currentVisibility = AST::Visibility::PUBLIC;
            match(TokenType::COLON);  // : اختيارية
            continue;
        }
        if (match(TokenType::KEYWORD_PRIVATE)) {
            currentVisibility = AST::Visibility::PRIVATE;
            match(TokenType::COLON);
            continue;
        }
        if (match(TokenType::KEYWORD_PROTECTED)) {
            currentVisibility = AST::Visibility::PROTECTED;
            match(TokenType::COLON);
            continue;
        }
        
        // تحليل أعضاء الصنف
        if (check(TokenType::KEYWORD_CONSTRUCTOR) || 
            check(TokenType::KEYWORD_CONSTRUCTOR_ALT)) {
            classDecl->constructor = parseConstructor();
        }
        else if (check(TokenType::KEYWORD_DESTRUCTOR) ||
                 check(TokenType::KEYWORD_DESTRUCTOR_ALT1) ||
                 check(TokenType::KEYWORD_DESTRUCTOR_ALT2)) {
            classDecl->destructor = parseDestructor();
        }
        else if (check(TokenType::KEYWORD_FUNC)) {
            classDecl->methods.push_back(parseMethod(currentVisibility));
        }
        else {
            classDecl->fields.push_back(parseField(currentVisibility));
        }
    }
    
    consume(TokenType::KEYWORD_END, "توقع 'نهاية' في نهاية الصنف");
    
    return classDecl;
}

std::unique_ptr<AST::FieldDecl> ParserCore::parseField(AST::Visibility vis) {
    // فحص ثابت (static)
    bool isStatic = match(TokenType::KEYWORD_STATIC);
    
    // نوع الخاصية
    Type* fieldType = parseType();
    
    // اسم الخاصية
    Token fieldName = consume(TokenType::IDENTIFIER, "توقع اسم الخاصية");
    
    auto field = std::make_unique<AST::FieldDecl>(vis, fieldType, fieldName.lexeme);
    field->isStatic = isStatic;
    
    // قيمة افتراضية (اختياري)
    if (match(TokenType::ASSIGN)) {
        field->initializer = expression();
    }
    
    // فاصلة منقوطة اختيارية
    match(TokenType::SEMICOLON);
    match(TokenType::ARABIC_SEMICOLON);
    
    return field;
}

std::unique_ptr<AST::MethodDecl> ParserCore::parseMethod(AST::Visibility vis) {
    // فحص ثابت (static)
    bool isStatic = match(TokenType::KEYWORD_STATIC);
    
    // دالة
    consume(TokenType::KEYWORD_FUNC, "توقع 'دالة'");
    
    // نوع الإرجاع (اختياري ويأتي قبل اسم الطريقة)
    Type* returnType = nullptr;
    if (isTypeKeyword(peek())) {
        returnType = parseType();
    }
    
    // اسم الطريقة
    Token methodName = consume(TokenType::IDENTIFIER, "توقع اسم الطريقة");
    
    auto method = std::make_unique<AST::MethodDecl>(vis, methodName.lexeme, returnType);
    method->isStatic = isStatic;
    
    // المعاملات
    consume(TokenType::LEFT_PAREN, "توقع '('");
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            method->parameters.push_back(parseParameter());
        } while (match(TokenType::COMMA) || match(TokenType::ARABIC_COMMA));
    }
    
    consume(TokenType::RIGHT_PAREN, "توقع ')'");
    
    // جسم الطريقة
    method->body = parseBlock();
    
    return method;
}

std::unique_ptr<AST::ConstructorDecl> ParserCore::parseConstructor() {
    // باني أو منشئ
    if (!match(TokenType::KEYWORD_CONSTRUCTOR) && 
        !match(TokenType::KEYWORD_CONSTRUCTOR_ALT)) {
        error("توقع 'باني' أو 'منشئ'");
    }
    
    auto ctor = std::make_unique<AST::ConstructorDecl>();
    
    // المعاملات
    consume(TokenType::LEFT_PAREN, "توقع '('");
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            ctor->parameters.push_back(parseParameter());
        } while (match(TokenType::COMMA) || match(TokenType::ARABIC_COMMA));
    }
    
    consume(TokenType::RIGHT_PAREN, "توقع ')'");
    
    // قائمة التهيئة (اختياري)
    if (match(TokenType::COLON)) {
        do {
            Token fieldName = consume(TokenType::IDENTIFIER, "توقع اسم الخاصية");
            consume(TokenType::LEFT_PAREN, "توقع '('");
            auto initExpr = expression();
            consume(TokenType::RIGHT_PAREN, "توقع ')'");
            
            AST::FieldInitializer init;
            init.fieldName = fieldName.lexeme;
            init.value = std::move(initExpr);
            ctor->initializers.push_back(std::move(init));
        } while (match(TokenType::COMMA) || match(TokenType::ARABIC_COMMA));
    }
    
    // جسم الباني
    ctor->body = parseBlock();
    
    return ctor;
}

std::unique_ptr<AST::DestructorDecl> ParserCore::parseDestructor() {
    // مدمر أو ~باني أو ~منشئ
    if (!match(TokenType::KEYWORD_DESTRUCTOR) &&
        !match(TokenType::KEYWORD_DESTRUCTOR_ALT1) &&
        !match(TokenType::KEYWORD_DESTRUCTOR_ALT2)) {
        error("توقع 'مدمر' أو '~باني' أو '~منشئ'");
    }
    
    auto dtor = std::make_unique<AST::DestructorDecl>();
    
    consume(TokenType::LEFT_PAREN, "توقع '('");
    consume(TokenType::RIGHT_PAREN, "توقع ')'");
    
    dtor->body = parseBlock();
    
    return dtor;
}

} // namespace Parser
} // namespace Sad
```

#### المهمة 3.2: تحليل إنشاء كائن (2 ساعات)

```cpp
std::unique_ptr<AST::NewExpr> ParserCore::parseNewExpr() {
    // جديد أو new
    consume(TokenType::KEYWORD_NEW, "توقع 'جديد'");
    
    // اسم الصنف
    Token className = consume(TokenType::IDENTIFIER, "توقع اسم الصنف");
    
    auto newExpr = std::make_unique<AST::NewExpr>(className.lexeme);
    
    // معاملات الباني
    consume(TokenType::LEFT_PAREN, "توقع '('");
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            newExpr->arguments.push_back(expression());
        } while (match(TokenType::COMMA) || match(TokenType::ARABIC_COMMA));
    }
    
    consume(TokenType::RIGHT_PAREN, "توقع ')'");
    
    return newExpr;
}
```

#### المهمة 3.3: تحليل الوصول للأعضاء (2 ساعات)

```cpp
std::unique_ptr<AST::Expr> ParserCore::parseMemberAccess() {
    auto expr = parsePrimary();
    
    while (match(TokenType::DOT)) {
        Token memberName = consume(TokenType::IDENTIFIER, "توقع اسم العضو");
        
        // تحقق: هل هي طريقة أم خاصية؟
        if (check(TokenType::LEFT_PAREN)) {
            // استدعاء طريقة
            consume(TokenType::LEFT_PAREN);
            
            auto methodCall = std::make_unique<AST::MethodCallExpr>(
                std::move(expr), memberName.lexeme);
            
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    methodCall->arguments.push_back(expression());
                } while (match(TokenType::COMMA) || match(TokenType::ARABIC_COMMA));
            }
            
            consume(TokenType::RIGHT_PAREN, "توقع ')'");
            expr = std::move(methodCall);
        }
        else {
            // وصول لخاصية
            expr = std::make_unique<AST::MemberAccessExpr>(
                std::move(expr), memberName.lexeme);
        }
    }
    
    return expr;
}
```

#### المخرجات - اليوم 3
- [ ] تحليل تعريف الصنف يعمل
- [ ] تحليل الخصائص والطرق يعمل
- [ ] تحليل البواني والهدامات يعمل
- [ ] تحليل إنشاء الكائنات يعمل
- [ ] تحليل الوصول للأعضاء يعمل

---

### اليوم 4-5: نظام البيانات (16 ساعة)

#### المهمة 4.1: تنفيذ ClassType (6 ساعات)

**الملف:** `src/data/types/class_type.cpp`

```cpp
#include "data/types/class_type.h"
#include <algorithm>

namespace Sad {
namespace Data {

ClassType::ClassType(const std::string& name)
    : name_(name), baseClass_(nullptr) {}

std::string ClassType::getName() const {
    return name_;
}

void ClassType::addField(const FieldDef& field) {
    fields_[field.name] = field;
}

FieldDef* ClassType::getField(const std::string& name) {
    auto it = fields_.find(name);
    if (it != fields_.end()) {
        return &it->second;
    }
    return nullptr;
}

FieldDef* ClassType::findField(const std::string& name) {
    // البحث في الصنف الحالي
    auto* field = getField(name);
    if (field) return field;
    
    // البحث في الصنف الأساس
    if (baseClass_) {
        return baseClass_->findField(name);
    }
    
    return nullptr;
}

void ClassType::addMethod(const MethodDef& method) {
    methods_[method.name] = method;
}

MethodDef* ClassType::getMethod(const std::string& name) {
    auto it = methods_.find(name);
    if (it != methods_.end()) {
        return &it->second;
    }
    return nullptr;
}

MethodDef* ClassType::findMethod(const std::string& name) {
    // البحث في الصنف الحالي
    auto* method = getMethod(name);
    if (method) return method;
    
    // البحث في الصنف الأساس
    if (baseClass_) {
        return baseClass_->findMethod(name);
    }
    
    return nullptr;
}

void ClassType::setBaseClass(ClassType* base) {
    baseClass_ = base;
}

bool ClassType::inheritsFrom(ClassType* other) const {
    if (this == other) return true;
    if (!baseClass_) return false;
    return baseClass_->inheritsFrom(other);
}

bool ClassType::canAccess(const std::string& memberName, 
                         ClassType* fromClass) const {
    // البحث عن العضو
    auto fieldIt = fields_.find(memberName);
    if (fieldIt != fields_.end()) {
        return canAccessMember(fieldIt->second.visibility, fromClass);
    }
    
    auto methodIt = methods_.find(memberName);
    if (methodIt != methods_.end()) {
        return canAccessMember(methodIt->second.visibility, fromClass);
    }
    
    return false;
}

bool ClassType::canAccessMember(Visibility vis, ClassType* fromClass) const {
    switch (vis) {
        case Visibility::PUBLIC:
            return true;
            
        case Visibility::PRIVATE:
            return fromClass == this;
            
        case Visibility::PROTECTED:
            return fromClass == this || 
                   (fromClass && fromClass->inheritsFrom(const_cast<ClassType*>(this)));
            
        default:
            return false;
    }
}

} // namespace Data
} // namespace Sad
```

#### المهمة 4.2: تنفيذ ObjectInstance (5 ساعات)

**الملف:** `src/data/types/object_instance.cpp`

```cpp
#include "data/types/object_instance.h"
#include "interpreter/core/interpreter_core.h"

namespace Sad {
namespace Data {

ObjectInstance::ObjectInstance(ClassType* classType)
    : classType_(classType), referenceCount_(0), destructorCalled_(false) {
    
    // تهيئة الخصائص بقيمها الافتراضية
    auto allFields = classType_->getAllFields();
    for (auto* field : allFields) {
        if (field->defaultValue) {
            // سيتم تقييمها لاحقاً
            fields_[field->name] = Value();  // قيمة فارغة مؤقتاً
        } else {
            fields_[field->name] = getDefaultValueForType(field->type);
        }
    }
}

ClassType* ObjectInstance::getClassType() const {
    return classType_;
}

void ObjectInstance::setField(const std::string& name, const Value& value) {
    fields_[name] = value;
}

Value ObjectInstance::getField(const std::string& name) const {
    auto it = fields_.find(name);
    if (it != fields_.end()) {
        return it->second;
    }
    
    throw std::runtime_error("Field not found: " + name);
}

bool ObjectInstance::hasField(const std::string& name) const {
    return fields_.find(name) != fields_.end();
}

Value ObjectInstance::callMethod(const std::string& name,
                                 const std::vector<Value>& args,
                                 InterpreterCore* interpreter) {
    auto* method = classType_->findMethod(name);
    if (!method) {
        throw std::runtime_error("Method not found: " + name);
    }
    
    // استدعاء الطريقة مع this
    return interpreter->executeMethod(this, method, args);
}

void ObjectInstance::addReference() {
    referenceCount_++;
}

void ObjectInstance::releaseReference() {
    referenceCount_--;
    if (referenceCount_ == 0) {
        callDestructor();
        delete this;
    }
}

int ObjectInstance::getReferenceCount() const {
    return referenceCount_;
}

void ObjectInstance::callDestructor() {
    if (destructorCalled_) return;
    
    auto* dtor = classType_->getDestructor();
    if (dtor) {
        // تنفيذ الهدام
        // TODO: استدعاء الهدام عبر المفسر
    }
    
    destructorCalled_ = true;
}

Value ObjectInstance::getDefaultValueForType(Type* type) {
    // إرجاع قيمة افتراضية حسب النوع
    switch (type->getKind()) {
        case TypeKind::INTEGER:
            return Value(0);
        case TypeKind::DOUBLE:
            return Value(0.0);
        case TypeKind::STRING:
            return Value("");
        case TypeKind::BOOLEAN:
            return Value(false);
        default:
            return Value();  // null
    }
}

} // namespace Data
} // namespace Sad
```

#### المهمة 4.3: تنفيذ ClassManager (5 ساعات)

**الملف:** `src/data/managers/class_manager.cpp`

```cpp
#include "data/managers/class_manager.h"
#include <stdexcept>

namespace Sad {
namespace Data {

void ClassManager::registerClass(ClassType* classType) {
    const std::string& name = classType->getName();
    
    if (classes_.find(name) != classes_.end()) {
        throw std::runtime_error("Class already registered: " + name);
    }
    
    classes_[name] = classType;
}

ClassType* ClassManager::getClass(const std::string& name) {
    auto it = classes_.find(name);
    if (it != classes_.end()) {
        return it->second;
    }
    return nullptr;
}

bool ClassManager::hasClass(const std::string& name) const {
    return classes_.find(name) != classes_.end();
}

void ClassManager::unregisterClass(const std::string& name) {
    classes_.erase(name);
}

std::vector<std::string> ClassManager::getAllClassNames() const {
    std::vector<std::string> names;
    for (const auto& pair : classes_) {
        names.push_back(pair.first);
    }
    return names;
}

} // namespace Data
} // namespace Sad
```

#### المخرجات - اليوم 4-5
- [ ] ClassType مُنفّذ بالكامل
- [ ] ObjectInstance مُنفّذ بالكامل
- [ ] ClassManager يعمل بشكل صحيح
- [ ] اختبارات الوحدة تمر
- [ ] توثيق شامل

---

## ✅ قائمة التحقق النهائية / Final Checklist

### Lexer
- [ ] جميع الكلمات المفتاحية مضافة ومختبرة
- [ ] الرموز الخاصة تعمل بشكل صحيح
- [ ] 10+ اختبار Lexer يمر

### Parser
- [ ] تحليل تعريف الأصناف يعمل
- [ ] تحليل الخصائص والطرق يعمل
- [ ] تحليل البواني والهدامات يعمل
- [ ] تحليل إنشاء الكائنات يعمل
- [ ] تحليل الوصول للأعضاء يعمل
- [ ] 15+ اختبار Parser يمر

### Data System
- [ ] ClassType مُنفّذ ومختبر
- [ ] ObjectInstance مُنفّذ ومختبر
- [ ] ClassManager يعمل بشكل صحيح
- [ ] 20+ اختبار Data يمر

### التوثيق
- [ ] جميع الواجهات العامة موثقة
- [ ] أمثلة كود لكل ميزة
- [ ] دليل الاستخدام محدث

---

## 🎯 معايير القبول / Acceptance Criteria

1. **الوظيفية:**
   - يمكن تعريف صنف بسيط بخصائص وطرق
   - يمكن إنشاء باني وهدام
   - البناء ينجح بدون أخطاء أو تحذيرات

2. **الجودة:**
   - تغطية اختبار 80%+
   - جميع الوظائف موثقة
   - لا أخطاء من المحلل الثابت

3. **الأداء:**
   - تحليل صنف < 10ms
   - إنشاء ClassType < 1ms

---

**التالي:** [المرحلة 2: الكائنات والإنشاء](03_phase2_objects.md)
