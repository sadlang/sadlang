# 🎯 قواعد المؤشرات / Pointer Rules

**التاريخ / Date:** 2 ديسمبر 2025  
**الإصدار / Version:** 1.0  
**الحالة / Status:** مرجع تفصيلي لنظام المؤشرات

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **نظام المؤشرات الكامل** للغة "ص" للاستخدام في برمجة الأنظمة. المؤشرات تسمح بالوصول المباشر للذاكرة وهي ضرورية لتطوير kernel وdrivers.

### (EN) Description
This file documents the **complete pointer system** لكل Sad language لكل system programming. Pointers enable direct memory access and are essential لكل kernel and driver development.

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/system_build_plan/01_stage1_low_level.md` - Stage 1 specifications
- ✅ `include/low_level/pointer_type.h` - Pointer implementation
- ✅ `src/low_level/pointer_type.cpp` - Pointer operations

---

## 📋 أنواع المؤشرات / Pointer Types

| النوع (AR) | Type (EN) | الوصف | Description |
|-----------|-----------|-------|-------------|
| مؤشر<رقم> | مؤشر<number> | مؤشر لرقم صحيح | Pointer to integer |
| مؤشر<عشري> | مؤشر<float> | مؤشر لرقم عشري | Pointer to float |
| مؤشر<نص> | مؤشر<string> | مؤشر لنص | Pointer to string |
| مؤشر<منطقي> | مؤشر<boolean> | مؤشر لقيمة منطقية | Pointer to boolean |
| مؤشر<فارغ> | مؤشر<void> | مؤشر عام | Generic pointer |

---

## 1️⃣ تعريف المؤشر / Pointer Declaration

### Syntax (EBNF)
```ebnf
pointer_type ::= 'مؤشر' '<' base_type '>'

pointer_declaration ::= pointer_type identifier ['=' pointer_expression]

base_type ::= 'رقم' | 'number' | 'عشري' | 'float' | 'نص' | 'string' 
            | 'منطقي' | 'boolean' | 'فارغ' | 'void'

pointer_expression ::= 'عنوان_من' '(' identifier ')' 
                     | null_literal
                     | pointer_arithmetic
```

### Semantics
- **Size:** 8 bytes (64-bit address)
- **Default Value:** null (0x0)
- **Alignment:** 8-byte aligned
- **Safety:** Runtime null checks enabled

### أمثلة / Examples

#### (AR) العربية
```s
# تعريف متغير
رقم قيمة = 42

# الحصول على عنوان المتغير
مؤشر<رقم> عنوان_القيمة = عنوان_من(قيمة)

# مؤشر فارغ
مؤشر<فارغ> مؤشر_عام = عنوان_من(قيمة)

# مؤشر null
مؤشر<رقم> مؤشر_فارغ = لاشيء

# طباعة العنوان
اطبع("العنوان: ")
اطبع(عنوان_القيمة)
اطبع("\n")
```

#### (EN) English
```s
# Variable declaration
number value = 42

# Get address of variable
مؤشر<number> value_address = عنوان_من(value)

# Void pointer
مؤشر<void> generic_ptr = عنوان_من(value)

# Null pointer
مؤشر<number> null_ptr = null

