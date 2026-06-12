# ======================================================================
# test_pointers_01.s - اختبار المؤشرات الأساسية
# Stage 1 Test: Basic Pointers
# ======================================================================
# الوصف / Description:
#   اختبار أساسيات المؤشرات: الإعلان، الإسناد، Dereferencing
#   Test pointer basics: declaration, assignment, dereferencing
#
# Expected Output:
#   42
#   99
# ======================================================================

دالة main()
    # تعريف متغير عادي / Declare normal variable
    رقم x = 42
    
    # تعريف مؤشر / Declare pointer
    مؤشر<رقم> ptr = &x
    
    # طباعة القيمة عبر المؤشر / Print value through pointer
    اطبع(*ptr)  # Expected: 42
    
    # تعديل القيمة عبر المؤشر / Modify value through pointer
    *ptr = 99
    
    # طباعة القيمة الجديدة / Print new value
    اطبع(x)  # Expected: 99
نهاية
