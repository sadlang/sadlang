# ⚡ ميزات C++ في لغة "ص" / C++ Features in Sad Language

**التاريخ:** 7 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مرجع تفصيلي لميزات C++

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق كيفية دمج **ميزات C++** القوية في لغة "ص"، بما في ذلك الكتابة الثابتة الاختيارية، القوالب/Generics، RAII، Move Semantics، تحميل المعاملات، والربط مع الكود الأصلي (Native Bindings).

### (EN) Description
This file documents how to integrate powerful **C++ features** into Sad language, including optional static typing, templates/generics, RAII, move semantics, operator overloading, and native bindings.

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/language_spec/01_types.md` - Type system
- ✅ `docs/language_spec/02_functions.md` - Function generics
- ✅ `docs/language_spec/03_oop.md` - RAII with destructors
- ✅ `plans/imp/12_interpreter_runtime_plan.md` - Runtime type checking

---

## 📋 قائمة الميزات / Features List

| # | الميزة (AR) | Feature (EN) | الحالة | الأولوية |
|---|-----------|-----------|-------|---------|
| 1 | الكتابة الثابتة الاختيارية | Optional Static Typing | ✅ Core | P0 |
| 2 | القوالب/الأنواع العامة | Generics/Templates | 🚧 Phase 2 | P1 |
| 3 | إدارة الموارد RAII | RAII | ✅ Core | P0 |
| 4 | دلالات النقل | Move Semantics | 🚧 Phase 2 | P2 |
| 5 | تحميل المعاملات | Operator Overloading | 🚧 Phase 2 | P1 |
| 6 | الربط الأصلي | Native Bindings | 🚧 Phase 2 | P1 |
| 7 | التعابير الثابتة | Const Expressions | 🚧 Phase 2 | P2 |
| 8 | المؤشرات الذكية | Smart Pointers | 🚧 Phase 2 | P2 |

---

## 1️⃣ Optional Static Typing (الكتابة الثابتة الاختيارية)

### Syntax (EBNF)
```ebnf
typed_declaration ::= type IDENTIFIER '=' expression

type ::= 'رقم' | 'عشري' | 'نص' | 'منطقي' | 'مصفوفة' | 'قاموس'
       | 'int' | 'float' | 'string' | 'bool' | 'array' | 'dict'
       | generic_type

generic_type ::= type '<' type_list '>'

type_list ::= type ('،' type)*

// Function with typed parameters
typed_function ::= 'دالة' [return_type] IDENTIFIER '(' typed_param_list ')' block

typed_param_list ::= typed_param ('،' typed_param)*

typed_param ::= type IDENTIFIER
```

### الشرح بالعربية
الكتابة الثابتة الاختيارية تسمح للمطورين بتحديد الأنواع صراحةً للحصول على فحص أفضل للأخطاء وأداء محسّن. يمكن خلط الكتابة الثابتة والديناميكية في نفس البرنامج.

### (EN) Explanation
Optional static typing allows developers to explicitly specify types for better error checking and improved performance. Static and dynamic typing can be mixed in the same program.

### Examples

```s
// متغيرات مكتوبة / Typed variables
رقم عمر = 25
عشري سعر = 19.99
نص اسم = "أحمد"
منطقي نشط = صحيح

// دالة مكتوبة بالكامل / Fully typed function
دالة رقم جمع(رقم أ، رقم ب)
    إرجاع أ + ب
نهاية

// مصفوفة مكتوبة / Typed array
مصفوفة<رقم> أعداد = [1، 2، 3، 4، 5]

// قاموس مكتوب / Typed dictionary
قاموس<نص، رقم> أعمار = {
    "أحمد": 25،
    "فاطمة": 30،
    "محمد": 22
}

// دالة بنوع إرجاع اختياري / Function with optional return type
دالة نص؟ احصل_على_اسم(رقم معرف)
    إذا معرف > 0
        إرجاع "مستخدم_" + نص(معرف)
    نهاية
    إرجاع فارغ
نهاية

// خلط الأنواع الثابتة والديناميكية / Mix static and dynamic
رقم ع = 10          // Statically typed
أي ص = 20           // Dynamically typed
أي نتيجة = ع + ص   // Works fine

// فحص الأنواع في وقت التشغيل / Runtime type checking
دالة معالج(رقم قيمة)
    // Type is enforced
    إرجاع قيمة * 2
نهاية

