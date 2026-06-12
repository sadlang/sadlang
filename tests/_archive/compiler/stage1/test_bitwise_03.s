# ======================================================================
# test_bitwise_03.s - اختبار معالجة الأعلام (Flags)
# Stage 1 Test: Flags Manipulation
# ======================================================================
# الوصف / Description:
#   اختبار تعيين وفحص وإزالة الأعلام
#   Test setting, checking, and clearing flags
#
# Expected Output:
#   5     # Original flags
#   7     # After setting bit 1
#   3     # After clearing bit 2
# ======================================================================

دالة main()
    رقم flags = 5  # 0b0101
    
    اطبع(flags)  # Expected: 5
    
    # تعيين bit 1 / Set bit 1
    flags = flags | (1 << 1)  # 0b0101 | 0b0010 = 0b0111 = 7
    اطبع(flags)  # Expected: 7
    
    # إزالة bit 2 / Clear bit 2
    flags = flags & ~(1 << 2)  # 0b0111 & ~0b0100 = 0b0111 & 0b1011 = 0b0011 = 3
    اطبع(flags)  # Expected: 3
نهاية
