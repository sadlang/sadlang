# ======================================================================
# test_control_01.s - اختبار التسميات والقفز (Labels & Goto)
# Stage 1 Test: Labels and Goto
# ======================================================================
# الوصف / Description:
#   اختبار استخدام التسميات والقفز المباشر
#   Test using labels and direct jumps (goto)
#
# Expected Output:
#   START
#   MIDDLE
#   END
# ======================================================================

دالة main()
    اطبع("START")
    
    # القفز إلى تسمية / Jump to label
    اقفز إلى middle_label
    
    # هذا السطر لن يُنفَّذ / This line won't execute
    اطبع("SKIPPED")
    
    # التسمية المتوسطة / Middle label
    تسمية middle_label:
    اطبع("MIDDLE")
    
    اقفز إلى end_label
    
    # هذا السطر لن يُنفَّذ / This line won't execute
    اطبع("ALSO_SKIPPED")
    
    # التسمية النهائية / End label
    تسمية end_label:
    اطبع("END")
نهاية
