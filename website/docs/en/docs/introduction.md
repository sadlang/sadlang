---
title: Introduction to Sad Language
description: Overview of Sad — the modern Arabic programming language
---

# Introduction to Sad Language

**Sad (لغة ص)** is a modern, open-source Arabic programming language designed so developers can write code in their native Arabic language.

## Why Sad?

Programming in your native language leads to faster understanding and deeper expression. Sad removes the language barrier between Arabic developers and their code.

```sad
# مرحباً! This is real Arabic code
متغير رسالة = "أهلاً بلغة ص!"
اطبع_سطر(رسالة)
```

## Key Features

### Instant Interpreter
```bash
sad my-program.ص
```
Run immediately without compilation.

### Native Compiler (via LLVM)
```bash
sadc my-program.ص -o my-program
./my-program
```
High performance native code.

### Bilingual Code Support
```sad
# Mix Arabic and English freely
متغير myList = [1، 2، 3]
لكل item في myList
  اطبع(item)
نهاية
```

## Language Keywords

Sad uses Arabic keywords:

| Sad Keyword | English Equivalent |
|-------------|--------------------|
| `دالة` | function |
| `متغير` | var/let |
| `ثابت` | const |
| `إذا` | if |
| `وإلا` | else |
| `بينما` | while |
| `لكل ... في` | for each ... in |
| `ارجع` | return |
| `صنف` | class |
| `يرث` | extends |
| `باني` | constructor |
| `هذا` | this |
| `جديد` | new |
| `نهاية` | end (closes blocks) |
| `اطبع` | print (builtin) |
| `صحيح` / `خطأ` | true / false |
| `لاشيء` | null |

---

Next: [Installation →](/en/docs/getting-started)
