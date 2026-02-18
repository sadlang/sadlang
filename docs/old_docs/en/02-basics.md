# Variables and Data Types

This chapter covers variables, data types, operators, and basic expressions in Sad language.

## Variables

### Declaration

In Sad, you declare variables by specifying the type followed by the name:

```
رقم age = 25           # Integer
عشري pi = 3.14159      # Float
نص name = "Ahmed"      # String
منطقي active = صحيح   # Boolean (true)
منطقي stopped = خطأ   # Boolean (false)
```

### Variable Names

- Can use Arabic letters, English letters, and numbers
- Cannot start with a number
- Cannot use reserved keywords
- Arabic names are preferred for readability

**Valid names:**
```
رقم العمر = 25
رقم counter = 0
نص اسم_المستخدم = "أحمد"
```

**Invalid names:**
```
رقم 123name = 5        # Starts with number
رقم إذا = 10           # Reserved keyword
```

### Constants

Use the `ثابت` keyword for constants that cannot be changed:

```
ثابت رقم MAX_VALUE = 100
ثابت نص APP_NAME = "My App"
```

## Data Types

### Integer (رقم)

Whole numbers without decimal points:

```
رقم العدد = 42
رقم سالب = -17
رقم كبير = 1000000
```

### Float (عشري)

Numbers with decimal points:

```
عشري السعر = 99.99
عشري درجة_الحرارة = -3.5
عشري الناتج = 2.5
```

### String (نص)

Text enclosed in double quotes:

```
نص الرسالة = "مرحباً بالعالم!"
نص فارغ = ""
نص مع_أرقام = "الرقم 42"
```

#### String Operations

```
نص أ = "مرحباً "
نص ب = "بالعالم"
نص ج = أ + ب              # "مرحباً بالعالم"

رقم الطول = طول(ج)        # 14
نص حرف = ج[0]            # "م"
نص جزء = قطع(ج، 0، 6)    # "مرحباً"
```

### Boolean (منطقي)

True or false values:

```
منطقي ناجح = صحيح       # true
منطقي فشل = خطأ        # false

منطقي نتيجة = 5 > 3      # صحيح
منطقي مقارنة = "أ" == "ب" # خطأ
```

### Array (مصفوفة)

Ordered collection of values:

```
مصفوفة أرقام = [1، 2، 3، 4، 5]
مصفوفة أسماء = ["أحمد"، "سارة"، "محمد"]
مصفوفة خليط = [1، "نص"، صحيح]  # Mixed types
```

#### Array Operations

```
رقم الأول = أرقام[0]         # 1
أرقام[0] = 10               # Change first element
أرقام.أضف(6)                # Add element
رقم الحجم = طول(أرقام)      # 6
```

### Dictionary (قاموس)

Key-value pairs:

```
قاموس شخص = {
    "الاسم": "أحمد"،
    "العمر": 25،
    "نشط": صحيح
}

نص الاسم = شخص["الاسم"]      # "أحمد"
شخص["المدينة"] = "الرياض"   # Add new key
```

## Operators

### Arithmetic Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `+` | Addition | `5 + 3` → `8` |
| `-` | Subtraction | `5 - 3` → `2` |
| `*` | Multiplication | `5 * 3` → `15` |
| `/` | Division | `6 / 2` → `3` |
| `%` | Modulo | `7 % 3` → `1` |

```
رقم أ = 10
رقم ب = 3

رقم مجموع = أ + ب      # 13
رقم فرق = أ - ب        # 7
رقم حاصل = أ * ب       # 30
رقم قسمة = أ / ب       # 3
رقم باقي = أ % ب       # 1
```

### Comparison Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `==` | Equal | `5 == 5` → `صحيح` |
| `!=` | Not equal | `5 != 3` → `صحيح` |
| `>` | Greater than | `5 > 3` → `صحيح` |
| `<` | Less than | `3 < 5` → `صحيح` |
| `>=` | Greater or equal | `5 >= 5` → `صحيح` |
| `<=` | Less or equal | `3 <= 5` → `صحيح` |

```
منطقي متساوي = (10 == 10)    # صحيح
منطقي مختلف = (5 != 3)       # صحيح
منطقي أكبر = (10 > 5)        # صحيح
منطقي أصغر = (3 < 7)         # صحيح
```

### Logical Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `&&` | AND | `صحيح && خطأ` → `خطأ` |
| `||` | OR | `صحيح || خطأ` → `صحيح` |
| `!` | NOT | `!صحيح` → `خطأ` |

