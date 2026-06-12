# ======================================================================
# test_functions_01.s - اختبار مؤشرات الدوال
# Stage 1 Test: Function Pointers
# ======================================================================
# الوصف / Description:
#   اختبار مؤشرات الدوال واستدعاءها
#   Test function pointers and calling them
#
# Expected Output:
#   SUM: 8
#   MUL: 15
# ======================================================================

دالة sum(رقم a، رقم b) -> رقم
    إرجاع a + b
نهاية

دالة mul(رقم a، رقم b) -> رقم
    إرجاع a * b
نهاية

دالة apply_operation(مؤشر<دالة> op، رقم x، رقم y) -> رقم
    إرجاع op(x، y)
نهاية

دالة main()
    # مؤشر لدالة الجمع / Pointer to sum function
    مؤشر<دالة> op_ptr = &sum
    رقم result1 = apply_operation(op_ptr، 5، 3)
    اطبع("SUM: " + نص(result1))  # Expected: SUM: 8
    
    # مؤشر لدالة الضرب / Pointer to mul function
    op_ptr = &mul
    رقم result2 = apply_operation(op_ptr، 5، 3)
    اطبع("MUL: " + نص(result2))  # Expected: MUL: 15
نهاية
