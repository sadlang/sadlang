# 📚 Standard Library Reference - Sad Programming Language
# مرجع المكتبة القياسية - لغة ص

**Complete reference for Sad Language built-in functions and utilities 🛠️**

---

## 📑 Table of Contents

1. [Overview](#overview)
2. [Core Functions](#core-functions)
3. [Math Functions](#math-functions)
4. [String Functions](#string-functions)
5. [Array Functions](#array-functions)
6. [Type Functions](#type-functions)
7. [I/O Functions](#io-functions)
8. [Utility Functions](#utility-functions)

---

## 🌟 Overview

The Sad Language Standard Library provides essential built-in functions that are available in all Sad programs. These functions use Arabic names and are designed for Arabic-speaking developers.

### Function Naming Convention
- Arabic verbs for actions: `اطبع` (print), `احسب` (calculate)
- Arabic nouns for properties: `طول` (length), `نوع` (type)
- Descriptive Arabic phrases: `جذر_تربيعي` (square root)

### Usage Pattern
```sad
# All built-in functions are available globally
اطبع("Hello World") ;              # No import needed
رقم الطول = طول([1, 2, 3]) ;       # Direct function call
```

---

## 🔢 Core Functions

### `اطبع` (Print)
**Purpose**: Output values to the console with Arabic formatting

```sad
اطبع(value1, value2, ..., valueN) ;
```

**Parameters**:
- `value1, value2, ..., valueN`: Any values to print

**Returns**: Nothing (void)

**Examples**:
```sad
اطبع("مرحبا") ;                    # مرحبا
اطبع("العمر:", 25) ;              # العمر: 25
اطبع("القيم:", 1, 2, 3) ;         # القيم: 1 2 3

# Print variables
نص الاسم = "أحمد" ;
رقم العمر = 30 ;
اطبع("اسمي", الاسم, "وعمري", العمر, "سنة") ;
# Output: اسمي أحمد وعمري 30 سنة
```

**Notes**:
- Automatically adds spaces between values
- Supports Arabic and English text
- Handles UTF-8 encoding properly

---

### `طول` (Length)
**Purpose**: Get the length of arrays, strings, or other collections

```sad
رقم الطول = طول(collection) ;
```

**Parameters**:
- `collection`: Array, string, or other collection

**Returns**: Integer representing the length

**Examples**:
```sad
# Array length
مصفوفة أرقام = [1, 2, 3, 4, 5] ;
رقم عدد_العناصر = طول(أرقام) ;      # 5

# String length  
نص الرسالة = "مرحبا بك" ;
رقم عدد_الحروف = طول(الرسالة) ;     # 8

# Empty collections
مصفوفة فارغة = [] ;
اطبع(طول(فارغة)) ;                  # 0
```

**Error Cases**:
```sad
# Type error - cannot get length of number
رقم العدد = 123 ;
طول(العدد) ;  # Error: Cannot get length of INTEGER type
```

---

### `نوع` (Type)
**Purpose**: Get the runtime type of a value

```sad
نص نوع_القيمة = نوع(value) ;
```

**Parameters**:
- `value`: Any value

**Returns**: String describing the type in Arabic

**Examples**:
```sad
اطبع(نوع(123)) ;           # "رقم" (Integer)
اطبع(نوع(3.14)) ;          # "عشري" (Float)  
اطبع(نوع("مرحبا")) ;       # "نص" (String)
اطبع(نوع(صحيح)) ;          # "منطق" (Boolean)
اطبع(نوع([1, 2, 3])) ;     # "مصفوفة" (Array)

# Useful for type checking
دالة تحقق_من_النوع( قيمة )
    إذا ( نوع(قيمة) == "رقم" )
        اطبع("هذا رقم صحيح") ;
    وإلا
        اطبع("ليس رقماً صحيحاً") ;
    نهاية
نهاية
```

**Return Values**:
| Type | Arabic Name | English |
|------|-------------|---------|
| Integer | `"رقم"` | `"INTEGER"` |
| Float | `"عشري"` | `"FLOAT"` |
| String | `"نص"` | `"STRING"` |
| Boolean | `"منطق"` | `"BOOLEAN"` |
| Array | `"مصفوفة"` | `"ARRAY"` |
| Function | `"دالة"` | `"FUNCTION"` |
| Object | `"كائن"` | `"OBJECT"` |
| Null | `"فارغ"` | `"NIL"` |

---

## 🧮 Math Functions

### `مطلق` (Absolute Value)
**Purpose**: Calculate the absolute value of a number

```sad
رقم القيمة_المطلقة = مطلق(number) ;
```

**Parameters**:
- `number`: Numeric value (integer or float)

**Returns**: Absolute value of the same type

**Examples**:
```sad
اطبع(مطلق(-5)) ;           # 5
اطبع(مطلق(3.14)) ;         # 3.14
اطبع(مطلق(-2.718)) ;       # 2.718

# Use in calculations
رقم الفرق = مطلق(10 - 15) ; # 5
عشري المسافة = مطلق(-3.5 - 2.1) ; # 5.6
```

---

### `جذر_تربيعي` (Square Root)
**Purpose**: Calculate the square root of a positive number

```sad
عشري الجذر = جذر_تربيعي(number) ;
```

**Parameters**:
- `number`: Non-negative numeric value

**Returns**: Square root as float

**Examples**:
```sad
اطبع(جذر_تربيعي(25)) ;      # 5.0
اطبع(جذر_تربيعي(2)) ;       # 1.41421...
اطبع(جذر_تربيعي(0)) ;       # 0.0

# Pythagorean theorem
دالة حساب_الوتر( رقم أ ، رقم ب )
    عشري مربع_أ = أ * أ ;
    عشري مربع_ب = ب * ب ;
    أرجع جذر_تربيعي(مربع_أ + مربع_ب) ;
نهاية

اطبع(حساب_الوتر(3, 4)) ;    # 5.0
```

**Error Cases**:
```sad
جذر_تربيعي(-1) ;  # Error: Cannot calculate square root of negative number
```

---

### `قوة` (Power)
**Purpose**: Raise a number to a power

```sad
عشري النتيجة = قوة(base, exponent) ;
```

**Parameters**:
- `base`: Base number
- `exponent`: Exponent value

**Returns**: Base raised to exponent as float

**Examples**:
```sad
اطبع(قوة(2, 8)) ;           # 256.0
اطبع(قوة(3, 3)) ;           # 27.0
اطبع(قوة(5, 0)) ;           # 1.0
اطبع(قوة(2, -2)) ;          # 0.25

# Scientific calculations
عشري E_مرفوع = قوة(2.718, 2) ; # e²
عشري مضاعف_القوة = قوة(قوة(2, 3), 2) ; # (2³)² = 64.0
```

---

### `أصغر` (Minimum)
**Purpose**: Find the minimum value among given numbers

```sad
رقم الأصغر = أصغر(value1, value2, ..., valueN) ;
```

**Parameters**:
- `value1, value2, ..., valueN`: Numeric values

**Returns**: Minimum value

**Examples**:
```sad
اطبع(أصغر(5, 2, 8, 1)) ;     # 1
اطبع(أصغر(-3, -1, -5)) ;     # -5
اطبع(أصغر(3.14, 2.718)) ;   # 2.718

# Find minimum in array
مصفوفة درجات = [85, 92, 78, 96, 89] ;
رقم أقل_درجة = أصغر(درجات[0], درجات[1], درجات[2], درجات[3], درجات[4]) ;
```

---

### `أكبر` (Maximum)
**Purpose**: Find the maximum value among given numbers

```sad
رقم الأكبر = أكبر(value1, value2, ..., valueN) ;
```

**Parameters**:
- `value1, value2, ..., valueN`: Numeric values

**Returns**: Maximum value

**Examples**:
```sad
اطبع(أكبر(5, 2, 8, 1)) ;     # 8
اطبع(أكبر(-3, -1, -5)) ;     # -1
اطبع(أكبر(3.14, 2.718)) ;   # 3.14

# Range validation
دالة تحقق_النطاق( رقم القيمة ، رقم الحد_الأدنى ، رقم الحد_الأعلى )
    رقم القيمة_المحدودة = أصغر(أكبر(القيمة, الحد_الأدنى), الحد_الأعلى) ;
    أرجع القيمة_المحدودة ;
نهاية
```

---

### `تدوير` (Round)
**Purpose**: Round a floating-point number to the nearest integer

```sad
رقم المدور = تدوير(number) ;
```

**Parameters**:
- `number`: Floating-point number

**Returns**: Rounded integer

**Examples**:
```sad
اطبع(تدوير(3.7)) ;          # 4
اطبع(تدوير(3.2)) ;          # 3
اطبع(تدوير(3.5)) ;          # 4 (rounds up)
اطبع(تدوير(-2.8)) ;         # -3

# Currency calculation
عشري السعر = 19.99 ;
رقم السعر_المدور = تدوير(السعر) ; # 20
```

---

## 📝 String Functions

### `طول_النص` (String Length)
**Purpose**: Get the character count of a string (UTF-8 aware)

```sad
رقم عدد_الأحرف = طول_النص(text) ;
```

**Parameters**:
- `text`: String value

**Returns**: Number of characters (not bytes)

**Examples**:
```sad
اطبع(طول_النص("مرحبا")) ;      # 5
اطبع(طول_النص("Hello")) ;     # 5
اطبع(طول_النص("")) ;          # 0

# Arabic text handling
نص النص_العربي = "مرحباً بكم في لغة ص" ;
اطبع("عدد الأحرف:", طول_النص(النص_العربي)) ; # 18
```

---

### `جزء_من_النص` (Substring)
**Purpose**: Extract a portion of a string

```sad
نص الجزء = جزء_من_النص(text, start_index, length) ;
```

**Parameters**:
- `text`: Source string
- `start_index`: Starting position (0-based)
- `length`: Number of characters to extract

**Returns**: Substring

**Examples**:
```sad
نص الكلمة = "البرمجة" ;
اطبع(جزء_من_النص(الكلمة, 0, 3)) ; # "البر"
اطبع(جزء_من_النص(الكلمة, 2, 4)) ; # "رمجة"

# Extract file extension
نص اسم_الملف = "document.pdf" ;
نص الامتداد = جزء_من_النص(اسم_الملف, 9, 3) ; # "pdf"
```

**Error Cases**:
```sad
جزء_من_النص("مرحبا", 10, 2) ; # Error: Start index out of bounds
جزء_من_النص("مرحبا", 0, 20) ; # Error: Length extends beyond string
```

---

### `ابحث_في_النص` (Find in Text)
**Purpose**: Find the position of a substring within a string

```sad
رقم الموضع = ابحث_في_النص(text, search_text) ;
```

**Parameters**:
- `text`: String to search in
- `search_text`: Substring to find

**Returns**: Index of first occurrence, or -1 if not found

**Examples**:
```sad
نص الجملة = "مرحباً بك في لغة ص" ;
اطبع(ابحث_في_النص(الجملة, "لغة")) ; # 12
اطبع(ابحث_في_النص(الجملة, "hello")) ; # -1

# Check if email is valid (simple)
دالة تحقق_الايميل( نص الايميل )
    إذا ( ابحث_في_النص(الايميل, "@") == -1 )
        أرجع خطأ ;
    نهاية
    أرجع صحيح ;
نهاية
```

---

### `استبدل_في_النص` (Replace in Text)
**Purpose**: Replace all occurrences of a substring with another string

```sad
نص النص_الجديد = استبدل_في_النص(text, old_text, new_text) ;
```

**Parameters**:
- `text`: Original string
- `old_text`: Text to replace
- `new_text`: Replacement text

**Returns**: New string with replacements made

**Examples**:
```sad
نص الرسالة = "مرحبا أحمد، كيف حالك أحمد؟" ;
نص الرسالة_الجديدة = استبدل_في_النص(الرسالة, "أحمد", "محمد") ;
اطبع(الرسالة_الجديدة) ; # "مرحبا محمد، كيف حالك محمد؟"

# Clean up text
نص النص = "هذا   نص    به   مسافات   كثيرة" ;
نص النص_المنظف = استبدل_في_النص(النص, "   ", " ") ;
```

---

### `أحرف_كبيرة` (Uppercase)
**Purpose**: Convert string to uppercase

```sad
نص النص_الكبير = أحرف_كبيرة(text) ;
```

**Parameters**:
- `text`: String to convert

**Returns**: Uppercase string

**Examples**:
```sad
اطبع(أحرف_كبيرة("hello world")) ; # "HELLO WORLD"
اطبع(أحرف_كبيرة("مرحبا")) ;       # "مرحبا" (Arabic unchanged)

# Case-insensitive comparison
دالة متساوي_بدون_حالة( نص أ ، نص ب )
    أرجع أحرف_كبيرة(أ) == أحرف_كبيرة(ب) ;
نهاية
```

---

### `أحرف_صغيرة` (Lowercase)
**Purpose**: Convert string to lowercase

```sad
نص النص_الصغير = أحرف_صغيرة(text) ;
```

**Parameters**:
- `text`: String to convert

**Returns**: Lowercase string

**Examples**:
```sad
اطبع(أحرف_صغيرة("HELLO WORLD")) ; # "hello world"
اطبع(أحرف_صغيرة("Programming")) ;  # "programming"
```

---

## 🗂️ Array Functions

### `أضف_عنصر` (Add Element)
**Purpose**: Add an element to the end of an array

```sad
مصفوفة النتيجة = أضف_عنصر(array, element) ;
```

**Parameters**:
- `array`: Target array
- `element`: Element to add

**Returns**: New array with element added

**Examples**:
```sad
مصفوفة الأرقام = [1, 2, 3] ;
مصفوفة الأرقام_الجديدة = أضف_عنصر(الأرقام, 4) ;
اطبع(الأرقام_الجديدة) ; # [1, 2, 3, 4]

# Build array dynamically
مصفوفة القائمة = [] ;
القائمة = أضف_عنصر(القائمة, "أحمد") ;
القائمة = أضف_عنصر(القائمة, "فاطمة") ;
القائمة = أضف_عنصر(القائمة, "علي") ;
اطبع(القائمة) ; # ["أحمد", "فاطمة", "علي"]
```

---

### `احذف_عنصر_أخير` (Remove Last Element)
**Purpose**: Remove and return the last element of an array

```sad
قيمة العنصر_المحذوف = احذف_عنصر_أخير(array) ;
```

**Parameters**:
- `array`: Source array (must not be empty)

**Returns**: The removed element

**Examples**:
```sad
مصفوفة الأرقام = [1, 2, 3, 4] ;
رقم العنصر_الأخير = احذف_عنصر_أخير(الأرقام) ;
اطبع(العنصر_الأخير) ; # 4
اطبع(الأرقام) ; # [1, 2, 3]

# Stack implementation
مصفوفة المكدس = [] ;
المكدس = أضف_عنصر(المكدس, "first") ;
المكدس = أضف_عنصر(المكدس, "second") ;
نص آخر_عنصر = احذف_عنصر_أخير(المكدس) ; # "second"
```

**Error Cases**:
```sad
احذف_عنصر_أخير([]) ; # Error: Cannot remove from empty array
```

---

### `اعكس_المصفوفة` (Reverse Array)
**Purpose**: Reverse the order of elements in an array

```sad
مصفوفة المعكوسة = اعكس_المصفوفة(array) ;
```

**Parameters**:
- `array`: Array to reverse

**Returns**: New array with elements in reverse order

**Examples**:
```sad
مصفوفة الأرقام = [1, 2, 3, 4, 5] ;
مصفوفة المعكوسة = اعكس_المصفوفة(الأرقام) ;
اطبع(المعكوسة) ; # [5, 4, 3, 2, 1]

# Palindrome check for arrays
دالة هل_متناظرة( مصفوفة القائمة )
    أرجع القائمة == اعكس_المصفوفة(القائمة) ;
نهاية
```

---

### `رتب_المصفوفة` (Sort Array)
**Purpose**: Sort elements in an array in ascending order

```sad
مصفوفة المرتبة = رتب_المصفوفة(array) ;
```

**Parameters**:
- `array`: Array of comparable elements

**Returns**: New sorted array

**Examples**:
```sad
مصفوفة الدرجات = [85, 92, 78, 96, 89] ;
مصفوفة الدرجات_المرتبة = رتب_المصفوفة(الدرجات) ;
اطبع(الدرجات_المرتبة) ; # [78, 85, 89, 92, 96]

# Sort strings alphabetically  
مصفوفة الأسماء = ["أحمد", "زينب", "محمد", "فاطمة"] ;
مصفوفة الأسماء_المرتبة = رتب_المصفوفة(الأسماء) ;
اطبع(الأسماء_المرتبة) ; # ["أحمد", "زينب", "فاطمة", "محمد"]
```

---

### `ابحث_في_المصفوفة` (Search in Array)
**Purpose**: Find the index of an element in an array

```sad
رقم الفهرس = ابحث_في_المصفوفة(array, element) ;
```

**Parameters**:
- `array`: Array to search in
- `element`: Element to find

**Returns**: Index of element, or -1 if not found

**Examples**:
```sad
مصفوفة الألوان = ["أحمر", "أخضر", "أزرق", "أصفر"] ;
رقم موقع_الأزرق = ابحث_في_المصفوفة(الألوان, "أزرق") ;
اطبع(موقع_الأزرق) ; # 2

# Check if element exists
دالة موجود( مصفوفة القائمة ، العنصر )
    أرجع ابحث_في_المصفوفة(القائمة, العنصر) != -1 ;
نهاية
```

---

### `فلتر_المصفوفة` (Filter Array)
**Purpose**: Create new array with elements that pass a test

```sad
مصفوفة المفلترة = فلتر_المصفوفة(array, test_function) ;
```

**Parameters**:
- `array`: Array to filter
- `test_function`: Function that returns boolean for each element

**Returns**: New array containing only elements that pass test

**Examples**:
```sad
# Filter even numbers
مصفوفة الأرقام = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10] ;

دالة زوجي( رقم العدد )
    أرجع العدد % 2 == 0 ;
نهاية

مصفوفة الأرقام_الزوجية = فلتر_المصفوفة(الأرقام, زوجي) ;
اطبع(الأرقام_الزوجية) ; # [2, 4, 6, 8, 10]

# Filter passing grades
مصفوفة الدرجات = [85, 45, 92, 58, 78, 96] ;

دالة ناجح( رقم الدرجة )
    أرجع الدرجة >= 60 ;
نهاية

مصفوفة الدرجات_الناجحة = فلتر_المصفوفة(الدرجات, ناجح) ;
اطبع(الدرجات_الناجحة) ; # [85, 92, 78, 96]
```

---

## 🔄 Type Functions

### `إلى_رقم` (To Integer)
**Purpose**: Convert a value to integer

```sad
رقم النتيجة = إلى_رقم(value) ;
```

**Parameters**:
- `value`: Value to convert (string, float, or boolean)

**Returns**: Integer representation

**Examples**:
```sad
اطبع(إلى_رقم("123")) ;        # 123
اطبع(إلى_رقم(3.14)) ;         # 3
اطبع(إلى_رقم(3.99)) ;         # 3 (truncates)
اطبع(إلى_رقم(صحيح)) ;         # 1
اطبع(إلى_رقم(خطأ)) ;          # 0

# Parse user input
نص الإدخال = "25" ;
رقم العمر = إلى_رقم(الإدخال) ;
اطبع("العمر هو:", العمر) ;
```

**Error Cases**:
```sad
إلى_رقم("hello") ; # Error: Cannot convert "hello" to integer
إلى_رقم("12.34.56") ; # Error: Invalid number format
```

---

### `إلى_عشري` (To Float)
**Purpose**: Convert a value to floating-point number

```sad
عشري النتيجة = إلى_عشري(value) ;
```

**Parameters**:
- `value`: Value to convert (string, integer, or boolean)

**Returns**: Float representation

**Examples**:
```sad
اطبع(إلى_عشري("3.14")) ;      # 3.14
اطبع(إلى_عشري(42)) ;          # 42.0
اطبع(إلى_عشري(صحيح)) ;        # 1.0

# Financial calculations
نص سعر_النص = "19.99" ;
عشري السعر = إلى_عشري(سعر_النص) ;
عشري الضريبة = السعر * 0.15 ;
```

---

### `إلى_نص` (To String)
**Purpose**: Convert any value to its string representation

```sad
نص النتيجة = إلى_نص(value) ;
```

**Parameters**:
- `value`: Any value

**Returns**: String representation

**Examples**:
```sad
اطبع(إلى_نص(123)) ;          # "123"
اطبع(إلى_نص(3.14)) ;         # "3.14"
اطبع(إلى_نص(صحيح)) ;         # "true"
اطبع(إلى_نص([1, 2, 3])) ;    # "[1, 2, 3]"

# Format output
رقم النقاط = 1500 ;
نص رسالة = "لديك " + إلى_نص(النقاط) + " نقطة" ;
اطبع(رسالة) ; # "لديك 1500 نقطة"
```

---

### `إلى_منطق` (To Boolean)
**Purpose**: Convert a value to boolean

```sad
منطق النتيجة = إلى_منطق(value) ;
```

**Parameters**:
- `value`: Any value

**Returns**: Boolean representation

**Examples**:
```sad
اطبع(إلى_منطق(1)) ;          # true
اطبع(إلى_منطق(0)) ;          # false
اطبع(إلى_منطق("")) ;         # false (empty string)
اطبع(إلى_منطق("مرحبا")) ;    # true (non-empty string)
اطبع(إلى_منطق([])) ;         # false (empty array)

# Truthiness rules:
# - 0, 0.0, "", [], false → false
# - Everything else → true
```

---

## 📤 I/O Functions

### `اقرأ_سطر` (Read Line)
**Purpose**: Read a line of text from standard input

```sad
نص الإدخال = اقرأ_سطر(prompt) ;
```

**Parameters**:
- `prompt` (optional): Text to display as prompt

**Returns**: String entered by user

**Examples**:
```sad
نص الاسم = اقرأ_سطر("ما اسمك؟ ") ;
اطبع("مرحباً", الاسم) ;

# Get number input
نص عمر_نص = اقرأ_سطر("كم عمرك؟ ") ;
رقم العمر = إلى_رقم(عمر_نص) ;
إذا ( العمر >= 18 )
    اطبع("أنت بالغ") ;
وإلا
    اطبع("أنت قاصر") ;
نهاية
```

---

### `اقرأ_رقم` (Read Number)
**Purpose**: Read a numeric value from input with validation

```sad
رقم الرقم = اقرأ_رقم(prompt) ;
```

**Parameters**:
- `prompt` (optional): Text to display as prompt

**Returns**: Integer value

**Examples**:
```sad
رقم العدد1 = اقرأ_رقم("أدخل العدد الأول: ") ;
رقم العدد2 = اقرأ_رقم("أدخل العدد الثاني: ") ;
رقم المجموع = العدد1 + العدد2 ;
اطبع("المجموع:", المجموع) ;
```

**Error Handling**:
- Automatically re-prompts for invalid input
- Displays error message in Arabic

---

## 🛠️ Utility Functions

### `عشوائي` (Random)
**Purpose**: Generate random numbers

```sad
عشري العدد_العشوائي = عشوائي() ;           # Random float 0.0-1.0
رقم العدد_العشوائي = عشوائي(max) ;        # Random int 0 to max-1
رقم العدد_العشوائي = عشوائي(min, max) ;   # Random int min to max-1
```

**Parameters**:
- No parameters: Returns float between 0.0 and 1.0
- `max`: Returns integer between 0 and max-1
- `min, max`: Returns integer between min and max-1

**Returns**: Random number

**Examples**:
```sad
# Random probability
عشري الاحتمال = عشوائي() ;
إذا ( الاحتمال > 0.5 )
    اطبع("رقم!") ;
وإلا
    اطبع("كتابة!") ;
نهاية

# Random dice roll
رقم النرد = عشوائي(1, 7) ;    # 1-6
اطبع("النتيجة:", النرد) ;

# Random array element
مصفوفة الألوان = ["أحمر", "أخضر", "أزرق", "أصفر"] ;
رقم الفهرس_العشوائي = عشوائي(طول(الألوان)) ;
اطبع("اللون المختار:", الألوان[الفهرس_العشوائي]) ;
```

---

### `وقت_الآن` (Current Time)
**Purpose**: Get current system time

```sad
رقم الوقت = وقت_الآن() ;        # Unix timestamp
نص التاريخ = وقت_نص() ;         # Formatted date string
```

**Returns**: 
- `وقت_الآن()`: Integer timestamp (seconds since 1970)
- `وقت_نص()`: Formatted date string in Arabic

**Examples**:
```sad
رقم البداية = وقت_الآن() ;
# ... some processing ...
رقم النهاية = وقت_الآن() ;
رقم المدة = النهاية - البداية ;
اطبع("استغرق", المدة, "ثانية") ;

# Display current date
اطبع("التاريخ الحالي:", وقت_نص()) ;
```

---

### `انتظر` (Wait/Sleep)
**Purpose**: Pause execution for specified time

```sad
انتظر(seconds) ;
```

**Parameters**:
- `seconds`: Number of seconds to wait (can be float)

**Returns**: Nothing

**Examples**:
```sad
اطبع("بداية العد التنازلي") ;
انتظر(1) ;
اطبع("3") ;
انتظر(1) ;
اطبع("2") ;
انتظر(1) ;
اطبع("1") ;
انتظر(1) ;
اطبع("انطلق!") ;

# Animation effect
رقم عداد = 0 ;
بينما ( عداد < 10 )
    اطبع(".", ) ;    # Print dot without newline
    انتظر(0.5) ;     # Wait half second
    عداد = عداد + 1 ;
نهاية
اطبع("مكتمل!") ;
```

---

### `أنهي_البرنامج` (Exit Program)
**Purpose**: Terminate program execution

```sad
أنهي_البرنامج(exit_code) ;
```

**Parameters**:
- `exit_code` (optional): Exit status code (default 0)

**Returns**: Does not return (terminates program)

**Examples**:
```sad
إذا ( خطأ_حدث )
    اطبع("خطأ فادح! إنهاء البرنامج") ;
    أنهي_البرنامج(1) ;    # Exit with error code 1
نهاية

# Successful completion
اطبع("البرنامج اكتمل بنجاح") ;
أنهي_البرنامج() ;          # Exit with code 0 (success)
```

---

## 📋 Function Summary

### Core Functions
| Function | Arabic | Purpose | Returns |
|----------|--------|---------|---------|
| `print` | `اطبع` | Output values | void |
| `length` | `طول` | Get collection size | int |
| `type` | `نوع` | Get value type | string |

### Math Functions
| Function | Arabic | Purpose | Returns |
|----------|--------|---------|---------|
| `abs` | `مطلق` | Absolute value | number |
| `sqrt` | `جذر_تربيعي` | Square root | float |
| `pow` | `قوة` | Power operation | float |
| `min` | `أصغر` | Minimum value | number |
| `max` | `أكبر` | Maximum value | number |
| `round` | `تدوير` | Round to integer | int |

### String Functions
| Function | Arabic | Purpose | Returns |
|----------|--------|---------|---------|
| `strlen` | `طول_النص` | String length | int |
| `substr` | `جزء_من_النص` | Extract substring | string |
| `find` | `ابحث_في_النص` | Find substring | int |
| `replace` | `استبدل_في_النص` | Replace text | string |
| `upper` | `أحرف_كبيرة` | Uppercase | string |
| `lower` | `أحرف_صغيرة` | Lowercase | string |

### Array Functions
| Function | Arabic | Purpose | Returns |
|----------|--------|---------|---------|
| `push` | `أضف_عنصر` | Add element | array |
| `pop` | `احذف_عنصر_أخير` | Remove last | element |
| `reverse` | `اعكس_المصفوفة` | Reverse order | array |
| `sort` | `رتب_المصفوفة` | Sort elements | array |
| `search` | `ابحث_في_المصفوفة` | Find element | int |
| `filter` | `فلتر_المصفوفة` | Filter elements | array |

### Type Conversion
| Function | Arabic | Purpose | Returns |
|----------|--------|---------|---------|
| `to_int` | `إلى_رقم` | Convert to integer | int |
| `to_float` | `إلى_عشري` | Convert to float | float |
| `to_string` | `إلى_نص` | Convert to string | string |
| `to_bool` | `إلى_منطق` | Convert to boolean | bool |

### I/O Functions
| Function | Arabic | Purpose | Returns |
|----------|--------|---------|---------|
| `read_line` | `اقرأ_سطر` | Read text input | string |
| `read_number` | `اقرأ_رقم` | Read numeric input | int |

### Utility Functions
| Function | Arabic | Purpose | Returns |
|----------|--------|---------|---------|
| `random` | `عشوائي` | Generate random | number |
| `time_now` | `وقت_الآن` | Current timestamp | int |
| `time_string` | `وقت_نص` | Formatted time | string |
| `sleep` | `انتظر` | Pause execution | void |
| `exit` | `أنهي_البرنامج` | Exit program | never |

---

**📚 This standard library provides comprehensive functionality for Sad Language programs**

All functions handle Arabic text properly and provide Arabic error messages. For usage examples and advanced patterns, see the [User Guide](USER_GUIDE_AR.md) and [examples directory](../examples/).

---

*Last updated: November 28, 2025*  
*Sad Language - Version 1.0*