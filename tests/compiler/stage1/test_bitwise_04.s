# ======================================================================
# test_bitwise_04.s - اختبار أقنعة البتات (Bit Masks)
# Stage 1 Test: Bit Masks
# ======================================================================
# الوصف / Description:
#   اختبار استخدام أقنعة البتات لاستخراج وتعديل الأجزاء
#   Test using bit masks to extract and modify parts
#
# Expected Output:
#   240   # High nibble of 255 (0xF0)
#   15    # Low nibble of 255 (0x0F)
# ======================================================================

دالة main()
    رقم value = 255  # 0xFF = 0b11111111
    
    # استخراج النصف العلوي / Extract high nibble
    رقم high_nibble = (value & 0xF0)
    اطبع(high_nibble)  # Expected: 240 (0xF0)
    
    # استخراج النصف السفلي / Extract low nibble
    رقم low_nibble = (value & 0x0F)
    اطبع(low_nibble)  # Expected: 15 (0x0F)
نهاية
