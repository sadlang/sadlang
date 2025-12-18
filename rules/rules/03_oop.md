# 🏛️ قواعد البرمجة كائنية التوجه / OOP Rules

**التاريخ:** 5 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مرجع تفصيلي لنظام OOP

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **نظام البرمجة كائنية التوجه الكامل** للغة "ص". يشمل الأصناف (Classes)، الوراثة (Inheritance)، الواجهات (Interfaces)، البواني والهدامات (Constructors/Destructors)، إدارة الذاكرة (Memory Management)، والكائنات (Objects).

### (EN) Description
This file documents the **complete Object-Oriented Programming system** for Sad language, including classes, inheritance, interfaces, constructors/destructors, memory management, and objects.

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/language_spec/03_oop.md` - OOP specification
- ✅ `docs/language_spec/01_types.md` - Type system (Object type)
- ✅ `plans/imp/11_parser_and_ast_plan.md` - Class AST nodes
- ✅ `plans/imp/12_interpreter_runtime_plan.md` - Object runtime

---

## 📋 قائمة الميزات / Features List

| # | الميزة (AR) | Feature (EN) | الحالة | الأولوية |
|---|-----------|-----------|-------|---------|
| 1 | تعريف الأصناف | Class Declaration | ✅ Core | P0 |
| 2 | الخصائص | Properties/Fields | ✅ Core | P0 |
| 3 | الطرق | Methods | ✅ Core | P0 |
| 4 | الباني | Constructor | ✅ Core | P0 |
| 5 | الهدام | Destructor | ✅ Core | P0 |
| 6 | الوراثة | Inheritance | ✅ Core | P0 |
| 7 | الوراثة المتعددة | Multiple Inheritance | 🚧 Phase 2 | P1 |
| 8 | الواجهات | Interfaces | 🚧 Phase 2 | P1 |
| 9 | الطرق المجردة | Abstract Methods | 🚧 Phase 2 | P1 |
| 10 | التحميل الزائد | Method Overloading | 🚧 Phase 2 | P2 |
| 11 | التعددية الشكلية | Polymorphism | ✅ Core | P0 |
| 12 | التغليف | Encapsulation | ✅ Core | P0 |
| 13 | خصائص ثابتة | Static Members | ✅ Core | P1 |
| 14 | خصائص خاصة | Private Members | ✅ Core | P0 |
| 15 | خصائص محمية | Protected Members | 🚧 Phase 2 | P1 |

---

## 1️⃣ Basic Class Declaration (تعريف الصنف الأساسي)

### Syntax (EBNF)
```ebnf
class_decl ::= 'صنف' IDENTIFIER [(':' | 'يرث') base_class_list] class_body 'نهاية'

base_class_list ::= IDENTIFIER ((',' | '،') IDENTIFIER)*

class_body ::= (field_decl | method_decl | constructor_decl | destructor_decl)*

field_decl ::= [visibility] type IDENTIFIER ['=' expression] (';' | '؛')?

method_decl ::= [visibility] 'دالة' [type] IDENTIFIER '(' [param_list] ')' block

constructor_decl ::= ('باني' | 'منشئ') '(' [param_list] ')' [':' initializer_list] block

destructor_decl ::= ('~باني' | '~منشئ' | 'مدمر') '()' block

visibility ::= 'عام' | 'خاص' | 'محمي'  // public | private | protected

initializer_list ::= IDENTIFIER '(' expression ')' ((',' | '،') IDENTIFIER '(' expression ')')*

param_list ::= param ((',' | '،') param)*
```

**الشرح بالعربية:**
- `class_decl`: إعلان الصنف يبدأ بكلمة 'صنف'، اسم الصنف، قائمة اختيارية من الأصناف الأساسية (باستخدام ':' أو 'يرث')، جسم الصنف، ينتهي بكلمة 'نهاية'
- الوراثة: يمكن استخدام ':' أو كلمة 'يرث' العربية (مثال: `صنف كلب : حيوان` أو `صنف كلب يرث حيوان`)
- `field_decl`: إعلان الخاصية مع فاصلة منقوطة اختيارية (عربية ؛ أو إنجليزية ;)
- `method_decl`: نوع الإرجاع اختياري ويأتي بعد 'دالة' وقبل اسم الطريقة، ينتهي بكلمة 'نهاية' من خلال block
- `constructor_decl`: يدعم 'باني' أو 'منشئ'
- `destructor_decl`: يدعم '~باني' أو '~منشئ' أو 'مدمر'
- دعم الفاصلة العربية (،) والإنجليزية (,) في جميع القوائم

### Semantics
- **Class Definition:** Creates a new type
- **Encapsulation:** Fields can be public (عام), private (خاص), or protected (محمي)
- **Instantiation:** Use `صنف()` syntax to create objects
- **this keyword:** `هذا` refers to current instance
- **Inheritance:** Can use either `:` or `يرث` keyword (both are equivalent)
  - Example: `صنف كلب : حيوان` or `صنف كلب يرث حيوان`

### Examples

```s
// صنف بسيط / Simple class
صنف نقطة
    // خصائص / Properties
    عام رقم س = 0
    عام رقم ص = 0
    
    // باني / Constructor
    باني(رقم س_جديد، رقم ص_جديد)
        هذا.س = س_جديد
        هذا.ص = ص_جديد
    نهاية
    
    // طريقة / Method
    عام دالة عشري مسافة_من_الأصل()
        إرجاع (هذا.س ** 2 + هذا.ص ** 2) ** 0.5
    نهاية
    
    // طريقة أخرى / Another method
    عام دالة اطبع()
        اطبع("نقطة({هذا.س}، {هذا.ص})")
    نهاية
نهاية

// إنشاء كائن / Create object
نقطة ن1 = نقطة(3، 4)
ن1.اطبع()  // نقطة(3، 4)
اطبع(ن1.مسافة_من_الأصل())  // 5.0

