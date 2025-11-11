# 🔧 الدوال في لغة "ص" / Functions in Sad Language

**تاريخ الإنشاء:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مسودة عمل

---

## 🎯 نظرة عامة / Overview

### (AR) نظرة عامة
نظام الدوال في لغة "ص" يجمع بين بساطة Python ومرونة JavaScript وقوة C++:
- دوال من الدرجة الأولى (First-class functions)
- إغلاقات (Closures) مع التقاط المتغيرات
- دوال Lambda والدوال المجهولة
- معاملات اختيارية وافتراضية
- Varargs و Kwargs
- Decorators للبرمجة meta
- Generic functions مع Type parameters

### (EN) Overview
Sad's function system combines Python's simplicity, JavaScript's flexibility, and C++'s power with first-class functions, closures, lambdas, optional/default parameters, varargs, decorators, and generics.

---

## 📋 جدول المحتويات / Table of Contents

1. [تعريف الدوال الأساسية](#1️⃣-تعريف-الدوال-الأساسية--basic-function-definition)
2. [المعاملات Parameters](#2️⃣-المعاملات--parameters)
3. [دوال Lambda](#3️⃣-دوال-lambda--lambda-functions)
4. [الإغلاقات Closures](#4️⃣-الإغلاقات--closures)
5. [الدوال العليا Higher-Order](#5️⃣-الدوال-العليا--higher-order-functions)
6. [Decorators](#6️⃣-المزخرفات--decorators)
7. [Generic Functions](#7️⃣-الدوال-العامة--generic-functions)
8. [قواعد BNF](#8️⃣-قواعد-bnf--bnf-grammar)
9. [تمثيل AST](#9️⃣-تمثيل-ast--ast-representation)
10. [مهام التنفيذ](#🔨-مهام-التنفيذ--implementation-tasks)

---

## 1️⃣ تعريف الدوال الأساسية / Basic Function Definition

### 1.1 الصيغة الأساسية / Basic Syntax

```s
# دالة بسيطة بدون معاملات
دالة قل_مرحبا()
    اطبع("مرحباً بك!")
نهاية

# استدعاء الدالة
قل_مرحبا()  # "مرحباً بك!"
```

### 1.2 دالة بمعاملات / Function with Parameters

```s
# دالة بمعامل واحد
دالة حي(نص اسم)
    اطبع(ن"مرحباً {اسم}!")
نهاية

حي("أحمد")  # "مرحباً أحمد!"

# دالة بمعاملات متعددة
دالة اجمع(رقم أ، رقم ب) = رقم
    إرجاع أ + ب
نهاية

رقم مجموع = اجمع(5, 3)  # 8
```

### 1.3 قيم الإرجاع / Return Values

```s
# إرجاع صريح
دالة مربع(رقم س) = رقم
    إرجاع س * س
نهاية

# إرجاع ضمني (آخر تعبير)
دالة ضعف(رقم س) = رقم
    س * 2  # إرجاع ضمني
نهاية

# إرجاع قيم متعددة (tuple)
دالة قسمة_مع_باقي(رقم أ، رقم ب) = (رقم، رقم)
    إرجاع (أ / ب، أ % ب)
نهاية

(خارج، باقي) = قسمة_مع_باقي(10, 3)  # (3, 1)

# دالة بدون قيمة إرجاع
دالة اطبع_رسالة(نص رسالة)
    اطبع(رسالة)
    # إرجاع ضمني: لاشيء
نهاية
```

### 1.4 الصيغة المختصرة / Short Syntax

```s
# دالة سطر واحد
دالة ضعف(رقم س) = رقم: س * 2

# مع معاملات متعددة
دالة اكبر(رقم أ، رقم ب) = رقم: أ إذا أ > ب وإلا ب
```

---

## 2️⃣ المعاملات / Parameters

### 2.1 معاملات اختيارية / Optional Parameters

```s
# معامل اختياري مع قيمة افتراضية
دالة حي(نص اسم، نص تحية = "مرحباً")
    اطبع(ن"{تحية} {اسم}!")
نهاية

حي("أحمد")              # "مرحباً أحمد!"
حي("أحمد"، "أهلاً")     # "أهلاً أحمد!"
```

### 2.2 معاملات مسماة / Named Parameters

```s
دالة أنشئ_مستخدم(نص اسم، رقم عمر، منطقي نشط = صحيح)
    # ...
نهاية

# استدعاء بالترتيب
أنشئ_مستخدم("أحمد"، 25)

# استدعاء بأسماء المعاملات
أنشئ_مستخدم(اسم: "أحمد"، عمر: 25، نشط: خطأ)

# خلط (المسماة تأتي بعد الموضعية)
أنشئ_مستخدم("أحمد"، عمر: 25، نشط: خطأ)
```

### 2.3 معاملات متغيرة Varargs

```s
# عدد متغير من المعاملات
دالة اجمع_كل(*مصفوفة<رقم> أرقام) = رقم
    رقم مجموع = 0
    لكل رقم في أرقام
        مجموع += رقم
    نهاية
    إرجاع مجموع
نهاية

اجمع_كل(1، 2، 3)              # 6
اجمع_كل(1، 2، 3، 4، 5)        # 15

# مع معاملات عادية
دالة اطبع_رسالة(نص عنوان، *مصفوفة<نص> أسطر)
    اطبع(عنوان)
    لكل سطر في أسطر
        اطبع(ن"  - {سطر}")
    نهاية
نهاية

اطبع_رسالة("المهام:"، "مهمة 1"، "مهمة 2"، "مهمة 3")
```

### 2.4 معاملات Keyword Arguments (Kwargs)

```s
# معاملات keyword متغيرة
دالة أنشئ_خيارات(**خريطة<نص، أي> خيارات)
    لكل (مفتاح، قيمة) في خيارات.عناصر()
        اطبع(ن"{مفتاح} = {قيمة}")
    نهاية
نهاية

أنشئ_خيارات(لون: "أحمر"، حجم: 14، نشط: صحيح)
# لون = أحمر
# حجم = 14
# نشط = صحيح

# دمج varargs و kwargs
دالة دالة_مرنة(نص اسم، *أرقام، **خيارات)
    اطبع(ن"الاسم: {اسم}")
    اطبع(ن"الأرقام: {أرقام}")
    اطبع(ن"الخيارات: {خيارات}")
نهاية

دالة_مرنة("أحمد"، 1، 2، 3، لون: "أزرق"، عمر: 25)
```

### 2.5 تمرير بالقيمة والمرجع / Pass by Value/Reference

```s
# الأنواع الأساسية: تمرير بالقيمة
دالة غير_العدد(رقم س)
    س = 100  # لا يؤثر على المتغير الأصلي
نهاية

رقم عدد = 5
غير_العدد(عدد)
اطبع(عدد)  # 5 (لم يتغير)

# المراجع (Objects): تمرير بالمرجع
دالة غير_القائمة(مصفوفة<رقم> قائمة)
    قائمة.أضف(100)  # يؤثر على القائمة الأصلية
نهاية

مصفوفة<رقم> أرقام = [1، 2، 3]
غير_القائمة(أرقام)
اطبع(أرقام)  # [1، 2، 3، 100]

# تمرير صريح بالمرجع
دالة ضعف_العدد(مرجع رقم س)
    س *= 2
نهاية

رقم قيمة = 10
ضعف_العدد(مرجع قيمة)
اطبع(قيمة)  # 20
```

---

## 3️⃣ دوال Lambda / Lambda Functions

### 3.1 الصيغة الأساسية / Basic Syntax

```s
# lambda بسيطة
أي ضعف = lambda (رقم س) = س * 2
اطبع(ضعف(5))  # 10

# lambda بمعاملات متعددة
أي اجمع = lambda (رقم أ، رقم ب) = أ + ب
اطبع(اجمع(3، 7))  # 10

# lambda بدون معاملات
أي رسالة = lambda () = "مرحباً!"
اطبع(رسالة())  # "مرحباً!"
```

### 3.2 Lambda كمعامل / Lambda as Argument

```s
# استخدام lambda مع دوال higher-order
مصفوفة<رقم> أرقام = [1، 2، 3، 4، 5]

# map
مصفوفة<رقم> مربعات = أرقام.خرطة(lambda (رقم س) = س * س)
# [1، 4، 9، 16، 25]

# filter
مصفوفة<رقم> زوجية = أرقام.رشح(lambda (رقم س) = س % 2 == 0)
# [2، 4]

# sort بمفتاح
مصفوفة<نص> أسماء = ["أحمد"، "فاطمة"، "علي"]
أسماء.رتب(مفتاح: lambda (نص اسم) = طول(اسم))
```

### 3.3 Lambda متعدد الأسطر / Multi-line Lambda

```s
# lambda معقدة
أي معالج = lambda (رقم س) = {
    إذا س < 0
        إرجاع 0
    وإلا_إذا س > 100
        إرجاع 100
    وإلا
        إرجاع س
    نهاية
}

اطبع(معالج(-5))   # 0
اطبع(معالج(50))   # 50
اطبع(معالج(150))  # 100
```

---

## 4️⃣ الإغلاقات / Closures

### 4.1 التقاط المتغيرات / Variable Capture

```s
# دالة تُرجع دالة
دالة منشئ_عداد() = دالة<رقم>
    رقم عداد = 0  # متغير محلي يُلتقط
    
    دالة زد()
        عداد += 1
        إرجاع عداد
    نهاية
    
    إرجاع زد
نهاية

أي عداد1 = منشئ_عداد()
اطبع(عداد1())  # 1
اطبع(عداد1())  # 2
اطبع(عداد1())  # 3

أي عداد2 = منشئ_عداد()
اطبع(عداد2())  # 1 (عداد منفصل)
```

### 4.2 إغلاقات مع معاملات / Closures with Parameters

```s
# مولد دوال ضرب
دالة منشئ_مضاعف(رقم معامل) = دالة<رقم>
    إرجاع lambda (رقم س) = س * معامل
نهاية

أي ضعف = منشئ_مضاعف(2)
أي ثلاثي = منشئ_مضاعف(3)

اطبع(ضعف(5))   # 10
اطبع(ثلاثي(5))  # 15
```

### 4.3 تطبيقات عملية / Practical Applications

```s
# Currying
دالة اجمع_مع(رقم أ) = دالة<رقم>
    إرجاع lambda (رقم ب) = أ + ب
نهاية

أي اجمع_5 = اجمع_مع(5)
اطبع(اجمع_5(3))   # 8
اطبع(اجمع_5(10))  # 15

# Memoization
دالة منشئ_ذاكرة(دالة<رقم، رقم> دالة) = دالة<رقم>
    خريطة<رقم، رقم> ذاكرة = {}
    
    إرجاع lambda (رقم س) = {
        إذا س في ذاكرة
            إرجاع ذاكرة[س]
        نهاية
        
        رقم نتيجة = دالة(س)
        ذاكرة[س] = نتيجة
        إرجاع نتيجة
    }
نهاية
```

---

## 5️⃣ الدوال العليا / Higher-Order Functions

### 5.1 الدوال المدمجة / Built-in Higher-Order Functions

```s
مصفوفة<رقم> أرقام = [1، 2، 3، 4، 5]

# خرطة / Map
مصفوفة<رقم> مربعات = أرقام.خرطة(lambda (رقم س) = س * س)
# [1، 4، 9، 16، 25]

# رشح / Filter
مصفوفة<رقم> كبيرة = أرقام.رشح(lambda (رقم س) = س > 2)
# [3، 4، 5]

# اطوِ / Reduce
رقم مجموع = أرقام.اطوِ(lambda (رقم جمع، رقم س) = جمع + س، 0)
# 15

# لكل / ForEach
أرقام.لكل(lambda (رقم س) = اطبع(س))
```

### 5.2 تعريف دوال عليا مخصصة / Custom Higher-Order Functions

```s
# دالة تُرجع دالة
دالة منشئ_مقارن(نص عملية) = دالة<منطقي، رقم، رقم>
    إذا عملية == "أكبر"
        إرجاع lambda (رقم أ، رقم ب) = أ > ب
    وإلا_إذا عملية == "أصغر"
        إرجاع lambda (رقم أ، رقم ب) = أ < ب
    وإلا
        إرجاع lambda (رقم أ، رقم ب) = أ == ب
    نهاية
نهاية

أي مقارن = منشئ_مقارن("أكبر")
اطبع(مقارن(5، 3))  # صحيح

# دالة تأخذ دالة كمعامل
دالة طبق_مرتين(دالة<رقم، رقم> دالة، رقم قيمة) = رقم
    إرجاع دالة(دالة(قيمة))
نهاية

رقم نتيجة = طبق_مرتين(lambda (رقم س) = س * 2، 5)
# 20 (5 * 2 * 2)
```

### 5.3 Function Composition

```s
# تركيب الدوال
دالة ركب(دالة<أي، أي> د1، دالة<أي، أي> د2) = دالة<أي>
    إرجاع lambda (أي س) = د1(د2(س))
نهاية

أي ضعف = lambda (رقم س) = س * 2
أي زد_واحد = lambda (رقم س) = س + 1

أي ضعف_ثم_زد = ركب(زد_واحد، ضعف)
اطبع(ضعف_ثم_زد(5))  # 11 ((5 * 2) + 1)
```

---

## 6️⃣ المزخرفات / Decorators

### 6.1 الصيغة الأساسية / Basic Syntax

```s
# تعريف decorator
دالة سجل(دالة<أي> دالة) = دالة<أي>
    دالة مغلفة(*معاملات، **kwargs)
        اطبع(ن"استدعاء {دالة.اسم} بـ {معاملات}")
        أي نتيجة = دالة(*معاملات، **kwargs)
        اطبع(ن"النتيجة: {نتيجة}")
        إرجاع نتيجة
    نهاية
    إرجاع مغلفة
نهاية

# استخدام decorator
@سجل
دالة اجمع(رقم أ، رقم ب) = رقم
    إرجاع أ + ب
نهاية

اجمع(3، 5)
# استدعاء اجمع بـ (3، 5)
# النتيجة: 8
```

### 6.2 Decorators بمعاملات / Parameterized Decorators

```s
# decorator بمعاملات
دالة كرر(رقم مرات) = دالة<دالة<أي>>
    دالة مزخرف(دالة<أي> دالة) = دالة<أي>
        دالة مغلفة(*معاملات، **kwargs)
            لكل _ في نطاق(مرات)
                دالة(*معاملات، **kwargs)
            نهاية
        نهاية
        إرجاع مغلفة
    نهاية
    إرجاع مزخرف
نهاية

@كرر(3)
دالة قل_مرحبا()
    اطبع("مرحباً!")
نهاية

قل_مرحبا()
# مرحباً!
# مرحباً!
# مرحباً!
```

### 6.3 Decorator Stack

```s
# تطبيق عدة decorators
@سجل
@كرر(2)
دالة اطبع_رسالة(نص رسالة)
    اطبع(رسالة)
نهاية

اطبع_رسالة("اختبار")
# يطبق من الأسفل للأعلى: كرر ثم سجل
```

### 6.4 Decorators مدمجة / Built-in Decorators

```s
# @ثابت - للدوال الثابتة في الفئات
فئة رياضيات
    @ثابت
    دالة اجمع(رقم أ، رقم ب) = رقم
        إرجاع أ + ب
    نهاية
نهاية

رياضيات.اجمع(3، 5)  # 8

# @خاصية - للخصائص
فئة مستطيل
    رقم عرض
    رقم ارتفاع
    
    @خاصية
    دالة مساحة() = رقم
        إرجاع عرض * ارتفاع
    نهاية
نهاية

مستطيل م = مستطيل(عرض: 5، ارتفاع: 3)
اطبع(م.مساحة)  # 15 (استدعاء بدون أقواس)

# @مخبأة - للنتائج المخزنة مؤقتاً
@مخبأة
دالة فيبوناتشي(رقم ن) = رقم
    إذا ن <= 1
        إرجاع ن
    نهاية
    إرجاع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)
نهاية
```

---

## 7️⃣ الدوال العامة / Generic Functions

### 7.1 الصيغة الأساسية / Basic Syntax

```s
# دالة عامة بمعامل نوع واحد
دالة<T> أول(مصفوفة<T> قائمة) = T
    إرجاع قائمة[0]
نهاية

رقم رقم_أول = أول<رقم>([1، 2، 3])     # 1
نص نص_أول = أول<نص>(["أ"، "ب"، "ج"])  # "أ"

# استنتاج النوع تلقائياً
رقم ر = أول([1، 2، 3])  # يستنتج T = رقم
```

### 7.2 معاملات نوع متعددة / Multiple Type Parameters

```s
# دالة عامة بمعاملي نوع
دالة<K، V> أنشئ_زوج(K مفتاح، V قيمة) = (K، V)
    إرجاع (مفتاح، قيمة)
نهاية

(نص مفتاح، رقم قيمة) = أنشئ_زوج("عمر"، 25)

# خرطة عامة
دالة<T، R> خرطة(مصفوفة<T> مصدر، دالة<R، T> محول) = مصفوفة<R>
    مصفوفة<R> نتيجة = []
    لكل عنصر في مصدر
        نتيجة.أضف(محول(عنصر))
    نهاية
    إرجاع نتيجة
نهاية

مصفوفة<نص> أرقام_نصية = خرطة<رقم، نص>(
    [1، 2، 3]،
    lambda (رقم س) = نص(س)
)
# ["1"، "2"، "3"]
```

### 7.3 قيود النوع / Type Constraints

```s
# قيد: T يجب أن يكون له عملية +
دالة<T: قابل_للجمع> اجمع_كل(مصفوفة<T> عناصر) = T
    T مجموع = عناصر[0]
    لكل عنصر في عناصر[1:]
        مجموع += عنصر
    نهاية
    إرجاع مجموع
نهاية

# يعمل مع الأرقام والنصوص
رقم مجموع_أرقام = اجمع_كل([1، 2، 3])        # 6
نص نص_مدموج = اجمع_كل(["أ"، "ب"، "ج"])     # "أبج"
```

---

## 8️⃣ قواعد BNF / BNF Grammar

```bnf
<function_def> ::= "دالة" [<type_params>] <identifier> 
                   "(" [<param_list>] ")" ["=" <type>] <function_body> "نهاية"
                 | "دالة" [<type_params>] <identifier> 
                   "(" [<param_list>] ")" "=" <type> ":" <expression>

<type_params> ::= "<" <identifier> ("," <identifier>)* ">"

<param_list> ::= <parameter> ("،" <parameter>)*

<parameter> ::= [<type>] <identifier> ["=" <expression>]
              | "*" <type> <identifier>           /* varargs */
              | "**" <type> <identifier>          /* kwargs */
              | "مرجع" <type> <identifier>        /* by reference */

<function_body> ::= <statement>+

<lambda_expr> ::= "lambda" "(" [<param_list>] ")" "=" <expression>
                | "lambda" "(" [<param_list>] ")" "=" "{" <statement>+ "}"

<function_call> ::= <identifier> "(" [<arg_list>] ")"

<arg_list> ::= <argument> ("،" <argument>)*

<argument> ::= <expression>
             | <identifier> ":" <expression>      /* named arg */
             | "*" <expression>                   /* unpack list */
             | "**" <expression>                  /* unpack dict */

<decorator> ::= "@" <identifier> ["(" [<arg_list>] ")"]

<decorated_function> ::= <decorator>+ <function_def>

<return_stmt> ::= "إرجاع" [<expression>]
```

---

## 9️⃣ تمثيل AST / AST Representation

### ملف الهيدر / Header File

```cpp
// include/parser/ast/function_ast.h

#pragma once
#include "ast_node.h"
#include <vector>
#include <optional>

namespace Sad {
namespace Parser {
namespace AST {

/**
 * @brief (AR) معامل دالة
 * @brief (EN) Function parameter
 */
struct Parameter {
    std::string name;
    std::optional<std::shared_ptr<Type>> type;
    std::optional<std::shared_ptr<Expression>> defaultValue;
    bool isVariadic = false;     // *args
    bool isKeyword = false;      // **kwargs
    bool isByReference = false;  // مرجع
};

/**
 * @brief (AR) تعريف دالة
 * @brief (EN) Function definition
 */
class FunctionDef : public Statement {
public:
    std::string name;
    std::vector<std::string> typeParameters;  // Generic type params
    std::vector<Parameter> parameters;
    std::optional<std::shared_ptr<Type>> returnType;
    std::vector<std::shared_ptr<Statement>> body;
    std::vector<std::string> decorators;
    
    FunctionDef(
        const std::string& name,
        const std::vector<Parameter>& params,
        const std::optional<std::shared_ptr<Type>>& retType,
        const std::vector<std::shared_ptr<Statement>>& body
    ) : name(name), parameters(params), returnType(retType), body(body) {}
    
    NodeType getType() const override { return NodeType::FUNCTION_DEF; }
};

/**
 * @brief (AR) تعبير Lambda
 * @brief (EN) Lambda expression
 */
class LambdaExpr : public Expression {
public:
    std::vector<Parameter> parameters;
    std::shared_ptr<Expression> body;  // Single expression
    
    LambdaExpr(
        const std::vector<Parameter>& params,
        std::shared_ptr<Expression> body
    ) : parameters(params), body(body) {}
    
    NodeType getType() const override { return NodeType::LAMBDA_EXPR; }
};

/**
 * @brief (AR) استدعاء دالة
 * @brief (EN) Function call
 */
class FunctionCall : public Expression {
public:
    std::shared_ptr<Expression> function;
    std::vector<std::shared_ptr<Expression>> arguments;
    std::map<std::string, std::shared_ptr<Expression>> namedArgs;
    
    FunctionCall(
        std::shared_ptr<Expression> func,
        const std::vector<std::shared_ptr<Expression>>& args
    ) : function(func), arguments(args) {}
    
    NodeType getType() const override { return NodeType::FUNCTION_CALL; }
};

/**
 * @brief (AR) عبارة الإرجاع
 * @brief (EN) Return statement
 */
class ReturnStmt : public Statement {
public:
    std::optional<std::shared_ptr<Expression>> value;
    
    explicit ReturnStmt(std::optional<std::shared_ptr<Expression>> val = std::nullopt)
        : value(val) {}
    
    NodeType getType() const override { return NodeType::RETURN_STMT; }
};

} // namespace AST
} // namespace Parser
} // namespace Sad
```

---

## 🔨 مهام التنفيذ / Implementation Tasks

### المرحلة 1: الدوال الأساسية (4 أيام)

#### الملفات المطلوبة:
```
include/parser/ast/
├── function_ast.h           # AST nodes للدوال
└── parameter.h              # تعريفات المعاملات

src/parser/
├── function_parser.cpp      # Parser للدوال
└── parameter_parser.cpp     # Parser للمعاملات

include/interpreter/
├── function_evaluator.h     # تقييم الدوال
└── call_stack.h             # Call stack management

src/interpreter/
├── function_evaluator.cpp
└── call_stack.cpp

tests/
└── function_tests/
    ├── basic_functions_test.s
    ├── parameters_test.s
    └── return_test.s
```

#### المهام التفصيلية:
**يوم 1-2:** AST + Parser
- تنفيذ `FunctionDef`, `FunctionCall`, `ReturnStmt`
- Parser للدوال الأساسية
- معاملات عادية واختيارية

**يوم 3:** Interpreter
- تقييم تعريفات الدوال
- استدعاء الدوال
- Call stack management

**يوم 4:** اختبارات
- 30+ test case
- Coverage ≥85%

#### فرع Git:
```powershell
git checkout -b feat/functions/basic
```

---

### المرحلة 2: Lambda و Closures (3 أيام)

#### فرع Git:
```powershell
git checkout -b feat/functions/lambda-closures
```

#### المهام:
- دوال Lambda
- إغلاقات مع التقاط المتغيرات
- دوال من الدرجة الأولى

---

### المرحلة 3: Higher-Order و Decorators (4 أيام)

#### فرع Git:
```powershell
git checkout -b feat/functions/advanced
```

#### المهام:
- Varargs و Kwargs
- Higher-order functions
- Decorator system

---

### المرحلة 4: Generic Functions (3 أيام)

#### فرع Git:
```powershell
git checkout -b feat/functions/generics
```

#### المهام:
- معاملات النوع العامة
- Type inference
- Type constraints

---

## 📊 ملخص / Summary

| الميزة / Feature | الصيغة / Syntax | مثال / Example |
|-----------------|----------------|---------------|
| Basic Function | `دالة اسم()` | `دالة قل_مرحبا()` |
| Parameters | `دالة اسم(نوع معامل)` | `دالة اجمع(رقم أ، رقم ب)` |
| Return Type | `= نوع` | `دالة مربع(رقم س) = رقم` |
| Lambda | `lambda (معاملات) = تعبير` | `lambda (س) = س * 2` |
| Varargs | `*نوع اسم` | `*مصفوفة<رقم> أرقام` |
| Kwargs | `**خريطة<نص، نوع> اسم` | `**خريطة<نص، أي> خيارات` |
| Decorator | `@مزخرف` | `@سجل` |
| Generic | `دالة<T>` | `دالة<T> أول(مصفوفة<T>)` |

---

## 🧪 اختبارات / Tests

```s
# tests/spec_tests/02_functions_test.s

# اختبار 1: دالة بسيطة
دالة ضعف(رقم س) = رقم
    إرجاع س * 2
نهاية

إذا ضعف(5) != 10
    ارمِ "فشل الاختبار: دالة بسيطة"
نهاية

# اختبار 2: معاملات اختيارية
دالة حي(نص اسم، نص تحية = "مرحباً") = نص
    إرجاع ن"{تحية} {اسم}"
نهاية

إذا حي("أحمد") != "مرحباً أحمد"
    ارمِ "فشل الاختبار: معاملات اختيارية"
نهاية

# اختبار 3: Lambda
أي مربع = lambda (رقم س) = س * س
إذا مربع(4) != 16
    ارمِ "فشل الاختبار: Lambda"
نهاية

# اختبار 4: Closure
دالة منشئ_مضاعف(رقم م) = دالة<رقم>
    إرجاع lambda (رقم س) = س * م
نهاية

أي ثلاثي = منشئ_مضاعف(3)
إذا ثلاثي(7) != 21
    ارمِ "فشل الاختبار: Closure"
نهاية

اطبع("✅ جميع اختبارات Functions نجحت")
```

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** جاهز للمراجعة والتنفيذ
