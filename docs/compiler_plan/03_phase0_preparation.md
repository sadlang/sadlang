# المرحلة 0: التحضير
# Phase 0: Preparation

**المدة / Duration:** 1-2 أسابيع / weeks  
**الأولوية / Priority:** P0 (حرجة / Critical)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### (AR) العربية

مرحلة التحضير تركز على:
- توثيق المفسّر الحالي وفهم بنيته
- تصميم SIR (Simple Intermediate Representation)
- إعداد بنية المشروع والأدوات
- إنشاء مجموعة اختبارات أساسية

### (EN) English

Preparation phase focuses on:
- Documenting current interpreter and understanding its structure
- Designing SIR (Simple Intermediate Representation)
- Setting up project structure and tools
- Creating baseline test suite

---

## 🎯 الأهداف / Goals

### (AR) العربية

✅ **الأهداف الرئيسية:**
1. فهم كامل للمفسّر الحالي
2. تصميم SIR واضح وبسيط
3. بنية مشروع نظيفة وقابلة للتوسع
4. 20+ اختبار من Stage 1 كـ baseline

### (EN) English

Same goals in checklist format.

---

## 📝 المهام التفصيلية / Detailed Tasks

### 1. توثيق المفسّر الحالي / Document Current Interpreter

#### (AR) العربية

**المهام:**
- [ ] فحص كود المفسّر في `src/interpreter/`
- [ ] توثيق AST structure الحالية
- [ ] توثيق Symbol Table implementation
- [ ] توثيق Type System
- [ ] توثيق Builtin Functions
- [ ] توثيق Memory Management

**المخرجات:**
- وثيقة معمارية للمفسّر (architecture.md)
- مخططات UML للمكونات الرئيسية
- قائمة بجميع الميزات المدعومة

---

### 2. تصميم SIR / Design SIR

#### (AR) العربية

**مواصفات SIR:**

```
# SIR Instruction Set

# Arithmetic
add    %dest, %src1, %src2    # dest = src1 + src2
sub    %dest, %src1, %src2    # dest = src1 - src2
mul    %dest, %src1, %src2    # dest = src1 * src2
div    %dest, %src1, %src2    # dest = src1 / src2
mod    %dest, %src1, %src2    # dest = src1 % src2

# Bitwise
and    %dest, %src1, %src2
or     %dest, %src1, %src2
xor    %dest, %src1, %src2
not    %dest, %src
shl    %dest, %src, amount
shr    %dest, %src, amount

# Comparison
eq     %dest, %src1, %src2    # dest = (src1 == src2)
ne     %dest, %src1, %src2
lt     %dest, %src1, %src2
le     %dest, %src1, %src2
gt     %dest, %src1, %src2
ge     %dest, %src1, %src2

# Control Flow
jmp    label                   # Unconditional jump
br     %cond, true_label, false_label
call   func, %args...
ret    %value

# Memory
load   %dest, %addr           # Load from memory
store  %addr, %value          # Store to memory
alloc  %dest, size            # Allocate memory
free   %addr                  # Free memory

# Pointers (Stage 1)
addr_of   %dest, %var         # Get address
deref     %dest, %ptr         # Dereference
write_to  %ptr, %value        # Write via pointer

# Special
phi    %dest, [%val1, bb1], [%val2, bb2]  # SSA phi
```

**مثال SIR:**
```
# Source:
دالة جمع(رقم أ, رقم ب) -> رقم
    إرجاع أ + ب
نهاية

# SIR:
function @جمع(%أ: i64, %ب: i64) -> i64 {
entry:
    %t0 = add %أ, %ب
    ret %t0
}
```

**المهام:**
- [ ] كتابة مواصفات SIR كاملة
- [ ] تصميم بنية البيانات لـ SIR
- [ ] تنفيذ SIR Builder
- [ ] تنفيذ SIR Printer (للتصحيح)
- [ ] كتابة اختبارات لـ SIR

---

### 3. إعداد بنية المشروع / Setup Project Structure

#### (AR) العربية

**المهام:**
- [ ] إنشاء مجلدات compiler/, vm/, runtime/
- [ ] إعداد CMakeLists.txt
- [ ] إعداد CI/CD (GitHub Actions)
- [ ] إعداد نظام Testing (Google Test)
- [ ] إعداد Code Formatting (clang-format)
- [ ] إعداد Linting (clang-tidy)

**البنية:**
```
compiler/
  CMakeLists.txt
  frontend/
    CMakeLists.txt
    lexer.cpp
    parser.cpp
  ir/
    CMakeLists.txt
    sir.cpp
  backends/
    bytecode/
      CMakeLists.txt
    llvm/
      CMakeLists.txt
```

---

### 4. إنشاء مجموعة الاختبارات / Create Test Suite

#### (AR) العربية

**اختبارات Stage 1 (20 tests):**

```s
# test_pointers_01.s
دالة اختبار_مؤشرات()
    رقم x = 42
    مؤشر<رقم> ptr = عنوان_من(x)
    اطبع(قيمة_من(ptr))  # يجب طباعة 42
نهاية

# test_bitwise_01.s
دالة اختبار_bitwise()
    رقم a = 0b1010
    رقم b = 0b1100
    اطبع(a & b)  # 0b1000 = 8
    اطبع(a | b)  # 0b1110 = 14
نهاية

# test_memory_01.s
دالة اختبار_ذاكرة()
    مؤشر<رقم> ptr = خصص_ذاكرة(8)
    اكتب_في(ptr, 100)
    اطبع(قيمة_من(ptr))
    حرر(ptr)
نهاية

# ... 17 more tests
```

**المهام:**
- [ ] كتابة 20 اختبار من Stage 1
- [ ] إنشاء golden outputs لكل اختبار
- [ ] كتابة test runner
- [ ] إعداد regression testing

---

## 📊 معايير النجاح / Success Criteria

### (AR) العربية

✅ **المرحلة مكتملة عندما:**
- [ ] وثائق المفسّر الحالي كاملة
- [ ] SIR مُحدد ومُطبّق
- [ ] بنية المشروع جاهزة ومُختبرة
- [ ] 20 اختبار Stage 1 تعمل على المفسّر
- [ ] CI/CD يبني ويختبر تلقائياً
- [ ] الفريق يفهم البنية المقترحة

---

## 🔗 الملفات ذات الصلة / Related Files

- [البنية المعمارية](02_architecture.md)
- [قواعد Stage 1](../language_spec/rules/system_dev_support/)
- [أمثلة Stage 1](../../examples/)

---

**السابق / Previous:** [البنية المعمارية](02_architecture.md)  
**التالي / Next:** [المرحلة 1: Frontend](04_phase1_frontend.md)