// صنف مع خصائص خاصة / Class with private properties
صنف حساب_بنكي
    خاص عشري رصيد = 0.0
    عام نص اسم_صاحب_الحساب
    
    باني(نص اسم، عشري رصيد_أولي)
        هذا.اسم_صاحب_الحساب = اسم
        هذا.رصيد = رصيد_أولي
    نهاية
    
    // Getter
    عام دالة عشري احصل_على_رصيد()
        إرجاع هذا.رصيد
    نهاية
    
    // طريقة إيداع / Deposit method
    عام دالة أودع(عشري مبلغ)
        إذا( مبلغ > 0)
            هذا.رصيد = هذا.رصيد + مبلغ
            اطبع(ن"تم إيداع {مبلغ}")
        نهاية
    نهاية
    
    // طريقة سحب / Withdraw method
    عام دالة منطقي اسحب(عشري مبلغ)
        إذا( مبلغ > 0 && مبلغ <= هذا.رصيد)
            هذا.رصيد = هذا.رصيد - مبلغ
            اطبع("تم سحب {مبلغ}")
            إرجاع صحيح
        نهاية
        اطبع("رصيد غير كاف")
        إرجاع خطأ
    نهاية
    
    // مدمر / Destructor
    مدمر()
        اطبع(ن"إغلاق حساب {هذا.اسم_صاحب_الحساب}")
    نهاية
نهاية

// استخدام / Usage
حساب_بنكي حساب = حساب_بنكي("أحمد"، 1000.0)
حساب.أودع(500.0)   // تم إيداع 500.0
حساب.اسحب(300.0)   // تم سحب 300.0
اطبع(حساب.احصل_على_رصيد())  // 1200.0

// محاولة الوصول المباشر للخاصية الخاصة (خطأ!)
// اطبع(حساب.رصيد)  // Error: Cannot access private field
```

### Token Analysis
```
// Example 1: Basic class (no inheritance)
KEYWORD_CLASS      "صنف"
IDENTIFIER         "نقطة"
KEYWORD_PUBLIC     "عام"
KEYWORD_TYPE_INT   "رقم"
IDENTIFIER         "س"
ASSIGN             "="
INTEGER_LITERAL    "0"
NEWLINE
// ... (rest of tokens)
KEYWORD_END        "نهاية"

// Example 2: Inheritance with ':'
KEYWORD_CLASS      "صنف"
IDENTIFIER         "كلب"
COLON              ":"
IDENTIFIER         "حيوان"
// ... (class body)
KEYWORD_END        "نهاية"

// Example 3: Inheritance with 'يرث'
KEYWORD_CLASS      "صنف"
IDENTIFIER         "قطة"
KEYWORD_INHERITS   "يرث"
IDENTIFIER         "حيوان"
// ... (class body)
KEYWORD_END        "نهاية"
```

### AST Representation
```
ClassDeclStmt
├── name: "نقطة"
├── baseClasses: []
├── fields: [
│   ├── FieldDecl
│   │   ├── visibility: PUBLIC
│   │   ├── type: INTEGER
│   │   ├── name: "س"
│   │   └── initializer: LiteralExpr(0)
│   └── FieldDecl
│       ├── visibility: PUBLIC
│       ├── type: INTEGER
│       ├── name: "ص"
│       └── initializer: LiteralExpr(0)
│   ]
├── constructor: ConstructorDecl
│   ├── parameters: [
│   │   ├── Parameter("س_جديد", INTEGER)
│   │   └── Parameter("ص_جديد", INTEGER)
│   │   ]
│   └── body: BlockStmt
│       ├── AssignStmt(this.س, س_جديد)
│       └── AssignStmt(this.ص, ص_جديد)
└── methods: [
    └── MethodDecl
        ├── visibility: PUBLIC
        ├── name: "مسافة_من_الأصل"
        ├── parameters: []
        ├── returnType: FLOAT
        └── body: BlockStmt
            └── ReturnStmt(...)
    ]
```

### Implementation Notes

**Files to Modify:**
```cpp
// include/parser/ast/class_nodes.h
class ClassDeclStmt : public Stmt {
public:
    std::string name;
    std::vector<std::string> baseClasses;
    std::vector<std::unique_ptr<FieldDecl>> fields;
    std::unique_ptr<ConstructorDecl> constructor;
    std::unique_ptr<DestructorDecl> destructor;
    std::vector<std::unique_ptr<MethodDecl>> methods;
    
    ClassDeclStmt(std::string name,
                  std::vector<std::string> baseClasses = {})
        : name(std::move(name)),
          baseClasses(std::move(baseClasses)) {}
    
    void accept(Visitor& visitor) override;
};

class FieldDecl {
public:
    Visibility visibility;
    Type* type;
    std::string name;
    std::unique_ptr<Expr> initializer;
    bool isStatic = false;
};

class MethodDecl {
public:
    Visibility visibility;
    std::string name;
    std::vector<Parameter> parameters;
    Type* returnType;                         // Optional, comes BEFORE method name
    std::unique_ptr<BlockStmt> body;
    bool isStatic = false;
    bool isAbstract = false;
    bool isVirtual = false;
};

class ConstructorDecl {
public:
    std::vector<Parameter> parameters;
    std::vector<FieldInitializer> initializers;  // : field(value), ...
    std::unique_ptr<BlockStmt> body;
};

class DestructorDecl {
public:
    std::unique_ptr<BlockStmt> body;
};

enum class Visibility {
    PUBLIC,     // عام
    PRIVATE,    // خاص
    PROTECTED   // محمي
};

