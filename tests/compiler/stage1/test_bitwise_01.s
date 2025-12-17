# ======================================================================
# test_bitwise_01.s - اختبار العمليات الثنائية الأساسية
# Stage 1 Test: Basic Bitwise Operations (AND, OR, XOR)
# ======================================================================
# الوصف / Description:
#   اختبار عمليات AND, OR, XOR الثنائية
#   Test AND, OR, XOR bitwise operations
#
# Expected Output:
#   8     # 0b1010 & 0b1100 = 0b1000 = 8
#   14    # 0b1010 | 0b1100 = 0b1110 = 14
#   6     # 0b1010 ^ 0b1100 = 0b0110 = 6
# ======================================================================

دالة main()
    رقم a = 10   # 0b1010
    رقم b = 12   # 0b1100
    
    # AND operation
    رقم and_result = a & b
    اطبع(and_result)  # Expected: 8
    
    # OR operation
    رقم or_result = a | b
    اطبع(or_result)  # Expected: 14
    
    # XOR operation
    رقم xor_result = a ^ b
    اطبع(xor_result)  # Expected: 6
نهاية
