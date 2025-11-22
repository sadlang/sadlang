# دليل استخدام مفسر لغة ص
# S Language Interpreter User Guide

## المقدمة / Introduction

هذا هو دليل استخدام مفسر لغة ص البرمجية. مفسر لغة ص هو برنامج يقرأ ملفات البرامج المكتوبة بلغة ص (.s) ويقوم بتنفيذها.

This is the user guide for S Language Interpreter. S Language Interpreter is a program that reads S language program files (.s) and executes them.

## التثبيت / Installation

### المتطلبات / Requirements
- Windows 10 أو أحدث / Windows 10 or later
- ملف التنفيذ s_interpreter.exe في مجلد build/ / Executable s_interpreter.exe in build/ folder

### الحصول على المفسر / Getting the Interpreter
المفسر موجود في: `build/s_interpreter.exe`

The interpreter is located at: `build/s_interpreter.exe`

## الاستخدام / Usage

### الأوامر الأساسية / Basic Commands

#### تشغيل برنامج / Run a program
```bash
s_interpreter <program.s>
```

مثال / Example:
```bash
s_interpreter examples/basics/hello.s
```

#### عرض المساعدة / Show help
```bash
s_interpreter --help
```
أو / or:
```bash
s_interpreter -h
```

#### عرض الإصدار / Show version
```bash
s_interpreter --version
```
أو / or:
```bash
s_interpreter -v
```

## الأمثلة / Examples

المفسر يأتي مع عدة أمثلة توضيحية:

The interpreter comes with several examples:

### 1. مثال مرحباً بالعالم / Hello World Example
```bash
s_interpreter examples/basics/hello.s
```
هذا المثال يوضح كيفية طباعة النصوص.
This example demonstrates how to print text.

### 2. مثال المتغيرات / Variables Example
```bash
s_interpreter examples/basics/variables.s
```
يوضح استخدام أنواع البيانات المختلفة:
Demonstrates use of different data types:
- أرقام صحيحة / Integers
- أرقام عشرية / Floats
- نصوص / Strings
- قيم منطقية / Booleans

### 3. مثال الشروط / Conditions Example
```bash
s_interpreter examples/basics/conditions.s
```
يوضح العبارات الشرطية:
Demonstrates conditional statements:
- if / إذا
- else if / وإلا إذا
- else / وإلا

## كتابة برامج لغة ص / Writing S Language Programs

### هيكل البرنامج الأساسي / Basic Program Structure

```s
# هذا تعليق
# This is a comment

# التصريح عن متغير
# Declare a variable
رقم س = 10

# طباعة
# Print
اطبع("مرحباً بالعالم!")
```

### أنواع البيانات / Data Types

لغة ص تدعم الأنواع التالية:
S Language supports the following types:

- **رقم / number**: الأرقام الصحيحة والعشرية
- **نص / string**: النصوص
- **منطق / boolean**: true أو false

### المتغيرات / Variables

تعريف متغير:
```s
رقم س = 10
نص الاسم = "أحمد"
منطق النتيجة = صحيح
```

### العمليات الحسابية / Arithmetic Operations

```s
رقم أ = 10
رقم ب = 20
رقم ج = أ + ب    # جمع
رقم د = أ - ب    # طرح
رقم هـ = أ * ب   # ضرب
رقم و = أ / ب    # قسمة
```

### العبارات الشرطية / Conditional Statements

```s
رقم العمر = 25

إذا (العمر >= 18) {
    اطبع("أنت بالغ")
} وإلا {
    اطبع("أنت قاصر")
}
```

## استكشاف الأخطاء / Troubleshooting

### المشكلة: "Failed to open file"
**الحل**: تأكد من:
- اسم الملف صحيح
- الملف موجود في المسار المحدد
- امتداد الملف هو .s

### المشكلة: "Error in parsing"
**الحل**: تحقق من:
- صيغة البرنامج صحيحة
- جميع الأقواس والأقواس المعقوفة مقفولة
- لا توجد أحرف غير صحيحة

### المشكلة: الإخراج مشوه
**الحل**: قد يكون هناك مشكلة في ترميز الملف. تأكد من أن الملف مرمز ب UTF-8.

## الميزات المدعومة / Supported Features

✅ المتغيرات / Variables
✅ أنواع البيانات الأساسية / Basic data types
✅ العمليات الحسابية / Arithmetic operations
✅ العمليات المنطقية / Logical operations
✅ العبارات الشرطية / Conditional statements
✅ الحلقات (في التطوير) / Loops (in development)
✅ الدوال (في التطوير) / Functions (in development)

## المعلومات الإضافية / Additional Information

- الإصدار / Version: 1.1.0
- لغة التطوير / Development Language: C++17
- منصات مدعومة / Supported Platforms: Windows

## الدعم والملاحظات / Support and Feedback

لأي أسئلة أو ملاحظات، يرجى الاطلاع على الوثائق في مجلد docs/

For questions or feedback, please refer to documentation in docs/ folder.

---

شكراً لاستخدام مفسر لغة ص!
Thank you for using S Language Interpreter!