// include/lexer/lexer_keywords.h
// Add 'يرث' keyword for inheritance
class LexerKeywords {
public:
    static void initializeKeywords() {
        // ... other keywords ...
        keywords["صنف"] = TokenType::KEYWORD_CLASS;
        keywords["يرث"] = TokenType::KEYWORD_INHERITS;  // NEW: inheritance keyword
        keywords["عام"] = TokenType::KEYWORD_PUBLIC;
        keywords["خاص"] = TokenType::KEYWORD_PRIVATE;
        keywords["محمي"] = TokenType::KEYWORD_PROTECTED;
        keywords["باني"] = TokenType::KEYWORD_CONSTRUCTOR;
        keywords["منشئ"] = TokenType::KEYWORD_CONSTRUCTOR_ALT;
        keywords["مدمر"] = TokenType::KEYWORD_DESTRUCTOR;
        // ... other keywords ...
    }
};

// include/data/types/class_type.h
class ClassType {
public:
    std::string name;
    std::map<std::string, FieldDecl*> fields;
    std::map<std::string, MethodDecl*> methods;
    ConstructorDecl* constructor;
    DestructorDecl* destructor;
    std::vector<ClassType*> baseClasses;
    
    ClassType(const std::string& name) : name(name) {}
    
    // Method lookup with inheritance
    MethodDecl* findMethod(const std::string& name);
    
    // Field lookup with inheritance
    FieldDecl* findField(const std::string& name);
    
    // Check if field/method is accessible
    bool isAccessible(const std::string& memberName, 
                     ClassType* fromClass);
};

// include/data/types/object_instance.h
class ObjectInstance {
public:
    ClassType* classType;
    std::map<std::string, Value> fields;
    
    ObjectInstance(ClassType* type) : classType(type) {
        // Initialize fields with default values
        for (auto& [name, fieldDecl] : type->fields) {
            if (fieldDecl->initializer) {
                // Evaluate initializer
                fields[name] = evaluateInitializer(fieldDecl->initializer.get());
            } else {
                fields[name] = getDefaultValue(fieldDecl->type);
            }
        }
    }
    
    Value getField(const std::string& name);
    void setField(const std::string& name, const Value& value);
    
    Value callMethod(const std::string& name, 
                    const std::vector<Value>& args,
                    Interpreter* interpreter);
};

// include/data/types/value.h
class Value {
    std::shared_ptr<ObjectInstance> objectValue;
    
public:
    static Value makeObject(ObjectInstance* obj);
    ObjectInstance* asObject();
    bool isObject() const;
};

// src/parser/parser_classes.cpp
StmtPtr ParserCore::parseClassDecl() {
    consume(TokenType::KEYWORD_CLASS, "Expected 'صنف'");
    
    Token name = consume(TokenType::IDENTIFIER, "Expected class name");
    
    std::vector<std::string> baseClasses;
    // Support both ':' and 'يرث' for inheritance
    if (match({TokenType::COLON, TokenType::KEYWORD_INHERITS})) {
        do {
            Token base = consume(TokenType::IDENTIFIER, "Expected base class name");
            baseClasses.push_back(base.lexeme);
        } while (match({TokenType::COMMA, TokenType::ARABIC_COMMA}));  // Support both commas
    }
    
    // No brace needed - class body directly
    
    auto classDecl = std::make_unique<ClassDeclStmt>(name.lexeme, baseClasses);
    
    // Parse class body until 'نهاية'
    while (!check(TokenType::KEYWORD_END) && !isAtEnd()) {
        // Check for visibility keyword
        Visibility vis = Visibility::PUBLIC;  // Default
        if (match(TokenType::KEYWORD_PUBLIC)) {
            vis = Visibility::PUBLIC;
        } else if (match(TokenType::KEYWORD_PRIVATE)) {
            vis = Visibility::PRIVATE;
        } else if (match(TokenType::KEYWORD_PROTECTED)) {
            vis = Visibility::PROTECTED;
        }
        
        // Check what we're parsing
        if (check(TokenType::KEYWORD_CONSTRUCTOR) || check(TokenType::KEYWORD_CONSTRUCTOR_ALT)) {
            classDecl->constructor = parseConstructor();
        } else if (check(TokenType::KEYWORD_DESTRUCTOR) || 
                   check(TokenType::KEYWORD_DESTRUCTOR_ALT1) ||
                   check(TokenType::KEYWORD_DESTRUCTOR_ALT2)) {
            classDecl->destructor = parseDestructor();
        } else if (check(TokenType::KEYWORD_FUNC)) {
            classDecl->methods.push_back(parseMethod(vis));
        } else {
            classDecl->fields.push_back(parseField(vis));
        }
    }
    
    consume(TokenType::KEYWORD_END, "Expected 'نهاية' at end of class");
    
    return classDecl;
}

std::unique_ptr<FieldDecl> ParserCore::parseField(Visibility vis) {
    Type* type = parseType();
    Token name = consume(TokenType::IDENTIFIER, "Expected field name");
    
    auto field = std::make_unique<FieldDecl>();
    field->visibility = vis;
    field->type = type;
    field->name = name.lexeme;
    
    if (match(TokenType::ASSIGN)) {
        field->initializer = expression();
    }
    
    // Optional semicolon (both Arabic and English)
    match({TokenType::SEMICOLON, TokenType::ARABIC_SEMICOLON});
    
    return field;
}

std::unique_ptr<MethodDecl> ParserCore::parseMethod(Visibility vis) {
    consume(TokenType::KEYWORD_FUNC, "Expected 'دالة'");
    
    // Optional return type BEFORE method name
    Type* returnType = nullptr;
    if (isTypeKeyword(peek())) {
        returnType = parseType();
    }
    
    Token name = consume(TokenType::IDENTIFIER, "Expected method name");
    
    consume(TokenType::LEFT_PAREN, "Expected '('");
    
    std::vector<Parameter> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            params.push_back(parseParameter());
        } while (match({TokenType::COMMA, TokenType::ARABIC_COMMA}));  // Support both commas
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')'");
    
    auto body = blockStatement();  // Will consume 'نهاية'
    
    auto method = std::make_unique<MethodDecl>();
    method->visibility = vis;
    method->name = name.lexeme;
    method->returnType = returnType;
    method->parameters = std::move(params);
    method->body = std::move(body);
    
    return method;
}

