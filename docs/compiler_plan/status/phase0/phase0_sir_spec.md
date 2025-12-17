# مواصفات التمثيل الوسيط البسيط (SIR)
# Simple Intermediate Representation (SIR) Specification

**التاريخ / Date:** 2 ديسمبر 2025  
**الحالة / Status:** المرحلة 0 - التصميم / Phase 0 - Design  
**الإصدار / Version:** 1.0

---

## 🎯 نظرة عامة / Overview

### (AR) العربية

**SIR** هو تمثيل وسيط بسيط (Simple Intermediate Representation) يُستخدم كجسر بين:
- **Frontend:** AST (الشجرة النحوية التجريدية)
- **Backend:** Bytecode VM أو LLVM IR

**الخصائص:**
- تمثيل رباعي (Three-Address Code - TAC)
- سجلات افتراضية غير محدودة
- تعليمات بسيطة وواضحة
- سهل التحسين (Optimization-friendly)
- مستقل عن المنصة (Platform-independent)

---

## 📐 البنية الأساسية / Basic Structure

### نموذج التعليمة / Instruction Model

```
result = op operand1, operand2
```

**مثال:**
```
# Source: رقم س = 5 + 3 * 2؛
%t1 = mul i64 3, 2      # t1 = 3 * 2
%t2 = add i64 5, %t1    # t2 = 5 + t1
%س = store i64 %t2       # س = t2
```

---

## 🔢 أنواع البيانات / Data Types

### (AR) العربية

```cpp
enum class SIRType {
    // أنواع أساسية / Primitive types
    VOID,       // فارغ
    I64,        // رقم صحيح 64-bit
    F64,        // رقم عشري 64-bit
    BOOL,       // منطقي (true/false)
    
    // أنواع مركبة / Composite types
    PTR,        // مؤشر (pointer)
    ARRAY,      // مصفوفة (array)
    STRING,     // نص (string)
    STRUCT,     // بنية (struct/object)
    FUNCTION,   // دالة (function pointer)
};
```

**تمثيل الأنواع:**
```
i64         # رقم صحيح
f64         # رقم عشري
bool        # منطقي
ptr<i64>    # مؤشر لرقم صحيح
array<i64>  # مصفوفة من الأرقام
string      # نص
%ClassName  # كائن من صنف
```

---

## 📝 السجلات الافتراضية / Virtual Registers

### (AR) العربية

**تسمية السجلات:**
```
%0, %1, %2, ...         # سجلات مؤقتة / Temporary registers
%variableName           # متغيرات مسماة / Named variables
@functionName           # أسماء الدوال / Function names
$globalVar              # متغيرات عامة / Global variables
```

**أمثلة:**
```sir
%t1 = add i64 5, 3      # سجل مؤقت
%س = store i64 %t1       # متغير مسمى
%result = call @جمع(%س، 10)  # استدعاء دالة
$PI = f64 3.14159       # ثابت عام
```

---

## 🔧 مجموعة التعليمات / Instruction Set

### 1. العمليات الحسابية / Arithmetic Operations (10 opcodes)

```sir
# الجمع / Addition
%result = add i64 %a, %b
%result = add f64 %x, %y

# الطرح / Subtraction
%result = sub i64 %a, %b
%result = sub f64 %x, %y

# الضرب / Multiplication
%result = mul i64 %a, %b
%result = mul f64 %x, %y

# القسمة / Division
%result = div i64 %a, %b      # قسمة صحيحة
%result = div f64 %x, %y      # قسمة عشرية

# الباقي / Modulo
%result = mod i64 %a, %b

# السالب / Negation
%result = neg i64 %a
%result = neg f64 %x
```

**مثال:**
```sir
# Source: رقم س = (5 + 3) * 2؛
%t1 = add i64 5, 3
%t2 = mul i64 %t1, 2
%س = store i64 %t2
```

---

### 2. العمليات الثنائية / Bitwise Operations (8 opcodes)

```sir
# AND
%result = and i64 %a, %b

# OR
%result = or i64 %a, %b

# XOR
%result = xor i64 %a, %b

# NOT
%result = not i64 %a

# Shift Left
%result = shl i64 %a, %b

# Shift Right (Logical)
%result = shr i64 %a, %b

# Shift Right (Arithmetic)
%result = sar i64 %a, %b

# Rotate Left
%result = rol i64 %a, %b
```

**مثال:**
```sir
# Source: رقم flags = 0b1010 | 0b0101؛
%t1 = or i64 10, 5
%flags = store i64 %t1
```

---

### 3. المقارنات / Comparison Operations (6 opcodes)

```sir
# Equal
%result = eq i64 %a, %b

# Not Equal
%result = ne i64 %a, %b

# Less Than
%result = lt i64 %a, %b

# Less Than or Equal
%result = le i64 %a, %b

# Greater Than
%result = gt i64 %a, %b

# Greater Than or Equal
%result = ge i64 %a, %b
```

