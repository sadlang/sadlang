# ⚙️ قواعد العمليات الثنائية / Bitwise Operations Rules

**التاريخ / Date:** 2 ديسمبر 2025  
**الإصدار / Version:** 1.0  
**الحالة / Status:** مرجع تفصيلي للعمليات الثنائية

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **العمليات الثنائية الكاملة** (Bitwise Operations) للغة "ص". هذه العمليات ضرورية للتعامل مع الأجهزة والذاكرة على مستوى البت، وهي أساسية لبرمجة الأنظمة والعتاد.

### (EN) Description
This file documents the **complete bitwise operations** لكل Sad language. These operations are essential لكل hardware and memory manipulation at bit level, and are fundamental لكل system and hardware programming.

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/system_build_plan/01_stage1_low_level.md` - Stage 1 specifications
- ✅ `include/low_level/bitwise_ops.h` - Bitwise implementation
- ✅ `src/low_level/bitwise_ops.cpp` - Bitwise operations

---

## 📋 قائمة العمليات / Operations List

| العملية (AR) | Operation (EN) | الرمز | Symbol | الوصف | Description |
|-------------|----------------|------|--------|-------|-------------|
| و ثنائي | Bitwise AND | & | & | ضرب منطقي | Logical multiplication |
| أو ثنائي | Bitwise OR | \| | \| | جمع منطقي | Logical addition |
| أو حصري | Bitwise XOR | ^ | ^ | جمع حصري | Exclusive OR |
| نفي ثنائي | Bitwise NOT | ~ | ~ | عكس البتات | Invert bits |
| إزاحة يسار | Left Shift | << | << | إزاحة لليسار | Shift left |
| إزاحة يمين | Right Shift | >> | >> | إزاحة لليمين | Shift right |

---

## 1️⃣ عملية AND الثنائية / Bitwise AND

### Syntax (EBNF)
```ebnf
bitwise_and ::= expression '&' expression
```

### Semantics
- **Operation:** Logical AND على كل بت
- **Truth Table:** 1 & 1 = 1, otherwise 0
- **Use Case:** Masking, flag checking

### جدول الحقيقة / Truth Table
```
A | B | A & B
--|---|------
0 | 0 |   0
0 | 1 |   0
1 | 0 |   0
1 | 1 |   1
```

### أمثلة / Examples

#### (AR) العربية
```s
# أرقام بالنظام الثنائي
رقم أ = 12      # 1100 ثنائي
رقم ب = 10      # 1010 ثنائي

# عملية AND
رقم نتيجة = أ & ب   # 1000 ثنائي = 8

اطبع("12 & 10 = ")
اطبع(نتيجة)
اطبع("\n")

# استخدام في القناع (Masking)
رقم بيانات = 0xFF    # 11111111
رقم قناع = 0x0F      # 00001111
رقم مقنع = بيانات & قناع  # 00001111 = 15

اطبع("255 & 15 = ")
اطبع(مقنع)
اطبع("\n")

# فحص البت (Bit Testing)
رقم حالة = 5        # 0101
رقم بت_0 = حالة & 1  # فحص البت الأول
اطبع("البت 0: ")
اطبع(بت_0)
اطبع("\n")
```

#### (EN) English
```s
# Numbers in binary
number a = 12      # 1100 binary
number b = 10      # 1010 binary

# AND operation
number result = a & b   # 1000 binary = 8

print("12 & 10 = ")
print(result)
print("\n")

# Masking usage
number data = 0xFF    # 11111111
number mask = 0x0F    # 00001111
number masked = data & mask  # 00001111 = 15

print("255 & 15 = ")
print(masked)
print("\n")

