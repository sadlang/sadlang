# ======================================================================
# اختبار بسيط لميزات المحلل المعجمي - المرحلة 1
# Simple test for Lexer Phase 1 features
# ======================================================================

# (AR) اختبار الأرقام بصيغ مختلفة
# (EN) Test numbers in different formats

# Binary numbers - أرقام ثنائية
رقم bin1 = 0b1010
رقم bin2 = 0b11111111

# Octal numbers - أرقام ثمانية  
رقم oct1 = 0o17
رقم oct2 = 0o377

# Hexadecimal numbers - أرقام ست عشرية
رقم hex1 = 0xFF
رقم hex2 = 0xDEAD

# Arabic digits - أرقام عربية
رقم عربي = ١٢٣
عشري عربي_مختلط = ٣.١٤

# (AR) اختبار escape sequences المتقدمة
# (EN) Test advanced escape sequences

# Unicode 16-bit
نص unicode16 = "مرحبا \u0628\u0643"

# Hex escape
نص hex_escape = "\x41\x42\x43"

# Octal escape  
نص octal_escape = "\101\102\103"

# (AR) اختبار Raw strings
# (EN) Test Raw strings

نص raw1 = r"C:\Users\test"
نص raw2 = r"pattern: \d+\.\d+"
نص raw3 = r"Line 1\nLine 2"

# (AR) اختبار F-strings
# (EN) Test F-strings

نص name = "أحمد"
رقم age = 25

نص fstring1 = f"اسمي {name}"
نص fstring2 = f"عمري {age}"
نص fstring3 = f"الحساب: {2 + 3}"

# (AR) طباعة النتائج
# (EN) Print results

print(bin1)
print(hex1)
print(عربي)
print(unicode16)
print(raw1)
print(fstring1)
