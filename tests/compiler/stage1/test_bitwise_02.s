# ======================================================================
# test_bitwise_02.s - اختبار NOT والإزاحة
# Stage 1 Test: NOT and Shift Operations
# ======================================================================
# الوصف / Description:
#   اختبار عمليات NOT, Left Shift, Right Shift
#   Test NOT, left shift, right shift operations
#
# Expected Output:
#   -11   # ~10 (NOT operation)
#   40    # 10 << 2 (shift left)
#   2     # 10 >> 2 (shift right)
# ======================================================================

دالة main()
    رقم x = 10  # 0b1010
    
    # NOT operation
    رقم not_result = ~x
    اطبع(not_result)  # Expected: -11 (two's complement)
    
    # Left shift
    رقم shl_result = x << 2  # 0b101000 = 40
    اطبع(shl_result)  # Expected: 40
    
    # Right shift
    رقم shr_result = x >> 2  # 0b10 = 2
    اطبع(shr_result)  # Expected: 2
نهاية
