# Sad Programming Language (لغة ص)

> The first Arabic programming language with native RTL support, designed for Arabic speakers.

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [Language Basics](#language-basics)
5. [Data Types](#data-types)
6. [Control Flow](#control-flow)
7. [Functions](#functions)
8. [Classes](#classes)
9. [Error Handling](#error-handling)
10. [Standard Library](#standard-library)
11. [Compiler (sadc)](#compiler-sadc)
12. [IDE Support](#ide-support)

---

## Introduction

**Sad** (ص) is a modern programming language designed specifically for Arabic speakers. It features:

- **Full Arabic keywords**: Write code in your native language
- **RTL support**: Natural right-to-left text handling
- **Modern syntax**: Inspired by Python and JavaScript
- **Multiple backends**: Interpreter, VM, and native compiler (LLVM)
- **Cross-platform**: Windows, macOS, Linux, WASM

### Why Sad?

- Lower barrier to entry for Arabic-speaking beginners
- Educational tool for teaching programming concepts
- Production-ready for real applications
- Full Unicode/UTF-8 support

---

## Installation

### Windows

```powershell
# Using winget (recommended)
winget install sad-lang.sad

# Or download installer from releases
# https://github.com/sad-lang/sad/releases
```

### macOS

```bash
# Using Homebrew
brew install sad-lang/tap/sad
```

### Linux

```bash
# Using apt (Debian/Ubuntu)
sudo apt install sad-lang

# Using dnf (Fedora)
sudo dnf install sad-lang

# Build from source
git clone https://github.com/sad-lang/sad.git
cd sad
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

### Verify Installation

```bash
sad --version
# Sad Language v1.0.0
```

---

## Quick Start

### Hello World

Create a file `hello.ص`:

```sad
اطبع_سطر("مرحباً بالعالم!")
```

Run it:

```bash
sad hello.ص
# Output: مرحباً بالعالم!
```

### Variables

```sad
# Variable declaration
متغير الاسم = "أحمد"
متغير العمر = 25

# Constant
ثابت PI = 3.14159

# Output
اطبع_سطر("الاسم: " + الاسم)
اطبع_سطر("العمر: " + العمر)
```

---

## Language Basics

### Keywords

| Arabic | English Equivalent |
|--------|-------------------|
| `دالة` | function |
| `صنف` | class |
| `إذا` | if |
| `وإلا` | else |
| `بينما` | while |
| `لكل` | for each |
| `ارجع` | return |
| `متغير` | var |
| `ثابت` | const |
| `نهاية` | end |
| `صحيح` | true |
| `خطأ` | false |
| `لاشيء` | null/nil |

### Comments

```sad
# Single line comment

#*
  Multi-line
  comment
*#

## Documentation comment
```

### Operators

```sad
# Arithmetic
+ - * / % **

# Comparison
== != < > <= >=

# Logical
و  (and)
أو (or)
ليس (not)
&& || !

# Assignment
= += -= *= /=
```

---

## Data Types

### Primitives

```sad
# Numbers
متغير عدد_صحيح = 42
متغير عدد_عشري = 3.14

# Strings
متغير نص = "مرحباً"
متغير نص_طويل = """
نص
متعدد
الأسطر
"""

# Booleans
متغير منطقي = صحيح

# Null
متغير فارغ = لاشيء
```

### Collections

```sad
# Arrays
متغير قائمة = [1, 2, 3, 4, 5]
متغير أسماء = ["أحمد", "محمد", "علي"]

# Access
اطبع_سطر(قائمة[0])  # 1

# Maps/Objects
متغير شخص = {
    "الاسم": "أحمد",
    "العمر": 25
}

# Access
اطبع_سطر(شخص["الاسم"])  # أحمد
اطبع_سطر(شخص.الاسم)     # أحمد
```

---

## Control Flow

### If/Else

```sad
متغير درجة = 85

إذا (درجة >= 90)
    اطبع_سطر("ممتاز")
وإلا إذا (درجة >= 80)
    اطبع_سطر("جيد جداً")
وإلا إذا (درجة >= 70)
    اطبع_سطر("جيد")
وإلا
    اطبع_سطر("يحتاج تحسين")
نهاية
```

### While Loop

```sad
متغير عداد = 0
بينما (عداد < 5)
    اطبع_سطر(عداد)
    عداد = عداد + 1
نهاية
```

### For Each Loop

```sad
متغير أرقام = [1, 2, 3, 4, 5]
لكل رقم في أرقام
    اطبع_سطر(رقم)
نهاية

# With range
لكل ي في [1..10]
    اطبع_سطر(ي)
نهاية
```

### Pattern Matching

```sad
متغير قيمة = 2

طابق (قيمة)
    عندما 1:
        اطبع_سطر("واحد")
    عندما 2:
        اطبع_سطر("اثنان")
    عندما 3..10:
        اطبع_سطر("بين 3 و 10")
    افتراضي:
        اطبع_سطر("قيمة أخرى")
نهاية
```

---

## Functions

### Basic Functions

```sad
دالة مرحبا()
    اطبع_سطر("مرحباً!")
نهاية

دالة جمع(أ, ب)
    ارجع أ + ب
نهاية

# Call
مرحبا()
متغير نتيجة = جمع(3, 5)
اطبع_سطر(نتيجة)  # 8
```

### Lambda Functions

```sad
متغير مربع = لامدا(س) -> س * س

متغير أرقام = [1, 2, 3, 4, 5]
متغير مربعات = أرقام.استخدم(لامدا(س) -> س * س)
# [1, 4, 9, 16, 25]
```

---

## Classes

### Basic Class

```sad
صنف شخص
    باني(الاسم, العمر)
        هذا.الاسم = الاسم
        هذا.العمر = العمر
    نهاية
    
    دالة تعريف()
        اطبع_سطر("أنا " + هذا.الاسم + " وعمري " + هذا.العمر)
    نهاية
نهاية

متغير أحمد = جديد شخص("أحمد", 25)
أحمد.تعريف()
```

### Inheritance

```sad
صنف موظف يرث شخص
    باني(الاسم, العمر, الراتب)
        الأساس(الاسم, العمر)
        هذا.الراتب = الراتب
    نهاية
نهاية
```

---

## Error Handling

```sad
حاول
    متغير نتيجة = 10 / 0
امسك (خطأ)
    اطبع_سطر("حدث خطأ: " + خطأ.رسالة)
أخيراً
    اطبع_سطر("تم التنفيذ")
نهاية
```

---

## Compiler (sadc)

```bash
# Basic compilation
sadc program.ص -o program

# With optimizations
sadc program.ص -O2 -o program
```

---

## Resources

- [Official Website](https://sad-lang.org)
- [GitHub Repository](https://github.com/sad-lang/sad)
- [API Documentation](https://docs.sad-lang.org)

---

## License

MIT License
