# ======================================================================
# test_pointers_02.s - اختبار حساب المؤشرات
# Stage 1 Test: Pointer Arithmetic
# ======================================================================
# الوصف / Description:
#   اختبار عمليات الحساب على المؤشرات
#   Test pointer arithmetic operations
#
# Expected Output:
#   1
#   2
#   3
# ======================================================================

دالة main()
    # إنشاء مصفوفة / Create array
    مصفوفة arr = [1، 2، 3، 4، 5]
    
    # مؤشر للعنصر الأول / Pointer to first element
    مؤشر<رقم> ptr = &arr[0]
    
    # طباعة العناصر باستخدام pointer arithmetic
    اطبع(*ptr)         # Expected: 1
    ptr = ptr + 1      # Move to next element
    اطبع(*ptr)         # Expected: 2
    ptr = ptr + 1      # Move to next element
    اطبع(*ptr)         # Expected: 3
نهاية
