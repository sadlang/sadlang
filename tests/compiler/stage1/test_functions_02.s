# ======================================================================
# test_functions_02.s - اختبار Inline Assembly (مستقبلي)
# Stage 1 Test: Inline Assembly (Future)
# ======================================================================
# الوصف / Description:
#   اختبار تضمين assembly code مباشرةً (ميزة مستقبلية)
#   Test embedding assembly code directly (future feature)
#
# Expected Output:
#   NATIVE
#   42
# ======================================================================

دالة main()
    اطبع("NATIVE")
    
    # Assembly مُضمَّن (x86-64)
    # Inline assembly (x86-64)
    # asm {
    #     mov rax, 42
    #     ret
    # }
    
    # حالياً: محاكاة النتيجة
    # Currently: simulate result
    رقم result = 42
    اطبع(result)  # Expected: 42
نهاية
