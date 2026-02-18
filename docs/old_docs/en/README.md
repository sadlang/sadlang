# 📚 Sad Language (لغة ص) Documentation

## Welcome to Arabic Programming! 🌟

**Sad Language (ص)** is a modern Arabic programming language designed to be easy to learn yet powerful. Whether you're a beginner who has never written a line of code, or an experienced developer looking for a new experience, Sad has something for you.

---

## 🎯 Why Sad?

### For Beginners
- **Program in Arabic**: Write code in your native language!
- **Simple syntax**: No confusing curly braces `{}`, blocks end with `نهاية`
- **Clear error messages**: Understand what went wrong in Arabic
- **Many examples**: Dozens of ready-to-use examples to learn from

### For Professionals
- **Strong type system**: Smart type inference with strict type checking
- **Network support**: TCP, UDP, WebSocket, HTTP
- **Concurrent programming**: goroutines, channels, async/await
- **Modern patterns**: Pattern matching, generators, decorators

---

## 📖 Table of Contents

### 🟢 Beginner Level
1. [Getting Started](./01-getting-started.md) - First program in 5 minutes
2. [Language Basics](./02-basics.md) - Variables, types, operations
3. [Control Flow](./03-control-flow.md) - Conditions and loops
4. [Functions](./04-functions.md) - How to organize your code
5. [Arrays and Strings](./05-arrays-strings.md) - Working with data

### 🟡 Intermediate Level
6. [Object-Oriented Programming](./06-oop.md) - Classes and objects
7. [Error Handling](./07-error-handling.md) - try/catch
8. [Type System](./08-type-system.md) - Advanced types
9. [Pattern Matching](./09-pattern-matching.md) - match/case
10. [Files and I/O](./10-io.md) - File handling

### 🔴 Advanced Level
11. [Concurrency](./11-concurrency.md) - Parallel tasks
12. [Networking](./12-networking.md) - TCP/UDP/WebSocket
13. [Memory and Performance](./13-memory.md) - Performance optimization
14. [Project Structure](./14-projects.md) - Organizing large projects

### 📚 References
- [Keywords Reference](./reference/keywords.md)
- [Built-in Functions](./reference/builtin-functions.md)
- [Error Messages](./reference/error-messages.md)
- [Examples](./reference/examples.md)

---

## ⚡ Quick Example

```sad
# Hello World program
دالة رئيسية()
    اطبع("مرحباً بالعالم! 🌍")
    ارجع 0
نهاية
```

**English comments version:**
```sad
# Hello World program
دالة رئيسية()        # Main function
    اطبع("Hello World! 🌍")  # Print message
    ارجع 0                     # Return 0
نهاية                        # End
```

---

## 🚀 Getting Started

```bash
# Clone and build Sad
git clone https://github.com/sad-lang/sad
cd sad
cmake -B build
cmake --build build

# Run your first program
./build/bin/sad examples/test_hello.s
```

---

## 📝 Language Syntax Overview

### Basic Structure

```sad
# Comments start with #

# Main function (entry point)
دالة رئيسية()
    # Your code here
    ارجع 0
نهاية
```

### Variables

```sad
رقم age = 25              # Integer
عشري price = 19.99        # Float
نص name = "أحمد"          # String
منطقي active = صحيح       # Boolean (true)
```

### Control Flow

```sad
# If/else
إذا (condition)
    # code
وإلا_إذا (other_condition)
    # code
وإلا
    # code
نهاية

# While loop
بينما (condition)
    # code
نهاية

# For-each loop
لكل (item في collection)
    # code
نهاية

# Pattern matching
طابق value
    حالة 1: # do something
    حالة 2: # do something else
    حالة _: # default case
نهاية
```

### Functions

```sad
دالة add(رقم a، رقم b) -> رقم
    ارجع a + b
نهاية

دالة greet(نص name)
    اطبع("Hello " + name)
نهاية
```

### Classes

```sad
فئة Person
    نص name
    رقم age
    
    دالة أنشئ(نص n، رقم a)
        هذا.name = n
        هذا.age = a
    نهاية
    
    دالة introduce()
        اطبع("I am " + هذا.name)
    نهاية
نهاية
```

---

## 🔑 Key Arabic Keywords

| Arabic | English Equivalent | Description |
|--------|-------------------|-------------|
| `دالة` | function | Define a function |
| `نهاية` | end | End a block |
| `ارجع` / `إرجاع` | return | Return from function |
| `رقم` | int | Integer type |
| `عشري` | float | Float type |
| `نص` | string | String type |
| `منطقي` | bool | Boolean type |
| `مصفوفة` | array | Array type |
| `إذا` | if | Conditional |
| `وإلا_إذا` | else if | Alternative condition |
| `وإلا` | else | Default case |
| `بينما` | while | While loop |
| `لكل` | for each | For-each loop |
| `طابق` | match/switch | Pattern matching |
| `حالة` | case | Case in match |
| `فئة` | class | Class definition |
| `هذا` | this/self | Current object |
| `صحيح` | true | Boolean true |
| `خطأ` | false | Boolean false |
| `فارغ` | null | Null value |
| `اطبع` | print | Print function |

---

## 💡 Tips for Beginners

> **Don't be afraid of mistakes!** Every professional programmer made thousands of errors before becoming proficient. Errors are not failures; they're how we learn.
>
> Start with simple examples, try modifying them, and see what happens. This is the best way to learn!

---

## 📞 Help and Support

- 📖 [FAQ](./faq.md)
- 💬 [Sad Community](https://github.com/sad-lang/sad/discussions)
- 🐛 [Report an Issue](https://github.com/sad-lang/sad/issues)

---

<div align="center">

**Sad Language (لغة ص)** - Programming in Your Language 💚

</div>