# Bit testing
number status = 5        # 0101
number bit_0 = status & 1  # Check first bit
print("Bit 0: ")
print(bit_0)
print("\n")
```

---

## 2️⃣ عملية OR الثنائية / Bitwise OR

### Syntax (EBNF)
```ebnf
bitwise_or ::= expression '|' expression
```

### Semantics
- **Operation:** Logical OR على كل بت
- **Truth Table:** 0 | 0 = 0, otherwise 1
- **Use Case:** Setting bits, combining flags

### جدول الحقيقة / Truth Table
```
A | B | A | B
--|---|------
0 | 0 |   0
0 | 1 |   1
1 | 0 |   1
1 | 1 |   1
```

### أمثلة / Examples

#### (AR) العربية
```s
# عملية OR الأساسية
رقم س = 12      # 1100
رقم ص = 10      # 1010
رقم نتيجة = س | ص   # 1110 = 14

اطبع("12 | 10 = ")
اطبع(نتيجة)
اطبع("\n")

# تفعيل البتات (Set Bits)
رقم أعلام = 0      # 0000
رقم علم_1 = 1      # 0001
رقم علم_2 = 2      # 0010
رقم علم_4 = 4      # 0100

# تفعيل عدة أعلام
أعلام = أعلام | علم_1  # تفعيل العلم 1
أعلام = أعلام | علم_4  # تفعيل العلم 4

اطبع("الأعلام المفعلة: ")
اطبع(أعلام)
اطبع("\n")

# دمج الأقنعة
رقم قناع_قراءة = 4    # 100
رقم قناع_كتابة = 2    # 010
رقم قناع_تنفيذ = 1    # 001

رقم صلاحيات = قناع_قراءة | قناع_كتابة  # 110 = 6

اطبع("صلاحيات القراءة والكتابة: ")
اطبع(صلاحيات)
اطبع("\n")
```

#### (EN) English
```s
# Basic OR operation
number x = 12      # 1100
number y = 10      # 1010
number result = x | y   # 1110 = 14

print("12 | 10 = ")
print(result)
print("\n")

# Setting bits
number flags = 0      # 0000
number flag_1 = 1     # 0001
number flag_2 = 2     # 0010
number flag_4 = 4     # 0100

# Set multiple flags
flags = flags | flag_1  # Set flag 1
flags = flags | flag_4  # Set flag 4

print("Active flags: ")
print(flags)
print("\n")

# Combining masks
number read_mask = 4    # 100
number write_mask = 2   # 010
number exec_mask = 1    # 001

number permissions = read_mask | write_mask  # 110 = 6

print("Read and write permissions: ")
print(permissions)
print("\n")
```

---

## 3️⃣ عملية XOR الثنائية / Bitwise XOR

### Syntax (EBNF)
```ebnf
bitwise_xor ::= expression '^' expression
```

### Semantics
- **Operation:** Exclusive OR على كل بت
- **Truth Table:** 1 ^ 1 = 0, 0 ^ 0 = 0, otherwise 1
- **Use Case:** Toggling bits, encryption, swap

### جدول الحقيقة / Truth Table
```
A | B | A ^ B
--|---|------
0 | 0 |   0
0 | 1 |   1
1 | 0 |   1
1 | 1 |   0
```

### أمثلة / Examples

#### (AR) العربية
```s
# عملية XOR الأساسية
رقم أ = 12      # 1100
رقم ب = 10      # 1010
رقم نتيجة = أ ^ ب   # 0110 = 6

اطبع("12 ^ 10 = ")
اطبع(نتيجة)
اطبع("\n")

# عكس البتات (Toggle Bits)
رقم حالة = 5       # 0101
رقم قناع = 3       # 0011
رقم معكوس = حالة ^ قناع  # 0110 = 6

اطبع("عكس البتات: ")
اطبع(معكوس)
اطبع("\n")

# تشفير بسيط (Simple Encryption)
رقم رسالة = 65     # 'A' في ASCII
رقم مفتاح = 42     # مفتاح التشفير
رقم مشفر = رسالة ^ مفتاح
رقم فك_تشفير = مشفر ^ مفتاح  # يعيد الرسالة الأصلية

