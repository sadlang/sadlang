# =========================================================================
# Set Comprehension Test - اختبار استيعاب المجموعة
# =========================================================================
# (AR) يختبر ميزة استيعاب المجموعة (Set Comprehensions)
# (EN) Tests set comprehension feature
# =========================================================================

اطبع("=== Set Comprehension Tests ===")

# Test 1: Simple set comprehension
# (AR) اختبار 1: استيعاب مجموعة بسيط
اطبع("\nTest 1: Simple set comprehension")
مجموعة1 = {س لكل س في مدى(5)}
اطبع(مجموعة1) # {0, 1, 2, 3, 4}

# Test 2: Set comprehension with transformation
# (AR) اختبار 2: استيعاب مجموعة مع تحويل
اطبع("\nTest 2: Set comprehension with transformation")
مجموعة2 = {س**2 لكل س في مدى(5)}
اطبع(مجموعة2) # {0, 1, 4, 9, 16}

# Test 3: Set comprehension with condition
# (AR) اختبار 3: استيعاب مجموعة مع شرط
اطبع("\nTest 3: Set comprehension with condition")
مجموعة3 = {س لكل س في مدى(10) إذا (س % 2 == 0)}
اطبع(مجموعة3) # {0, 2, 4, 6, 8}

# Test 4: Set comprehension removes duplicates
# (AR) اختبار 4: استيعاب المجموعة يزيل المكررات
اطبع("\nTest 4: Set comprehension removes duplicates")
أرقام = [1، 2، 2، 3، 3، 3، 4، 4، 4، 4]
مجموعة4 = {ع لكل ع في أرقام}
اطبع(مجموعة4) # {1, 2, 3, 4} - without duplicates

# Test 5: Set comprehension with expression
# (AR) اختبار 5: استيعاب مجموعة مع تعبير
اطبع("\nTest 5: Set comprehension with expression")
مجموعة5 = {س*2 + 1 لكل س في مدى(5)}
اطبع(مجموعة5) # {1, 3, 5, 7, 9}

# Test 6: Set comprehension with complex condition
# (AR) اختبار 6: استيعاب مجموعة مع شرط معقد
اطبع("\nTest 6: Set comprehension with complex condition")
مجموعة6 = {س لكل س في مدى(-5، 6) إذا (س > 0 && س < 5)}
اطبع(مجموعة6) # {1, 2, 3, 4}

# Test 7: Empty set comprehension
# (AR) اختبار 7: استيعاب مجموعة فارغ
اطبع("\nTest 7: Empty set comprehension")
مجموعة_فارغة = {س لكل س في []}
اطبع(مجموعة_فارغة) # []

# Test 8: Set comprehension from list with duplicates
# (AR) اختبار 8: استيعاب مجموعة من قائمة مع مكررات
اطبع("\nTest 8: Set from list with duplicates")
بيانات = [1، 1، 2، 2، 3، 3، 4، 5، 5]
مجموعة_فريدة = {ع*ع لكل ع في بيانات}
اطبع(مجموعة_فريدة) # Squares without duplicates

اطبع("\n=== All Set Comprehension Tests Completed! ===")
