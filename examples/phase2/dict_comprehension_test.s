# =========================================================================
# Dict Comprehension Test - اختبار استيعاب القاموس
# =========================================================================
# (AR) يختبر ميزة استيعاب القاموس (Dictionary Comprehensions)
# (EN) Tests dictionary comprehension feature
# =========================================================================

اطبع("=== Dict Comprehension Tests ===")

# Test 1: Simple dict comprehension
# (AR) اختبار 1: استيعاب قاموس بسيط
اطبع("\nTest 1: Simple dict comprehension")
رقم قاموس1 = {س: س**2 لكل س في مدى(5)}
اطبع(قاموس1) # {0: 0, 1: 1, 2: 4, 3: 9, 4: 16}

# Test 2: Dict comprehension with condition
# (AR) اختبار 2: استيعاب قاموس مع شرط
اطبع("\nTest 2: Dict comprehension with condition")
رقم قاموس2 = {س: س*2 لكل س في مدى(10) إذا س % 2 == 0}
اطبع(قاموس2) # {0: 0, 2: 4, 4: 8, 6: 12, 8: 16}

# Test 3: Dict comprehension from list
# (AR) اختبار 3: استيعاب قاموس من قائمة
# Note: String length function might not work, using static values instead
اطبع("\nTest 3: Dict comprehension from list")
مصفوفة أسماء = ["1", "02", "003", "0004"]
رقم قاموس_أسماء = {اسم :طول(اسم)  * 10 لكل اسم في أسماء}
اطبع(قاموس_أسماء) # {"1": 10, "2": 20, "3": 30, "4": 40}

# Test 4: Dict comprehension with transformation
# (AR) اختبار 4: استيعاب قاموس مع تحويل
اطبع("\nTest 4: Dict comprehension with string keys\n")
رقم قاموس_حروف = {ع: "مفتاح_" + طول(ع)*10 لكل ع في ["1", "02", "003", "0004"]}
اطبع(قاموس_حروف) # {"مفتاح_1": 10, "مفتاح_2": 20, "مفتاح_3": 30, "مفتاح_4": 40}







# Test 5: Nested dict comprehension
# (AR) اختبار 5: استيعاب قاموس متداخل
اطبع("\nTest 5: Dict with expression values\n")
أرقام = [1، 2، 3، 4، 5]
رقم مربعات_مكعبات = {رقم: رقم**2 + رقم**3 لكل رقم في أرقام}
اطبع(مربعات_مكعبات) # {1: 2, 2: 12, 3: 36, 4: 80, 5: 150}

# Test 6: Dict comprehension with complex condition
# (AR) اختبار 6: استيعاب قاموس مع شرط معقد
اطبع("\nTest 6: Dict comprehension with complex condition\n")
رقم أرقام_موجبة = {س: س*س لكل س في مدى(-5، 6) إذا س > 0 && س < 5}
اطبع(أرقام_موجبة) # {1: 1, 2: 4, 3: 9, 4: 16}

# Test 7: Empty dict comprehension (filtered out)
# (AR) اختبار 7: استيعاب قاموس فارغ (تمت تصفيته)
اطبع("\nTest 7: Empty dict comprehension\n")
رقم قاموس_فارغ = {س: س لكل س في [] }
اطبع(قاموس_فارغ) # {}

# Test 8: Simple key-value dict comprehension
# (AR) اختبار 8: استيعاب قاموس بسيط مع مفاتيح وقيم
اطبع("\nTest 8: Simple key-value dict comprehension\n")
أرقام_بسيطة = [10، 20، 30]
رقم قاموس_بسيط = {طول(ع): ع لكل ع في أرقام_بسيطة}
اطبع(قاموس_بسيط)

اطبع("\n=== All Dict Comprehension Tests Completed! ===")