**مثال:**
```sir
# Source: إذا (س > 10) {...}
%t1 = gt i64 %س, 10
br %t1, label %then, label %else
```

---

### 4. التحكم في التدفق / Control Flow (8 opcodes)

```sir
# قفز غير مشروط / Unconditional branch
br label %target

# قفز مشروط / Conditional branch
br %condition, label %then, label %else

# إرجاع / Return
ret i64 %value
ret void

# استدعاء دالة / Function call
%result = call @functionName(%arg1, %arg2)

# استدعاء غير مباشر / Indirect call
%result = call %funcPtr(%arg1, %arg2)

# قفز محسوب / Computed jump
switch %value, label %default, [
    i64 0, label %case0,
    i64 1, label %case1,
    i64 2, label %case2
]
```

**مثال:**
```sir
# Source: إذا (س > 10) { إرجاع س؛ } وإلا { إرجاع 0؛ }
%t1 = gt i64 %س, 10
br %t1, label %then, label %else

label %then:
    ret i64 %س

label %else:
    ret i64 0
```

---

### 5. الذاكرة والمؤشرات / Memory & Pointers (10 opcodes)

```sir
# تخصيص ذاكرة / Allocate memory
%ptr = alloc i64               # على الـ stack
%ptr = alloc_heap i64 1024     # على الـ heap

# تحرير ذاكرة / Free memory
free %ptr

# تحميل / Load
%value = load ptr<i64> %ptr

# تخزين / Store
store i64 %value, %ptr

# عنوان المتغير / Get address
%ptr = addr %variable

# Pointer arithmetic
%ptr2 = ptr_add %ptr, i64 8

# Pointer cast
%ptr2 = ptr_cast ptr<i64> %ptr1, ptr<f64>

# Copy memory
memcpy %dest, %src, i64 1024

# Set memory
memset %ptr, i8 0, i64 1024

# Compare memory
%result = memcmp %ptr1, %ptr2, i64 1024
```

**مثال:**
```sir
# Source: مؤشر<رقم> ptr = &س؛ *ptr = 42؛
%ptr = addr %س
store i64 42, %ptr
```

---

### 6. المصفوفات / Arrays (6 opcodes)

```sir
# إنشاء مصفوفة / Create array
%arr = array_new array<i64>, i64 10

# الوصول للعنصر / Get element
%value = array_get %arr, i64 5

# تعديل عنصر / Set element
array_set %arr, i64 5, i64 42

# الطول / Get length
%len = array_len %arr

# إضافة عنصر / Append
array_append %arr, i64 99

# حذف عنصر / Remove
array_remove %arr, i64 5
```

**مثال:**
```sir
# Source: مصفوفة arr = [1، 2، 3]؛ arr[1] = 10؛
%arr = array_new array<i64>, i64 3
array_set %arr, i64 0, i64 1
array_set %arr, i64 1, i64 2
array_set %arr, i64 2, i64 3
array_set %arr, i64 1, i64 10
```

---

### 7. النصوص / Strings (8 opcodes)

```sir
# إنشاء نص / Create string
%str = string_new "مرحبا"

# الطول / Get length
%len = string_len %str

# الدمج / Concatenation
%result = string_concat %str1, %str2

# المقارنة / Compare
%result = string_cmp %str1, %str2

# استخراج جزء / Substring
%result = string_substr %str, i64 0, i64 5

# البحث / Find
%index = string_find %str, %needle

# الاستبدال / Replace
%result = string_replace %str, %old, %new

# التحويل / Conversion
%str = string_from_i64 %num
%num = string_to_i64 %str
```

---

### 8. البرمجة الكائنية / OOP (10 opcodes)

```sir
# إنشاء كائن / Create object
%obj = object_new %ClassName

# الوصول لخاصية / Get property
%value = object_get %obj, "propertyName"

# تعديل خاصية / Set property
object_set %obj, "propertyName", %value

# استدعاء دالة / Call method
%result = object_call %obj, "methodName", %arg1, %arg2

# تحقق من النوع / Type check
%result = instanceof %obj, %ClassName

# الوراثة / Inheritance
%child = object_cast %parent, %ChildClass

# تعريف صنف / Define class
class %ClassName {
    field %property1 : i64
    field %property2 : string
    
    method @constructor(%arg1, %arg2) { ... }
    method @methodName(%arg1) -> i64 { ... }
}
```

**مثال:**
```sir
# Source: Person p = جديد Person("أحمد"، 25)؛
%name = string_new "أحمد"
%age = i64 25
%p = object_new %Person
%result = object_call %p, "constructor", %name, %age
```

---

### 9. تحويل الأنواع / Type Conversion (8 opcodes)

```sir
# رقم صحيح → عشري
%f = i64_to_f64 %i

# عشري → رقم صحيح
%i = f64_to_i64 %f

# رقم → منطقي
%b = i64_to_bool %i

# منطقي → رقم
%i = bool_to_i64 %b

# رقم → نص
%s = i64_to_string %i

# نص → رقم
%i = string_to_i64 %s

# تحويل مؤشر
%ptr2 = ptr_cast %ptr1, ptr<NewType>

# تحويل كائن
%child = object_cast %parent, %ChildClass
```

