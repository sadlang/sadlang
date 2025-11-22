# مفسر لغة ص / S Language Interpreter 🚀

**الإصدار / Version:** 1.1.0  
**الحالة / Status:** ✅ جاهز للاستخدام / Ready for Use  
**التاريخ / Date:** 22 نوفمبر 2025 | November 22, 2025

---

## نظرة عامة / Overview

**لغة ص** هي لغة برمجية حديثة مصممة للمطورين الناطقين بالعربية. توفر سيناتكس سهل وقريب من اللغة الطبيعية مع دعم كامل للعربية.

**S Language** is a modern programming language designed for Arabic-speaking developers. It provides easy syntax close to natural language with full Arabic support.

---

## المميزات الرئيسية / Key Features

✅ **دعم كامل للعربية** - Full Arabic Support  
✅ **سيناتكس سهل** - Easy Syntax  
✅ **أنواع بيانات متعددة** - Multiple Data Types  
✅ **عمليات حسابية ومنطقية** - Arithmetic & Logical Operations  
✅ **شروط وحلقات** - Conditions & Loops  
✅ **دوال وفئات** - Functions & Classes (في التطوير)  
✅ **معالجة الأخطاء** - Error Handling

---

## التثبيت السريع / Quick Start

### 1. الحصول على المفسر / Get the Interpreter
```bash
# الملف التنفيذي موجود في:
# Executable is located at:
build/s_interpreter.exe
```

### 2. كتابة برنامج / Write a Program
```s
# hello.s
اطبع("مرحباً بالعالم!")
```

### 3. تشغيل البرنامج / Run the Program
```bash
build/s_interpreter.exe hello.s
```

---

## الأمثلة / Examples

### مثال 1: طباعة نص
```s
اطبع("مرحباً بالعالم!")
```

### مثال 2: المتغيرات
```s
رقم س = 10
نص الاسم = "أحمد"
اطبع("اسمي " + الاسم)
```

### مثال 3: الشروط
```s
رقم العمر = 25

إذا (العمر >= 18) {
    اطبع("أنت بالغ")
} وإلا {
    اطبع("أنت قاصر")
}
```

### مثال 4: الحلقات
```s
لـ (رقم ك = 1; ك <= 5; ك = ك + 1) {
    اطبع(ك)
}
```

---

## الأوامر / Commands

```bash
# تشغيل برنامج
s_interpreter program.s

# عرض الإصدار
s_interpreter --version

# عرض المساعدة
s_interpreter --help
```

---

## البناء من المصدر / Build from Source

### المتطلبات / Requirements
- Windows 10+
- GCC مع دعم C++17 / GCC with C++17 support

### خطوات البناء / Build Steps
```bash
cd C:\s\s_language

g++ -std=c++17 -O2 -I include ^
  src/main.cpp ^
  src/lexer/*.cpp ^
  src/parser/parser_core.cpp ^
  src/parser/parser_core_impl.cpp ^
  src/parser/parser_core_helpers.cpp ^
  src/parser/ast/*.cpp ^
  src/interpreter/core/*.cpp ^
  src/interpreter/visitors/*.cpp ^
  src/data/types/*.cpp ^
  src/data/managers/*.cpp ^
  src/data/scope/*.cpp ^
  src/utils/*.cpp ^
  src/errors/*.cpp ^
  -o build/s_interpreter.exe
```

---

## هيكل المشروع / Project Structure

```
s_language/
├── build/
│   └── s_interpreter.exe          # المفسر التنفيذي
├── src/
│   ├── main.cpp                   # نقطة الدخول الرئيسية
│   ├── lexer/                     # المحلل المعجمي
│   ├── parser/                    # المحلل النحوي
│   ├── interpreter/               # المفسر
│   ├── data/                      # طبقة البيانات
│   └── ...
├── examples/
│   └── basics/
│       ├── hello.s                # برنامج مرحباً
│       ├── variables.s            # أمثلة متغيرات
│       └── conditions.s           # أمثلة شروط
├── docs/
│   ├── USER_GUIDE.md              # دليل الاستخدام
│   ├── INTERPRETER_BUILD_AND_RUN_PLAN.md
│   └── ...
└── include/                       # ملفات الرأس
```

