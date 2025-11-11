# 📊 أنواع البيانات في لغة "ص" / Data Types in Sad Language

**تاريخ الإنشاء:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مسودة عمل

---

## 🎯 نظرة عامة / Overview

### (AR) نظرة عامة
نظام الأنواع في لغة "ص" يجمع بين المرونة الديناميكية لـ Python والأمان النوعي لـ C++. يدعم النظام:
- أنواع ديناميكية مع اختيارية للأنواع الثابتة
- تحويلات نوعية ضمنية وصريحة آمنة
- Generic types للبرمجة العامة
- Type inference ذكي

### (EN) Overview
Sad's type system combines Python's dynamic flexibility with C++'s type safety, supporting dynamic types with optional static typing, safe implicit/explicit conversions, generics, and smart type inference.

---

## 📋 جدول المحتويات / Table of Contents

1. [الأنواع الأساسية](#الأنواع-الأساسية--primitive-types)
2. [الأنواع المركبة](#الأنواع-المركبة--compound-types)
3. [الأنواع المرجعية](#الأنواع-المرجعية--reference-types)
4. [التحويلات النوعية](#التحويلات-النوعية--type-conversions)
5. [قواعد BNF](#قواعد-bnf--bnf-grammar)
6. [تمثيل AST](#تمثيل-ast--ast-representation)
7. [مهام التنفيذ](#مهام-التنفيذ--implementation-tasks)

---

## 1️⃣ الأنواع الأساسية / Primitive Types

### 1.1 رقم صحيح / Integer (`رقم`)

#### (AR) الوصف
نوع للأعداد الصحيحة (موجبة وسالبة وصفر).

#### (EN) Description
Type for whole numbers (positive, negative, and zero).

#### الخصائص / Properties
- **الاسم العربي:** `رقم`
- **الاسم الإنجليزي:** `int`, `integer`
- **النطاق:** -2³¹ إلى 2³¹-1 (32-bit) أو -2⁶³ إلى 2⁶³-1 (64-bit)
- **الحجم:** 4 أو 8 bytes
- **القيمة الافتراضية:** 0

#### الصيغة النحوية / Syntax
```s
# تعريف متغير رقم صحيح
رقم عمر = 25
رقم عدد_سالب = -100
رقم صفر = 0

# أرقام بقواعد مختلفة
رقم ثنائي = 0b1010      # 10 في النظام العشري
رقم ثماني = 0o17         # 15 في النظام العشري
رقم ست_عشري = 0xFF       # 255 في النظام العشري
```

#### العمليات المدعومة / Supported Operations
```s
رقم أ = 10
رقم ب = 3

# عمليات حسابية / Arithmetic
رقم مجموع = أ + ب        # 13
رقم فرق = أ - ب          # 7
رقم ضرب = أ * ب          # 30
رقم قسمة = أ / ب         # 3 (قسمة صحيحة)
رقم باقي = أ % ب         # 1
رقم أس = أ ** 2          # 100

# عمليات مقارنة / Comparison
منطقي متساوي = أ == ب    # خطأ
منطقي أكبر = أ > ب       # صحيح
منطقي أصغر = أ < ب       # خطأ

# عمليات منطقية / Bitwise
رقم و = أ & ب            # AND
رقم أو = أ | ب           # OR
رقم نفي = ~أ            # NOT
رقم إزاحة_يسار = أ << 2  # Left shift
رقم إزاحة_يمين = أ >> 1  # Right shift
```

#### التمثيل في AST / AST Representation
```cpp
class IntegerLiteralExpr : public Expression {
public:
    int64_t value;
    IntegerLiteralExpr(int64_t val) : value(val) {}
    NodeType getType() const override { return NodeType::INTEGER_LITERAL; }
};
```

#### الاختبارات / Tests
```s
# tests/spec_tests/01_integers_test.s

# اختبار 1: الأرقام الصحيحة الأساسية
رقم س = 42
إذا س != 42
    ارمِ "فشل الاختبار: التعريف الأساسي"
نهاية

# اختبار 2: العمليات الحسابية
رقم مجموع = 5 + 3
إذا مجموع != 8
    ارمِ "فشل الاختبار: الجمع"
نهاية

# اختبار 3: الأرقام السالبة
رقم سالب = -50
إذا سالب >= 0
    ارمِ "فشل الاختبار: الأرقام السالبة"
نهاية

اطبع("✅ جميع اختبارات Integer نجحت")
```

---

### 1.2 عشري / Float (`عشري`)

#### (AR) الوصف
نوع للأعداد ذات الفاصلة العشرية.

#### (EN) Description
Type for floating-point numbers.

#### الخصائص / Properties
- **الاسم العربي:** `عشري`
- **الاسم الإنجليزي:** `float`, `double`
- **الدقة:** 64-bit (IEEE 754)
- **النطاق:** ±1.7 × 10³⁰⁸
- **القيمة الافتراضية:** 0.0

#### الصيغة النحوية / Syntax
```s
# تعريف أعداد عشرية
عشري درجة_الحرارة = 36.5
عشري باي = 3.14159
عشري صغير_جداً = 1.5e-10
عشري كبير_جداً = 2.5e308

# كتابة علمية
عشري سرعة_الضوء = 3.0e8  # 300,000,000
```

#### العمليات المدعومة / Supported Operations
```s
عشري أ = 10.5
عشري ب = 3.2

# عمليات حسابية
عشري مجموع = أ + ب        # 13.7
عشري فرق = أ - ب          # 7.3
عشري ضرب = أ * ب          # 33.6
عشري قسمة = أ / ب         # 3.28125
عشري أس = أ ** 2          # 110.25

# دوال رياضية (من المكتبة القياسية)
عشري جذر = جذر_تربيعي(أ)
عشري مطلق = قيمة_مطلقة(-5.5)
عشري مقرب = تقريب(3.7)    # 4.0
```

#### التحذيرات / Warnings
```s
# ⚠️ دقة الفاصلة العائمة
عشري س = 0.1 + 0.2
# س قد يساوي 0.30000000000000004 وليس 0.3 بالضبط!

# ✅ الحل: استخدم تقريب أو مقارنة مع هامش خطأ
منطقي متساوي = قيمة_مطلقة(س - 0.3) < 0.0001
```

---

### 1.3 منطقي / Boolean (`منطقي`)

#### (AR) الوصف
نوع للقيم المنطقية (صحيح أو خطأ).

#### (EN) Description
Type for logical values (true or false).

#### الخصائص / Properties
- **الاسم العربي:** `منطقي`
- **الاسم الإنجليزي:** `bool`, `boolean`
- **القيم الممكنة:** `صحيح` (true), `خطأ` (false)
- **الحجم:** 1 byte
- **القيمة الافتراضية:** `خطأ`

#### الصيغة النحوية / Syntax
```s
# تعريف قيم منطقية
منطقي موجود = صحيح
منطقي غير_موجود = خطأ

# من مقارنة
منطقي أكبر = 10 > 5      # صحيح
منطقي يساوي = 3 == 3     # صحيح

# من دالة
منطقي فارغ = قائمة.فارغة()
```

#### العمليات المدعومة / Supported Operations
```s
منطقي أ = صحيح
منطقي ب = خطأ

# عمليات منطقية / Logical operations
منطقي و = أ && ب          # خطأ (AND)
منطقي أو = أ || ب         # صحيح (OR)
منطقي نفي = !أ           # خطأ (NOT)

# عمليات مقارنة
منطقي متساوي = أ == ب    # خطأ
منطقي مختلف = أ != ب     # صحيح
```

#### التحويل من أنواع أخرى / Conversion from Other Types
```s
# الأرقام → منطقي
منطقي من_رقم = منطقي(0)      # خطأ
منطقي من_رقم2 = منطقي(42)    # صحيح

# النصوص → منطقي
منطقي من_نص = منطقي("")      # خطأ (نص فارغ)
منطقي من_نص2 = منطقي("نص")   # صحيح (نص غير فارغ)

# القوائم → منطقي
منطقي من_قائمة = منطقي([])   # خطأ (قائمة فارغة)
```

---

### 1.4 نص / String (`نص`)

#### (AR) الوصف
نوع لتمثيل النصوص والسلاسل الحرفية. يدعم UTF-8 بالكامل للحروف العربية.

#### (EN) Description
Type for text and character strings. Full UTF-8 support for Arabic characters.

#### الخصائص / Properties
- **الاسم العربي:** `نص`
- **الاسم الإنجليزي:** `str`, `string`
- **الترميز:** UTF-8
- **قابل للتغيير:** لا (immutable)
- **القيمة الافتراضية:** `""`

#### الصيغة النحوية / Syntax
```s
# نصوص بسيطة
نص اسم = "أحمد محمد"
نص تحية = 'مرحباً بك'

# نصوص متعددة الأسطر
نص قصيدة = """
البيت الأول
البيت الثاني
البيت الثالث
"""

# حروف خاصة
نص مع_حروف_خاصة = "سطر1\nسطر2\tتاب"
نص علامات = "قال: \"مرحباً\""

# نصوص خام (بدون معالجة حروف خاصة)
نص خام = ر"C:\مجلد\ملف.txt"
```

#### العمليات المدعومة / Supported Operations
```s
نص أ = "مرحباً"
نص ب = "بك"

# دمج النصوص / Concatenation
نص كامل = أ + " " + ب      # "مرحباً بك"

# تكرار / Repetition
نص مكرر = أ * 3            # "مرحباًمرحباًمرحباً"

# الطول / Length
رقم طول = طول(أ)           # 6 (أحرف)

# الوصول للأحرف / Indexing
نص حرف = أ[0]              # "م"
نص قطعة = أ[1:4]           # "رحب"

# البحث / Search
منطقي يحتوي = "رحب" في أ   # صحيح
رقم موقع = أ.ابحث("حب")   # 2

# التحويل / Case conversion
نص كبير = أ.إلى_كبير()     # "مرحباً" (لا تغيير للعربية)
نص صغير = "HELLO".إلى_صغير() # "hello"

# التقسيم / Split
مصفوفة<نص> كلمات = "أحمد محمد علي".قسم(" ")
# ["أحمد", "محمد", "علي"]

# الربط / Join
نص مدموج = "-".ربط(["أ", "ب", "ج"])  # "أ-ب-ج"
```

#### دوال إضافية / Additional Methods
```s
نص نص_مثال = "  مرحباً بك  "

# إزالة المسافات
نص منظف = نص_مثال.تنظيف()    # "مرحباً بك"

# استبدال
نص جديد = نص_مثال.استبدل("مرحباً", "أهلاً")

# التحقق
منطقي يبدأ = نص_مثال.يبدأ_بـ("مرحباً")
منطقي ينتهي = نص_مثال.ينتهي_بـ("بك")

# تنسيق (f-strings)
نص اسم = "أحمد"
رقم عمر = 25
نص رسالة = ن"الاسم: {اسم}, العمر: {عمر}"
# "الاسم: أحمد, العمر: 25"
```

#### التمثيل في AST / AST Representation
```cpp
class StringLiteralExpr : public Expression {
public:
    std::string value;  // UTF-8 encoded
    StringLiteralExpr(const std::string& val) : value(val) {}
    NodeType getType() const override { return NodeType::STRING_LITERAL; }
};
```

---

### 1.5 لاشيء / None (`لاشيء`)

#### (AR) الوصف
نوع خاص يمثل غياب القيمة أو القيمة الفارغة.

#### (EN) Description
Special type representing absence of value or null value.

#### الخصائص / Properties
- **الاسم العربي:** `لاشيء`
- **الاسم الإنجليزي:** `none`, `null`, `nil`
- **القيمة الوحيدة:** `لاشيء`
- **الحجم:** pointer size

#### الصيغة النحوية / Syntax
```s
# تعريف قيمة فارغة
نص اسم = لاشيء

# دالة بدون قيمة إرجاع
دالة طباعة_رسالة(نص رسالة)
    اطبع(رسالة)
    # إرجاع ضمني: لاشيء
نهاية

# التحقق من لاشيء
إذا اسم == لاشيء
    اطبع("الاسم غير محدد")
نهاية

# استخدام آمن (safe navigation)
نص طول = اسم?.طول() ?? 0  # إذا كان لاشيء، أرجع 0
```

---

## 2️⃣ الأنواع المركبة / Compound Types

### 2.1 مصفوفة / Array/List (`مصفوفة`)

#### (AR) الوصف
بنية بيانات ديناميكية تحتوي على عناصر مرتبة من نفس النوع أو أنواع مختلفة.

#### (EN) Description
Dynamic data structure containing ordered elements of same or different types.

#### الصيغة النحوية / Syntax
```s
# مصفوفة فارغة
مصفوفة<رقم> أرقام = []

# مصفوفة بقيم ابتدائية
مصفوفة<نص> أسماء = ["أحمد", "فاطمة", "خالد"]

# مصفوفة متنوعة الأنواع
مصفوفة مختلطة = [1, "نص", صحيح, 3.14]

# مصفوفة متعددة الأبعاد
مصفوفة<مصفوفة<رقم>> مصفوفة_2د = [
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9]
]
```

#### العمليات / Operations
```s
مصفوفة<رقم> قائمة = [1, 2, 3]

# إضافة عناصر
قائمة.أضف(4)                # [1, 2, 3, 4]
قائمة.أدرج(0, 0)            # [0, 1, 2, 3, 4]

# حذف عناصر
قائمة.احذف(0)               # [1, 2, 3, 4]
قائمة.أزل_القيمة(3)         # [1, 2, 4]

# الوصول
رقم عنصر = قائمة[0]         # 1
رقم آخر = قائمة[-1]         # 4

# التقطيع / Slicing
مصفوفة<رقم> جزء = قائمة[1:3]  # [2, 4]

# الطول
رقم حجم = قائمة.طول()        # 3

# التحقق
منطقي يحتوي = 2 في قائمة    # صحيح

# الفرز
قائمة.رتب()                 # ترتيب تصاعدي
قائمة.رتب(عكسي: صحيح)       # ترتيب تنازلي

# التكرار
لكل رقم في قائمة
    اطبع(رقم)
نهاية
```

#### List Comprehension
```s
# إنشاء قائمة بواسطة comprehension
مصفوفة<رقم> مربعات = [س * س لكل س في نطاق(1, 10)]
# [1, 4, 9, 16, 25, 36, 49, 64, 81]

# مع شرط
مصفوفة<رقم> زوجية = [س لكل س في نطاق(1, 20) إذا س % 2 == 0]
# [2, 4, 6, 8, 10, 12, 14, 16, 18]
```

---

### 2.2 خريطة / Map/Dictionary (`خريطة`)

#### (AR) الوصف
بنية بيانات تربط مفاتيح بقيم (key-value pairs).

#### (EN) Description
Data structure mapping keys to values.

#### الصيغة النحوية / Syntax
```s
# خريطة فارغة
خريطة<نص, رقم> أعمار = {}

# خريطة بقيم ابتدائية
خريطة<نص, نص> هواتف = {
    "أحمد": "0501234567",
    "فاطمة": "0559876543",
    "خالد": "0551112233"
}

# أنواع مختلفة
خريطة بيانات = {
    "اسم": "أحمد",
    "عمر": 25,
    "نشط": صحيح
}
```

#### العمليات / Operations
```s
خريطة<نص, رقم> نقاط = {"أحمد": 95, "فاطمة": 88}

# إضافة/تحديث
نقاط["خالد"] = 92
نقاط["أحمد"] = 97          # تحديث

# الحذف
نقاط.احذف("خالد")

# الوصول
رقم نقاط_أحمد = نقاط["أحمد"]

# الوصول الآمن
رقم نقاط_علي = نقاط.احصل("علي", 0)  # 0 إذا لم يوجد

# التحقق
منطقي موجود = "أحمد" في نقاط

# المفاتيح والقيم
مصفوفة<نص> مفاتيح = نقاط.مفاتيح()
مصفوفة<رقم> قيم = نقاط.قيم()
مصفوفة<(نص, رقم)> عناصر = نقاط.عناصر()

# التكرار
لكل (اسم, نقطة) في نقاط.عناصر()
    اطبع(ن"{اسم}: {نقطة}")
نهاية
```

---

### 2.3 صف / Tuple (`صف`)

#### (AR) الوصف
مجموعة مرتبة وغير قابلة للتغيير من العناصر.

#### (EN) Description
Ordered, immutable collection of elements.

#### الصيغة النحوية / Syntax
```s
# صف بسيط
صف نقطة = (10, 20)

# صف متنوع الأنواع
صف بيانات = ("أحمد", 25, صحيح)

# تفكيك الصف / Unpacking
(س, ص) = نقطة
(اسم, عمر, نشط) = بيانات

# صف أحادي العنصر
صف واحد = (42,)  # الفاصلة ضرورية!
```

#### الاستخدام / Usage
```s
# إرجاع قيم متعددة من دالة
دالة احصل_على_الاحداثيات() = صف<رقم, رقم>
    إرجاع (100, 200)
نهاية

(س, ص) = احصل_على_الاحداثيات()

# كمفاتيح في خريطة (immutable)
خريطة<صف<رقم, رقم>, نص> مواقع = {
    (0, 0): "المركز",
    (10, 20): "نقطة أ"
}
```

---

## 3️⃣ قواعد BNF / BNF Grammar

```bnf
<type> ::= <primitive_type> 
         | <compound_type>
         | <reference_type>

<primitive_type> ::= "رقم" | "int" | "integer"
                   | "عشري" | "float" | "double"
                   | "منطقي" | "bool" | "boolean"
                   | "نص" | "str" | "string"
                   | "لاشيء" | "none" | "null"

<compound_type> ::= "مصفوفة" "<" <type> ">"
                  | "خريطة" "<" <type> "," <type> ">"
                  | "صف" "<" <type_list> ">"

<type_list> ::= <type> ("," <type>)*

<literal> ::= <integer_literal>
            | <float_literal>
            | <boolean_literal>
            | <string_literal>
            | <none_literal>
            | <array_literal>
            | <map_literal>
            | <tuple_literal>

<integer_literal> ::= ["-"] <digit>+
                    | "0b" <binary_digit>+
                    | "0o" <octal_digit>+
                    | "0x" <hex_digit>+

<float_literal> ::= ["-"] <digit>+ "." <digit>+ [<exponent>]

<boolean_literal> ::= "صحيح" | "true" | "خطأ" | "false"

<string_literal> ::= '"' <char>* '"' 
                   | "'" <char>* "'"
                   | '"""' <char>* '"""'

<array_literal> ::= "[" [<expression_list>] "]"

<map_literal> ::= "{" [<key_value_list>] "}"

<key_value_list> ::= <expression> ":" <expression> 
                     ("," <expression> ":" <expression>)*
```

---

## 4️⃣ تمثيل AST / AST Representation

### ملف الهيدر المقترح / Suggested Header File
```cpp
// include/data/types/type_system.h

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

namespace Sad {
namespace Data {

/**
 * @brief (AR) أنواع البيانات الأساسية
 * @brief (EN) Primitive data types
 */
enum class PrimitiveType {
    INTEGER,    // رقم
    FLOAT,      // عشري
    BOOLEAN,    // منطقي
    STRING,     // نص
    NONE        // لاشيء
};

/**
 * @brief (AR) فئة النوع الموحد
 * @brief (EN) Unified type class
 */
class Type {
public:
    enum class Kind {
        PRIMITIVE,
        ARRAY,
        MAP,
        TUPLE,
        FUNCTION,
        CLASS
    };
    
    virtual Kind getKind() const = 0;
    virtual std::string toString() const = 0;
    virtual ~Type() = default;
};

/**
 * @brief (AR) نوع أساسي
 * @brief (EN) Primitive type
 */
class PrimitiveTypeNode : public Type {
public:
    PrimitiveType primitive;
    
    explicit PrimitiveTypeNode(PrimitiveType p) : primitive(p) {}
    
    Kind getKind() const override { return Kind::PRIMITIVE; }
    std::string toString() const override;
};

/**
 * @brief (AR) نوع مصفوفة
 * @brief (EN) Array type
 */
class ArrayType : public Type {
public:
    std::shared_ptr<Type> elementType;
    
    explicit ArrayType(std::shared_ptr<Type> elem) 
        : elementType(elem) {}
    
    Kind getKind() const override { return Kind::ARRAY; }
    std::string toString() const override;
};

} // namespace Data
} // namespace Sad
```

---

## 5️⃣ مهام التنفيذ / Implementation Tasks

### المرحلة 1: الأنواع الأساسية (3 أيام)

#### الملفات المطلوبة / Required Files:
```
include/data/types/
├── type_system.h          # تعريفات الأنواع الأساسية
├── value.h                # فئة Value الموحدة
├── primitive_types.h      # الأنواع الأساسية
└── type_checker.h         # فحص الأنواع

src/data/types/
├── type_system.cpp
├── value.cpp
├── primitive_types.cpp
└── type_checker.cpp
```

#### المهام التفصيلية / Detailed Tasks:
1. **يوم 1:** تنفيذ `Value` class و primitive types
   - الوقت: 6-8 ساعات
   - الاختبارات: integer, float, boolean basics

2. **يوم 2:** تنفيذ String type مع UTF-8
   - الوقت: 6-8 ساعات
   - الاختبارات: string operations, Arabic support

3. **يوم 3:** Type conversions واختبارات
   - الوقت: 4-6 ساعات
   - الاختبارات: implicit/explicit conversions

#### فرع Git / Git Branch:
```powershell
git checkout -b feat/types/primitives
```

#### معايير القبول / Acceptance Criteria:
- [ ] جميع الأنواع الأساسية محددة
- [ ] Value class يدعم كل الأنواع
- [ ] UTF-8 يعمل للعربية
- [ ] التحويلات النوعية تعمل
- [ ] الاختبارات تمر (≥20 test)
- [ ] DEBUG_PRINT في نقاط حرجة
- [ ] توثيق Doxygen كامل

---

### المرحلة 2: الأنواع المركبة (4 أيام)

#### الملفات المطلوبة:
```
include/data/types/
├── array_type.h
├── map_type.h
└── tuple_type.h

src/data/types/
├── array_type.cpp
├── map_type.cpp
└── tuple_type.cpp
```

#### فرع Git:
```powershell
git checkout -b feat/types/compound
```

---

## 6️⃣ أوامر Git / Git Commands

```powershell
# إنشاء الفرع
cd C:\s\s_language
git checkout -b feat/spec/types

# إضافة الملفات
git add docs/language_spec/01_types.md
git add tests/spec_tests/01_integers_test.s
git add tests/spec_tests/02_strings_test.s

# Commit
git commit -m "[spec] add comprehensive type system specification

- Define all primitive types (int, float, bool, string, none)
- Define compound types (array, map, tuple)
- Add BNF grammar
- Add AST representation
- Add implementation tasks
- Add test files"

# Push
git push --set-upstream origin feat/spec/types
```

---

## 📊 ملخص / Summary

| النوع / Type | الاسم العربي | الحجم / Size | مثال / Example |
|-------------|-------------|-------------|---------------|
| Integer | رقم | 4-8 bytes | `42` |
| Float | عشري | 8 bytes | `3.14` |
| Boolean | منطقي | 1 byte | `صحيح` |
| String | نص | variable | `"مرحباً"` |
| None | لاشيء | pointer | `لاشيء` |
| Array | مصفوفة | variable | `[1, 2, 3]` |
| Map | خريطة | variable | `{"مفتاح": قيمة}` |
| Tuple | صف | variable | `(1, 2)` |

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** جاهز للمراجعة والتنفيذ