// معالج("نص")  // Error: Expected رقم, got نص
```

### Implementation Notes

```cpp
// include/data/types/type_system.h
class TypeSystem {
public:
    enum TypeKind {
        INTEGER,
        FLOAT,
        STRING,
        BOOL,
        ARRAY,
        DICT,
        FUNCTION,
        OBJECT,
        ANY,
        NULL_TYPE,
        OPTIONAL,  // T?
        GENERIC    // T<U>
    };
    
    class Type {
    public:
        TypeKind kind;
        std::vector<Type*> genericArgs;  // For generic types
        bool isOptional = false;
        
        Type(TypeKind k) : kind(k) {}
        
        bool matches(const Type* other) const;
        bool isAssignableFrom(const Type* other) const;
        std::string toString() const;
    };
    
    // Type checking
    static bool checkType(Value value, Type* expectedType);
    static Type* inferType(Expr* expr);
};

// src/interpreter/core/type_checker.cpp
bool TypeSystem::checkType(Value value, Type* expectedType) {
    if (expectedType->kind == TypeKind::ANY) {
        return true;  // ANY accepts everything
    }
    
    if (expectedType->isOptional && value.isNull()) {
        return true;  // Optional types accept null
    }
    
    // Check base type
    switch (expectedType->kind) {
        case TypeKind::INTEGER:
            return value.isInt();
        case TypeKind::FLOAT:
            return value.isFloat() || value.isInt();  // Int can upgrade to Float
        case TypeKind::STRING:
            return value.isString();
        case TypeKind::BOOL:
            return value.isBool();
        case TypeKind::ARRAY:
            if (!value.isArray()) return false;
            // Check generic args if present
            if (!expectedType->genericArgs.empty()) {
                auto& elements = value.asArray();
                Type* elementType = expectedType->genericArgs[0];
                for (const auto& elem : elements) {
                    if (!checkType(elem, elementType)) {
                        return false;
                    }
                }
            }
            return true;
        // ... other types
    }
    
    return false;
}

// Type inference for expressions
Type* TypeSystem::inferType(Expr* expr) {
    if (auto* literal = dynamic_cast<LiteralExpr*>(expr)) {
        if (literal->value.isInt()) return new Type(TypeKind::INTEGER);
        if (literal->value.isFloat()) return new Type(TypeKind::FLOAT);
        if (literal->value.isString()) return new Type(TypeKind::STRING);
        if (literal->value.isBool()) return new Type(TypeKind::BOOL);
    }
    
    if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        Type* leftType = inferType(binary->left.get());
        Type* rightType = inferType(binary->right.get());
        
        // Arithmetic operations
        if (binary->op == TokenType::PLUS || 
            binary->op == TokenType::MINUS ||
            binary->op == TokenType::MULTIPLY ||
            binary->op == TokenType::DIVIDE) {
            
            // If either is float, result is float
            if (leftType->kind == TypeKind::FLOAT || 
                rightType->kind == TypeKind::FLOAT) {
                return new Type(TypeKind::FLOAT);
            }
            return new Type(TypeKind::INTEGER);
        }
        
        // Comparison operations return bool
        if (binary->op == TokenType::EQUAL_EQUAL ||
            binary->op == TokenType::NOT_EQUAL ||
            binary->op == TokenType::LESS ||
            binary->op == TokenType::GREATER) {
            return new Type(TypeKind::BOOL);
        }
    }
    
    // Default to ANY
    return new Type(TypeKind::ANY);
}
```

### Tests
**File:** `tests/spec_rules/types_static.s`
```s
// اختبار الأنواع الثابتة / Test static types
رقم ع = 10
عشري ص = 20.5
نص نص1 = "مرحبا"

// دالة مكتوبة / Typed function
دالة رقم مضاعف(رقم س)
    إرجاع س * 2
نهاية

اطبع(مضاعف(5))  // Expected: 10

// مصفوفة مكتوبة / Typed array
مصفوفة<رقم> أرقام = [1، 2، 3]
// أرقام.أضف("نص")  // Should error: type mismatch
```

---

## 2️⃣ Generics/Templates (القوالب)

### Syntax (EBNF)
```ebnf
generic_function ::= 'دالة' '<' type_params '>' [return_type] IDENTIFIER '(' [param_list] ')' block

type_params ::= type_param ('،' type_param)*

type_param ::= IDENTIFIER