```
منطقي أ = صحيح
منطقي ب = خطأ

منطقي و = أ && ب      # خطأ
منطقي أو = أ || ب     # صحيح
منطقي نفي = !أ        # خطأ
```

### Assignment Operators

| Operator | Description | Equivalent |
|----------|-------------|------------|
| `=` | Assign | `x = 5` |
| `+=` | Add and assign | `x = x + 5` |
| `-=` | Subtract and assign | `x = x - 5` |
| `*=` | Multiply and assign | `x = x * 5` |
| `/=` | Divide and assign | `x = x / 5` |

```
رقم x = 10
x += 5        # x = 15
x -= 3        # x = 12
x *= 2        # x = 24
x /= 4        # x = 6
```

## Type Conversion

### Explicit Conversion

```
نص رقم_نصي = "42"
رقم العدد = لرقم(رقم_نصي)        # 42

رقم العدد = 100
نص نصي = لنص(العدد)             # "100"

عشري عشرية = 3.7
رقم صحيح = لرقم(عشرية)          # 3 (truncated)
```

### Type Checking

```
متغير قيمة = 42
نص النوع = نوع(قيمة)    # "رقم"

إذا (نوع(قيمة) == "رقم")
    اطبع("هذا رقم")
نهاية
```

## Comments

### Single Line Comments

```
# هذا تعليق
رقم x = 5  # تعليق في نهاية السطر
```

### Multi-line Comments

```
/*
هذا تعليق
متعدد الأسطر
*/

رقم y = 10
```

## Practical Examples

### Example 1: Temperature Converter

```
دالة رئيسية()
    # Celsius to Fahrenheit
    عشري celsius = 25.0
    عشري fahrenheit = (celsius * 9 / 5) + 32
    
    اطبع("الحرارة بالمئوية: ")
    اطبع(celsius)
    اطبع("°C\n")
    
    اطبع("الحرارة بالفهرنهايت: ")
    اطبع(fahrenheit)
    اطبع("°F\n")
    
    ارجع 0
نهاية
```

### Example 2: BMI Calculator

```
دالة احسب_كتلة_الجسم(عشري الوزن، عشري الطول) -> عشري
    ارجع الوزن / (الطول * الطول)
نهاية

دالة رئيسية()
    عشري الوزن = 70.5    # kg
    عشري الطول = 1.75    # meters
    
    عشري مؤشر = احسب_كتلة_الجسم(الوزن، الطول)
    
    اطبع("مؤشر كتلة الجسم: ")
    اطبع(مؤشر)
    اطبع("\n")
    
    إذا (مؤشر < 18.5)
        اطبع("النتيجة: نقص في الوزن")
    وإلا_إذا (مؤشر < 25)
        اطبع("النتيجة: وزن طبيعي")
    وإلا_إذا (مؤشر < 30)
        اطبع("النتيجة: زيادة في الوزن")
    وإلا
        اطبع("النتيجة: سمنة")
    نهاية
    
    ارجع 0
نهاية
```

### Example 3: Shopping Cart

```
دالة رئيسية()
    مصفوفة المنتجات = ["قهوة"، "خبز"، "حليب"]
    مصفوفة الأسعار = [25.50، 5.00، 8.75]
    
    عشري المجموع = 0
    
    لكل ي في مدى(0، طول(المنتجات))
        اطبع(المنتجات[ي])
        اطبع(": ")
        اطبع(الأسعار[ي])
        اطبع(" ريال\n")
        المجموع = المجموع + الأسعار[ي]
    نهاية
    
    اطبع("─────────────\n")
    اطبع("المجموع: ")
    اطبع(المجموع)
    اطبع(" ريال\n")
    
    عشري الضريبة = المجموع * 0.15
    اطبع("الضريبة (15%): ")
    اطبع(الضريبة)
    اطبع(" ريال\n")
    
    اطبع("الإجمالي: ")
    اطبع(المجموع + الضريبة)
    اطبع(" ريال\n")
    
    ارجع 0
نهاية
```

## Exercises

1. **Temperature Converter**: Write a program that converts Fahrenheit to Celsius
2. **Interest Calculator**: Calculate simple and compound interest
3. **Student Grades**: Store and calculate average grades
4. **Shopping List**: Create a list with items and prices, calculate total

---

**Next Chapter**: [Control Flow](03-control-flow.md) - Learn conditions and loops
