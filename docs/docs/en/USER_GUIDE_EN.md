# 📖 User Guide - Sad Programming Language
# دليل المستخدم - لغة ص

**Welcome to the world of Arabic programming! 🇸🇦**

---

## 📑 Table of Contents

1. [Introduction to Sad Language](#introduction-to-sad-language)
2. [Installation & Setup](#installation--setup)
3. [Your First Program](#your-first-program)
4. [Variables and Types](#variables-and-types)
5. [Arithmetic Operations](#arithmetic-operations)
6. [Conditional Statements](#conditional-statements)
7. [Loops](#loops)
8. [Arrays](#arrays)
9. [Functions](#functions)
10. [Object-Oriented Programming](#object-oriented-programming)
11. [Error Handling](#error-handling)
12. [Advanced Examples](#advanced-examples)

---

## 🌟 Introduction to Sad Language

**Sad Language** is the first modern and complete Arabic programming language. It aims to:

### ✨ Vision
- 🇸🇦 **Programming in Arabic**: Using Arabic keywords and syntax
- 🎓 **Easy to Learn**: Suitable for beginners and professionals
- 🛠️ **Modern Features**: Support for OOP and error handling
- 🌍 **Global Standards**: UTF-8 support and Arabic encoding

### 🎯 Why Sad Language?
```sad
# Instead of:
if (age >= 18) {
    print("Adult");
}

# Write in Arabic:
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
نهاية
```

---

## 💻 Installation & Setup

### System Requirements
- **Operating System**: Windows 10/11, Linux, macOS
- **Memory**: At least 512 MB RAM
- **Storage**: 100 MB free space
- **Processor**: Any modern processor

### Installation Steps

#### On Windows:
```bash
# 1. Download latest version
git clone https://github.com/sadlang/sad-language.git
cd sad-language

# 2. Build the project
cmake --build build --config Debug

# 3. Test installation
.\build\bin\Debug\sad.exe --version
```

#### On Linux/macOS:
```bash
# 1. Download the project
git clone https://github.com/sadlang/sad-language.git
cd sad-language

# 2. Build the project
mkdir build && cd build
cmake ..
make

# 3. Test installation
./sad --version
```

---

## 🚀 Your First Program

Let's write a "Hello World" program:

### Creating the File
Create a new file called `hello.s`:

```sad
# Hello World program
اطبع("Hello World!") ;
اطبع("Welcome to Sad Language") ;
```

### Running the Program
```bash
sad hello.s
```

### Expected Output:
```
Hello World!
Welcome to Sad Language
```

---

## 🔢 Variables and Types

### Basic Data Types

#### Integers (رقم)
```sad
رقم عمر = 25 ;          # age = 25
رقم السنة = 2025 ;      # year = 2025
رقم درجات = 95 ;        # grades = 95
```

#### Floating Point (عشري)
```sad
عشري الطول = 175.5 ;    # height = 175.5
عشري الوزن = 68.2 ;     # weight = 68.2
عشري الدرجة = 98.7 ;    # grade = 98.7
```

#### Strings (نص)
```sad
نص الاسم = "Ahmed Mohammed" ;     # name = "Ahmed Mohammed"
نص المدينة = "Riyadh" ;          # city = "Riyadh"
نص الرسالة = "Welcome" ;         # message = "Welcome"
```

#### Booleans (منطق)
```sad
منطق مُسجل = صحيح ;      # registered = true
منطق مُكتمل = خطأ ;       # completed = false
منطق نشط = صحيح ;        # active = true
```

### Comprehensive Example:
```sad
# Personal data
نص الاسم = "Sara Ahmed" ;       # name = "Sara Ahmed"
رقم العمر = 22 ;               # age = 22
عشري المعدل = 3.85 ;           # gpa = 3.85
منطق طالبة = صحيح ;            # student = true

# Print data
اطبع("Name: ", الاسم) ;
اطبع("Age: ", العمر, " years") ;
اطبع("GPA: ", المعدل) ;
```

---

## ⚡ Arithmetic Operations

### Basic Operations
```sad
رقم أ = 10 ;                    # a = 10
رقم ب = 3 ;                     # b = 3

رقم الجمع = أ + ب ;             # addition = a + b (13)
رقم الطرح = أ - ب ;             # subtraction = a - b (7)
رقم الضرب = أ * ب ;             # multiplication = a * b (30)
رقم القسمة = أ / ب ;            # division = a / b (3)
رقم الباقي = أ % ب ;            # remainder = a % b (1)

اطبع("Addition: ", الجمع) ;
اطبع("Division: ", القسمة) ;
```

### Comparison Operations
```sad
رقم درجة = 85 ;                 # grade = 85

منطق ممتاز = درجة >= 90 ;       # excellent = grade >= 90 (false)
منطق جيد = درجة >= 80 ;         # good = grade >= 80 (true)
منطق مقبول = درجة >= 60 ;       # acceptable = grade >= 60 (true)

اطبع("Excellent grade: ", ممتاز) ;
اطبع("Good grade: ", جيد) ;
```

### Increment and Decrement Operations
```sad
رقم عداد = 5 ;                  # counter = 5

عداد = عداد + 1 ;               # counter = counter + 1 (6)
عداد += 2 ;                     # counter += 2 (8)
عداد -= 3 ;                     # counter -= 3 (5)
عداد *= 2 ;                     # counter *= 2 (10)
```

---

## 🔀 Conditional Statements

### Simple If Statement
```sad
رقم العمر = 18 ;                # age = 18

إذا ( العمر >= 18 )              # if (age >= 18)
    اطبع("Welcome, you can vote!") ;
نهاية                           # end
```

### If-Else Statement
```sad
رقم الدرجة = 85 ;               # grade = 85

إذا ( الدرجة >= 90 )            # if (grade >= 90)
    اطبع("Excellent") ;
وإلا                            # else
    اطبع("Good") ;
نهاية                          # end
```

### Nested Conditional Statements
```sad
رقم الدرجة = 78 ;               # grade = 78

إذا ( الدرجة >= 90 )            # if (grade >= 90)
    اطبع("Excellent") ;
وإلا                            # else
    إذا ( الدرجة >= 80 )        # if (grade >= 80)
        اطبع("Very Good") ;
    وإلا                        # else
        إذا ( الدرجة >= 70 )    # if (grade >= 70)
            اطبع("Good") ;
        وإلا                    # else
            اطبع("Acceptable") ;
        نهاية                  # end
    نهاية                      # end
نهاية                          # end
```

### Complex Conditions
```sad
رقم العمر = 25 ;                # age = 25
منطق لديه_رخصة = صحيح ;         # has_license = true

إذا ( العمر >= 18 && لديه_رخصة )  # if (age >= 18 && has_license)
    اطبع("Can drive") ;
وإلا                            # else
    اطبع("Cannot drive") ;
نهاية                          # end
```

---

## 🔄 Loops

### While Loop (بينما)
```sad
رقم عداد = 1 ;                  # counter = 1

بينما ( عداد <= 5 )             # while (counter <= 5)
    اطبع("Number: ", عداد) ;
    عداد = عداد + 1 ;
نهاية                          # end

# Output:
# Number: 1
# Number: 2
# Number: 3
# Number: 4
# Number: 5
```

### Practical Example - Calculate Sum
```sad
رقم مجموع = 0 ;                 # sum = 0
رقم عداد = 1 ;                  # counter = 1

بينما ( عداد <= 10 )            # while (counter <= 10)
    مجموع = مجموع + عداد ;
    عداد = عداد + 1 ;
نهاية                          # end

اطبع("Sum from 1 to 10: ", مجموع) ;
# Output: 55
```

### For Each Loop (لكل) - Coming in future versions
```sad
# Will be supported soon
مصفوفة أرقام = [1, 2, 3, 4, 5] ;  # array numbers = [1, 2, 3, 4, 5]

لكل ( رقم في أرقام )             # for each (number in numbers)
    اطبع("Number: ", رقم) ;
نهاية                           # end
```

---

## 📦 Arrays

### Creating Arrays
```sad
# Number array
مصفوفة أرقام = [10, 20, 30, 40, 50] ;

# String array
مصفوفة أسماء = ["Ahmed", "Fatima", "Ali", "Zainab"] ;

# Mixed array
مصفوفة بيانات = [25, "Sara", صحيح, 3.14] ;
```

### Accessing Elements
```sad
مصفوفة درجات = [85, 92, 78, 95, 88] ;  # grades array

اطبع("First grade: ", درجات[0]) ;      # 85
اطبع("Third grade: ", درجات[2]) ;      # 78
اطبع("Last grade: ", درجات[4]) ;       # 88
```

### Modifying Elements
```sad
مصفوفة نقاط = [100, 200, 300] ;        # points array

نقاط[1] = 250 ;                        # modify second value
اطبع("Updated points: ", نقاط[1]) ;     # 250
```

### Practical Example - Calculate Average
```sad
مصفوفة درجات = [85, 90, 78, 92, 88] ;  # grades array
رقم مجموع = 0 ;                        # sum = 0
رقم عداد = 0 ;                         # counter = 0

بينما ( عداد < 5 )                     # while (counter < 5)
    مجموع = مجموع + درجات[عداد] ;
    عداد = عداد + 1 ;
نهاية                                  # end

عشري متوسط = مجموع / 5.0 ;             # average = sum / 5.0
اطبع("Average grade: ", متوسط) ;
```

---

## 🔧 Functions

### Simple Function Definition
```sad
دالة رحب()                             # function welcome()
    اطبع("Welcome to Sad Language!") ;
نهاية                                  # end

# Function call
رحب() ;
```

### Function with Parameters
```sad
دالة احسب_المساحة( رقم الطول ، رقم العرض )    # function calculate_area(length, width)
    رقم المساحة = الطول * العرض ;
    اطبع("Area: ", المساحة) ;
نهاية                                      # end

# Function call
احسب_المساحة(10, 5) ;                      # Area: 50
```

### Function with Return Value
```sad
دالة مربع( رقم العدد )                     # function square(number)
    أرجع العدد * العدد ;                    # return number * number
نهاية                                      # end

رقم النتيجة = مربع(7) ;                    # result = square(7)
اطبع("Square of 7 is: ", النتيجة) ;        # 49
```

### More Complex Example
```sad
دالة احسب_المعدل( مصفوفة درجات )           # function calculate_average(grades)
    رقم مجموع = 0 ;                        # sum = 0
    رقم عداد = 0 ;                         # counter = 0
    
    بينما ( عداد < طول(درجات) )            # while (counter < length(grades))
        مجموع = مجموع + درجات[عداد] ;
        عداد = عداد + 1 ;
    نهاية                                  # end
    
    أرجع مجموع / طول(درجات) ;             # return sum / length(grades)
نهاية                                      # end

مصفوفة درجات_الطالب = [85, 90, 78, 92] ;
عشري المعدل = احسب_المعدل(درجات_الطالب) ;
اطبع("Student average: ", المعدل) ;
```

---

## 🏗️ Object-Oriented Programming

### Class Definition
```sad
صنف طالب                                 # class Student
    # Properties
    نص الاسم ;                            # string name
    رقم العمر ;                           # int age
    عشري المعدل ;                         # float gpa
    
    # Constructor
    دالة طالب( نص اسم ، رقم عمر )          # function Student(name, age)
        الاسم = اسم ;
        العمر = عمر ;
        المعدل = 0.0 ;
    نهاية                                 # end
    
    # Method to display information
    دالة اعرض_المعلومات()                  # function display_info()
        اطبع("Name: ", الاسم) ;
        اطبع("Age: ", العمر) ;
        اطبع("GPA: ", المعدل) ;
    نهاية                                 # end
    
    # Method to update GPA
    دالة حدث_المعدل( عشري معدل_جديد )      # function update_gpa(new_gpa)
        المعدل = معدل_جديد ;
    نهاية                                 # end
نهاية                                     # end
```

### Using the Class
```sad
# Create new object
طالب أحمد = جديد طالب("Ahmed Mohammed", 20) ;

# Use methods
أحمد.حدث_المعدل(3.75) ;
أحمد.اعرض_المعلومات() ;
```

### Inheritance
```sad
صنف طالب_جامعي يرث طالب                  # class UniversityStudent inherits Student
    نص التخصص ;                           # string major
    
    دالة طالب_جامعي( نص اسم ، رقم عمر ، نص تخصص )  # constructor
        # Call parent constructor
        عام(اسم, عمر) ;
        التخصص = تخصص ;
    نهاية                                     # end
    
    دالة اعرض_التخصص()                        # function display_major()
        اطبع("Major: ", التخصص) ;
    نهاية                                     # end
نهاية                                         # end
```

---

## 🚨 Error Handling

### Basic Error Handling
```sad
حاول                                       # try
    رقم نتيجة = 10 / 0 ;                   # Error: division by zero
نهاية                                      # end
امسك ( استثناء )                           # catch (exception)
    اطبع("Error occurred: ", استثناء) ;
نهاية                                      # end
```

### Array Error Handling
```sad
مصفوفة أرقام = [1, 2, 3] ;                 # numbers array

حاول                                       # try
    اطبع(أرقام[10]) ;                      # Error: index out of bounds
نهاية                                      # end
امسك ( خطأ_فهرس )                         # catch (index_error)
    اطبع("Index out of bounds: ", خطأ_فهرس) ;
نهاية                                      # end
```

### Using Finally Block
```sad
حاول                                       # try
    # Code that might error
    رقم قيمة = عملية_معقدة() ;              # value = complex_operation()
نهاية                                      # end
امسك ( استثناء )                           # catch (exception)
    اطبع("Error handled") ;
نهاية                                      # end
أخيراً                                     # finally
    اطبع("This code always runs") ;
نهاية                                      # end
```

---

## 🏆 Advanced Examples

### Example 1: Simple Calculator
```sad
دالة حاسبة( رقم أ ، نص العملية ، رقم ب )    # function calculator(a, operation, b)
    إذا ( العملية == "+" )                 # if (operation == "+")
        أرجع أ + ب ;                       # return a + b
    وإلا                                   # else
        إذا ( العملية == "-" )             # if (operation == "-")
            أرجع أ - ب ;                   # return a - b
        وإلا                               # else
            إذا ( العملية == "*" )         # if (operation == "*")
                أرجع أ * ب ;               # return a * b
            وإلا                           # else
                إذا ( العملية == "/" )     # if (operation == "/")
                    إذا ( ب != 0 )         # if (b != 0)
                        أرجع أ / ب ;       # return a / b
                    وإلا                   # else
                        اطبع("Error: Cannot divide by zero") ;
                        أرجع 0 ;           # return 0
                    نهاية                 # end
                نهاية                     # end
            نهاية                         # end
        نهاية                             # end
    نهاية                                 # end
نهاية                                     # end

# Use calculator
رقم نتيجة1 = حاسبة(15, "+", 5) ;          # result1 = 20
رقم نتيجة2 = حاسبة(10, "*", 3) ;          # result2 = 30
رقم نتيجة3 = حاسبة(20, "/", 4) ;          # result3 = 5

اطبع("15 + 5 = ", نتيجة1) ;
اطبع("10 * 3 = ", نتيجة2) ;
اطبع("20 / 4 = ", نتيجة3) ;
```

### Example 2: Student Grade Management
```sad
صنف إدارة_الدرجات                         # class GradeManager
    مصفوفة الطلاب ;                        # array students
    
    دالة إدارة_الدرجات()                   # constructor
        الطلاب = [] ;                      # students = []
    نهاية                                 # end
    
    دالة أضف_طالب( نص الاسم ، مصفوفة الدرجات )  # function add_student(name, grades)
        مصفوفة بيانات_الطالب = [الاسم, الدرجات] ;
        # Add to array (will be available in future versions)
    نهاية                                     # end
    
    دالة احسب_متوسط_طالب( مصفوفة درجات )      # function calculate_student_average(grades)
        رقم مجموع = 0 ;                        # sum = 0
        رقم عداد = 0 ;                         # counter = 0
        
        بينما ( عداد < طول(درجات) )            # while (counter < length(grades))
            مجموع = مجموع + درجات[عداد] ;
            عداد = عداد + 1 ;
        نهاية                                  # end
        
        أرجع مجموع / طول(درجات) ;             # return sum / length(grades)
    نهاية                                     # end
نهاية                                         # end
```

---

## 🎯 Tips and Best Practices

### Writing Clear Code
```sad
# ✅ Good - Clear variable names
رقم عمر_الطالب = 20 ;                      # student_age = 20
نص اسم_المدرسة = "King Saud University" ;  # school_name = "King Saud University"

# ❌ Avoid - Unclear names
رقم x = 20 ;
نص n = "King Saud University" ;
```

### Using Comments
```sad
# This is a single line comment
رقم السعر = 100 ;                          # price = 100

/*
This is a
multi-line
comment
*/
نص المنتج = "Book" ;                       # product = "Book"
```

### Organizing Code
```sad
# ===== VARIABLES =====
رقم عدد_الطلاب = 30 ;                      # number_of_students = 30
عشري متوسط_الدرجات = 0.0 ;                # average_grades = 0.0

# ===== FUNCTIONS =====
دالة احسب_المتوسط()                        # function calculate_average()
    # Function code here
نهاية                                      # end

# ===== MAIN PROGRAM =====
اطبع("Program started") ;
```

---

## 🚀 Next Steps

### To Continue Learning:
1. 📖 **Read More**: [Detailed Syntax Guide](SYNTAX_GUIDE_EN.md)
2. 💻 **Practice**: [Interactive Examples](../examples/BASIC_EXAMPLES.md)
3. 🏗️ **Build Projects**: [Real Projects](../examples/PROJECTS.md)
4. 🤝 **Join Community**: [Discord](https://discord.gg/sadlang)

### For Advanced Developers:
- 🔧 [API Reference](../api/INTERPRETER_API.md)
- 🛠️ [Developer Guide](DEVELOPER_GUIDE_EN.md)
- 📚 [Standard Library](../api/STDLIB_API.md)

---

## 📞 Getting Help

### If You Encounter Issues:
1. 🔍 Check [FAQ](FAQ_EN.md)
2. 📖 Review [Troubleshooting Guide](TROUBLESHOOTING_EN.md)
3. 💬 Ask in [Discord Community](https://discord.gg/sadlang)
4. 📧 Email us: support@sadlang.org

---

**🎉 Congratulations! You've completed the basic user guide for Sad Language**

You're now ready to start programming in Arabic. Begin writing your first programs and enjoy coding in your native language!

---

*Last updated: November 28, 2025*  
*Sad Language - Version 1.0*