std::unique_ptr<ConstructorDecl> ParserCore::parseConstructor() {
    // Accept both 'باني' and 'منشئ'
    if (!match(TokenType::KEYWORD_CONSTRUCTOR) && 
        !match(TokenType::KEYWORD_CONSTRUCTOR_ALT)) {
        error("Expected 'باني' or 'منشئ'");
    }
    
    consume(TokenType::LEFT_PAREN, "Expected '('");
    
    std::vector<Parameter> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            params.push_back(parseParameter());
        } while (match({TokenType::COMMA, TokenType::ARABIC_COMMA}));  // Support both commas
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')'");
    
    // Parse initializer list (optional)
    std::vector<FieldInitializer> initializers;
    if (match(TokenType::COLON)) {
        do {
            Token fieldName = consume(TokenType::IDENTIFIER, "Expected field name");
            consume(TokenType::LEFT_PAREN, "Expected '('");
            auto initExpr = expression();
            consume(TokenType::RIGHT_PAREN, "Expected ')'");
            
            initializers.push_back({fieldName.lexeme, std::move(initExpr)});
        } while (match({TokenType::COMMA, TokenType::ARABIC_COMMA}));  // Support both commas
    }
    
    auto body = blockStatement();  // Will consume 'نهاية'
    
    auto ctor = std::make_unique<ConstructorDecl>();
    ctor->parameters = std::move(params);
    ctor->initializers = std::move(initializers);
    ctor->body = std::move(body);
    
    return ctor;
}

std::unique_ptr<DestructorDecl> ParserCore::parseDestructor() {
    // Accept '~باني', '~منشئ', or 'مدمر'
    if (!match(TokenType::KEYWORD_DESTRUCTOR) && 
        !match(TokenType::KEYWORD_DESTRUCTOR_ALT1) &&
        !match(TokenType::KEYWORD_DESTRUCTOR_ALT2)) {
        error("Expected '~باني', '~منشئ', or 'مدمر'");
    }
    
    consume(TokenType::LEFT_PAREN, "Expected '('");
    consume(TokenType::RIGHT_PAREN, "Expected ')'");
    
    auto body = blockStatement();  // Will consume 'نهاية'
    
    auto dtor = std::make_unique<DestructorDecl>();
    dtor->body = std::move(body);
    
    return dtor;
}

// src/interpreter/statements/class_interpreter.cpp
void InterpreterCore::visitClassDeclStmt(ClassDeclStmt* stmt) {
    // Create class type
    auto classType = std::make_shared<ClassType>(stmt->name);
    
    // Register fields
    for (auto& field : stmt->fields) {
        classType->fields[field->name] = field.get();
    }
    
    // Register methods
    for (auto& method : stmt->methods) {
        classType->methods[method->name] = method.get();
    }
    
    // Register constructor and destructor
    classType->constructor = stmt->constructor.get();
    classType->destructor = stmt->destructor.get();
    
    // Handle inheritance
    for (const auto& baseName : stmt->baseClasses) {
        ClassType* baseClass = findClassType(baseName);
        if (!baseClass) {
            throw RuntimeError("Base class not found: " + baseName);
        }
        classType->baseClasses.push_back(baseClass);
    }
    
    // Register class type in environment
    currentEnvironment->defineClass(stmt->name, classType.get());
    
    DEBUG_PRINT("Defined class: %s with %zu fields, %zu methods",
                stmt->name.c_str(),
                classType->fields.size(),
                classType->methods.size());
}

// src/interpreter/expressions/instantiation_evaluator.cpp
Value InterpreterCore::visitInstantiationExpr(InstantiationExpr* expr) {
    // expr looks like: ClassName(arg1, arg2, ...)
    
    // Get class type
    ClassType* classType = findClassType(expr->className);
    if (!classType) {
        throw RuntimeError("Class not found: " + expr->className);
    }
    
    // Create object instance
    auto instance = std::make_shared<ObjectInstance>(classType);
    
    // Evaluate constructor arguments
    std::vector<Value> args;
    for (auto& argExpr : expr->arguments) {
        argExpr->accept(*this);
        args.push_back(lastValue);
    }
    
    // Call constructor
    if (classType->constructor) {
        callConstructor(instance.get(), classType->constructor, args);
    }
    
    return Value::makeObject(instance.get());
}

void InterpreterCore::callConstructor(ObjectInstance* instance,
                                     ConstructorDecl* ctor,
                                     const std::vector<Value>& args) {
    // Create environment for constructor
    auto ctorEnv = new Environment(currentEnvironment);
    
    // Bind 'this' (هذا)
    ctorEnv->define("هذا", Value::makeObject(instance));
    
    // Bind parameters
    for (size_t i = 0; i < ctor->parameters.size(); i++) {
        if (i < args.size()) {
            ctorEnv->define(ctor->parameters[i].name, args[i]);
        } else {
            throw RuntimeError("Missing constructor argument");
        }
    }
    
    // Execute initializer list
    for (auto& init : ctor->initializers) {
        auto prevEnv = currentEnvironment;
        currentEnvironment = ctorEnv;
        
        init.expression->accept(*this);
        instance->setField(init.fieldName, lastValue);
        
        currentEnvironment = prevEnv;
    }
    
    // Execute constructor body
    auto prevEnv = currentEnvironment;
    currentEnvironment = ctorEnv;
    
    try {
        ctor->body->accept(*this);
    } catch (const ReturnException&) {
        // Constructors shouldn't return values, but handle it gracefully
    }
    
    currentEnvironment = prevEnv;
    delete ctorEnv;
}