اطبع("الرسالة الأصلية: ")
اطبع(رسالة)
اطبع("\n")

اطبع("بعد التشفير: ")
اطبع(مشفر)
اطبع("\n")

اطبع("بعد فك التشفير: ")
اطبع(فك_تشفير)
اطبع("\n")

# خاصية XOR: a ^ a = 0
رقم قيمة = 42
رقم صفر = قيمة ^ قيمة

اطبع("42 ^ 42 = ")
اطبع(صفر)
اطبع("\n")
```

#### (EN) English
```s
# Basic XOR operation
number a = 12      # 1100
number b = 10      # 1010
number result = a ^ b   # 0110 = 6

print("12 ^ 10 = ")
print(result)
print("\n")

# Toggle bits
number state = 5       # 0101
number mask = 3        # 0011
number toggled = state ^ mask  # 0110 = 6

print("Toggled bits: ")
print(toggled)
print("\n")

# Simple encryption
number message = 65     # 'A' in ASCII
number key = 42        # Encryption key
number encrypted = message ^ key
number decrypted = encrypted ^ key  # Recovers original

print("Original message: ")
print(message)
print("\n")

print("Encrypted: ")
print(encrypted)
print("\n")

print("Decrypted: ")
print(decrypted)
print("\n")

# XOR property: a ^ a = 0
number value = 42
number zero = value ^ value

print("42 ^ 42 = ")
print(zero)
print("\n")
```

---

## 4️⃣ عملية NOT الثنائية / Bitwise NOT

### Syntax (EBNF)
```ebnf
bitwise_not ::= '~' expression
```

### Semantics
- **Operation:** عكس كل بت
- **Result:** ~n = -(n+1) (Two's complement)
- **Use Case:** Bitwise inversion, creating masks

### أمثلة / Examples

#### (AR) العربية
```s
# عكس الأرقام
رقم عدد = 5        # 0101
رقم معكوس = ~عدد   # 1010 (في نظام 4 بت)

اطبع("~5 = ")
اطبع(معكوس)
اطبع("\n")

# إنشاء قناع
رقم قناع_كامل = 0xFF    # 11111111
رقم قناع_عكسي = ~قناع_كامل  # 00000000

اطبع("~255 = ")
اطبع(قناع_عكسي)
اطبع("\n")

# خاصية NOT المزدوجة: ~~n = n
رقم أصلي = 42
رقم عكس_مرة = ~أصلي
رقم عكس_مرتين = ~عكس_مرة

اطبع("الرقم الأصلي: ")
اطبع(أصلي)
اطبع("\n")

اطبع("بعد عكس مزدوج: ")
اطبع(عكس_مرتين)
اطبع("\n")
```

#### (EN) English
```s
# Number inversion
number num = 5        # 0101
number inverted = ~num   # 1010 (in 4-bit system)

print("~5 = ")
print(inverted)
print("\n")

# Creating masks
number full_mask = 0xFF    # 11111111
number inv_mask = ~full_mask  # 00000000

print("~255 = ")
print(inv_mask)
print("\n")

# Double NOT property: ~~n = n
number original = 42
number not_once = ~original
number not_twice = ~not_once

print("Original number: ")
print(original)
print("\n")

print("After double NOT: ")
print(not_twice)
print("\n")
```

---

## 5️⃣ الإزاحة لليسار / Left Shift

### Syntax (EBNF)
```ebnf
left_shift ::= expression '<<' integer_literal
```

### Semantics
- **Operation:** إزاحة البتات لليسار
- **Effect:** Multiplication by 2^n
- **Bits:** تضاف أصفار من اليمين

### أمثلة / Examples

#### (AR) العربية
```s
# إزاحة أساسية
رقم عدد = 5        # 0101
رقم مزاح = عدد << 1   # 1010 = 10

اطبع("5 << 1 = ")
اطبع(مزاح)
اطبع("\n")

