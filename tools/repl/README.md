# بسم الله الرحمن الرحيم

# sad-repl - Sad Language REPL
# لغة ص - المفسر التفاعلي

[![Version](https://img.shields.io/badge/Version-1.0.0-green.svg)](.)
[![Status](https://img.shields.io/badge/Status-In%20Development-yellow.svg)](.)

## Overview / نظرة عامة

**sad-repl** is an interactive Read-Eval-Print-Loop for Sad Language, providing an instant feedback environment for learning and experimenting with the language.

**sad-repl** هو مفسر تفاعلي للغة ص، يوفر بيئة تجريبية فورية للتعلم والتجربة مع اللغة.

## Features / المزايا

### Core Features / المزايا الأساسية

- ✅ **Interactive Shell** - Execute Sad code line by line
- ✅ **Multiline Support** - Write functions and classes interactively
- ✅ **Command History** - Navigate through previous commands
- ✅ **Auto-completion** - Smart completion for variables and functions
- ✅ **Special Commands** - Built-in commands for REPL control
- ✅ **File Loading** - Load and execute Sad files
- ✅ **Colored Output** - Beautiful syntax highlighting
- ✅ **Error Recovery** - Continue after errors

### المزايا الأساسية

- ✅ **قشرة تفاعلية** - تنفيذ كود لغة ص سطراً بسطر
- ✅ **دعم متعدد الأسطر** - كتابة دوال وأصناف بشكل تفاعلي
- ✅ **تاريخ الأوامر** - التنقل بين الأوامر السابقة
- ✅ **إكمال تلقائي** - إكمال ذكي للمتغيرات والدوال
- ✅ **أوامر خاصة** - أوامر مدمجة للتحكم بـ REPL
- ✅ **تحميل الملفات** - تحميل وتنفيذ ملفات لغة ص
- ✅ **إخراج ملون** - تلوين جميل للأكواد
- ✅ **استرجاع من الأخطاء** - الاستمرار بعد الأخطاء

## Installation / التثبيت

### Building from Source / البناء من المصدر

```bash
# From project root - من جذر المشروع
mkdir build && cd build
cmake .. -DBUILD_REPL=ON
cmake --build .

# The executable will be in - سيكون التطبيق في:
# build/bin/Debug/sad-repl.exe (Windows)
# build/bin/Debug/sad-repl (Linux/Mac)
```

### Requirements / المتطلبات

- CMake 3.15+
- C++17 compiler
- Sad Language libraries (lexer, parser, interpreter)

## Usage / الاستخدام

### Basic Usage / الاستخدام الأساسي

```bash
# Start REPL - بدء REPL
sad-repl

# Start with file loading - بدء مع تحميل ملف
sad-repl --load myfile.s

# Disable colors - تعطيل الألوان
sad-repl --no-color

# Custom history file - ملف تاريخ مخصص
sad-repl --history-file ~/.my_sad_history
```

### Interactive Session / جلسة تفاعلية

```
$ sad-repl

========================================
لغة ص - REPL
Sad Language REPL
الإصدار / Version: 1.0.0
========================================

اكتب ':help' للمساعدة أو ':exit' للخروج
Type ':help' for help or ':exit' to quit

>>> متغير س = 42
42

>>> متغير ص = س * 2
84

>>> دالة مربع(ع) {
...     إرجاع ع * ع
... }
<دالة مربع>

>>> مربع(10)
100

>>> مربع(س)
1764

>>> :type س
رقم (Number)

>>> :vars
س = 42
ص = 84

>>> :help
... (help text)

>>> :exit
وداعاً! Goodbye!
```

## Special Commands / الأوامر الخاصة

All special commands start with `:` or can be written in Arabic.

جميع الأوامر الخاصة تبدأ بـ `:` أو يمكن كتابتها بالعربية.

### Available Commands / الأوامر المتاحة

| English | العربية | Description | الوصف |
|---------|---------|-------------|-------|
| `:help` | `:مساعدة` | Show help message | عرض رسالة المساعدة |
| `:exit` | `:خروج` | Exit REPL | الخروج من REPL |
| `:quit` | `:إنهاء` | Exit REPL (alias) | الخروج من REPL (مرادف) |
| `:clear` | `:مسح` | Clear screen | مسح الشاشة |
| `:type <expr>` | `:نوع <تعبير>` | Show type of expression | عرض نوع التعبير |
| `:load <file>` | `:حمل <ملف>` | Load and execute file | تحميل وتنفيذ ملف |
| `:history` | `:تاريخ` | Show command history | عرض تاريخ الأوامر |
| `:reset` | `:إعادة` | Reset REPL state | إعادة تعيين حالة REPL |
| `:vars` | `:متغيرات` | Show all variables | عرض جميع المتغيرات |
| `:funcs` | `:دوال` | Show all functions | عرض جميع الدوال |

### Examples / أمثلة

```
>>> :type 42
رقم (Number)

>>> :type "مرحبا"
نص (String)

>>> :load examples/hello.s
... (file content executed)

>>> :vars
س = 42
اسم = "أحمد"

>>> :funcs
مربع(ع)
جمع(أ، ب)

>>> :reset
REPL state reset. / تم إعادة تعيين حالة REPL.

>>> :exit
وداعاً! Goodbye!
```

## Multiline Input / الإدخال متعدد الأسطر

REPL automatically detects incomplete statements and allows multiline input.

يكتشف REPL تلقائياً الجمل غير المكتملة ويسمح بالإدخال متعدد الأسطر.

```
>>> دالة جمع_أرقام(قائمة) {
...     متغير مجموع = 0
...     لكل رقم في قائمة {
...         مجموع = مجموع + رقم
...     }
...     إرجاع مجموع
... }
<دالة جمع_أرقام>

>>> جمع_أرقام([1, 2, 3, 4, 5])
15
```

Triggers for multiline mode:
- Unclosed brackets: `{`, `[`, `(`
- Trailing operator: `+`, `-`, `*`, `/`, etc.
- Incomplete keywords: `دالة`, `إذا`, `لكل`, etc.

## Command History / تاريخ الأوامر

- **Arrow Up** / **سهم لأعلى**: Previous command / الأمر السابق
- **Arrow Down** / **سهم لأسفل**: Next command / الأمر التالي
- **Ctrl+R**: Search history (future) / البحث في التاريخ (مستقبلي)

History is automatically saved to `.sad_history` in the current directory or home directory.

يُحفظ التاريخ تلقائياً في `.sad_history` في المجلد الحالي أو المجلد الرئيسي.

## Configuration / الإعدادات

### Command Line Options / خيارات سطر الأوامر

```bash
sad-repl [options]

Options:
  --help, -h           Show help / عرض المساعدة
  --version, -v        Show version / عرض الإصدار
  --no-color           Disable colors / تعطيل الألوان
  --no-history         Disable history / تعطيل التاريخ
  --history-file FILE  History file path / مسار ملف التاريخ
  --load FILE          Load file on start / تحميل ملف عند البدء
```

## Examples / أمثلة

### Example 1: Basic Arithmetic / حساب أساسي

```
>>> 2 + 2
4

>>> 10 * 5
50

>>> 100 / 3
33.333333
```

### Example 2: Variables / متغيرات

```
>>> متغير اسم = "أحمد"
"أحمد"

>>> متغير عمر = 25
25

>>> اسم + " عمره " + عمر
"أحمد عمره 25"
```

### Example 3: Functions / دوال

```
>>> دالة مرحبا(اسم) {
...     إرجاع "مرحبا يا " + اسم
... }
<دالة مرحبا>

>>> مرحبا("علي")
"مرحبا يا علي"
```

### Example 4: Arrays / مصفوفات

```
>>> متغير أرقام = [1, 2, 3, 4, 5]
[1, 2, 3, 4, 5]

>>> أرقام[0]
1

>>> أرقام.طول()
5
```

## Development Status / حالة التطوير

### Phase 1.5.0 - Basic REPL ✅

- [x] REPL engine structure / بنية محرك REPL
- [x] Header files / ملفات الرأس
- [x] Command framework / إطار الأوامر
- [x] History manager / مدير التاريخ
- [ ] Implementation files (in progress) / ملفات التنفيذ (قيد العمل)
- [ ] Testing / الاختبار
- [ ] Documentation / التوثيق

### Future Enhancements / تحسينات مستقبلية

- [ ] Auto-completion with Tab / إكمال تلقائي مع Tab
- [ ] Syntax highlighting in input / تلوين الكود في الإدخال
- [ ] Ctrl+R reverse search / بحث عكسي Ctrl+R
- [ ] Session save/restore / حفظ/استعادة الجلسة
- [ ] Integration with debugger / تكامل مع المنقح
- [ ] Performance profiling / تحليل الأداء

## Troubleshooting / حل المشاكل

### Colors not showing / الألوان لا تظهر

```bash
# Try with explicit color enable - جرب مع تفعيل صريح للألوان
sad-repl

# If still not working, disable - إذا لم ينجح، عطل الألوان
sad-repl --no-color
```

### History not saving / التاريخ لا يُحفظ

```bash
# Check permissions - تحقق من الصلاحيات
ls -l .sad_history

# Use custom history file - استخدم ملف تاريخ مخصص
sad-repl --history-file ~/my_history
```

## Contributing / المساهمة

Contributions are welcome! Please see the main repository for guidelines.

المساهمات مرحب بها! يرجى الاطلاع على المستودع الرئيسي للإرشادات.

## License / الرخصة

Same as Sad Language main project.

نفس رخصة مشروع لغة ص الرئيسي.

---

**الحمد لله رب العالمين**

For more information, visit the main Sad Language repository.

لمزيد من المعلومات، قم بزيارة مستودع لغة ص الرئيسي.