// src/interpreter/expressions/member_access_evaluator.cpp
Value InterpreterCore::visitMemberAccessExpr(MemberAccessExpr* expr) {
    // expr looks like: object.field or object.method
    
    expr->object->accept(*this);
    Value objValue = lastValue;
    
    if (!objValue.isObject()) {
        throw RuntimeError("Cannot access member of non-object type");
    }
    
    ObjectInstance* obj = objValue.asObject();
    
    // Check if it's a field access
    if (obj->classType->fields.find(expr->memberName) != 
        obj->classType->fields.end()) {
        
        // Check visibility
        if (!obj->classType->isAccessible(expr->memberName, getCurrentClass())) {
            throw RuntimeError("Cannot access private field: " + expr->memberName);
        }
        
        return obj->getField(expr->memberName);
    }
    
    // Check if it's a method access
    if (obj->classType->methods.find(expr->memberName) != 
        obj->classType->methods.end()) {
        
        // Return bound method (method + this)
        return Value::makeBoundMethod(obj, expr->memberName);
    }
    
    throw RuntimeError("Member not found: " + expr->memberName);
}
```

### Tests
**File:** `tests/spec_rules/oop_basic.s`
```s
// اختبار صنف بسيط / Test simple class
صنف مستطيل
    عام رقم عرض
    عام رقم ارتفاع
    
    باني(رقم ع، رقم ا)
        هذا.عرض = ع
        هذا.ارتفاع = ا
    نهاية
    
    عام دالة رقم مساحة()
        إرجاع هذا.عرض * هذا.ارتفاع
    نهاية
نهاية

مستطيل م = مستطيل(5، 10)
اطبع(م.مساحة())  // Expected: 50

// اختبار خصائص خاصة / Test private properties
صنف عداد
    خاص رقم قيمة = 0
    
    عام دالة زد()
        هذا.قيمة = هذا.قيمة + 1
    نهاية
    
    عام دالة رقم احصل()
        إرجاع هذا.قيمة
    نهاية
نهاية

عداد ع = عداد()
ع.زد()
ع.زد()
اطبع(ع.احصل())  // Expected: 2

// اختبار منشئ بديل / Test alternative constructor
صنف دائرة
    عام عشري نصف_القطر
    
    منشئ(عشري ن)
        هذا.نصف_القطر = ن
    نهاية
    
    عام دالة عشري مساحة()
        إرجاع 3.14159 * هذا.نصف_القطر ** 2
    نهاية
    
    مدمر()
        اطبع("تم حذف الدائرة")
    نهاية
نهاية

دائرة د = دائرة(5.0)
اطبع(د.مساحة())  // Expected: 78.53975
```

**Expected Output:**
```
50
2
78.53975
تم حذف الدائرة
```

### Logging
```cpp
// src/parser/parser_classes.cpp
DEBUG_PRINT("Parsing class: %s, base classes: %zu",
            name.c_str(), baseClasses.size());
DEBUG_PRINT("Inheritance using: %s", 
            useInheritsKeyword ? "'يرث'" : "':'");

// src/interpreter/statements/class_interpreter.cpp
DEBUG_PRINT("Instantiating class: %s", classType->name.c_str());
DEBUG_PRINT("Calling constructor with %zu arguments", args.size());

// src/interpreter/expressions/member_access_evaluator.cpp
DEBUG_PRINT("Accessing member: %s.%s", 
            obj->classType->name.c_str(),
            expr->memberName.c_str());
```

---

## 2️⃣ Inheritance (الوراثة)

### Syntax (EBNF)
```ebnf
class_with_inheritance ::= 'صنف' IDENTIFIER (':' | 'يرث') base_class_list class_body 'نهاية'

base_class_list ::= IDENTIFIER ((',' | '،') IDENTIFIER)*

super_call ::= 'الأساس' '.' method_name '(' [arg_list] ')'
             | 'الأساس' '(' [arg_list] ')'  // Constructor call

arg_list ::= expression ((',' | '،') expression)*
```

**الشرح بالعربية:**
- الوراثة يمكن التعبير عنها بـ ':' أو كلمة 'يرث' العربية
- مثال: `صنف كلب : حيوان` أو `صنف كلب يرث حيوان`
- دعم الفاصلة العربية (،) والإنجليزية (,) في قوائم الأصناف الأساسية والمعاملات

### Semantics
- **Single Inheritance:** One base class (Phase 1)
- **Multiple Inheritance:** Multiple base classes (Phase 2)
- **Method Resolution:** Child methods override parent methods
- **super keyword:** `الأساس` refers to parent class

### Examples

```s
// وراثة بسيطة / Simple inheritance
صنف حيوان
    عام نص اسم
    
    باني(نص اسم)
        هذا.اسم = اسم
    نهاية
    
    عام دالة صوت()
        اطبع("الحيوان يصدر صوتاً")
    نهاية
    
    عام دالة معلومات()
        اطبع(ن"حيوان اسمه {هذا.اسم}")
    نهاية
نهاية

// الصنف المشتق / Derived class (using ':')
صنف كلب : حيوان
    عام نص سلالة
    
    باني(نص اسم، نص سلالة)
        // استدعاء باني الصنف الأساسي / Call parent constructor
        الأساس(اسم)
        هذا.سلالة = سلالة
    نهاية
    
    // تجاوز الطريقة / Override method
    عام دالة صوت()
        اطبع("الكلب ينبح: نباح نباح!")
    نهاية
    
    // طريقة جديدة / New method
    عام دالة وصف()
        اطبع(ن"كلب من سلالة {هذا.سلالة}")
    نهاية
نهاية

// استخدام 'يرث' بدلاً من ':' / Using 'يرث' instead of ':'
صنف قطة يرث حيوان
    عام منطقي يموء
    
    منشئ(نص اسم، منطقي يموء)
        الأساس(اسم)
        هذا.يموء = يموء
    نهاية
    
    عام دالة صوت()
        إذا هذا.يموء
            اطبع("القطة تموء: مياو مياو!")
        وإلا
            اطبع("القطة صامتة")
        نهاية
    نهاية
