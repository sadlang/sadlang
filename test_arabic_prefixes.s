# ======================================================================
# اختبار البادئات العربية للسلاسل
# Test Arabic Prefixes for Strings
# ======================================================================

print("=== اختبار النصوص الخام بالعربية ===")
print("=== Test Raw Strings with Arabic Prefix ===")

# Raw string with Arabic prefix ح
# نص خام بالبادئة العربية ح
نص path1 = ح"C:\Users\test\file.txt"
نص path2 = ح"D:\Data\files\dataset.csv"
نص regex = ح"\d+\.\d+"

print("Path 1: ")
print(path1)
print("\n")
print("Path 2: ")
print(path2)
print("\n")
print("Regex: ")
print(regex)
print("\n")

print("")
print("=== اختبار النصوص المنسقة بالعربية ===")
print("=== Test F-Strings with Arabic Prefix ===")

# F-string with Arabic prefix م
# نص منسق بالبادئة العربية م
نص الاسم = "أحمد"
رقم العمر = 25
نص المدينة = "الرياض"

نص greeting = م"مرحبا {الاسم}!"
print(greeting)

نص info = م"{الاسم} عمره {العمر} سنة"
print(info)

نص location = م"يسكن في {المدينة}"
print(location)

print("")
print("=== اختبار مختلط ===")
print("=== Mixed Test ===")

# مزيج من البادئات العربية والإنجليزية
# Mix of Arabic and English prefixes
نص raw_en = r"English\raw\path"
نص raw_ar = ح"عربي\خام\مسار"
نص fmt_en = f"Hello {الاسم}"
نص fmt_ar = م"مرحبا {الاسم}"

print("Raw EN: ")
print(raw_en)
print("Raw AR: ")
print(raw_ar)
print("Format EN: ")
print(fmt_en)
print("Format AR: ")
print(fmt_ar)

print("")
print("✅ جميع الاختبارات نجحت!")
print("✅ All tests passed!")