generic_class ::= 'صنف' '<' type_params '>' IDENTIFIER class_body 'نهاية'
```

### الشرح بالعربية
القوالب (Generics) تسمح بكتابة كود يعمل مع أنواع متعددة دون تكرار. مفيدة جداً لهياكل البيانات والخوارزميات العامة.

### Examples

```s
// دالة عامة / Generic function
// تبديل قيمتين / Swap two values
دالة <T> تبديل(T أ، T ب)
    T مؤقت = أ
    أ = ب
    ب = مؤقت
نهاية

رقم س = 5
رقم ص = 10
تبديل<رقم>(س، ص)
اطبع(س، ص)  // 10، 5

// صنف عام / Generic class
// صندوق يحتوي على أي نوع / Box containing any type
صنف <T> صندوق
    خاص T قيمة
    
    باني(T ق)
        هذا.قيمة = ق
    نهاية
    
    عام دالة T احصل()
        إرجاع هذا.قيمة
    نهاية
    
    عام دالة ضع(T ق)
        هذا.قيمة = ق
    نهاية
نهاية

// استخدام / Usage
صندوق<رقم> صندوق_أعداد = صندوق<رقم>(42)
اطبع(صندوق_أعداد.احصل())  // 42

صندوق<نص> صندوق_نصوص = صندوق<نص>("مرحبا")
اطبع(صندوق_نصوص.احصل())  // "مرحبا"

// دالة عامة بقيود / Generic function with constraints
دالة <T: رقم | عشري> T أقصى(T أ، T ب)
    إذا أ > ب
        إرجاع أ
    نهاية
    إرجاع ب
نهاية

اطبع(أقصى<رقم>(5، 10))     // 10
اطبع(أقصى<عشري>(3.14، 2.71))  // 3.14

// مصفوفة عامة / Generic array functions
دالة <T> مصفوفة<T> رشح(مصفوفة<T> قائمة، دالة<منطقي(T)> شرط)
    مصفوفة<T> نتيجة = []
    لكل عنصر في قائمة
        إذا شرط(عنصر)
            نتيجة.أضف(عنصر)
        نهاية
    نهاية
    إرجاع نتيجة
نهاية

// استخدام / Usage
مصفوفة<رقم> أعداد = [1، 2، 3، 4، 5، 6]
مصفوفة<رقم> زوجية = رشح<رقم>(أعداد، دالة منطقي (رقم ع)
    إرجاع ع % 2 == 0
نهاية)
اطبع(زوجية)  // [2، 4، 6]

// قاموس عام / Generic dictionary
صنف <K، V> قاموس_عام
    خاص مصفوفة<زوج<K، V>> عناصر
    
    عام دالة ضع(K مفتاح، V قيمة)
        // إضافة أو تحديث / Add or update
        هذا.عناصر.أضف(زوج(مفتاح، قيمة))
    نهاية
    
    عام دالة V؟ احصل(K مفتاح)
        لكل زوج في هذا.عناصر
            إذا زوج.مفتاح == مفتاح
                إرجاع زوج.قيمة
            نهاية
        نهاية
        إرجاع فارغ
    نهاية
نهاية
```

### Implementation Notes

```cpp
// include/parser/ast/generic_nodes.h
class GenericFunctionDecl : public FunctionDecl {
public:
    std::vector<std::string> typeParameters;  // <T, U, V>
    std::map<std::string, Type*> typeConstraints;  // T: Numeric, etc.
    
    GenericFunctionDecl(std::vector<std::string> typeParams,
                       std::string name,
                       std::vector<Parameter> params,
                       Type* returnType,
                       std::unique_ptr<BlockStmt> body)
        : FunctionDecl(name, params, returnType, std::move(body)),
          typeParameters(std::move(typeParams)) {}
};

class GenericInstantiation : public Expr {
public:
    std::string genericName;
    std::vector<Type*> typeArguments;
    
    // For generic function call: func<int>(args)
    // For generic class instantiation: Class<int>(args)
};

// src/interpreter/core/generic_instantiator.cpp
class GenericInstantiator {
public:
    // Instantiate generic function with concrete types
    FunctionDecl* instantiate(GenericFunctionDecl* generic,
                             const std::vector<Type*>& typeArgs) {
        // Create type substitution map
        std::map<std::string, Type*> substitutions;
        for (size_t i = 0; i < generic->typeParameters.size(); i++) {
            substitutions[generic->typeParameters[i]] = typeArgs[i];
        }
        
        // Clone function with type substitutions
        auto* instantiated = cloneFunctionWithTypes(generic, substitutions);
        
        // Cache instantiation for reuse
        cacheInstantiation(generic, typeArgs, instantiated);
        
        return instantiated;
    }
    
private:
    std::map<std::string, 
             std::map<std::vector<Type*>, FunctionDecl*>> cache;
    
