# ❓ FAQ - Sad Programming Language
# الأسئلة الشائعة - لغة ص

**Answers to the most frequently asked questions about Sad Language 🤔**

---

## 📑 Table of Contents

1. [Language Basics](#language-basics)
2. [Installation & Setup](#installation--setup)
3. [Common Errors](#common-errors)
4. [Performance & Optimization](#performance--optimization)
5. [Object-Oriented Programming](#object-oriented-programming)
6. [Standard Library](#standard-library)
7. [Development & Contributing](#development--contributing)

---

## 🌟 Language Basics

### ❓ What is Sad Language?
**Answer**: Sad Language is the first modern and complete Arabic programming language. It's designed to enable Arabic-speaking developers to program in their native language, with support for all modern features like object-oriented programming and error handling.

### ❓ Why is it called "Sad Language"?
**Answer**: The name "Sad" comes from the Arabic letter "ص" (Sad), which is the 14th letter of the Arabic alphabet and symbolizes "truth" (صدق) in Arabic programming expression.

### ❓ Is Sad Language suitable for beginners?
**Answer**: Absolutely! Sad Language is designed to be beginner-friendly:
- **Simple syntax**: Clear and understandable rules
- **Arabic error messages**: Better understanding of issues
- **Interactive examples**: Comprehensive learning resources
- **Supportive community**: Help from Arab developers

### ❓ What's the difference between Sad Language and other languages?
**Answer**:

| Feature | Sad Language | Traditional Languages |
|---------|-------------|----------------------|
| **Language** | Arabic | English |
| **Keywords** | `إذا`, `بينما`, `دالة` | `if`, `while`, `function` |
| **Error Messages** | Arabic | English |
| **Documentation** | Arabic/English | English only |
| **Community** | Arabic | Global |

### ❓ Can I use English names for variables?
**Answer**: Yes, you can use:
```sad
# Arabic names (preferred)
رقم العمر = 25 ;            # int age = 25;
نص الاسم = "أحمد" ;          # string name = "Ahmed";

# English names (acceptable)
رقم age = 25 ;
نص name = "Ahmed" ;

# Mixed (acceptable)
رقم student_age = 25 ;
نص اسم_الطالب = "أحمد" ;
```

### ❓ Does Sad Language support Unicode?
**Answer**: Yes, Sad Language fully supports UTF-8 and Unicode, which means:
- **Arabic text**: Perfect handling of Arabic characters
- **Arabic numerals**: Support for ١٢٣٤٥٦٧٨٩٠
- **Punctuation**: All Arabic punctuation marks
- **Direction**: RTL support in appropriate editors

---

## 💻 Installation & Setup

### ❓ What are the system requirements for Sad Language?
**Answer**:

**Minimum**:
- Windows 10, Linux, or macOS
- 512 MB RAM
- 100 MB storage space
- 1 GHz processor or higher

**Recommended**:
- 2 GB RAM or more
- 1 GB storage for projects
- Multi-core processor for large projects

### ❓ How do I install Sad Language on Windows?
**Answer**:
```bash
# 1. Download source code
git clone https://github.com/sadlang/sad-language.git
cd sad-language

# 2. Create build directory
mkdir build
cd build

# 3. Configure project
cmake ..

# 4. Build project
cmake --build . --config Release

# 5. Test installation
.\bin\Release\sad.exe --version
```

### ❓ The `sad` command doesn't work in Terminal, what's the solution?
**Answer**:

**On Windows**:
```bash
# Add directory to PATH
$env:PATH += ";C:\path\to\sad-language\build\bin\Release"

# Or use full path
C:\path\to\sad-language\build\bin\Release\sad.exe myfile.s
```

**On Linux/macOS**:
```bash
# Add to .bashrc or .zshrc
export PATH=$PATH:/path/to/sad-language/build/bin

# Reload shell
source ~/.bashrc
```

### ❓ Can I use Sad Language with VS Code?
**Answer**: Yes! You can set up VS Code to work with Sad Language:

**Syntax highlighting setup**:
1. Create `.vscode/settings.json` file
2. Add these settings:
```json
{
    "files.associations": {
        "*.s": "sad"
    },
    "editor.unicodeHighlight.allowedCharacters": {
        "ا": true,
        "أ": true,
        "إ": true,
        "ؤ": true,
        "ئ": true,
        "ة": true
    }
}
```

**Running files**:
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Run Sad Program",
            "type": "shell",
            "command": "sad",
            "args": ["${file}"],
            "group": "build",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            }
        }
    ]
}
```

---

## 🚨 Common Errors

### ❓ I get a "Syntax Error", what's the cause?
**Answer**: Common syntax errors:

**1. Missing semicolon**:
```sad
# ❌ Error
رقم العمر = 25
اطبع(العمر)

# ✅ Correct
رقم العمر = 25 ;
اطبع(العمر) ;
```

**2. Wrong parentheses**:
```sad
# ❌ Error
إذا العمر >= 18
    اطبع("Adult") ;
نهاية

# ✅ Correct
إذا ( العمر >= 18 )
    اطبع("Adult") ;
نهاية
```

**3. Unclosed strings**:
```sad
# ❌ Error
اطبع("Hello world) ;

# ✅ Correct
اطبع("Hello world") ;
```

### ❓ I get "Undefined Variable Error", how do I fix it?
**Answer**: This happens when you try to use a variable that hasn't been defined:

```sad
# ❌ Error - variable not defined
اطبع(الاسم) ;

# ✅ Correct - define then use
نص الاسم = "Ahmed" ;
اطبع(الاسم) ;
```

**Tips to avoid**:
- Make sure to define all variables before use
- Check spelling (Case Sensitive)
- Ensure correct scope

### ❓ "Type Mismatch Error" - what does it mean?
**Answer**: Occurs when mixing incompatible types:

```sad
# ❌ Error - can't add string and number directly
نص الرسالة = "Age: " + 25 ;

# ✅ Correct - convert number to string
نص الرسالة = "Age: " + إلى_نص(25) ;

# Or
رقم العمر = 25 ;
اطبع("Age:", العمر) ; # print handles different types
```

### ❓ "Array Index Out of Bounds" - how do I avoid it?
**Answer**: Occurs when trying to access a non-existent element:

```sad
مصفوفة أرقام = [1, 2, 3] ;

# ❌ Error - index 5 doesn't exist
اطبع(أرقام[5]) ;

# ✅ Correct - check length first
إذا ( 5 < طول(أرقام) )
    اطبع(أرقام[5]) ;
وإلا
    اطبع("Index out of bounds") ;
نهاية

# Or use safe function
دالة وصول_آمن( مصفوفة القائمة ، رقم الفهرس )
    إذا ( الفهرس >= 0 && الفهرس < طول(القائمة) )
        أرجع القائمة[الفهرس] ;
    وإلا
        أرجع فارغ ;
    نهاية
نهاية
```

### ❓ "Division by Zero Error" - how do I handle it?
**Answer**: Use checking or error handling:

```sad
# Method 1: Pre-checking
دالة قسمة_آمنة( رقم أ ، رقم ب )
    إذا ( ب == 0 )
        اطبع("Error: Cannot divide by zero") ;
        أرجع 0 ;
    نهاية
    أرجع أ / ب ;
نهاية

# Method 2: Error handling
حاول
    رقم النتيجة = 10 / 0 ;
نهاية
امسك ( خطأ_قسمة )
    اطبع("Handled division by zero error") ;
نهاية
```

---

## ⚡ Performance & Optimization

### ❓ My program is slow, how can I improve performance?
**Answer**: Optimization tips:

**1. Avoid unnecessary nested loops**:
```sad
# ❌ Slow
مصفوفة كبيرة = [1, 2, ..., 1000] ;
رقم عداد = 0 ;
بينما ( عداد < طول(كبيرة) )
    رقم عداد2 = 0 ;
    بينما ( عداد2 < طول(كبيرة) )
        # complex operation
        عداد2 = عداد2 + 1 ;
    نهاية
    عداد = عداد + 1 ;
نهاية

# ✅ Faster - avoid unnecessary complexity
```

**2. Use local variables**:
```sad
# ❌ Slow - calculating length every time
رقم عداد = 0 ;
بينما ( عداد < طول(المصفوفة_الكبيرة) )
    # processing
    عداد = عداد + 1 ;
نهاية

# ✅ Faster - cache the length
رقم الطول = طول(المصفوفة_الكبيرة) ;
رقم عداد = 0 ;
بينما ( عداد < الطول )
    # processing
    عداد = عداد + 1 ;
نهاية
```

**3. Avoid creating large unnecessary arrays**:
```sad
# ❌ Memory intensive
مصفوفة نتائج = [] ;
رقم عداد = 0 ;
بينما ( عداد < 1000000 )
    نتائج = أضف_عنصر(نتائج, عداد * 2) ;
    عداد = عداد + 1 ;
نهاية

# ✅ Better - direct processing
رقم عداد = 0 ;
بينما ( عداد < 1000000 )
    رقم النتيجة = عداد * 2 ;
    # use result directly
    عداد = عداد + 1 ;
نهاية
```

### ❓ Does using Arabic text affect performance?
**Answer**: No, it doesn't significantly affect performance:
- **Impact**: Less than 1% in most cases
- **Reason**: The interpreter converts Arabic keywords to internal tokens
- **Recommendation**: Use Arabic confidently for clarity and understanding

---

## 🏗️ Object-Oriented Programming

### ❓ How do I create a simple class?
**Answer**:
```sad
صنف شخص                           # class Person
    # Properties
    نص الاسم ;                    # string name;
    رقم العمر ;                   # int age;
    
    # Constructor
    دالة شخص( نص اسم ، رقم عمر )    # function Person(string name, int age)
        الاسم = اسم ;              # this.name = name;
        العمر = عمر ;              # this.age = age;
    نهاية                         # }
    
    # Class methods
    دالة اعرض_المعلومات()          # function displayInfo()
        اطبع("Name:", الاسم, "Age:", العمر) ;
    نهاية                         # }
نهاية                             # }

# Usage
شخص أحمد = جديد شخص("Ahmed Mohammed", 25) ;
أحمد.اعرض_المعلومات() ;
```

### ❓ How do I implement inheritance?
**Answer**:
```sad
# Base class
صنف حيوان                        # class Animal
    نص الاسم ;                   # string name;
    
    دالة حيوان( نص اسم )          # function Animal(string name)
        الاسم = اسم ;             # this.name = name;
    نهاية                        # }
    
    دالة أصدر_صوت()              # function makeSound()
        اطبع("Animal makes a sound") ;
    نهاية                        # }
نهاية                            # }

# Derived class
صنف قطة يرث حيوان               # class Cat inherits Animal
    دالة قطة( نص اسم )           # function Cat(string name)
        عام(اسم) ;               # super(name);
    نهاية                        # }
    
    # Override method
    دالة أصدر_صوت()              # function makeSound()
        اطبع("Meow!") ;          # print("Meow!");
    نهاية                        # }
نهاية                            # }

# Usage
قطة قطتي = جديد قطة("Whiskers") ;
قطتي.أصدر_صوت() ; # "Meow!"
```

### ❓ What's the difference between `عام` and `خاص`?
**Answer**:

```sad
صنف حساب_بنكي                    # class BankAccount {
    # Private properties - cannot be accessed from outside
    خاص رقم الرصيد ;              #     private int balance;
    خاص نص رقم_الحساب ;           #     private string accountNumber;
    
    # Public function - can be called from outside
    عام دالة ايداع( رقم المبلغ )   #     public function deposit(int amount) {
        الرصيد = الرصيد + المبلغ ;   #         balance = balance + amount;
    نهاية                         #     }
    
    عام دالة احصل_على_الرصيد()    #     public function getBalance() {
        أرجع الرصيد ;              #         return balance;
    نهاية                         #     }
نهاية                             # }

حساب_بنكي حسابي = جديد حساب_بنكي() ;
حسابي.ايداع(100) ; # ✅ Works - public function

# حسابي.الرصيد = 1000000 ; # ❌ Error - private property
```

---

## 📚 Standard Library

### ❓ What functions are available in the standard library?
**Answer**: The library contains:

**Core functions**:
- `اطبع()` - print values
- `طول()` - length of arrays and strings
- `نوع()` - get value type

**Math functions**:
- `مطلق()` - absolute value
- `جذر_تربيعي()` - square root
- `قوة()` - power
- `أصغر()` and `أكبر()` - min and max values

**String functions**:
- `طول_النص()` - string length
- `جزء_من_النص()` - substring extraction
- `ابحث_في_النص()` - search
- `استبدل_في_النص()` - replacement

### ❓ How do I read input from the user?
**Answer**:
```sad
# Read text
نص الاسم = اقرأ_سطر("What's your name? ") ;
اطبع("Hello", الاسم) ;

# Read number
رقم العمر = اقرأ_رقم("How old are you? ") ;
اطبع("You are", العمر, "years old") ;

# Handle input errors
حاول
    نص عمر_نص = اقرأ_سطر("Enter your age: ") ;
    رقم العمر = إلى_رقم(عمر_نص) ;
    اطبع("Age is valid:", العمر) ;
نهاية
امسك ( خطأ_تحويل )
    اطبع("Error: Must enter a valid number") ;
نهاية
```

### ❓ How do I generate random numbers?
**Answer**:
```sad
# Random float between 0.0 and 1.0
عشري احتمال = عشوائي() ;

# Random integer between 0 and 9
رقم رقم_عشوائي = عشوائي(10) ;

# Random integer between 1 and 6 (dice)
رقم النرد = عشوائي(1, 7) ;

# Choose random element from array
مصفوفة الألوان = ["Red", "Green", "Blue"] ;
رقم فهرس_عشوائي = عشوائي(طول(الألوان)) ;
نص لون_مختار = الألوان[فهرس_عشوائي] ;
```

---

## 🛠️ Development & Contributing

### ❓ How can I contribute to Sad Language development?
**Answer**: You can contribute in various ways:

**1. Report bugs**:
- Use GitHub Issues
- Detailed problem description
- Example demonstrating the error

**2. Suggest new features**:
- Share your ideas in the forum
- Write clear specifications
- Provide usage examples

**3. Improve documentation**:
- Add new examples
- Translate content
- Fix errors

**4. Write code**:
- Implement new features
- Improve performance
- Add tests

### ❓ Where can I find the Sad Language community?
**Answer**:
- **GitHub**: [github.com/sadlang/sad-language](https://github.com/sadlang/sad-language)
- **Discord**: [discord.gg/sadlang](https://discord.gg/sadlang)
- **Arabic Forum**: Under development
- **Twitter**: [@SadLanguage](https://twitter.com/SadLanguage)

### ❓ How do I add a new function to the standard library?
**Answer**: Process for adding a new function:

**1. Modify `builtin_functions.cpp` file**:
```cpp
// Add new function
Value builtin_my_function(const std::vector<Value>& args) {
    // Function implementation
    return Value(/* result */);
}

// Register function in interpreter
interpreter.defineBuiltin("دالتي", builtin_my_function);
```

**2. Update documentation**:
- Add function to standard library guide
- Write usage examples
- Add tests

**3. Test the function**:
```sad
# Test new function
رقم النتيجة = دالتي(10, 20) ;
اطبع("Result:", النتيجة) ;
```

### ❓ Can I create a VS Code extension for Sad Language?
**Answer**: Yes! Here are the basic steps:

**1. Create extension folder**:
```json
{
    "name": "sad-language-support",
    "displayName": "Sad Language Support",
    "description": "Support for Sad Programming Language",
    "version": "1.0.0",
    "engines": {
        "vscode": "^1.60.0"
    },
    "categories": ["Programming Languages"],
    "contributes": {
        "languages": [{
            "id": "sad",
            "aliases": ["Sad", "sad"],
            "extensions": [".s"],
            "configuration": "./language-configuration.json"
        }],
        "grammars": [{
            "language": "sad",
            "scopeName": "source.sad",
            "path": "./syntaxes/sad.tmLanguage.json"
        }]
    }
}
```

**2. Define grammar rules**:
```json
{
    "name": "Sad",
    "patterns": [
        {
            "match": "\\b(رقم|عشري|نص|منطق|مصفوفة)\\b",
            "name": "storage.type.sad"
        },
        {
            "match": "\\b(إذا|وإلا|بينما|دالة|صنف|أرجع)\\b", 
            "name": "keyword.control.sad"
        }
    ]
}
```

---

## 💡 Advanced Tips

### ❓ How do I write clean and readable code?
**Answer**:

**1. Use descriptive names**:
```sad
# ❌ Unclear
رقم x = 25 ;
رقم y = 30 ;

# ✅ Clear and descriptive
رقم عمر_الطالب = 25 ;
رقم عدد_الدرجات = 30 ;
```

**2. Write helpful comments**:
```sad
# Calculate student average based on grades
دالة احسب_المعدل( مصفوفة الدرجات )
    رقم مجموع_الدرجات = 0 ;
    
    # Sum all grades
    رقم عداد = 0 ;
    بينما ( عداد < طول(الدرجات) )
        مجموع_الدرجات += الدرجات[عداد] ;
        عداد += 1 ;
    نهاية
    
    # Calculate average
    أرجع مجموع_الدرجات / طول(الدرجات) ;
نهاية
```

**3. Split code into small functions**:
```sad
# ✅ Specialized and clear functions
دالة تحقق_من_صحة_الدرجة( رقم الدرجة )
    أرجع الدرجة >= 0 && الدرجة <= 100 ;
نهاية

دالة احسب_مجموع_الدرجات( مصفوفة الدرجات )
    رقم المجموع = 0 ;
    # ... sum calculation code
    أرجع المجموع ;
نهاية

دالة احسب_المعدل( مصفوفة الدرجات )
    رقم المجموع = احسب_مجموع_الدرجات(الدرجات) ;
    أرجع المجموع / طول(الدرجات) ;
نهاية
```

### ❓ How do I handle large files?
**Answer**: Split your project into multiple files:

**File: أصناف_الطلاب.s**
```sad
# Student class definitions
صنف طالب
    # ... class code
نهاية
```

**File: دوال_الحساب.s**
```sad
# Mathematical calculation functions
دالة احسب_المعدل( مصفوفة الدرجات )
    # ... function code
نهاية
```

**File: البرنامج_الرئيسي.s**
```sad
# Import other files (under development)
# Use everything together

# Create new student
طالب أحمد = جديد طالب("Ahmed Mohammed", 123) ;

# Calculate their average
عشري المعدل = احسب_المعدل(درجات_أحمد) ;
```

---

**🎯 This is a comprehensive guide to frequently asked questions about Sad Language**

Don't hesitate to ask additional questions in the [Discord community](https://discord.gg/sadlang) or [GitHub Issues](https://github.com/sadlang/sad-language/issues).

---

*Last updated: November 28, 2025*  
*Sad Language - Version 1.0*