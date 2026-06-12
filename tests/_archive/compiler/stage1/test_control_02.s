# ======================================================================
# test_control_02.s - اختبار Switch/Case
# Stage 1 Test: Switch/Case Statement
# ======================================================================
# الوصف / Description:
#   اختبار جملة switch/case للقفز متعدد الاتجاهات
#   Test switch/case statement for multi-way branching
#
# Expected Output:
#   CASE_1
#   CASE_2
#   DEFAULT
# ======================================================================

دالة test_switch(رقم value)
    اختر (value)
        حالة 1:
            اطبع("CASE_1")
            توقف
        حالة 2:
            اطبع("CASE_2")
            توقف
        حالة 3:
            اطبع("CASE_3")
            توقف
        افتراضي:
            اطبع("DEFAULT")
    نهاية
نهاية

دالة main()
    test_switch(1)  # Expected: CASE_1
    test_switch(2)  # Expected: CASE_2
    test_switch(99) # Expected: DEFAULT
نهاية
