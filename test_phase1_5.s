# ======================================================================
# اختبار المرحلة 1.5: Raw Strings و F-strings
# Phase 1.5 Test: Raw Strings and F-strings
# ======================================================================

print("=== اختبار Raw Strings === \n")
print("=== Raw Strings Test ===\n")

# Raw string - backslashes are literal
# نص خام - الـ backslashes حرفية
نص path = r"C:\Users\test\file.txt"
print("Path: ")
print(path)
print("\n")
نص regex = r"\d+\.\d+"
print("Regex: ")
print(regex)
print("\n")

print("")
print("=== اختبار F-Strings ===\n")
print("=== F-Strings Test ===\n")

# F-string with single variable
# f-string مع متغير واحد
نص name = "أحمد"
نص greeting = f"مرحبا {name}!"
print(greeting)
print("\n")
# F-string with multiple variables
# f-string مع متغيرات متعددة
رقم age = 25
نص info = f"{name} عمره {age} سنة"
print(info)
print("\n")
# F-string with Arabic variable
# f-string مع متغير عربي
رقم العدد = 42
نص msg = f"العدد هو: {العدد}"
print(msg)
print("\n")
print("")
print("✅ جميع الاختبارات نجحت")
print("\n")
print("✅ All tests passed")
