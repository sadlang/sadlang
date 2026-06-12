# ======================================================================
# test_pointers_05.s - اختبار تحويل المؤشرات
# Stage 1 Test: Pointer Casting
# ======================================================================
# الوصف / Description:
#   اختبار تحويل أنواع المؤشرات
#   Test pointer type casting
#
# Expected Output:
#   42
# ======================================================================

دالة main()
    رقم x = 42
    
    # تحويل مؤشر رقم إلى مؤشر عام / Cast number pointer to void pointer
    مؤشر<رقم> ptr_num = &x
    مؤشر<فارغ> ptr_void = ptr_num
    
    # تحويل مرة أخرى / Cast back
    مؤشر<رقم> ptr_num2 = ptr_void
    
    اطبع(*ptr_num2)  # Expected: 42
نهاية