# الضرب السريع في 2
رقم قيمة = 7
رقم ضعف = قيمة << 1     # 7 * 2 = 14
رقم أربعة_أضعاف = قيمة << 2  # 7 * 4 = 28

اطبع("7 * 2 = ")
اطبع(ضعف)
اطبع("\n")

اطبع("7 * 4 = ")
اطبع(أربعة_أضعاف)
اطبع("\n")

# حساب القوى (Powers of 2)
رقم قوة_0 = 1 << 0   # 2^0 = 1
رقم قوة_1 = 1 << 1   # 2^1 = 2
رقم قوة_2 = 1 << 2   # 2^2 = 4
رقم قوة_3 = 1 << 3   # 2^3 = 8

اطبع("2^3 = ")
اطبع(قوة_3)
اطبع("\n")

# إنشاء أقنعة البتات
رقم بت_4 = 1 << 4   # 00010000 = 16
اطبع("البت الرابع: ")
اطبع(بت_4)
اطبع("\n")
```

#### (EN) English
```s
# Basic shift
number num = 5        # 0101
number shifted = num << 1   # 1010 = 10

print("5 << 1 = ")
print(shifted)
print("\n")

# Fast multiplication by 2
number value = 7
number double = value << 1     # 7 * 2 = 14
number quadruple = value << 2  # 7 * 4 = 28

print("7 * 2 = ")
print(double)
print("\n")

print("7 * 4 = ")
print(quadruple)
print("\n")

# Computing powers of 2
number pow_0 = 1 << 0   # 2^0 = 1
number pow_1 = 1 << 1   # 2^1 = 2
number pow_2 = 1 << 2   # 2^2 = 4
number pow_3 = 1 << 3   # 2^3 = 8

print("2^3 = ")
print(pow_3)
print("\n")

# Creating bit masks
number bit_4 = 1 << 4   # 00010000 = 16
print("Bit 4: ")
print(bit_4)
print("\n")
```

---

## 6️⃣ الإزاحة لليمين / Right Shift

### Syntax (EBNF)
```ebnf
right_shift ::= expression '>>' integer_literal
```

### Semantics
- **Operation:** إزاحة البتات لليمين
- **Effect:** Division by 2^n (integer division)
- **Types:** Logical (unsigned) or Arithmetic (signed)

### أمثلة / Examples

#### (AR) العربية
```s
# إزاحة أساسية
رقم عدد = 10       # 1010
رقم مزاح = عدد >> 1   # 0101 = 5

اطبع("10 >> 1 = ")
اطبع(مزاح)
اطبع("\n")

# القسمة السريعة على 2
رقم قيمة = 32
رقم نصف = قيمة >> 1     # 32 / 2 = 16
رقم ربع = قيمة >> 2     # 32 / 4 = 8

اطبع("32 / 2 = ")
اطبع(نصف)
اطبع("\n")

اطبع("32 / 4 = ")
اطبع(ربع)
اطبع("\n")

# استخراج البتات العليا
رقم بيانات = 0xABCD   # 1010101111001101
رقم عالي = بيانات >> 8  # 10101011 = 171

اطبع("البايت العالي من 0xABCD: ")
اطبع(عالي)
اطبع("\n")

# فحص البتات المختلفة
رقم حالة = 20         # 10100
رقم بت_2 = (حالة >> 2) & 1  # فحص البت الثاني
اطبع("البت 2: ")
اطبع(بت_2)
اطبع("\n")
```

#### (EN) English
```s
# Basic shift
number num = 10       # 1010
number shifted = num >> 1   # 0101 = 5

print("10 >> 1 = ")
print(shifted)
print("\n")

# Fast division by 2
number value = 32
number half = value >> 1     # 32 / 2 = 16
number quarter = value >> 2  # 32 / 4 = 8

print("32 / 2 = ")
print(half)
print("\n")

print("32 / 4 = ")
print(quarter)
print("\n")

