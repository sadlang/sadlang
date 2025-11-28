# 📝 Syntax Guide - Sad Programming Language
# دليل التركيب والنحو - لغة ص

**Comprehensive reference for language rules and programming syntax 📚**

---

## 📑 Table of Contents

1. [Basic Rules](#basic-rules)
2. [Reserved Keywords](#reserved-keywords)
3. [Data Types](#data-types)
4. [Operations](#operations)
5. [Control Flow](#control-flow)
6. [Functions](#functions)
7. [Object-Oriented Programming](#object-oriented-programming)
8. [Error Handling](#error-handling)
9. [Comments](#comments)
10. [Advanced Examples](#advanced-examples)

---

## 📐 Basic Rules

### File Structure
```sad
# File name must end with .s
# Example: my_program.s or برنامجي.s

# Program beginning
اطبع("Program started") ;

# Program code here
رقم متغير = 10 ;

# Program end
اطبع("Program ended") ;
```

### Syntax Rules
1. **Case Sensitivity**: The language is case-sensitive
2. **Semicolons**: Every statement must end with `;`
3. **Parentheses**: Use `( )` for parameters and `{ }` for blocks (optional)
4. **Whitespace**: Spaces and tabs don't matter except inside strings

```sad
# ✅ Correct
رقم عمر = 25 ;             # age = 25
اطبع("Age:", عمر) ;        # print("Age:", age)

# ✅ Also correct
رقم عمر=25;
اطبع( "Age:" , عمر );

# ❌ Error - missing semicolon
رقم عمر = 25
```

### Variable Naming
```sad
# ✅ Valid names
رقم العمر ;                 # age
نص الاسم_الأول ;            # first_name
عشري متوسط_الدرجات ;       # average_grades
منطق طالب_نشط ;            # active_student

# ✅ English is allowed
رقم age ;
نص firstName ;
عشري average_grade ;

# ❌ Invalid names
رقم 123متغير ;             # Can't start with number
نص اسم-خاطئ ;              # No hyphens allowed
رقم اسم متغير ;            # No spaces in names
```

---

## 🔤 Reserved Keywords

### Data Types
| Arabic Word | Meaning | Usage |
|-------------|---------|--------|
| `رقم` | Integer | `رقم العمر = 25 ;` |
| `عشري` | Float | `عشري الطول = 175.5 ;` |
| `نص` | String | `نص الاسم = "Ahmed" ;` |
| `منطق` | Boolean | `منطق صحيح = true ;` |
| `مصفوفة` | Array | `مصفوفة أرقام = [1,2,3] ;` |

### Boolean Values
| Arabic Word | Meaning | Usage |
|-------------|---------|--------|
| `صحيح` | true | `منطق نشط = صحيح ;` |
| `خطأ` | false | `منطق مُكتمل = خطأ ;` |

### Control Flow
| Arabic Word | Meaning | Usage |
|-------------|---------|--------|
| `إذا` | if | `إذا ( condition )` |
| `وإلا` | else | `وإلا` |
| `بينما` | while | `بينما ( condition )` |
| `لكل` | for each | `لكل ( item في array )` |
| `نهاية` | end | `نهاية` |

### Functions
| Arabic Word | Meaning | Usage |
|-------------|---------|--------|
| `دالة` | function | `دالة الاسم()` |
| `أرجع` | return | `أرجع القيمة ;` |
| `اطبع` | print | `اطبع("text") ;` |

### Object-Oriented Programming
| Arabic Word | Meaning | Usage |
|-------------|---------|--------|
| `صنف` | class | `صنف الطالب` |
| `يرث` | inherits | `صنف أ يرث ب` |
| `جديد` | new | `جديد الطالب()` |
| `هذا` | this | `هذا.الخاصية` |
| `عام` | public | `عام دالة()` |
| `خاص` | private | `خاص رقم السن ;` |

### Error Handling
| Arabic Word | Meaning | Usage |
|-------------|---------|--------|
| `حاول` | try | `حاول` |
| `امسك` | catch | `امسك ( exception )` |
| `أخيراً` | finally | `أخيراً` |
| `ارمي` | throw | `ارمي exception ;` |

---

## 🎯 Data Types

### Integers (رقم)
```sad
# Declaration without value
رقم العمر ;                 # int age;

# Declaration with value
رقم السنة = 2025 ;          # int year = 2025;

# Arithmetic operations
رقم أ = 10 ;                # int a = 10;
رقم ب = 5 ;                 # int b = 5;
رقم المجموع = أ + ب ;       # int sum = a + b; (15)
رقم الضرب = أ * ب ;         # int product = a * b; (50)

# Negative numbers
رقم درجة_الحرارة = -5 ;     # int temperature = -5;
```

### Floating Point (عشري)
```sad
# Floating point numbers
عشري الطول = 175.5 ;        # float height = 175.5;
عشري الوزن = 68.75 ;        # float weight = 68.75;

# Scientific notation
عشري قيمة_كبيرة = 1.5e6 ;   # float large_value = 1500000;
عشري قيمة_صغيرة = 2.3e-4 ; # float small_value = 0.00023;

# High precision
عشري النسبة = 3.14159265359 ; # float ratio = 3.14159265359;
```

### Strings (نص)
```sad
# Simple strings
نص الاسم = "Ahmed Mohammed" ;      # string name = "Ahmed Mohammed";
نص المدينة = "Riyadh" ;            # string city = "Riyadh";

# Multi-line strings
نص رسالة_طويلة = "Hello
This is a
multi-line string" ;

# Empty strings
نص فارغ = "" ;                     # string empty = "";

# Strings with special characters
نص مع_علامات = "He said: \"Hello\"" ;
نص مع_سطر_جديد = "Line 1\nLine 2" ;
```

### Booleans (منطق)
```sad
# Basic values
منطق نشط = صحيح ;           # bool active = true;
منطق مُكتمل = خطأ ;          # bool completed = false;

# Boolean operation results
منطق نتيجة1 = 5 > 3 ;        # bool result1 = true;
منطق نتيجة2 = 10 < 8 ;       # bool result2 = false;
منطق نتيجة3 = (5 == 5) ;     # bool result3 = true;

# Logical operations
منطق و = صحيح && خطأ ;       # bool and = false;
منطق أو = صحيح || خطأ ;      # bool or = true;
منطق ليس = !صحيح ;          # bool not = false;
```

### Arrays (مصفوفة)
```sad
# Number array
مصفوفة أرقام = [1, 2, 3, 4, 5] ;      # int[] numbers = [1, 2, 3, 4, 5];

# String array
مصفوفة أسماء = ["Ahmed", "Fatima", "Ali"] ; # string[] names = ["Ahmed", "Fatima", "Ali"];

# Mixed array
مصفوفة مختلطة = [25, "Sara", صحيح, 3.14] ; # mixed[] data = [25, "Sara", true, 3.14];

# Empty array
مصفوفة فارغة = [] ;                        # empty[] array = [];

# Nested array
مصفوفة متداخلة = [[1, 2], [3, 4], [5, 6]] ; # int[][] nested = [[1, 2], [3, 4], [5, 6]];
```

---

## ⚙️ Operations

### Arithmetic Operations
```sad
رقم أ = 15 ;                # int a = 15;
رقم ب = 4 ;                 # int b = 4;

# Basic operations
رقم جمع = أ + ب ;           # int add = a + b; (19)
رقم طرح = أ - ب ;           # int subtract = a - b; (11)
رقم ضرب = أ * ب ;           # int multiply = a * b; (60)
رقم قسمة = أ / ب ;          # int divide = a / b; (3 - integer division)
رقم باقي = أ % ب ;          # int remainder = a % b; (3)

# Floating point operations
عشري قسمة_عشرية = 15.0 / 4.0 ;  # float float_divide = 3.75;

# Compound assignment operations
أ += 5 ;                     # a += 5; (a = a + 5) -> 20
أ -= 3 ;                     # a -= 3; (a = a - 3) -> 17
أ *= 2 ;                     # a *= 2; (a = a * 2) -> 34
أ /= 2 ;                     # a /= 2; (a = a / 2) -> 17
```

### Comparison Operations
```sad
رقم x = 10 ;                # int x = 10;
رقم y = 20 ;                # int y = 20;

# Basic comparison operations
منطق مساوي = x == y ;       # bool equal = (x == y); // false
منطق غير_مساوي = x != y ;   # bool not_equal = (x != y); // true
منطق أكبر = x > y ;         # bool greater = (x > y); // false
منطق أصغر = x < y ;         # bool less = (x < y); // true
منطق أكبر_أو_مساوي = x >= y ; # bool greater_equal = (x >= y); // false
منطق أصغر_أو_مساوي = x <= y ; # bool less_equal = (x <= y); // true

# String comparison
نص اسم1 = "Ahmed" ;          # string name1 = "Ahmed";
نص اسم2 = "Ahmed" ;          # string name2 = "Ahmed";
منطق أسماء_متساوية = اسم1 == اسم2 ;  # bool names_equal = true;
```

### Logical Operations
```sad
منطق أ = صحيح ;             # bool a = true;
منطق ب = خطأ ;              # bool b = false;

# Logical operations
منطق و = أ && ب ;           # bool and = a && b; // false (AND)
منطق أو = أ || ب ;          # bool or = a || b; // true (OR)
منطق ليس_أ = !أ ;           # bool not_a = !a; // false (NOT)

# Complex operations
منطق معقد = (أ && ب) || (!ب && أ) ;  # bool complex = (a && b) || (!b && a); // true
```

### Array Operations
```sad
مصفوفة قائمة = [10, 20, 30] ;      # int[] list = [10, 20, 30];

# Element access
رقم أول = قائمة[0] ;              # int first = list[0]; // 10
رقم ثاني = قائمة[1] ;             # int second = list[1]; // 20

# Element modification
قائمة[2] = 35 ;                   # list[2] = 35; // [10, 20, 35]

# Array length (built-in function)
رقم الطول = طول(قائمة) ;          # int length = length(list); // 3
```

---

## 🔀 Control Flow

### Conditional Statements (إذا/وإلا)

#### Basic Structure
```sad
إذا ( condition )              # if (condition) {
    # code to execute when condition is true
نهاية                          # }
```

#### Practical Examples
```sad
رقم العمر = 18 ;               # int age = 18;

# Simple condition
إذا ( العمر >= 18 )            # if (age >= 18) {
    اطبع("Adult") ;            #     print("Adult");
نهاية                          # }

# If-else condition
إذا ( العمر >= 18 )            # if (age >= 18) {
    اطبع("Adult") ;            #     print("Adult");
وإلا                           # } else {
    اطبع("Minor") ;            #     print("Minor");
نهاية                          # }

# Nested conditions
رقم الدرجة = 85 ;              # int grade = 85;

إذا ( الدرجة >= 90 )           # if (grade >= 90) {
    اطبع("Excellent") ;        #     print("Excellent");
وإلا                           # } else {
    إذا ( الدرجة >= 80 )       #     if (grade >= 80) {
        اطبع("Very Good") ;    #         print("Very Good");
    وإلا                       #     } else {
        إذا ( الدرجة >= 70 )   #         if (grade >= 70) {
            اطبع("Good") ;     #             print("Good");
        وإلا                   #         } else {
            إذا ( الدرجة >= 60 ) #           if (grade >= 60) {
                اطبع("Pass") ; #               print("Pass");
            وإلا               #           } else {
                اطبع("Fail") ; #               print("Fail");
            نهاية             #           }
        نهاية                 #         }
    نهاية                     #     }
نهاية                         # }
```

#### Complex Conditions
```sad
رقم العمر = 25 ;               # int age = 25;
منطق لديه_رخصة = صحيح ;        # bool has_license = true;
منطق نظره_جيد = صحيح ;         # bool good_vision = true;

# AND condition
إذا ( العمر >= 18 && لديه_رخصة )  # if (age >= 18 && has_license) {
    اطبع("Can drive") ;           #     print("Can drive");
نهاية                            # }

# OR condition
إذا ( العمر < 18 || !لديه_رخصة )  # if (age < 18 || !has_license) {
    اطبع("Cannot drive") ;         #     print("Cannot drive");
نهاية                             # }

# Complex conditions
إذا ( (العمر >= 18 && لديه_رخصة) && نظره_جيد )  # if ((age >= 18 && has_license) && good_vision) {
    اطبع("Fully qualified to drive") ;          #     print("Fully qualified to drive");
نهاية                                           # }
```

### Loop Statements

#### While Loop (بينما)
```sad
# Basic structure
بينما ( condition )            # while (condition) {
    # repeating code            #     // repeating code
نهاية                          # }

# Practical example
رقم عداد = 1 ;                 # int counter = 1;
بينما ( عداد <= 5 )            # while (counter <= 5) {
    اطبع("Number:", عداد) ;     #     print("Number:", counter);
    عداد = عداد + 1 ;           #     counter = counter + 1;
نهاية                          # }
```

#### Advanced Loop Examples
```sad
# Calculate factorial
رقم عدد = 5 ;                  # int number = 5;
رقم مضروب = 1 ;                # int factorial = 1;
رقم مؤشر = 1 ;                 # int index = 1;

بينما ( مؤشر <= عدد )          # while (index <= number) {
    مضروب = مضروب * مؤشر ;      #     factorial = factorial * index;
    مؤشر = مؤشر + 1 ;           #     index = index + 1;
نهاية                          # }

اطبع("Factorial of", عدد, "is:", مضروب) ;  # print("Factorial of", number, "is:", factorial); // 120

# Array search
مصفوفة أرقام = [10, 25, 30, 45, 50] ;     # int[] numbers = [10, 25, 30, 45, 50];
رقم المطلوب = 30 ;                         # int target = 30;
رقم فهرس = 0 ;                            # int index = 0;
منطق وُجد = خطأ ;                         # bool found = false;

بينما ( فهرس < طول(أرقام) && !وُجد )      # while (index < length(numbers) && !found) {
    إذا ( أرقام[فهرس] == المطلوب )         #     if (numbers[index] == target) {
        وُجد = صحيح ;                      #         found = true;
        اطبع("Found at index:", فهرس) ;   #         print("Found at index:", index);
    نهاية                                 #     }
    فهرس = فهرس + 1 ;                     #     index = index + 1;
نهاية                                     # }

إذا ( !وُجد )                            # if (!found) {
    اطبع("Number not found") ;            #     print("Number not found");
نهاية                                     # }
```

---

## 🔧 Functions

### Function Definition

#### Simple Function without Parameters
```sad
دالة رحب()                            # function welcome() {
    اطبع("Welcome to Sad Language!") ; #     print("Welcome to Sad Language!");
    اطبع("Hello and welcome") ;        #     print("Hello and welcome");
نهاية                                  # }

# Function call
رحب() ;                                # welcome();
```

#### Function with Parameters
```sad
دالة احسب_المساحة( رقم الطول ، رقم العرض )    # function calculate_area(int length, int width) {
    رقم المساحة = الطول * العرض ;              #     int area = length * width;
    اطبع("Area =", المساحة) ;                #     print("Area =", area);
نهاية                                      # }

# Function call
احسب_المساحة(10, 5) ;                      # calculate_area(10, 5); // Area = 50
احسب_المساحة(7, 3) ;                       # calculate_area(7, 3);  // Area = 21
```

#### Function with Return Value
```sad
دالة مربع( رقم العدد )                     # function square(int number) {
    رقم النتيجة = العدد * العدد ;            #     int result = number * number;
    أرجع النتيجة ;                          #     return result;
نهاية                                      # }

# Using the returned value
رقم رقم7_مربع = مربع(7) ;                  # int square_of_7 = square(7);
اطبع("Square of 7 =", رقم7_مربع) ;         # print("Square of 7 =", square_of_7); // 49
```

#### Complex Functions
```sad
دالة احسب_المعدل( مصفوفة درجات )            # function calculate_average(int[] grades) {
    رقم مجموع = 0 ;                        #     int sum = 0;
    رقم عداد = 0 ;                         #     int counter = 0;
    
    # Check for empty array                 #     // Check for empty array
    إذا ( طول(درجات) == 0 )                #     if (length(grades) == 0) {
        أرجع 0.0 ;                         #         return 0.0;
    نهاية                                  #     }
    
    # Calculate sum                        #     // Calculate sum
    بينما ( عداد < طول(درجات) )            #     while (counter < length(grades)) {
        مجموع = مجموع + درجات[عداد] ;        #         sum = sum + grades[counter];
        عداد = عداد + 1 ;                   #         counter = counter + 1;
    نهاية                                  #     }
    
    # Calculate average                    #     // Calculate average
    عشري المعدل = مجموع / طول(درجات) ;     #     float average = sum / length(grades);
    أرجع المعدل ;                          #     return average;
نهاية                                      # }

# Using the function
مصفوفة درجات_الطالب = [85, 90, 78, 92, 88] ;  # int[] student_grades = [85, 90, 78, 92, 88];
عشري معدل_الطالب = احسب_المعدل(درجات_الطالب) ; # float student_average = calculate_average(student_grades);
اطبع("Student average:", معدل_الطالب) ;         # print("Student average:", student_average);
```

### Specialized Functions
```sad
# Function to check prime numbers
دالة هل_عدد_أولي( رقم العدد )              # function is_prime(int number) {
    إذا ( العدد < 2 )                      #     if (number < 2) {
        أرجع خطأ ;                          #         return false;
    نهاية                                  #     }
    
    رقم مقسوم = 2 ;                        #     int divisor = 2;
    بينما ( مقسوم * مقسوم <= العدد )        #     while (divisor * divisor <= number) {
        إذا ( العدد % مقسوم == 0 )          #         if (number % divisor == 0) {
            أرجع خطأ ;                      #             return false;
        نهاية                              #         }
        مقسوم = مقسوم + 1 ;                 #         divisor = divisor + 1;
    نهاية                                  #     }
    
    أرجع صحيح ;                            #     return true;
نهاية                                      # }

# Function to reverse text (future implementation)
دالة اعكس_النص( نص الكلمة )                # function reverse_text(string word) {
    # Will be developed in future versions    #     // Will be developed in future versions
    اطبع("Reverse of", الكلمة, "coming soon") ; #   print("Reverse of", word, "coming soon");
نهاية                                         # }
```

---

## 🏗️ Object-Oriented Programming

### Class Definition

#### Simple Class
```sad
صنف شخص                               # class Person {
    # Properties (Private by default)   #     // Properties (Private by default)
    نص الاسم ;                         #     string name;
    رقم العمر ;                        #     int age;
    
    # Constructor                       #     // Constructor
    دالة شخص( نص اسم ، رقم عمر )        #     function Person(string name, int age) {
        الاسم = اسم ;                   #         this.name = name;
        العمر = عمر ;                   #         this.age = age;
    نهاية                              #     }
    
    # Class methods                     #     // Class methods
    دالة اعرض_المعلومات()              #     function display_info() {
        اطبع("Name:", الاسم) ;          #         print("Name:", name);
        اطبع("Age:", العمر, "years") ;  #         print("Age:", age, "years");
    نهاية                              #     }
    
    دالة حدث_العمر( رقم عمر_جديد )     #     function update_age(int new_age) {
        العمر = عمر_جديد ;              #         age = new_age;
    نهاية                              #     }
    
    دالة احصل_على_الاسم()              #     function get_name() {
        أرجع الاسم ;                    #         return name;
    نهاية                              #     }
نهاية                                  # }
```

#### Using the Class
```sad
# Create new object
شخص أحمد = جديد شخص("Ahmed Mohammed", 25) ;  # Person ahmed = new Person("Ahmed Mohammed", 25);

# Call object methods
أحمد.اعرض_المعلومات() ;                      # ahmed.display_info();
أحمد.حدث_العمر(26) ;                         # ahmed.update_age(26);

نص اسم_أحمد = أحمد.احصل_على_الاسم() ;        # string ahmed_name = ahmed.get_name();
اطبع("Person's name:", اسم_أحمد) ;           # print("Person's name:", ahmed_name);
```

### Inheritance
```sad
# Base class
صنف مركبة                              # class Vehicle {
    نص النوع ;                          #     string type;
    رقم السرعة_القصوى ;                 #     int max_speed;
    
    دالة مركبة( نص نوع ، رقم سرعة )      #     function Vehicle(string type, int speed) {
        النوع = نوع ;                    #         this.type = type;
        السرعة_القصوى = سرعة ;           #         this.max_speed = speed;
    نهاية                               #     }
    
    دالة احرك()                         #     function move() {
        اطبع("Vehicle is moving") ;     #         print("Vehicle is moving");
    نهاية                               #     }
نهاية                                   # }

# Derived class
صنف سيارة يرث مركبة                    # class Car inherits Vehicle {
    رقم عدد_الأبواب ;                   #     int number_of_doors;
    
    دالة سيارة( نص نوع ، رقم سرعة ، رقم أبواب )  # function Car(string type, int speed, int doors) {
        # Call parent constructor       #         // Call parent constructor
        عام(نوع, سرعة) ;                #         super(type, speed);
        عدد_الأبواب = أبواب ;            #         this.number_of_doors = doors;
    نهاية                               #     }
    
    # Override method from base class  #     // Override method from base class
    دالة احرك()                         #     function move() {
        اطبع("Car is driving on road") ; #       print("Car is driving on road");
    نهاية                               #     }
    
    # New method specific to Car       #     // New method specific to Car
    دالة اطبع_المواصفات()               #     function print_specs() {
        اطبع("Vehicle type:", النوع) ;   #         print("Vehicle type:", type);
        اطبع("Max speed:", السرعة_القصوى, "km/h") ; # print("Max speed:", max_speed, "km/h");
        اطبع("Number of doors:", عدد_الأبواب) ;     #  print("Number of doors:", number_of_doors);
    نهاية                                        #     }
نهاية                                            # }

# Using inheritance
سيارة سيارتي = جديد سيارة("Sedan", 180, 4) ;      # Car my_car = new Car("Sedan", 180, 4);
سيارتي.احرك() ;                                  # my_car.move();
سيارتي.اطبع_المواصفات() ;                        # my_car.print_specs();
```

### Encapsulation
```sad
صنف حساب_بنكي                          # class BankAccount {
    # Private properties              #     // Private properties
    خاص رقم الرصيد ;                   #     private int balance;
    خاص نص رقم_الحساب ;                #     private string account_number;
    
    # Public constructor              #     // Public constructor
    عام دالة حساب_بنكي( نص رقم ، رقم رصيد_أولي )  # public function BankAccount(string number, int initial_balance) {
        رقم_الحساب = رقم ;              #         account_number = number;
        الرصيد = رصيد_أولي ;            #         balance = initial_balance;
    نهاية                              #     }
    
    # Public methods to access private properties  # // Public methods to access private properties
    عام دالة احصل_على_الرصيد()         #     public function get_balance() {
        أرجع الرصيد ;                   #         return balance;
    نهاية                              #     }
    
    عام دالة ايداع( رقم المبلغ )        #     public function deposit(int amount) {
        إذا ( المبلغ > 0 )              #         if (amount > 0) {
            الرصيد = الرصيد + المبلغ ;    #             balance = balance + amount;
            اطبع("Deposited", المبلغ, "SAR") ; #       print("Deposited", amount, "SAR");
        وإلا                           #         } else {
            اطبع("Deposit amount must be positive") ; # print("Deposit amount must be positive");
        نهاية                          #         }
    نهاية                              #     }
    
    عام دالة سحب( رقم المبلغ )          #     public function withdraw(int amount) {
        إذا ( المبلغ > 0 && المبلغ <= الرصيد )  # if (amount > 0 && amount <= balance) {
            الرصيد = الرصيد - المبلغ ;     #           balance = balance - amount;
            اطبع("Withdrawn", المبلغ, "SAR") ; #     print("Withdrawn", amount, "SAR");
        وإلا                            #         } else {
            اطبع("Cannot withdraw this amount") ; #     print("Cannot withdraw this amount");
        نهاية                           #         }
    نهاية                               #     }
نهاية                                   # }
```

---

## 🚨 Error Handling

### Basic Structure
```sad
حاول                                   # try {
    # Code that might error            #     // Code that might error
نهاية                                  # }
امسك ( exception_name )                # catch (exception_name) {
    # Handle the error                 #     // Handle the error
نهاية                                  # }
```

### Practical Examples
```sad
# Handle division by zero error
دالة قسمة_آمنة( رقم المقسوم ، رقم المقسوم_عليه )  # function safe_division(int dividend, int divisor) {
    حاول                                          # try {
        عشري النتيجة = المقسوم / المقسوم_عليه ;       #     float result = dividend / divisor;
        أرجع النتيجة ;                              #     return result;
    نهاية                                         # }
    امسك ( خطأ_قسمة )                             # catch (division_error) {
        اطبع("Error: Cannot divide by zero") ;    #     print("Error: Cannot divide by zero");
        أرجع 0.0 ;                               #     return 0.0;
    نهاية                                         # }
نهاية                                             # }

# Handle array errors
دالة الوصول_الآمن_للمصفوفة( مصفوفة القائمة ، رقم الفهرس )  # function safe_array_access(int[] list, int index) {
    حاول                                                  # try {
        رقم القيمة = القائمة[الفهرس] ;                     #     int value = list[index];
        أرجع القيمة ;                                     #     return value;
    نهاية                                                 # }
    امسك ( خطأ_فهرس )                                    # catch (index_error) {
        اطبع("Error: Index out of bounds") ;             #     print("Error: Index out of bounds");
        أرجع -1 ;                                        #     return -1;
    نهاية                                                 # }
نهاية                                                     # }
```

### Using Finally Block
```sad
دالة عملية_معقدة()                      # function complex_operation() {
    حاول                                # try {
        # Complex code that might fail   #     // Complex code that might fail
        اطبع("Starting complex operation") ; # print("Starting complex operation");
        رقم نتيجة = عملية_خطرة() ;        #     int result = risky_operation();
        اطبع("Operation succeeded") ;    #     print("Operation succeeded");
        أرجع نتيجة ;                     #     return result;
    نهاية                               # }
    امسك ( استثناء )                    # catch (exception) {
        اطبع("Operation failed:", استثناء) ; # print("Operation failed:", exception);
        أرجع -1 ;                       #     return -1;
    نهاية                               # }
    أخيراً                             # finally {
        اطبع("Cleaning up resources") ; #     print("Cleaning up resources");
        # Cleanup code always runs      #     // Cleanup code always runs
    نهاية                               # }
نهاية                                   # }
```

### Throwing Exceptions
```sad
دالة تحقق_من_العمر( رقم العمر )         # function validate_age(int age) {
    إذا ( العمر < 0 )                   #     if (age < 0) {
        ارمي "Age cannot be negative" ; #         throw "Age cannot be negative";
    نهاية                               #     }
    
    إذا ( العمر > 150 )                 #     if (age > 150) {
        ارمي "Age is unrealistic" ;     #         throw "Age is unrealistic";
    نهاية                               #     }
    
    اطبع("Age is valid:", العمر) ;      #     print("Age is valid:", age);
نهاية                                   # }

# Using the function with error handling
حاول                                   # try {
    تحقق_من_العمر(-5) ;                #     validate_age(-5);
نهاية                                  # }
امسك ( رسالة_خطأ )                     # catch (error_message) {
    اطبع("Age error:", رسالة_خطأ) ;    #     print("Age error:", error_message);
نهاية                                  # }
```

---

## 💬 Comments

### Single Line Comments
```sad
# This is a single line comment
رقم العمر = 25 ;                       # Comment at end of line

# Comments can be used to explain code
# or temporarily disable parts of code
# رقم متغير = 10 ;
```

### Multi-line Comments
```sad
/*
This is a multi-line comment
that can span across several lines
and is used for detailed explanations
*/

رقم قيمة = 100 ;

/*
Complex function for calculating something important
Parameters:
- البيانات: array of numbers to process
- الخيارات: processing settings
Returns: final result
*/
دالة معالجة_معقدة( مصفوفة البيانات )
    # Function code here
نهاية
```

### Best Practices for Comments
```sad
/*
===========================================
Library Management System
Version: 1.0
Developer: Ahmed Mohammed
Date: 2025-11-28
===========================================
*/

# ===== PROGRAM SETTINGS =====
رقم MAX_BOOKS = 1000 ;                     # Maximum number of books
نص LIBRARY_NAME = "King's Library" ;        # Library name

# ===== HELPER FUNCTIONS =====

# Function to validate book data
# Checks that title and author are not empty
دالة تحقق_من_بيانات_الكتاب( نص العنوان ، نص المؤلف )  # function validate_book_data(string title, string author) {
    # TODO: Add more validations                           #     // TODO: Add more validations
    إذا ( العنوان == "" )                                 #     if (title == "") {
        أرجع خطأ ;                                         #         return false;
    نهاية                                                  #     }
    
    إذا ( المؤلف == "" )                                  #     if (author == "") {
        أرجع خطأ ;                                         #         return false;
    نهاية                                                  #     }
    
    أرجع صحيح ;                                           #     return true;
نهاية                                                      # }
```

---

## 🔬 Advanced Examples

### Example 1: Student Management System
```sad
/*
Student Management System
Manages student information and grades
*/

صنف طالب                                  # class Student {
    خاص نص الاسم ;                        #     private string name;
    خاص رقم الهوية ;                      #     private int id;
    خاص مصفوفة الدرجات ;                   #     private int[] grades;
    
    عام دالة طالب( نص اسم ، رقم هوية )     #     public function Student(string name, int id) {
        الاسم = اسم ;                      #         this.name = name;
        الهوية = هوية ;                   #         this.id = id;
        الدرجات = [] ;                     #         this.grades = [];
    نهاية                                 #     }
    
    عام دالة أضف_درجة( رقم الدرجة )       #     public function add_grade(int grade) {
        إذا ( الدرجة >= 0 && الدرجة <= 100 ) #       if (grade >= 0 && grade <= 100) {
            # Add to array (will be available soon)  #         // Add to array (will be available soon)
            اطبع("Grade added:", الدرجة) ;  #             print("Grade added:", grade);
        وإلا                              #         } else {
            اطبع("Invalid grade") ;       #             print("Invalid grade");
        نهاية                             #         }
    نهاية                                 #     }
    
    عام دالة احسب_المعدل()                #     public function calculate_average() {
        إذا ( طول(الدرجات) == 0 )          #         if (length(grades) == 0) {
            أرجع 0.0 ;                    #             return 0.0;
        نهاية                             #         }
        
        رقم مجموع = 0 ;                   #         int sum = 0;
        رقم عداد = 0 ;                    #         int counter = 0;
        
        بينما ( عداد < طول(الدرجات) )     #         while (counter < length(grades)) {
            مجموع = مجموع + الدرجات[عداد] ; #             sum = sum + grades[counter];
            عداد = عداد + 1 ;              #             counter = counter + 1;
        نهاية                             #         }
        
        أرجع مجموع / طول(الدرجات) ;       #         return sum / length(grades);
    نهاية                                 #     }
    
    عام دالة اعرض_التقرير()               #     public function display_report() {
        اطبع("====== Student Report ======") ; #       print("====== Student Report ======");
        اطبع("Name:", الاسم) ;             #         print("Name:", name);
        اطبع("ID:", الهوية) ;              #         print("ID:", id);
        اطبع("Number of subjects:", طول(الدرجات)) ; # print("Number of subjects:", length(grades));
        اطبع("Average:", احسب_المعدل()) ;   #         print("Average:", calculate_average());
        
        عشري المعدل = احسب_المعدل() ;       #         float average = calculate_average();
        نص التقدير = "Not determined" ;    #         string grade = "Not determined";
        
        إذا ( المعدل >= 90 )               #         if (average >= 90) {
            التقدير = "Excellent" ;        #             grade = "Excellent";
        وإلا                              #         } else {
            إذا ( المعدل >= 80 )           #             if (average >= 80) {
                التقدير = "Very Good" ;    #                 grade = "Very Good";
            وإلا                          #             } else {
                إذا ( المعدل >= 70 )       #                 if (average >= 70) {
                    التقدير = "Good" ;     #                     grade = "Good";
                وإلا                      #                 } else {
                    إذا ( المعدل >= 60 )   #                     if (average >= 60) {
                        التقدير = "Pass" ; #                         grade = "Pass";
                    وإلا                  #                     } else {
                        التقدير = "Fail" ; #                         grade = "Fail";
                    نهاية                #                     }
                نهاية                    #                 }
            نهاية                        #             }
        نهاية                            #         }
        
        اطبع("Grade:", التقدير) ;          #         print("Grade:", grade);
        اطبع("==========================") ; #       print("==========================");
    نهاية                                 #     }
نهاية                                     # }

# Using the system
طالب سارة = جديد طالب("Sara Ahmed", 12345) ;  # Student sara = new Student("Sara Ahmed", 12345);
سارة.أضف_درجة(85) ;                          # sara.add_grade(85);
سارة.أضف_درجة(90) ;                          # sara.add_grade(90);
سارة.أضف_درجة(78) ;                          # sara.add_grade(78);
سارة.اعرض_التقرير() ;                        # sara.display_report();
```

### Example 2: Scientific Calculator
```sad
/*
Advanced Scientific Calculator
Supports basic and advanced operations
*/

صنف حاسبة_علمية                          # class ScientificCalculator {
    # Mathematical constants             #     // Mathematical constants
    عام عشري PI = 3.14159265359 ;       #     public float PI = 3.14159265359;
    عام عشري E = 2.71828182846 ;        #     public float E = 2.71828182846;
    
    # Basic operations                   #     // Basic operations
    عام دالة جمع( عشري أ ، عشري ب )     #     public function add(float a, float b) {
        أرجع أ + ب ;                     #         return a + b;
    نهاية                               #     }
    
    عام دالة طرح( عشري أ ، عشري ب )     #     public function subtract(float a, float b) {
        أرجع أ - ب ;                     #         return a - b;
    نهاية                               #     }
    
    عام دالة ضرب( عشري أ ، عشري ب )     #     public function multiply(float a, float b) {
        أرجع أ * ب ;                     #         return a * b;
    نهاية                               #     }
    
    عام دالة قسمة( عشري أ ، عشري ب )    #     public function divide(float a, float b) {
        حاول                            #         try {
            إذا ( ب == 0.0 )            #             if (b == 0.0) {
                ارمي "Division by zero not allowed" ; #   throw "Division by zero not allowed";
            نهاية                        #             }
            أرجع أ / ب ;                 #             return a / b;
        نهاية                           #         }
        امسك ( خطأ )                    #         catch (error) {
            اطبع("Division error:", خطأ) ; #          print("Division error:", error);
            أرجع 0.0 ;                  #             return 0.0;
        نهاية                           #         }
    نهاية                               #     }
    
    # Advanced operations                #     // Advanced operations
    عام دالة قوة( عشري الأساس ، رقم الأس )  #   public function power(float base, int exponent) {
        إذا ( الأس == 0 )               #         if (exponent == 0) {
            أرجع 1.0 ;                  #             return 1.0;
        نهاية                           #         }
        
        إذا ( الأس < 0 )                #         if (exponent < 0) {
            أرجع 1.0 / قوة(الأساس, -الأس) ; #        return 1.0 / power(base, -exponent);
        نهاية                           #         }
        
        عشري النتيجة = 1.0 ;             #         float result = 1.0;
        رقم عداد = 0 ;                  #         int counter = 0;
        
        بينما ( عداد < الأس )           #         while (counter < exponent) {
            النتيجة = النتيجة * الأساس ;   #             result = result * base;
            عداد = عداد + 1 ;            #             counter = counter + 1;
        نهاية                           #         }
        
        أرجع النتيجة ;                   #         return result;
    نهاية                               #     }
    
    عام دالة جذر_تربيعي( عشري العدد )   #     public function square_root(float number) {
        إذا ( العدد < 0 )               #         if (number < 0) {
            اطبع("Cannot calculate square root of negative number") ; #  print("Cannot calculate square root of negative number");
            أرجع -1.0 ;                 #             return -1.0;
        نهاية                           #         }
        
        إذا ( العدد == 0 )              #         if (number == 0) {
            أرجع 0.0 ;                  #             return 0.0;
        نهاية                           #         }
        
        # Newton's algorithm for square root  #     // Newton's algorithm for square root
        عشري تخمين = العدد / 2.0 ;        #         float guess = number / 2.0;
        رقم تكرارات = 0 ;               #         int iterations = 0;
        
        بينما ( تكرارات < 10 )          #         while (iterations < 10) { // Sufficient accuracy
            عشري تخمين_جديد = (تخمين + العدد / تخمين) / 2.0 ; # float new_guess = (guess + number / guess) / 2.0;
            تخمين = تخمين_جديد ;          #             guess = new_guess;
            تكرارات = تكرارات + 1 ;       #             iterations = iterations + 1;
        نهاية                           #         }
        
        أرجع تخمين ;                     #         return guess;
    نهاية                               #     }
    
    عام دالة مضروب( رقم العدد )         #     public function factorial(int number) {
        إذا ( العدد < 0 )               #         if (number < 0) {
            اطبع("Factorial undefined for negative numbers") ; # print("Factorial undefined for negative numbers");
            أرجع -1 ;                   #             return -1;
        نهاية                           #         }
        
        إذا ( العدد == 0 || العدد == 1 ) #        if (number == 0 || number == 1) {
            أرجع 1 ;                    #             return 1;
        نهاية                           #         }
        
        رقم النتيجة = 1 ;               #         int result = 1;
        رقم عداد = 2 ;                  #         int counter = 2;
        
        بينما ( عداد <= العدد )         #         while (counter <= number) {
            النتيجة = النتيجة * عداد ;     #             result = result * counter;
            عداد = عداد + 1 ;            #             counter = counter + 1;
        نهاية                           #         }
        
        أرجع النتيجة ;                   #         return result;
    نهاية                               #     }
    
    # Comprehensive calculation function #     // Comprehensive calculation function
    عام دالة احسب( نص العملية ، عشري أ ، عشري ب )  # public function calculate(string operation, float a, float b) {
        إذا ( العملية == "+" )          #         if (operation == "+") {
            أرجع جمع(أ, ب) ;            #             return add(a, b);
        وإلا                            #         } else {
            إذا ( العملية == "-" )      #             if (operation == "-") {
                أرجع طرح(أ, ب) ;        #                 return subtract(a, b);
            وإلا                        #             } else {
                إذا ( العملية == "*" )  #                 if (operation == "*") {
                    أرجع ضرب(أ, ب) ;    #                     return multiply(a, b);
                وإلا                    #                 } else {
                    إذا ( العملية == "/" ) #                   if (operation == "/") {
                        أرجع قسمة(أ, ب) ; #                      return divide(a, b);
                    وإلا                #                     } else {
                        اطبع("Unsupported operation:", العملية) ; #  print("Unsupported operation:", operation);
                        أرجع 0.0 ;      #                         return 0.0;
                    نهاية              #                     }
                نهاية                  #                 }
            نهاية                      #             }
        نهاية                          #         }
    نهاية                               #     }
نهاية                                   # }

# Using the scientific calculator
حاسبة_علمية الآلة = جديد حاسبة_علمية() ;    # ScientificCalculator calculator = new ScientificCalculator();

# Testing different operations
اطبع("10 + 5 =", الآلة.احسب("+", 10.0, 5.0)) ; # print("10 + 5 =", calculator.calculate("+", 10.0, 5.0));
اطبع("10 - 5 =", الآلة.احسب("-", 10.0, 5.0)) ; # print("10 - 5 =", calculator.calculate("-", 10.0, 5.0));
اطبع("10 * 5 =", الآلة.احسب("*", 10.0, 5.0)) ; # print("10 * 5 =", calculator.calculate("*", 10.0, 5.0));
اطبع("10 / 5 =", الآلة.احسب("/", 10.0, 5.0)) ; # print("10 / 5 =", calculator.calculate("/", 10.0, 5.0));

اطبع("2^8 =", الآلة.قوة(2.0, 8)) ;              # print("2^8 =", calculator.power(2.0, 8));
اطبع("√25 =", الآلة.جذر_تربيعي(25.0)) ;        # print("√25 =", calculator.square_root(25.0));
اطبع("5! =", الآلة.مضروب(5)) ;                  # print("5! =", calculator.factorial(5));
```

---

## 📋 Quick Syntax Reference

### Writing Basics
- ✅ Every statement ends with `;`
- ✅ Use `( )` for parameters and conditions
- ✅ Use `نهاية` keyword to close blocks
- ✅ Arabic keywords for basic commands

### Data Types
- `رقم` for integers
- `عشري` for floating-point numbers
- `نص` for strings
- `منطق` for booleans (`صحيح`/`خطأ`)
- `مصفوفة` for arrays

### Control Flow
- `إذا`...`وإلا`...`نهاية` for conditions
- `بينما`...`نهاية` for loops
- `حاول`...`امسك`...`أخيراً`...`نهاية` for error handling

### Object-Oriented Programming
- `صنف`...`نهاية` to define classes
- `دالة` to define functions and methods
- `يرث` for inheritance
- `جديد` to create objects

---

**🎯 This is a comprehensive guide to Sad Language syntax and structure**

Use this reference as a quick guide while programming. For more interactive examples, check the [User Guide](USER_GUIDE_EN.md) and [Practical Examples](../examples/).

---

*Last updated: November 28, 2025*  
*Sad Language - Version 1.0*