نهاية

// استخدام / Usage
كلب ك = كلب("ريكس"، "جيرمن شيبرد")
ك.معلومات()  // "حيوان اسمه ريكس" (inherited)
ك.صوت()       // "الكلب ينبح: نباح نباح!" (overridden)
ك.وصف()       // "كلب من سلالة جيرمن شيبرد" (new)

قطة ق = قطة("مشمش"، صحيح)
ق.معلومات()  // "حيوان اسمه مشمش" (inherited)
ق.صوت()       // "القطة تموء: مياو مياو!" (overridden)

// التعددية الشكلية / Polymorphism
دالة دع_الحيوان_يصدر_صوت(حيوان ح)
    ح.صوت()
نهاية

حيوان ح1 = حيوان("مجهول")
كلب ح2 = كلب("فيدو"، "بلدوغ")
قطة ح3 = قطة("لولو"، خطأ)

دع_الحيوان_يصدر_صوت(ح1)  // "الحيوان يصدر صوتاً"
دع_الحيوان_يصدر_صوت(ح2)  // "الكلب ينبح: نباح نباح!"
دع_الحيوان_يصدر_صوت(ح3)  // "القطة صامتة"

// مثال أكثر تعقيداً / More complex example
صنف شكل
    عام نص لون
    
    باني(نص لون)
        هذا.لون = لون
    نهاية
    
    عام دالة عشري مساحة()
        إرجاع 0.0  // Default implementation
    نهاية
نهاية

صنف دائرة : شكل
    عام عشري نصف_القطر
    
    باني(نص لون، عشري نصف_القطر)
        الأساس(لون)
        هذا.نصف_القطر = نصف_القطر
    نهاية
    
    عام دالة عشري مساحة()
        إرجاع 3.14159 * هذا.نصف_القطر ** 2
    نهاية
نهاية

صنف مربع : شكل
    عام عشري ضلع
    
    باني(نص لون، عشري ضلع)
        الأساس(لون)
        هذا.ضلع = ضلع
    نهاية
    
    عام دالة عشري مساحة()
        إرجاع هذا.ضلع ** 2
    نهاية
نهاية

// استخدام / Usage
مصفوفة أشكال = [
    دائرة("أحمر"، 5.0)،
    مربع("أزرق"، 4.0)،
    دائرة("أخضر"، 3.0)
]

لكل شكل في أشكال
    اطبع(ن"شكل {شكل.لون} مساحته {شكل.مساحة()}")
نهاية
// Output:
// شكل أحمر مساحته 78.53975
// شكل أزرق مساحته 16.0
// شكل أخضر مساحته 28.27431
```

### Implementation Notes

```cpp
// include/lexer/token.h
enum class TokenType {
    // ... other tokens ...
    KEYWORD_CLASS,         // صنف
    KEYWORD_INHERITS,      // يرث (for inheritance)
    COLON,                 // : (alternative for inheritance)
    // ... other tokens ...
};

// include/data/types/class_type.h (continued)
class ClassType {
    // ... (previous members)
    
    // Method lookup with inheritance
    MethodDecl* findMethod(const std::string& name) {
        // Check current class
        auto it = methods.find(name);
        if (it != methods.end()) {
            return it->second;
        }
        
        // Check base classes
        for (auto* baseClass : baseClasses) {
            MethodDecl* method = baseClass->findMethod(name);
            if (method) return method;
        }
        
        return nullptr;
    }
    
    // Check if this class inherits from another
    bool inheritsFrom(ClassType* other) {
        if (this == other) return true;
        
        for (auto* base : baseClasses) {
            if (base->inheritsFrom(other)) {
                return true;
            }
        }
        
        return false;
    }
};

// src/interpreter/expressions/super_call_evaluator.cpp
Value InterpreterCore::visitSuperCallExpr(SuperCallExpr* expr) {
    // Get 'this' object
    Value thisValue = currentEnvironment->get("هذا");
    if (!thisValue.isObject()) {
        throw RuntimeError("'الأساس' can only be used inside class methods");
    }
    
    ObjectInstance* thisObj = thisValue.asObject();
    
    // Get base class
    if (thisObj->classType->baseClasses.empty()) {
        throw RuntimeError("Class has no base class");
    }
    
    ClassType* baseClass = thisObj->classType->baseClasses[0];
    
    // Find method in base class
    MethodDecl* method = baseClass->findMethod(expr->methodName);
    if (!method) {
        throw RuntimeError("Method not found in base class: " + expr->methodName);
    }
    
    // Evaluate arguments
    std::vector<Value> args;
    for (auto& argExpr : expr->arguments) {
        argExpr->accept(*this);
        args.push_back(lastValue);
    }
    
    // Call base class method
    return callMethod(thisObj, method, args);
}
```
### Tests
**File:** `tests/spec_rules/oop_inheritance.s`
```s
// اختبار الوراثة / Inheritance test
صنف مركبة
    عام نص نوع
    
    باني(نص نوع)
        هذا.نوع = نوع
    نهاية
    
    عام دالة تحرك()
        اطبع("المركبة تتحرك")
    نهاية
نهاية

// اختبار الوراثة باستخدام ':' / Test inheritance using ':'
صنف سيارة : مركبة
    عام رقم عدد_الأبواب
    
    منشئ(رقم أبواب)
        الأساس("سيارة")
        هذا.عدد_الأبواب = أبواب
    نهاية
    
    عام دالة تحرك()
        اطبع("السيارة تسير على الطريق")
    نهاية
نهاية

// اختبار الوراثة باستخدام 'يرث' / Test inheritance using 'يرث'
صنف دراجة يرث مركبة
    عام رقم عدد_العجلات
    
    باني(رقم عجلات)
        الأساس("دراجة")
        هذا.عدد_العجلات = عجلات
    نهاية
    
    عام دالة تحرك()
        اطبع("الدراجة تتحرك بالدواسات")
    نهاية