# Extracting high bits
number data = 0xABCD   # 1010101111001101
number high = data >> 8  # 10101011 = 171

print("High byte of 0xABCD: ")
print(high)
print("\n")

# Testing different bits
number status = 20         # 10100
number bit_2 = (status >> 2) & 1  # Check bit 2
print("Bit 2: ")
print(bit_2)
print("\n")
```

---

## 7️⃣ عمليات متقدمة / Advanced Operations

### دوران البتات / Bit Rotation

#### (AR) العربية
```s
# محاكاة دوران لليسار (Rotate Left)
دالة دوران_يسار(رقم قيمة, رقم عدد, رقم حجم_بت) -> رقم {
    # ROL simulation: (value << n) | (value >> (size - n))
    رقم مزاح_يسار = قيمة << عدد
    رقم مزاح_يمين = قيمة >> (حجم_بت - عدد)
    أرجع مزاح_يسار | مزاح_يمين
}

# محاكاة دوران لليمين (Rotate Right)
دالة دوران_يمين(رقم قيمة, رقم عدد, رقم حجم_بت) -> رقم {
    # ROR simulation: (value >> n) | (value << (size - n))
    رقم مزاح_يمين = قيمة >> عدد
    رقم مزاح_يسار = قيمة << (حجم_بت - عدد)
    أرجع مزاح_يمين | مزاح_يسار
}

# استخدام
رقم بيانات = 0b10110010  # 178
رقم مدور = دوران_يسار(بيانات, 2, 8)

اطبع("بعد الدوران: ")
اطبع(مدور)
اطبع("\n")
```

#### (EN) English
```s
# Rotate left simulation
دالة rotate_left(number value, number count, number bit_size) -> number {
    # ROL simulation: (value << n) | (value >> (size - n))
    number left_shift = value << count
    number right_shift = value >> (bit_size - count)
    إرجاع left_shift | right_shift
}

# Rotate right simulation
دالة rotate_right(number value, number count, number bit_size) -> number {
    # ROR simulation: (value >> n) | (value << (size - n))
    number right_shift = value >> count
    number left_shift = value << (bit_size - count)
    إرجاع right_shift | left_shift
}

# Usage
number data = 0b10110010  # 178
number rotated = rotate_left(data, 2, 8)

