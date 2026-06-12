# ======================================================================
# test_bitwise_05.s - اختبار عمليات الدوران (Rotate)
# Stage 1 Test: Rotate Operations
# ======================================================================
# الوصف / Description:
#   اختبار عمليات دوران البتات (محاكاة باستخدام shift + OR)
#   Test bit rotation operations (simulated using shift + OR)
#
# Expected Output:
#   20    # Rotate left 10 (0b1010) by 1 = 0b10100 = 20
#   5     # Rotate right 10 (0b1010) by 1 = 0b0101 = 5
# ======================================================================

دالة main()
    رقم x = 10  # 0b1010
    رقم bits = 8  # عدد البتات / Number of bits
    
    # Rotate Left (ROL)
    # ROL(x, n) = (x << n) | (x >> (bits - n))
    رقم rol_result = (x << 1) | (x >> (bits - 1))
    اطبع(rol_result)  # Expected: 20 (0b10100)
    
    # Rotate Right (ROR)
    # ROR(x, n) = (x >> n) | (x << (bits - n))
    رقم ror_result = (x >> 1) | (x << (bits - 1))
    اطبع(ror_result)  # Expected: 5 (0b0101)
نهاية
