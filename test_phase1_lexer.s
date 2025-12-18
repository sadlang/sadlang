## اختبار ميزات المرحلة 1 - Lexer المتقدم
## Test Phase 1 Features - Advanced Lexer

# (AR) اختبار الأرقام بصيغ مختلفة
# (EN) Test numbers in different formats

# Binary numbers - أرقام ثنائية
رقم bin1 = 0b1010        # 10 in decimal
رقم bin2 = 0b11111111    # 255 in decimal

# Octal numbers - أرقام ثمانية
رقم oct1 = 0o17          # 15 in decimal
رقم oct2 = 0o377         # 255 in decimal

# Hexadecimal numbers - أرقام ست عشرية
رقم hex1 = 0xFF          # 255 in decimal
رقم hex2 = 0xDEADBEEF    # 3735928559 in decimal

# Arabic digits - أرقام عربية
رقم عربي = ١٢٣٤٥
عشري عربي_مختلط = ٣.١٤١٥٩

# (AR) اختبار escape sequences المتقدمة
# (EN) Test advanced escape sequences

# Unicode 16-bit: \uXXXX
نص unicode16 = "مرحبا \u0628\u0627\u0644\u0639\u0627\u0644\u0645"

# Unicode 32-bit: \UXXXXXXXX  
نص unicode32 = "\U0001F600"  # 😀 emoji

# Hex escape: \xHH
نص hex_escape = "\x41\x42\x43"  # ABC

# Octal escape: \0-377
نص octal_escape = "\101\102\103"  # ABC

# (AR) اختبار Raw strings - بدون تفسير حروف الهروب
# (EN) Test Raw strings - no escape interpretation

نص raw1 = r"C:\Users\محمد\Documents"
نص raw2 = r"regex pattern: \d+\.\d+"
نص raw3 = r"Line 1\nLine 2\tTab"  # \n و \t حرفية

# (AR) اختبار F-strings - النصوص المنسقة
# (EN) Test F-strings - formatted strings

نص name = "أحمد"
رقم age = 25
عشري pi = 3.14159

نص fstring1 = f"اسمي {name} وعمري {age} سنة"
نص fstring2 = f"قيمة pi تقريباً: {pi}"
نص fstring3 = f"الحساب: {2 + 3 * 4} = 14"
نص fstring4 = f"متداخل: {{قوس حرفي}} و {name}"

# (AR) اختبار Doc comments - التعليقات التوثيقية
# (EN) Test Doc comments - documentation comments

## هذه دالة لحساب المضروب
## This function calculates factorial
دالة factorial(n: رقم): رقم {
    إذا (n <= 1) {
        إرجاع 1
    }
    إرجاع n * factorial(n - 1)
}

#**
 * دالة متقدمة مع توثيق متعدد الأسطر
 * Advanced function with multi-line documentation
 * 
 * @param a - العدد الأول / First number
 * @param b - العدد الثاني / Second number
 * @return المجموع / The sum
 **#
دالة add(a: رقم, b: رقم): رقم {
    إرجاع a + b
}

# (AR) اختبار مدمج - جميع الميزات معاً
# (EN) Combined test - all features together

#** 
 * برنامج اختبار شامل
 * Comprehensive test program
 **#
دالة main() {
    ## طباعة الأرقام بصيغ مختلفة
    print(f"Binary: {bin1}, Octal: {oct1}, Hex: {hex1}")
    
    ## استخدام raw strings للمسارات
    نص path = r"C:\Projects\Sad\tests"
    print(f"Path: {path}")
    
    ## Unicode في F-strings
    print(f"Unicode: \u{0645}\u{0631}\u{062D}\u{0628}\u{0627}")
    
    ## الأرقام العربية
    print(f"Arabic: عربي = {عربي}")
}
