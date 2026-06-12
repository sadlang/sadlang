# ======================================================================
# test_control_03.s - اختبار القفز المحسوب
# Stage 1 Test: Computed Jumps
# ======================================================================
# الوصف / Description:
#   اختبار القفز المحسوب (القفز بناءً على قيمة متغيرة)
#   Test computed jumps (jump based on variable value)
#
# Expected Output:
#   LABEL_0
#   LABEL_2
# ======================================================================

دالة test_computed_jump(رقم index)
    # جدول القفز / Jump table
    مصفوفة jump_table = [label_0، label_1، label_2]
    
    # القفز المحسوب / Computed jump
    اقفز_محسوب jump_table[index]
    
    تسمية label_0:
        اطبع("LABEL_0")
        إرجاع
    
    تسمية label_1:
        اطبع("LABEL_1")
        إرجاع
    
    تسمية label_2:
        اطبع("LABEL_2")
        إرجاع
نهاية

دالة main()
    test_computed_jump(0)  # Expected: LABEL_0
    test_computed_jump(2)  # Expected: LABEL_2
نهاية