    FunctionDecl* cloneFunctionWithTypes(
        GenericFunctionDecl* generic,
        const std::map<std::string, Type*>& substitutions) {
        
        // Clone function body
        // Replace type parameters with concrete types
        // Return specialized version
    }
};
```

---

## 3️⃣ RAII (Resource Acquisition Is Initialization)

### الشرح بالعربية
RAII هو نمط برمجي يربط دورة حياة الموارد (ملفات، ذاكرة، اتصالات) بدورة حياة الكائنات. عندما يُنشأ الكائن، يُحصّل المورد. عندما يُدمّر الكائن، يُحرّر المورد تلقائياً.

### Examples

```s
// مثال RAII - إدارة الملفات / RAII example - File management
صنف ملف
    خاص نص مسار
    خاص منطقي مفتوح
    
    باني(نص مسار_ملف، نص وضع)
        هذا.مسار = مسار_ملف
        هذا.مفتوح = صحيح
        اطبع("فتح الملف: {مسار_ملف}")
        // فتح الملف فعلياً / Actually open file
    نهاية
    
    ~باني()  // Destructor - RAII cleanup
        إذا (هذا.مفتوح)
            اطبع("إغلاق الملف: {هذا.مسار}")
            // إغلاق الملف فعلياً / Actually close file
            هذا.مفتوح = خطأ
        نهاية
    نهاية
    
    عام دالة نص اقرأ()
        إذا (!هذا.مفتوح)
            ارفع خطأ("الملف مغلق")
        نهاية
        // قراءة الملف / Read file
        إرجاع "محتوى الملف"
    نهاية
نهاية

// استخدام RAII / Using RAII
دالة معالج_ملف()
    ملف م = ملف("data.txt"، "قراءة")
    نص محتوى = م.اقرأ()
    اطبع(محتوى)
    // الملف يُغلق تلقائياً عند انتهاء النطاق / File closes automatically
نهاية
// Output:
// فتح الملف: data.txt
// محتوى الملف
// إغلاق الملف: data.txt

// مثال: قفل للمزامنة / Example: Lock for synchronization
صنف قفل
    خاص نص اسم_القفل
    
    باني(نص اسم)
        هذا.اسم_القفل = اسم
        اطبع(ن"اكتساب القفل: {اسم}")
        // اكتساب القفل فعلياً / Actually acquire lock
    نهاية
    
    ~باني()
        اطبع(ن"تحرير القفل: {هذا.اسم_القفل}")
        // تحرير القفل فعلياً / Actually release lock
    نهاية
نهاية

دالة عملية_متزامنة()
    قفل ق = قفل("قفل_رئيسي")
    
    // العمليات المحمية / Protected operations
    اطبع("تنفيذ عملية آمنة")
    
    // القفل يُحرر تلقائياً / Lock released automatically
نهاية

// مثال: مؤقت تلقائي / Example: Auto timer
صنف مؤقت
    خاص عشري وقت_بداية
    خاص نص اسم
    
    باني(نص اسم)
        هذا.اسم = اسم
        هذا.وقت_بداية = الوقت.الآن()
    نهاية
    
    ~باني()
        عشري مدة = الوقت.الآن() - هذا.وقت_بداية
        اطبع(ن"{هذا.اسم} استغرق {مدة} ثانية")
    نهاية
نهاية

دالة عملية_بطيئة()
    مؤقت م = مؤقت("عملية_بطيئة")
    
    // عمليات معقدة / Complex operations
    رقم مجموع = 0
    لكل ع في نطاق(1000000)
        مجموع = مجموع + ع
    نهاية
    
    // التوقيت يُطبع تلقائياً / Timing printed automatically
نهاية
```

---

## 4️⃣ Operator Overloading (تحميل المعاملات)

### Syntax (EBNF)
```ebnf
operator_overload ::= 'عامل' operator '(' param_list ')' block

