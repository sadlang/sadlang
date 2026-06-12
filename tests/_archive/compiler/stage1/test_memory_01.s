# ======================================================================
# test_memory_01.s - اختبار تخصيص الذاكرة على Stack
# Stage 1 Test: Stack Memory Allocation
# ======================================================================
# الوصف / Description:
#   اختبار تخصيص الذاكرة على الـ Stack
#   Test stack memory allocation
#
# Expected Output:
#   42
#   99
# ======================================================================

دالة test_function()
    # متغيرات محلية على الـ Stack
    رقم local_var = 42
    إرجاع local_var
نهاية

دالة main()
    رقم result = test_function()
    اطبع(result)  # Expected: 42
    
    # المتغيرات المحلية تُدمَّر بعد الخروج من الدالة
    رقم x = 99
    اطبع(x)  # Expected: 99
نهاية
