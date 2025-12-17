# ======================================================================
# test_pointers_03.s - اختبار المؤشرات المزدوجة
# Stage 1 Test: Double Pointers
# ======================================================================
# الوصف / Description:
#   اختبار المؤشرات للمؤشرات (pointer to pointer)
#   Test pointer to pointer (double pointers)
#
# Expected Output:
#   42
#   100
# ======================================================================

دالة main()
    رقم x = 42
    مؤشر<رقم> ptr1 = &x
    مؤشر<مؤشر<رقم>> ptr2 = &ptr1
    
    # الوصول للقيمة عبر مؤشر مزدوج / Access through double pointer
    اطبع(**ptr2)  # Expected: 42
    
    # تعديل القيمة عبر مؤشر مزدوج / Modify through double pointer
    **ptr2 = 100
    اطبع(x)  # Expected: 100
نهاية