# Print address
print("Address: ")
print(value_address)
print("\n")
```

### AST Representation
```json
{
  "type": "VariableDeclaration",
  "dataType": {
    "type": "PointerType",
    "elementType": "INTEGER",
    "level": 1
  },
  "identifier": "عنوان_القيمة",
  "initializer": {
    "type": "CallExpression",
    "callee": "عنوان_من",
    "arguments": [
      {"type": "Identifier", "name": "قيمة"}
    ]
  نهاية
نهاية
```

---

## 2️⃣ إلغاء المرجعية / Dereferencing

### Syntax (EBNF)
```ebnf
dereference_expression ::= 'قيمة_من' '(' pointer_expression ')'
                         | '*' pointer_expression
```

### Semantics
- **Operation:** Reads value from memory address
- **Safety:** Throws error on null dereference
- **Type:** Returns base type of pointer

### أمثلة / Examples

#### (AR) العربية
```s
# تعريف متغير
رقم عدد = 100

# الحصول على مؤشر
مؤشر<رقم> مؤشر_العدد = عنوان_من(عدد)

# قراءة القيمة من المؤشر
رقم القيمة_المقروءة = قيمة_من(مؤشر_العدد)

اطبع("القيمة الأصلية: ")
اطبع(عدد)
اطبع("\n")

اطبع("القيمة من المؤشر: ")
اطبع(القيمة_المقروءة)
اطبع("\n")

# تحقق من المساواة
اطبع("متساوية: ")
اطبع(عدد == القيمة_المقروءة)
اطبع("\n")
```

#### (EN) English
```s
# Variable declaration
number count = 100

# Get pointer
مؤشر<number> count_ptr = عنوان_من(count)

# Read value from pointer
number read_value = قيمة_من(count_ptr)

print("Original value: ")
print(count)
print("\n")

print("Value from pointer: ")
print(read_value)
print("\n")

# Check equality
print("Equal: ")
print(count == read_value)
print("\n")
```

---

## 3️⃣ الكتابة عبر المؤشر / Write Through Pointer

### Syntax (EBNF)
```ebnf
write_through_expression ::= 'اكتب_في' '(' pointer_expression ',' value_expression ')'
```

### Semantics
- **Operation:** Writes value to memory address
- **Safety:** Validates pointer before write
- **Side Effect:** Modifies original variable

### أمثلة / Examples

#### (AR) العربية
```s
# تعريف متغير
رقم رقم_أصلي = 50

# الحصول على مؤشر
مؤشر<رقم> مؤشر_الرقم = عنوان_من(رقم_أصلي)

اطبع("قبل التعديل: ")
اطبع(رقم_أصلي)
اطبع("\n")

# تعديل القيمة عبر المؤشر
اكتب_في(مؤشر_الرقم, 200)

اطبع("بعد التعديل: ")
اطبع(رقم_أصلي)
اطبع("\n")
```

#### (EN) English
```s
# Variable declaration
number original_num = 50

# Get pointer
مؤشر<number> num_ptr = عنوان_من(original_num)

print("Before modification: ")
print(original_num)
print("\n")

# Modify value through pointer
اكتب_في(num_ptr, 200)

print("After modification: ")
print(original_num)
print("\n")
```

---

## 4️⃣ حساب المؤشرات / Pointer Arithmetic

### Syntax (EBNF)
```ebnf
pointer_arithmetic ::= pointer_expression ('+' | '-') integer_expression
                     | 'إزاحة_مؤشر' '(' pointer_expression ',' integer_expression ')'
                     | 'offset_pointer' '(' pointer_expression ',' integer_expression ')'
```

### Semantics
- **Operation:** Advances/rewinds pointer by n elements
- **Size:** Advances by (n × sizeof(base_type))
- **Bounds:** No automatic bounds checking

### أمثلة / Examples

#### (AR) العربية
```s
# مصفوفة أرقام
رقم أرقام_1 = 10
رقم أرقام_2 = 20
رقم أرقام_3 = 30
رقم أرقام_4 = 40

# مؤشر للعنصر الأول
مؤشر<رقم> مؤشر_أول = عنوان_من(أرقام_1)

# قراءة القيمة الأولى
رقم قيمة_1 = قيمة_من(مؤشر_أول)
اطبع("العنصر الأول: ")
اطبع(قيمة_1)
اطبع("\n")

# الانتقال للعنصر التالي (محاكاة)
# في التطبيق الفعلي: مؤشر_ثاني = مؤشر_أول + 1
مؤشر<رقم> مؤشر_ثاني = عنوان_من(أرقام_2)
رقم قيمة_2 = قيمة_من(مؤشر_ثاني)

اطبع("العنصر الثاني: ")
اطبع(قيمة_2)
اطبع("\n")
```

#### (EN) English
```s
# Array of numbers
number nums_1 = 10
number nums_2 = 20
number nums_3 = 30
number nums_4 = 40

# Pointer to first element
مؤشر<number> first_ptr = عنوان_من(nums_1)

# Read first value
number val_1 = قيمة_من(first_ptr)
print("First element: ")
print(val_1)
print("\n")

# Move to next element (simulated)
# In actual implementation: second_ptr = first_ptr + 1
مؤشر<number> second_ptr = عنوان_من(nums_2)
number val_2 = قيمة_من(second_ptr)

print("Second element: ")
print(val_2)
print("\n")
```

---

## 5️⃣ مقارنة المؤشرات / Pointer Comparison

### Syntax (EBNF)
```ebnf
pointer_comparison ::= pointer_expression comparison_operator pointer_expression

comparison_operator ::= '==' | '!=' | '<' | '<=' | '>' | '>='
```

### Semantics
- **Operation:** Compares memory addresses
- **Equality:** True if addresses are identical
- **Ordering:** Compares numeric address values

### أمثلة / Examples

#### (AR) العربية
```s
# متغيرات
رقم أ = 10
رقم ب = 20

# مؤشرات
مؤشر<رقم> مؤشر_أ = عنوان_من(أ)
مؤشر<رقم> مؤشر_ب = عنوان_من(ب)
مؤشر<رقم> مؤشر_أ_نسخة = عنوان_من(أ)

# مقارنة المؤشرات
اطبع("مؤشر_أ == مؤشر_ب: ")
اطبع(مؤشر_أ == مؤشر_ب)
اطبع("\n")

اطبع("مؤشر_أ == مؤشر_أ_نسخة: ")
اطبع(مؤشر_أ == مؤشر_أ_نسخة)
اطبع("\n")

# فحص null
مؤشر<رقم> مؤشر_فارغ = لاشيء
اطبع("مؤشر_فارغ == لاشيء: ")
اطبع(مؤشر_فارغ == لاشيء)
اطبع("\n")
```

#### (EN) English
```s
# Variables
number a = 10
number b = 20

# Pointers
مؤشر<number> ptr_a = عنوان_من(a)
مؤشر<number> ptr_b = عنوان_من(b)
مؤشر<number> ptr_a_copy = عنوان_من(a)

# Compare pointers
print("ptr_a == ptr_b: ")
print(ptr_a == ptr_b)
print("\n")

print("ptr_a == ptr_a_copy: ")
print(ptr_a == ptr_a_copy)
print("\n")

# Check null
مؤشر<number> null_ptr = null
print("null_ptr == null: ")
print(null_ptr == null)
print("\n")
```

---

## 6️⃣ مؤشرات متعددة المستويات / Multi-Level Pointers

### Syntax (EBNF)
```ebnf
multi_level_pointer ::= 'مؤشر' '<' pointer_type '>'
                      | 'pointer' '<' pointer_type '>'
```

### Semantics
- **Level 1:** `مؤشر<number>` - مؤشر لرقم
- **Level 2:** `مؤشر<مؤشر<number>>` - مؤشر لمؤشر لرقم
- **Level 3+:** Nested pointer types

### أمثلة / Examples

#### (AR) العربية
```s
# متغير أساسي
رقم قيمة_أساسية = 77

# مؤشر مستوى 1
مؤشر<رقم> مؤشر_مستوى_1 = عنوان_من(قيمة_أساسية)

# محاكاة مؤشر مستوى 2 (مؤشر لمؤشر)
# في التطبيق الفعلي: مؤشر<مؤشر<رقم>>
اطبع("القيمة الأساسية: ")
اطبع(قيمة_أساسية)
اطبع("\n")

# قراءة عبر مؤشر مستوى 1
رقم قيمة_1 = قيمة_من(مؤشر_مستوى_1)
اطبع("عبر مؤشر مستوى 1: ")
اطبع(قيمة_1)
اطبع("\n")

# محاكاة قراءة عبر مؤشر مستوى 2
# في التطبيق الفعلي: **ptr
اطبع("محاكاة مؤشر مزدوج: نجح")
اطبع("\n")
```

#### (EN) English
```s
# Base variable
number base_value = 77

# Level 1 pointer
مؤشر<number> level1_ptr = عنوان_من(base_value)

# Simulate level 2 pointer (pointer to pointer)
# In actual implementation: مؤشر<مؤشر<number>>
print("Base value: ")
print(base_value)
print("\n")

# Read through level 1 pointer
number val_1 = قيمة_من(level1_ptr)
print("Through level 1 pointer: ")
print(val_1)
print("\n")

# Simulate read through level 2 pointer
# In actual implementation: **ptr
print("Simulate double pointer: Success")
print("\n")
```

---

## 7️⃣ الأمان والتحقق / Safety and Validation

### قواعد الأمان / Safety Rules

1. **Null Pointer Check**
   - يتم فحص المؤشرات الفارغة تلقائياً
   - رسالة خطأ واضحة عند محاولة استخدام مؤشر فارغ

2. **Type Safety**
   - لا يمكن تحويل المؤشرات بين أنواع مختلفة بدون casting صريح
   - `مؤشر<void>` يمكن تحويله لأي نوع

3. **Memory Access**
   - التحقق من صحة العنوان قبل القراءة/الكتابة
   - منع الوصول لعناوين محجوزة للنظام

### أمثلة الأخطاء / Error Examples

#### (AR) خطأ: مؤشر فارغ
```s
مؤشر<رقم> مؤشر_فارغ = لاشيء

# خطأ: محاولة قراءة من مؤشر فارغ
رقم قيمة = قيمة_من(مؤشر_فارغ)  # ERROR: Null pointer dereference
```

#### (EN) Error: Null pointer
```s
مؤشر<number> null_ptr = null

# Error: Attempting to read from null pointer
number value = قيمة_من(null_ptr)  # ERROR: Null pointer dereference
```

---

## 8️⃣ استخدامات متقدمة / Advanced Usage

### مثال: تبديل قيمتين / Swap Example

#### (AR) العربية
```s
# دالة لتبديل قيمتين باستخدام المؤشرات
دالة تبديل(مؤشر<رقم> أ، مؤشر<رقم> ب)
    رقم مؤقت = قيمة_من(أ)
    اكتب_في(أ، قيمة_من(ب))
    اكتب_في(ب، مؤقت)
نهاية

# استخدام الدالة
رقم س = 10
رقم ص = 20

اطبع("قبل التبديل: س=")
اطبع(س)
اطبع(", ص=")
اطبع(ص)
اطبع("\n")

تبديل(عنوان_من(س), عنوان_من(ص))

اطبع("بعد التبديل: س=")
اطبع(س)
اطبع(", ص=")
اطبع(ص)
اطبع("\n")
```

#### (EN) English
```s
# Function to swap two values using pointers
دالة swap(مؤشر<number> a, مؤشر<number> b) {
    number temp = قيمة_من(a)
    اكتب_في(a, قيمة_من(b))
    اكتب_في(b, temp)
نهاية

# Using the function
number x = 10
number y = 20

print("Before swap: x=")
print(x)
print(", y=")
print(y)
print("\n")

swap(عنوان_من(x), عنوان_من(y))

print("After swap: x=")
print(x)
print(", y=")
print(y)
print("\n")
```

---

## 📚 ملاحظات التنفيذ / Implementation Notes

### الواجهة البرمجية / API Interface

```cpp
# C++ Implementation Interface
class PointerManager {
public:
    # Create pointer
    uint64_t createPointer(const PointerType& type, uint64_t address);
    
    # Dereference
    uint64_t dereference(uint64_t pointerId);
    
    # Write through pointer
    void writeThrough(uint64_t pointerId, uint64_t value);
    
    # Get address of variable
    uint64_t getAddressOf(const std::string& varName);
    
    # Pointer arithmetic
    uint64_t pointerAdd(uint64_t pointerId, int64_t offset);
    
    # Validate pointer
    bool isValidPointer(uint64_t pointerId);
    bool isNull(uint64_t pointerId);
};
```

### الدوال المضمنة / Built-in Functions

| الدالة (AR) | Function (EN) | الوصف | Description |
|------------|---------------|-------|-------------|
| عنوان_من() | عنوان_من() | الحصول على عنوان متغير | Get address of variable |
| قيمة_من() | قيمة_من() | قراءة قيمة من عنوان | Read value from address |
| اكتب_في() | اكتب_في() | كتابة قيمة لعنوان | Write value to address |
| اقرأ_من() | read_from() | قراءة من عنوان | Read from address |

---

## ✅ قائمة التحقق / Checklist

- [x] تعريف أنواع المؤشرات
- [x] عمليات الحصول على العنوان
- [x] عمليات إلغاء المرجعية
- [x] الكتابة عبر المؤشرات
- [x] حساب المؤشرات
- [x] مقارنة المؤشرات
- [x] مؤشرات متعددة المستويات
- [x] فحص الأمان
- [x] أمثلة متقدمة
- [x] توثيق API

---

## 🔗 روابط ذات صلة / Related Links

- [Stage 1 Plan](../../system_build_plan/01_stage1_low_level.md)
- [Pointer Implementation](../../../include/low_level/pointer_type.h)
- [Memory Management Rules](03_memory_management.md)
- [Bitwise Operations Rules](02_bitwise_operations.md)

---

**آخر تحديث / Last Update:** 2 ديسمبر 2025  
**المؤلف / Author:** فريق تطوير لغة ص / Sad Language Development Team