---

### 10. دوال مضمنة / Builtin Functions (15 opcodes)

```sir
# I/O
call @اطبع(%value)
%value = call @اقرأ()

# Math
%result = call @جذر(%x)
%result = call @قوة(%base, %exp)
%result = call @جا(%x)
%result = call @جتا(%x)

# Debug
call @debug_print(%msg)
call @assert(%condition, %msg)
```

---

## 📋 أمثلة كاملة / Complete Examples

### مثال 1: دالة بسيطة / Simple Function

**Source Code:**
```s
دالة جمع(رقم أ، رقم ب) -> رقم
    إرجاع أ + ب
نهاية
```

**SIR:**
```sir
function @جمع(%أ : i64, %ب : i64) -> i64 {
entry:
    %t1 = add i64 %أ, %ب
    ret i64 %t1
}
```

---

### مثال 2: حلقة / Loop

**Source Code:**
```s
دالة factorial(رقم n) -> رقم
    رقم result = 1
    بينما (n > 1)
        result = result * n
        n = n - 1
    نهاية
    إرجاع result
نهاية
```

**SIR:**
```sir
function @factorial(%n : i64) -> i64 {
entry:
    %result = i64 1
    br label %loop_cond

loop_cond:
    %t1 = gt i64 %n, 1
    br %t1, label %loop_body, label %loop_end

loop_body:
    %t2 = mul i64 %result, %n
    %result = store i64 %t2
    %t3 = sub i64 %n, 1
    %n = store i64 %t3
    br label %loop_cond

loop_end:
    ret i64 %result
}
```

---

### مثال 3: مصفوفة / Array

**Source Code:**
```s
دالة sum_array(مصفوفة arr) -> رقم
    رقم sum = 0
    لكل (item في arr)
        sum = sum + item
    نهاية
    إرجاع sum
نهاية
```

**SIR:**
```sir
function @sum_array(%arr : array<i64>) -> i64 {
entry:
    %sum = i64 0
    %len = array_len %arr
    %i = i64 0
    br label %loop_cond

loop_cond:
    %t1 = lt i64 %i, %len
    br %t1, label %loop_body, label %loop_end

loop_body:
    %item = array_get %arr, %i
    %t2 = add i64 %sum, %item
    %sum = store i64 %t2
    %t3 = add i64 %i, 1
    %i = store i64 %t3
    br label %loop_cond

loop_end:
    ret i64 %sum
}
```

---

### مثال 4: صنف / Class

**Source Code:**
```s
صنف Person
    نص name
    رقم age
    
    دالة constructor(نص n، رقم a)
        name = n
        age = a
    نهاية
    
    دالة greet() -> نص
        إرجاع "مرحبا، أنا " + name
    نهاية
نهاية
```

**SIR:**
```sir
class %Person {
    field %name : string
    field %age : i64
    
    method @constructor(%this, %n : string, %a : i64) -> void {
    entry:
        object_set %this, "name", %n
        object_set %this, "age", %a
        ret void
    }
    
    method @greet(%this) -> string {
    entry:
        %name = object_get %this, "name"
        %prefix = string_new "مرحبا، أنا "
        %result = string_concat %prefix, %name
        ret string %result
    }
}
```

---

## 🎯 تحسينات SIR / SIR Optimizations

### (AR) العربية

SIR مصمم ليكون سهل التحسين:

1. **Constant Folding:**
   ```sir
   # Before
   %t1 = add i64 2, 3
   %t2 = mul i64 %t1, 4
   
   # After
   %t2 = i64 20
   ```

2. **Dead Code Elimination:**
   ```sir
   # Before
   %t1 = add i64 5, 3    # unused
   %t2 = mul i64 2, 4
   ret i64 %t2
   
   # After
   %t2 = mul i64 2, 4
   ret i64 %t2
   ```

3. **Common Subexpression Elimination:**
   ```sir
   # Before
   %t1 = add i64 %a, %b
   %t2 = mul i64 %t1, 2
   %t3 = add i64 %a, %b  # duplicate!
   %t4 = div i64 %t3, 3
   
   # After
   %t1 = add i64 %a, %b
   %t2 = mul i64 %t1, 2
   %t4 = div i64 %t1, 3
   ```

---

## 📊 إحصائيات / Statistics

**إجمالي التعليمات:** ~90 opcode

| الفئة | العدد |
|-------|-------|
| الحسابية | 10 |
| الثنائية | 8 |
| المقارنة | 6 |
| التحكم | 8 |
| الذاكرة | 10 |
| المصفوفات | 6 |
| النصوص | 8 |
| OOP | 10 |
| التحويل | 8 |
| المضمنة | 15 |
| **المجموع** | **89** |

---

**السابق / Previous:** [Interpreter Architecture](phase0_architecture.md)  
**التالي / Next:** [Project Structure](phase0_structure.md)