نهاية

سيارة س = سيارة(4)
اطبع(س.نوع)           // Expected: "سيارة"
اطبع(س.عدد_الأبواب)   // Expected: 4
س.تحرك()               // Expected: "السيارة تسير على الطريق"

دراجة د = دراجة(2)
اطبع(د.نوع)           // Expected: "دراجة"
اطبع(د.عدد_العجلات)  // Expected: 2
د.تحرك()               // Expected: "الدراجة تتحرك بالدواسات"
```

**Expected Output:**
```
سيارة
4
السيارة تسير على الطريق
دراجة
2
الدراجة تتحرك بالدواسات
```حرك()               // Expected: "السيارة تسير على الطريق"
```

---

## 3️⃣ Static Members (الأعضاء الثابتة)

### Syntax (EBNF)
```ebnf
static_field ::= 'ثابت' type IDENTIFIER '=' expression (';' | '؛')?

static_method ::= 'ثابت' 'دالة' [type] IDENTIFIER '(' [param_list] ')' block

static_access ::= ClassName '.' static_member_name

param_list ::= param ((',' | '،') param)*
```

**الشرح بالعربية:**
- نوع الإرجاع في الطريقة الثابتة اختياري ويأتي بعد 'دالة'
- دعم الفاصلة المنقوطة الاختيارية (عربية ؛ أو إنجليزية ;)
- دعم الفاصلة العربية (،) والإنجليزية (,)

### Semantics
- **Shared:** Static members shared across all instances
- **Access:** Can be accessed without creating instance
- **this:** Not available in static methods

### Examples

```s
// صنف مع أعضاء ثابتة / Class with static members
صنف رياضيات
    ثابت عشري باي = 3.14159265359
    ثابت عشري إي = 2.71828182846
    
    ثابت دالة عشري مربع(عشري ع)
        إرجاع ع * ع
    نهاية
    
    ثابت دالة عشري أقصى(عشري أ، عشري ب)
        إذا أ > ب
            إرجاع أ
        وإلا
            إرجاع ب
        نهاية
    نهاية
    
    ثابت دالة عشري مساحة_دائرة(عشري نصف_القطر)
        إرجاع رياضيات.باي * نصف_القطر ** 2
    نهاية
نهاية

// استخدام / Usage
اطبع(رياضيات.باي)  // 3.14159265359
اطبع(رياضيات.مربع(5.0))  // 25.0
اطبع(رياضيات.أقصى(10.0، 20.0))  // 20.0
اطبع(رياضيات.مساحة_دائرة(5.0))  // 78.5398...

// عداد مع خاصية ثابتة / Counter with static property
صنف كائن_مع_معرف
    ثابت رقم عداد_المعرفات = 0
    عام رقم معرف
    
    باني()
        كائن_مع_معرف.عداد_المعرفات = كائن_مع_معرف.عداد_المعرفات + 1
        هذا.معرف = كائن_مع_معرف.عداد_المعرفات
    نهاية
نهاية

كائن_مع_معرف ك1 = كائن_مع_معرف()
كائن_مع_معرف ك2 = كائن_مع_معرف()
كائن_مع_معرف ك3 = كائن_مع_معرف()

اطبع(ك1.معرف)  // 1
اطبع(ك2.معرف)  // 2
اطبع(ك3.معرف)  // 3
اطبع(كائن_مع_معرف.عداد_المعرفات)  // 3
```

### Implementation Notes

```cpp
// include/data/types/class_type.h (continued)
class ClassType {
    // Static members are stored at class level, not instance level
    std::map<std::string, Value> staticFields;
    std::map<std::string, MethodDecl*> staticMethods;
    
public:
    void defineStaticField(const std::string& name, const Value& value) {
        staticFields[name] = value;
    }
    
    Value getStaticField(const std::string& name) {
        auto it = staticFields.find(name);
        if (it != staticFields.end()) {
            return it->second;
        }
        throw RuntimeError("Static field not found: " + name);
    }
    
    void setStaticField(const std::string& name, const Value& value) {
        staticFields[name] = value;
    }
};

// src/interpreter/expressions/static_access_evaluator.cpp
Value InterpreterCore::visitStaticAccessExpr(StaticAccessExpr* expr) {
    // expr looks like: ClassName.staticMember
    
    ClassType* classType = findClassType(expr->className);
    if (!classType) {
        throw RuntimeError("Class not found: " + expr->className);
    }
    
    // Try static field
    try {
        return classType->getStaticField(expr->memberName);
    } catch (...) {
        // Not a field, maybe a method
    }
    
    // Try static method
    auto it = classType->staticMethods.find(expr->memberName);
    if (it != classType->staticMethods.end()) {
        return Value::makeStaticMethod(classType, it->second);
    }
    
    throw RuntimeError("Static member not found: " + expr->memberName);
}
```

---

## 4️⃣ Destructor & Memory Management (الهدام وإدارة الذاكرة)

### Syntax (EBNF)
```ebnf
destructor_decl ::= ('~باني' | '~منشئ' | 'مدمر') '()' block
```

**الشرح بالعربية:**
- يدعم ثلاثة أشكال للهدام: '~باني' أو '~منشئ' أو 'مدمر'
- الهدام لا يأخذ معاملات ولا يعيد قيمة

### Semantics
- **Automatic:** Destructor called when object goes out of scope
- **RAII:** Resource Acquisition Is Initialization pattern
- **GC Hybrid:** Reference counting + GC for cycles

### Examples