---

## مراحل تطور المشروع / Project Phases

### ✅ المرحلة 1: إعداد الأساسيات
- إنشاء هيكل المشروع
- بناء نظام الأنواع / Type System
- إنشاء محتويات المشروع

### ✅ المرحلة 2: تطوير المحلل المعجمي (Lexer)
- تحليل الرموز / Tokenization
- دعم العربية / Arabic Support
- معالجة الكلمات المفتاحية / Keywords

### ✅ المرحلة 3: تطوير المحلل النحوي (Parser)
- بناء شجرة AST
- معالجة الأولويات / Operator Precedence
- معالجة الأخطاء / Error Handling

### ✅ المرحلة 4: تطوير المفسر (Interpreter)
- تنفيذ البرامج / Program Execution
- إدارة المتغيرات / Variable Management
- تنفيذ الدوال / Function Execution

---

## الميزات المدعومة / Supported Features

### أنواع البيانات / Data Types
- `رقم` / `number` - الأرقام الصحيحة والعشرية
- `نص` / `string` - النصوص
- `منطق` / `boolean` - القيم المنطقية

### العمليات / Operations
- العمليات الحسابية / Arithmetic: `+`, `-`, `*`, `/`
- المقارنات / Comparisons: `>`, `<`, `>=`, `<=`, `==`, `!=`
- العمليات المنطقية / Logical: `و` (AND), `أو` (OR), `ليس` (NOT)

### التحكم بالبرنامج / Control Flow
- `إذا` / `if` - الشروط
- `وإلا إذا` / `else if` - شروط إضافية
- `وإلا` / `else` - الحالة الافتراضية
- `لـ` / `for` - حلقات (في التطوير)
- `بينما` / `while` - حلقات (في التطوير)

### الدوال / Functions (قيد التطوير)
```s
دالة اجمع(رقم أ, رقم ب) {
    أرجع أ + ب
}

رقم النتيجة = اجمع(5, 3)
```

---

## استكشاف الأخطاء / Troubleshooting

### الخطأ: "Failed to open file"
**الحل**: تأكد من:
- اسم الملف صحيح
- الملف موجود في المسار
- امتداد الملف `.s`

### الخطأ: "Error in parsing"
**الحل**: تحقق من:
- صيغة البرنامج صحيحة
- جميع الأقواس مقفولة
- لا توجد أحرف غير صحيحة

### الملف لم يتم تنفيذه
**الحل**:
- تحقق من صيغة البرنامج
- استخدم أمثلة بسيطة للاختبار
- تحقق من رسالة الخطأ

---

## الملاحظات / Notes

- المشروع قيد التطوير المستمر
- الميزات قد تتغير أو تُحدّث
- الترميز: UTF-8 لدعم العربية
- متوافق مع Windows 10+

---

## المساهمون / Contributors

**فريق تطوير لغة ص / S Language Development Team**

---

## الترخيص / License

MIT License

---

## روابط مفيدة / Useful Links

- [دليل الاستخدام](docs/USER_GUIDE.md)
- [تعليمات البناء](BUILD_INSTRUCTIONS.md)
- [الخطة الكاملة](plans/00_MASTER_PLAN.md)

---

## الدعم / Support

لأي أسئلة أو مشاكل:
1. تحقق من `USER_GUIDE.md`
2. راجع الأمثلة في `examples/`
3. اطلع على الوثائق في `docs/`

---

## شكر وتقدير / Acknowledgments

شكراً لاستخدام مفسر لغة ص!

صُنع بـ ❤️ للمطورين العرب

**Made with ❤️ for Arabic developers**

---

**آخر تحديث / Last Updated:** 22 نوفمبر 2025

**حالة البناء / Build Status:** ✅ **PASSING**
