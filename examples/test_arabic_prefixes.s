# ======================================================================
# اختبار البادئات العربية للسلاسل
# Test Arabic Prefixes for Strings
# ======================================================================

print("=== اختبار النصوص الخام بالعربية ===")
print("\n")
print("=== Test Raw Strings with Arabic Prefix ===")
print("\n")
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
print("\n")
print("=== Test F-Strings with Arabic Prefix ===")
print("\n")
# F-string with Arabic prefix م
# نص منسق بالبادئة العربية م
نص الاسم = "أحمد"
رقم العمر = 25
نص المدينة = "الرياض"
print("\n")
نص greeting = م"مرحبا {الاسم}!"
print(greeting)
print("\n")
نص info = م"{الاسم} عمره {العمر} سنة"
print(info)
print("\n")
نص location = م"يسكن في {المدينة}"
print(location)
print("\n")
print("")
print("=== اختبار مختلط ===")
print("\n")
print("=== Mixed Test ===")
print("\n")
# مزيج من البادئات العربية والإنجليزية
# Mix of Arabic and English prefixes
نص raw_en = r"English\raw\path"
نص raw_ar = ح"عربي\خام\مسار"
نص fmt_en = f"Hello {الاسم}"
نص fmt_ar = م"مرحبا {الاسم}"

print("Raw EN: ")
print("\n")
print(raw_en)
print("\n")
print("Raw AR: ")
print(raw_ar)
print("\n")
print("Format EN: ")
print(fmt_en)
print("\n")
print("Format AR: ")
print(fmt_ar)
print("\n")
print("")
print("✅ جميع الاختبارات نجحت!")
print("✅ All tests passed!")