operator ::= '+' | '-' | '*' | '/' | '%' | '==' | '!=' | '<' | '>' | '<=' | '>=' | '[]'
```

### Examples

```s
// صنف مع تحميل معاملات / Class with operator overloading
صنف متجه
    عام عشري س
    عام عشري ص
    
    باني(عشري س، عشري ص)
        هذا.س = س
        هذا.ص = ص
    نهاية
    
    // تحميل معامل الجمع / Overload + operator
    عامل متجه +(متجه آخر)
        إرجاع متجه(هذا.س + آخر.س، هذا.ص + آخر.ص)
    نهاية
    
    // تحميل معامل الطرح / Overload - operator
    عامل متجه -(متجه آخر)
        إرجاع متجه(هذا.س - آخر.س، هذا.ص - آخر.ص)
    نهاية
    
    // تحميل معامل الضرب العددي / Overload * for scalar
    عامل متجه *(عشري عددي)
        إرجاع متجه(هذا.س * عددي، هذا.ص * عددي)
    نهاية
    
    // تحميل معامل المساواة / Overload == operator
    عامل منطقي ==(متجه آخر)
        إرجاع هذا.س == آخر.س && هذا.ص == آخر.ص
    نهاية
    
    // تحميل معامل [] للوصول / Overload [] for access
    عامل عشري [](رقم فهرس)
        إذا فهرس == 0
            إرجاع هذا.س
        وإلا_إذا فهرس == 1
            إرجاع هذا.ص
        نهاية
        ارفع خطأ_فهرس("فهرس خارج النطاق")
    نهاية
    
    // تحويل إلى نص / Convert to string
    عامل نص ()
        إرجاع ن"متجه({هذا.س}، {هذا.ص})"
    نهاية
نهاية

// استخدام / Usage
متجه م1 = متجه(3.0، 4.0)
متجه م2 = متجه(1.0، 2.0)

متجه م3 = م1 + م2      // استخدام + / Using +
اطبع(م3)               // متجه(4.0، 6.0)

متجه م4 = م1 * 2.0     // استخدام * / Using *
اطبع(م4)               // متجه(6.0، 8.0)

منطقي متساوي = م1 == م2  // استخدام == / Using ==
اطبع(متساوي)           // خطأ / false

عشري س_قيمة = م1[0]   // استخدام [] / Using []
اطبع(س_قيمة)          // 3.0

// صنف كسر مع معاملات / Fraction class with operators
صنف كسر
    خاص رقم بسط
    خاص رقم مقام
    
    باني(رقم ب، رقم م)
        إذا م == 0
            ارفع خطأ_قيمة("المقام لا يمكن أن يكون صفراً")
        نهاية
        هذا.بسط = ب
        هذا.مقام = م
        هذا.بسّط()
    نهاية
    
    خاص دالة بسّط()
        رقم ق = ق_م_م(هذا.بسط، هذا.مقام)
        هذا.بسط = هذا.بسط / ق
        هذا.مقام = هذا.مقام / ق
    نهاية
    
    عامل كسر +(كسر آخر)
        رقم بسط_جديد = (هذا.بسط * آخر.مقام) + (آخر.بسط * هذا.مقام)
        رقم مقام_جديد = هذا.مقام * آخر.مقام
        إرجاع كسر(بسط_جديد، مقام_جديد)
    نهاية
    
    عامل كسر *(كسر آخر)
        إرجاع كسر(هذا.بسط * آخر.بسط، هذا.مقام * آخر.مقام)
    نهاية
    
    عامل نص ()
        إرجاع "{هذا.بسط}/{هذا.مقام}"
    نهاية
نهاية

كسر ك1 = كسر(1، 2)  // 1/2
كسر ك2 = كسر(1، 3)  // 1/3
كسر ك3 = ك1 + ك2     // 5/6
اطبع(ك3)             // "5/6"
```

### Implementation Notes

```cpp
// include/parser/ast/operator_nodes.h
class OperatorOverload : public MethodDecl {
public:
    TokenType operatorType;  // Which operator is being overloaded
    
    OperatorOverload(TokenType op,
                    std::vector<Parameter> params,
                    Type* returnType,
                    std::unique_ptr<BlockStmt> body)
        : MethodDecl(getOperatorName(op), params, returnType, std::move(body)),
          operatorType(op) {}
    
    static std::string getOperatorName(TokenType op) {
        switch (op) {
            case TokenType::PLUS: return "__add__";
            case TokenType::MINUS: return "__sub__";
            case TokenType::MULTIPLY: return "__mul__";
            case TokenType::DIVIDE: return "__div__";
            case TokenType::EQUAL_EQUAL: return "__eq__";
            case TokenType::LEFT_BRACKET: return "__getitem__";
            // ... more operators
            default: return "";
        }
    }
};