print("After rotation: ")
print(rotated)
print("\n")
```

### عد البتات المفعلة / Count Set Bits (Popcount)

#### (AR) العربية
```s
# عد عدد البتات المساوية لـ 1
دالة عد_بتات(رقم قيمة) -> رقم {
    رقم عداد = 0
    
    # استخدام خوارزمية Brian Kernighan
    # n & (n-1) تزيل أقل بت مفعل
    رقم مؤقت = قيمة
    
    بينما (مؤقت > 0)
        عداد = عداد + 1
        مؤقت = مؤقت & (مؤقت - 1)
    نهاية
    
    إرجاع عداد
نهاية

رقم عدد = 0b10110110  # 182
رقم بتات_مفعلة = عد_بتات(عدد)

اطبع("عدد البتات المفعلة: ")
اطبع(بتات_مفعلة)
اطبع("\n")
```

#### (EN) English
```s
# Count number of 1 bits
دالة count_bits(number value) -> number {
    number counter = 0
    
    # Using Brian Kernighan's algorithm
    # n & (n-1) removes the lowest set bit
    number temp = value
    
    بينما (temp > 0)
        counter = counter + 1
        temp = temp & (temp - 1)
    نهاية
    
    إرجاع counter
نهاية

number num = 0b10110110  # 182
number set_bits = count_bits(num)

print("Number of set bits: ")
print(set_bits)
print("\n")
```

---

## 8️⃣ استخدامات في البرمجة النظامية / System Programming Usage

### أقنعة الصلاحيات / Permission Masks

#### (AR) العربية
```s
# ثوابت الصلاحيات
رقم قراءة = 4    # 100
رقم كتابة = 2    # 010
رقم تنفيذ = 1    # 001

# دالة فحص الصلاحية
دالة له_صلاحية(رقم صلاحيات, رقم صلاحية_مطلوبة) -> منطقي {
    أرجع (صلاحيات & صلاحية_مطلوبة) == صلاحية_مطلوبة
}

# إنشاء صلاحيات
رقم صلاحيات_ملف = قراءة | كتابة  # 110 = 6

# فحص الصلاحيات
منطقي يمكن_القراءة = له_صلاحية(صلاحيات_ملف, قراءة)
منطقي يمكن_التنفيذ = له_صلاحية(صلاحيات_ملف, تنفيذ)

اطبع("يمكن القراءة: ")
اطبع(يمكن_القراءة)
اطبع("\n")

اطبع("يمكن التنفيذ: ")
اطبع(يمكن_التنفيذ)
اطبع("\n")
```

#### (EN) English
```s
# Permission constants
number read = 4    # 100
number write = 2   # 010
number exec = 1    # 001

# Check permission function
دالة has_permission(number perms, number req_perm) -> boolean {
    إرجاع (perms & req_perm) == req_perm
}

# Create permissions
number file_perms = read | write  # 110 = 6

# Check permissions
boolean can_read = has_permission(file_perms, read)
boolean can_execute = has_permission(file_perms, exec)

print("Can read: ")
print(can_read)
print("\n")

print("Can execute: ")
print(can_execute)
print("\n")
```

---

## 📚 ملاحظات التنفيذ / Implementation Notes

### الواجهة البرمجية / API Interface

```cpp
// C++ Implementation Interface
class BitwiseOperations {
public:
    // Basic operations
    static uint64_t bitwiseAnd(uint64_t a, uint64_t b);
    static uint64_t bitwiseOr(uint64_t a, uint64_t b);
    static uint64_t bitwiseXor(uint64_t a, uint64_t b);
    static uint64_t bitwiseNot(uint64_t a);
    
    // Shift operations
    static uint64_t leftShift(uint64_t value, uint32_t count);
    static uint64_t rightShiftLogical(uint64_t value, uint32_t count);
    static int64_t rightShiftArithmetic(int64_t value, uint32_t count);
    
    // Bit manipulation
    static bool testBit(uint64_t value, uint32_t bit);
    static uint64_t setBit(uint64_t value, uint32_t bit);
    static uint64_t clearBit(uint64_t value, uint32_t bit);
    static uint64_t toggleBit(uint64_t value, uint32_t bit);
    
    // Advanced operations
    static uint32_t countSetBits(uint64_t value);
    static int32_t findFirstSetBit(uint64_t value);
    static uint64_t reverseBits(uint64_t value, uint32_t bits);
    static uint64_t rotateLeft(uint64_t value, uint32_t count, uint32_t bits);
    static uint64_t rotateRight(uint64_t value, uint32_t count, uint32_t bits);
};
```

---

## ✅ قائمة التحقق / Checklist

- [x] عملية AND الثنائية
- [x] عملية OR الثنائية
- [x] عملية XOR الثنائية
- [x] عملية NOT الثنائية
- [x] الإزاحة لليسار
- [x] الإزاحة لليمين
- [x] دوران البتات
- [x] عد البتات المفعلة
- [x] أقنعة الصلاحيات
- [x] أمثلة متقدمة

---

## 🔗 روابط ذات صلة / Related Links

- [Stage 1 Plan](../../system_build_plan/01_stage1_low_level.md)
- [Bitwise Implementation](../../../include/low_level/bitwise_ops.h)
- [Pointer Rules](01_pointers.md)
- [Fixed Size Types](06_fixed_size_types.md)

---

**آخر تحديث / Last Update:** 2 ديسمبر 2025  
**المؤلف / Author:** فريق تطوير لغة ص / Sad Language Development Team