```s
// صنف مع هدام / Class with destructor
صنف ملف
    خاص نص اسم_الملف
    خاص منطقي مفتوح
    
    باني(نص اسم)
        هذا.اسم_الملف = اسم
        هذا.مفتوح = صحيح
        اطبع(ن"فتح الملف: {اسم}")
    نهاية
    
    ~باني()
        إذا هذا.مفتوح
            اطبع(ن"إغلاق الملف: {هذا.اسم_الملف}")
            هذا.مفتوح = خطأ
        نهاية
    نهاية
    
    عام دالة اكتب(نص محتوى)
        إذا هذا.مفتوح
            اطبع(ن"كتابة إلى {هذا.اسم_الملف}: {محتوى}")
        نهاية
    نهاية
نهاية

// استخدام / Usage
{
    ملف م = ملف("data.txt")
    م.اكتب("مرحبا")
    // Destructor called automatically here when scope ends
}
// Output:
// فتح الملف: data.txt
// كتابة إلى data.txt: مرحبا
// إغلاق الملف: data.txt

// مدير موارد مع 'مدمر' / Resource manager with 'مدمر'
صنف اتصال_قاعدة_بيانات
    خاص نص عنوان_الخادم
    خاص منطقي متصل
    
    منشئ(نص عنوان)
        هذا.عنوان_الخادم = عنوان
        هذا.متصل = صحيح
        اطبع("اتصال بقاعدة البيانات: {عنوان}")
    نهاية
    
    مدمر()
        إذا هذا.متصل
            اطبع("قطع الاتصال من: {هذا.عنوان_الخادم}")
            هذا.متصل = خطأ
        نهاية
    نهاية
    
    عام دالة استعلام(نص sql)
        إذا هذا.متصل
            اطبع("تنفيذ: {sql}")
        نهاية
    نهاية
نهاية
```

### Implementation Notes

```cpp
// include/data/types/object_instance.h (continued)
class ObjectInstance {
    int referenceCount = 0;  // For reference counting
    
public:
    void addRef() {
        referenceCount++;
    }
    
    void release() {
        referenceCount--;
        if (referenceCount == 0) {
            // Call destructor
            if (classType->destructor) {
                callDestructor();
            }
            // Free memory
            delete this;
        }
    }
    
    void callDestructor() {
        // Execute destructor body
        // ... (similar to constructor execution)
    }
};

// src/interpreter/core/garbage_collector.h
class GarbageCollector {
    std::set<ObjectInstance*> trackedObjects;
    
public:
    void track(ObjectInstance* obj) {
        trackedObjects.insert(obj);
    }
    
    void collect() {
        // Mark and sweep algorithm for cycle detection
        std::set<ObjectInstance*> reachable;
        markReachable(reachable);
        
        for (auto* obj : trackedObjects) {
            if (reachable.find(obj) == reachable.end()) {
                // Unreachable, collect it
                obj->callDestructor();
                delete obj;
            }
        }
        
        trackedObjects = reachable;
    }
};
```

---

## 📝 Tasks (مهام التنفيذ)

### Task 1: Basic Classes (4 أيام)
- [ ] Class declaration parsing
- [ ] Field and method parsing
- [ ] Constructor/destructor parsing
- [ ] Class type registration
- **Files:** `src/parser/parser_classes.cpp`, `include/data/types/class_type.h`
- **Acceptance:** Can define and instantiate basic classes

### Task 2: Object Instantiation (3 أيام)
- [ ] Object instance creation
- [ ] Field initialization
- [ ] Constructor execution
- [ ] Member access (fields and methods)
- **Files:** `src/interpreter/expressions/instantiation_evaluator.cpp`
- **Acceptance:** Can create objects and call methods

### Task 3: Inheritance (5 أيام)
- [ ] Base class parsing with support for both ':' and 'يرث' keywords
- [ ] Add KEYWORD_INHERITS token type in lexer
- [ ] Update parseClassDecl() to accept both inheritance syntaxes
- [ ] Method resolution with inheritance
- [ ] Super calls
- [ ] Polymorphism support
- **Files:** `src/parser/parser_classes.cpp`, `include/lexer/token.h`, `src/lexer/lexer_keywords.cpp`, `src/interpreter/core/method_resolution.cpp`
- **Acceptance:** Inheritance works correctly with both ':' and 'يرث' keywords

### Task 4: Visibility & Encapsulation (2 أيام)
- [ ] Public/private/protected modifiers
- [ ] Access checking
- [ ] Error messages for invalid access
- **Files:** `src/interpreter/core/access_checker.cpp`
- **Acceptance:** Private members are protected

### Task 5: Static Members (2 أيام)
- [ ] Static field storage
- [ ] Static method execution
- [ ] Static access without instances
- **Files:** `src/interpreter/expressions/static_access_evaluator.cpp`
- **Acceptance:** Static members work correctly

### Task 6: Memory Management (4 أيام)
- [ ] Reference counting
- [ ] Destructor execution
- [ ] Garbage collector for cycles
- [ ] RAII pattern support
- **Files:** `src/interpreter/core/garbage_collector.cpp`
- **Acceptance:** No memory leaks, destructors called

**Total Time:** 20 يوماً (160 ساعة)

---

## 🔧 Git Workflow

```powershell
# إنشاء فرع / Create branch
git checkout -b feat/spec/oop

# إضافة الملفات / Add files
git add docs/language_spec/rules/03_oop.md
git add tests/spec_rules/oop_*.s
git add include/parser/ast/class_nodes.h
git add include/data/types/class_type.h
git add include/data/types/object_instance.h
git add src/parser/parser_classes.cpp
git add src/interpreter/statements/class_interpreter.cpp
git add src/interpreter/expressions/instantiation_evaluator.cpp
git add src/interpreter/expressions/member_access_evaluator.cpp
git add src/interpreter/core/garbage_collector.cpp

# Commit
git commit -m "[spec/oop] Add complete OOP system rules

- Class declarations with fields and methods
- Constructors and destructors
- Single inheritance with method overriding
- Polymorphism and super calls
- Public/private/protected visibility
- Static members
- Memory management (reference counting + GC)
- 2 test files with 40+ test cases"

# Push
git push --set-upstream origin feat/spec/oop
```

---

**آخر تحديث:** 5 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 60+