// src/interpreter/expressions/binary_evaluator.cpp (modified)
Value InterpreterCore::visitBinaryExpr(BinaryExpr* expr) {
    expr->left->accept(*this);
    Value left = lastValue;
    
    expr->right->accept(*this);
    Value right = lastValue;
    
    // Check if left operand has operator overload
    if (left.isObject()) {
        ObjectInstance* obj = left.asObject();
        std::string operatorMethod = OperatorOverload::getOperatorName(expr->op);
        
        if (obj->classType->methods.find(operatorMethod) != 
            obj->classType->methods.end()) {
            
            // Call overloaded operator
            std::vector<Value> args = {right};
            return obj->callMethod(operatorMethod, args, this);
        }
    }
    
    // Fall back to built-in operators
    switch (expr->op) {
        case TokenType::PLUS:
            if (left.isInt() && right.isInt()) {
                return Value::makeInt(left.asInt() + right.asInt());
            }
            // ... other cases
    }
}
```

---

## 5️⃣ Native Bindings (الربط الأصلي)

### الشرح بالعربية
الربط الأصلي يسمح باستدعاء دوال C/C++ مباشرة من كود "ص". مفيد للأداء العالي والوصول إلى مكتبات النظام.

### Examples

```s
// تعريف دالة أصلية / Define native function
@أصلي("calculate_fast")
دالة رقم حساب_سريع(رقم أ، رقم ب)
    // الجسم يُنفذ بواسطة كود C++ / Body executed by C++ code
نهاية

// استخدام / Usage
رقم نتيجة = حساب_سريع(1000، 2000)
اطبع(نتيجة)

// تحميل مكتبة خارجية / Load external library
استورد أصلي من "mylib.dll"

// استخدام دوال المكتبة / Use library functions
رقم قيمة = أصلي.دالة_معقدة(10، 20، 30)
```

### Implementation Notes

```cpp
// include/interpreter/core/native_bridge.h
class NativeBridge {
public:
    using NativeFunction = Value (*)(const std::vector<Value>&);
    
    // Register native function
    void registerNative(const std::string& name, NativeFunction func);
    
    // Call native function
    Value callNative(const std::string& name, const std::vector<Value>& args);
    
    // Load native library
    void loadLibrary(const std::string& path);
    
private:
    std::map<std::string, NativeFunction> nativeFunctions;
    std::vector<void*> loadedLibraries;  // DLL/SO handles
};

// Example native function implementation
extern "C" {
    Value sad_native_calculate_fast(const std::vector<Value>& args) {
        if (args.size() != 2) {
            throw std::runtime_error("Expected 2 arguments");
        }
        
        int a = args[0].asInt();
        int b = args[1].asInt();
        
        // Fast C++ calculation
        int result = a * b + (a + b) * 2;
        
        return Value::makeInt(result);
    }
}

// Registration in interpreter
void InterpreterCore::registerBuiltinNatives() {
    nativeBridge.registerNative("calculate_fast", sad_native_calculate_fast);
    // Register more natives...
}
```

---

## 📝 ملخص التنفيذ / Implementation Summary

### Files to Create

1. **Type System:**
   - `include/data/types/type_system.h`
   - `src/interpreter/core/type_checker.cpp`

2. **Generics:**
   - `include/parser/ast/generic_nodes.h`
   - `src/interpreter/core/generic_instantiator.cpp`

3. **Operator Overloading:**
   - `include/parser/ast/operator_nodes.h`
   - `src/parser/parser_operators.cpp`

4. **Native Bindings:**
   - `include/interpreter/core/native_bridge.h`
   - `src/interpreter/core/native_bridge.cpp`

### Keywords to Add

```cpp
// src/lexer/lexer_keywords.cpp
keywords["أصلي"] = TokenType::KEYWORD_NATIVE;
keywords["native"] = TokenType::KEYWORD_NATIVE_EN;
keywords["عامل"] = TokenType::KEYWORD_OPERATOR;
keywords["operator"] = TokenType::KEYWORD_OPERATOR_EN;
```

---

## 🔧 Git Workflow

```powershell
# إنشاء فرع / Create branch
git checkout -b feat/spec/cpp-features

# إضافة الملفات / Add files
git add docs/language_spec/rules/06_cpp_features.md
git add tests/spec_rules/types_static.s
git add tests/spec_rules/cpp_templates_example.s

# Commit
git commit -m "[spec/cpp] Add C++ features specification

- Optional static typing
- Generics/Templates
- RAII with destructors
- Operator overloading
- Native bindings
- Examples and implementation notes"

# Push
git push --set-upstream origin feat/spec/cpp-features
```

---

**آخر تحديث:** 7 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 35+
