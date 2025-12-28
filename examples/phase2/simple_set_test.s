# Simple set comprehension test
# (AR) اختبار بسيط لاستيعاب المجموعة

# Test 1: Simple set without comprehension
# (AR) اختبار 1: مجموعة بسيطة بدون استيعاب
اطبع("Test 1: Basic list")
أرقام = [0، 1، 2، 3، 4]
اطبع(أرقام)

#Test 2: List comprehension (should work)
# (AR) اختبار 2: استيعاب قائمة (يجب أن يعمل)
اطبع("\nTest 2: List comprehension")
قائمة = [س لكل س في [0، 1، 2، 3، 4]]
اطبع(قائمة)

# Test 3: Simple set comprehension syntax
# (AR) اختبار 3: صيغة استيعاب مجموعة بسيطة
اطبع("\nTest 3: Set comprehension attempt")
# This should create {0, 1, 2, 3, 4}
# مجموعة = {س لكل س في [0، 1، 2، 3، 4]}
# اطبع(مجموعة)

اطبع("Tests complete")
