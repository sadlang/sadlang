# 🚀 Getting Started

## Welcome to Your Programming Journey!

Ready to write your first program? It won't take more than 5 minutes!

---

## 📦 Step 1: Installation

### On Windows

```powershell
# Open PowerShell and run these commands
git clone https://github.com/sad-lang/sad
cd sad
cmake -B build
cmake --build build --config Release
```

### On Linux/macOS

```bash
git clone https://github.com/sad-lang/sad
cd sad
cmake -B build
cmake --build build
```

### Verify Installation

```bash
./build/bin/sad --version
# Should show: لغة ص - Version X.X.X
```

---

## 👋 Step 2: Hello World!

### Create Your Program File

1. Open your favorite text editor (VS Code, Notepad++, or even Notepad)
2. Create a new file named `hello.s`
3. Write the following code:

```sad
# My first Sad program
دالة رئيسية()
    اطبع("مرحباً بالعالم! 🌍")
    اطبع("Hello World!")
    ارجع 0
نهاية
```

### Line-by-Line Explanation

| Line | Explanation |
|------|-------------|
| `# My first...` | Comment - computer ignores this, for documentation only |
| `دالة رئيسية()` | Main function - every program needs this entry point |
| `اطبع("...")` | Print text to the screen |
| `ارجع 0` | End the program successfully |
| `نهاية` | End of the function |

### Run the Program

```bash
./build/bin/sad hello.s
```

**Output:**
```
مرحباً بالعالم! 🌍
Hello World!
```

🎉 **Congratulations!** You've written your first program!

---

## 🔢 Step 3: Calculator Program

Let's write something more useful - a simple calculator:

```sad
# Simple calculator
دالة رئيسية()
    اطبع("=== Calculator ===\n")
    
    # Define numbers
    رقم a = 10
    رقم b = 5
    
    # Arithmetic operations
    اطبع("Addition: ")
    اطبع(a + b)  # prints: 15
    
    اطبع("\nSubtraction: ")
    اطبع(a - b)  # prints: 5
    
    اطبع("\nMultiplication: ")
    اطبع(a * b)  # prints: 50
    
    اطبع("\nDivision: ")
    اطبع(a / b)  # prints: 2
    
    ارجع 0
نهاية
```

---

## 🤔 Step 4: Making Decisions

Programs need to make decisions. Here's how:

```sad
# Grade evaluation program
دالة رئيسية()
    رقم grade = 85
    
    اطبع("Your grade: ")
    اطبع(grade)
    اطبع("\nRating: ")
    
    إذا (grade >= 90)
        اطبع("Excellent! 🌟")
    وإلا_إذا (grade >= 80)
        اطبع("Very Good! 👍")
    وإلا_إذا (grade >= 70)
        اطبع("Good")
    وإلا_إذا (grade >= 60)
        اطبع("Pass")
    وإلا
        اطبع("Fail 😢")
    نهاية
    
    ارجع 0
نهاية
```

### Understanding Conditions

```sad
إذا (condition)
    # executes if condition is true
وإلا_إذا (another_condition)
    # executes if this condition is true
وإلا
    # executes if all conditions are false
نهاية
```

**Important Note:** 
- We use `نهاية` to close blocks
- We don't use curly braces `{}` like other languages

---

## 🔄 Step 5: Loops

Often we need to repeat an operation. Here's how:

### While Loop

```sad
# Count from 1 to 5
دالة رئيسية()
    رقم counter = 1
    
    بينما (counter <= 5)
        اطبع(counter)
        اطبع(" ")
        counter = counter + 1
    نهاية
    
    # prints: 1 2 3 4 5
    ارجع 0
نهاية
```

### For-Each Loop

```sad
# Print array elements
دالة رئيسية()
    مصفوفة names = ["أحمد"، "سارة"، "محمد"، "فاطمة"]
    
    لكل (name في names)
        اطبع("Hello ")
        اطبع(name)
        اطبع("!\n")
    نهاية
    
    ارجع 0
نهاية
```

---

## ✅ Step 6: Practice Exercises

### Exercise 1: Personal Greeting
Write a program that prints your name three times.

<details>
<summary>💡 Solution</summary>

```sad
دالة رئيسية()
    نص my_name = "Ahmed"
    رقم counter = 1
    
    بينما (counter <= 3)
        اطبع("Hello ")
        اطبع(my_name)
        اطبع("!\n")
        counter = counter + 1
    نهاية
    
    ارجع 0
نهاية
```
</details>

### Exercise 2: Sum of Numbers
Write a program that calculates the sum of numbers from 1 to 10.

<details>
<summary>💡 Solution</summary>

```sad
دالة رئيسية()
    رقم sum = 0
    رقم i = 1
    
    بينما (i <= 10)
        sum = sum + i
        i = i + 1
    نهاية
    
    اطبع("Sum: ")
    اطبع(sum)  # prints: 55
    
    ارجع 0
نهاية
```
</details>

### Exercise 3: Even Numbers
Write a program that prints even numbers from 2 to 20.

<details>
<summary>💡 Solution</summary>

```sad
دالة رئيسية()
    رقم i = 2
    
    بينما (i <= 20)
        اطبع(i)
        اطبع(" ")
        i = i + 2
    نهاية
    
    ارجع 0
نهاية
```
</details>

---

## 🎯 Next Steps

Well done! 🎉 You've learned:
- ✅ How to write a simple program
- ✅ Variables and types
- ✅ Conditions (if/else)
- ✅ Loops (while/foreach)

**Next:** Read [Language Basics](./02-basics.md) to deepen your understanding!

---

## ❓ FAQ

### Q: Why `نهاية` and not `}`?
**A:** Sad is designed to be readable in Arabic. The word `نهاية` (meaning "end") is clearer than a strange bracket!

### Q: Do I have to write `دالة رئيسية()`?
**A:** Yes! This is the entry point of every program. Without it, the computer doesn't know where to start.

### Q: What's the difference between `ارجع` and `إرجاع`?
**A:** They're the same! You can use whichever you prefer.

### Q: Why `ارجع 0`?
**A:** The number `0` means "the program ended successfully". Any other number indicates an error.

---

<div align="center">

[→ Next: Language Basics](./02-basics.md)

</div>
